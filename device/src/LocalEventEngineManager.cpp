

#include "LocalEventEngineManager.h"

#include "EventEngine.h"
#include "GlobalLoadAccessPolicy.h"
#include "LoadAccessPolicy.h"

#include "Trigger.h"
#include "ParsingResultsHandler.h"

#include "EngineException.h"
#include "EngineTimestampException.h"

#include "TimingMeasurementGroup.h"

#include "MeasurementResultsHandler.h"
#include "PlayOptions.h"
#include "EngineCallbackHandler.h"
#include "NullEngineCallbackHandler.h"

#include <boost/thread/locks.hpp>

//#include "LocalTrigger.h"

using STI::TimingEngine::LocalEventEngineManager;
using STI::TimingEngine::EventEngineState;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EngineInstance;
using STI::TimingEngine::TimingEventVector_ptr;
using STI::TimingEngine::EventEngine_ptr;
using STI::TimingEngine::MasterTrigger_ptr;
using STI::TimingEngine::ParsingResultsHandler_ptr;
using STI::TimingEngine::DocumentationOptions_ptr;
using STI::TimingEngine::EngineTimestampException;
using STI::TimingEngine::EngineException;
using STI::TimingEngine::TimingMeasurementGroup_ptr;
using STI::TimingEngine::LoadAccessPolicy_ptr;
using STI::TimingEngine::GlobalLoadAccessPolicy;
using STI::TimingEngine::EngineIDSet;
using STI::TimingEngine::MeasurementResultsHandler_ptr;
using STI::TimingEngine::PlayOptions_ptr;
using STI::TimingEngine::EngineCallbackHandler_ptr;

#include <iostream>
using namespace std;

LocalEventEngineManager::LocalEventEngineManager()
{
	timeout_s = 1;


	setupStateLists();

	//Default load policy restricts being loaded to one engine at a time.
	LoadAccessPolicy_ptr defaultPolicy = LoadAccessPolicy_ptr( new GlobalLoadAccessPolicy(false, false) );
	setLoadPolicy(defaultPolicy);

	//Default trigger
//	localTrigger = Trigger_ptr( new LocalTrigger() );

}
LocalEventEngineManager::~LocalEventEngineManager()
{
//	cout << "Destroying LocalEventEngineManager" << endl;
}
void LocalEventEngineManager::setLoadPolicy(const LoadAccessPolicy_ptr& newPolicy)
{
	loadPolicy = newPolicy;
}

void LocalEventEngineManager::setupStateLists()
{
	playStates.insert(PreparingToPlay);
	playStates.insert(WaitingForTrigger);
	playStates.insert(Triggered);
	playStates.insert(Playing);
	playStates.insert(Pausing);
	playStates.insert(Paused);
	playStates.insert(PreparingToResume);

	loadStates.insert(PreparingToLoad);
	loadStates.insert(Loading);
}
bool LocalEventEngineManager::addEventEngine(const EngineID& engineID, EventEngine_ptr& engine)
{
	if(engine != 0)
		return engines.add(engineID, engine);
	else
		return false;
}

bool LocalEventEngineManager::hasEngine(const STI::TimingEngine::EngineID& engineID) const
{
	return engines.contains(engineID);
}

void LocalEventEngineManager::removeAllEngines()
{
	engines.clear();
}

void LocalEventEngineManager::getEngineIDs(EngineIDSet& ids) const
{
	engines.getKeys(ids);
}

bool LocalEventEngineManager::getEngine(const EngineID& engineID, EventEngine_ptr& engine) const
{
	return engines.get(engineID, engine) && engine != 0;
}

EventEngineState LocalEventEngineManager::getState(const EngineID& engineID) const
{
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))	
		return STI::TimingEngine::EngineStateUnknown;		//can't find engine
	return engine->getState();
}

bool LocalEventEngineManager::inState(const EngineID& engineID, EventEngineState state) const
{
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))	
		return false;		//can't find engine
	return engine->inState(state);
}


void LocalEventEngineManager::clear(const EngineID& engineID, const EngineCallbackHandler_ptr& clearCallback)
{	
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))	
		return;		//can't find engine

	if(!setState(engine, Clearing)) {
		stop(engine);
		if(!setState(engine, Clearing)) {
			return;
		}
	}

	engine->preClear();
	engine->clear(clearCallback);
	engine->postClear();

	setState(engine, Empty);

	if(clearCallback != 0) {
		clearCallback->callback(engine->getState());
	}
}


void LocalEventEngineManager::parse(const STI::TimingEngine::EngineInstance& engineInstance, 
									const TimingEventVector_ptr& eventsIn, 
									const ParsingResultsHandler_ptr& results)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))	
		return;		//can't find engine

	//only one can parse at a time; 
	//doesn't need to lock the engines though (can be playing or loading on other engines)


	//Important! Establishes that the last static state is Empty. 
	//In case of an stop/abort during Parsing, the engine will revert 
	//to the last static state).
	if(!engine->inState(Empty)) {
		stop(engine);
		return;
	}

	if(!setState(engine, Parsing)) {
		stop(engine);
		return;
	}
	
	//timed lock, followed by check for Stop, repeat
	boost::unique_lock<boost::timed_mutex> parserLock(parserMutex, 
		boost::get_system_time() + boost::posix_time::seconds(timeout_s));

	if( !parserLock.owns_lock() ) {
		stop(engine);		//should keep trying until stop...
		return;	//timed out
	}

	engine->preParse();
	
//	results->engineID = engineInstance.id;
	engine->parse(engineInstance.parseTimestamp, eventsIn, results);
	
	if(!results->parseSucceeded()) {
		stop(engine);
		clear(engineInstance.id, STI::TimingEngine::NullEngineCallbackHandler::createNullHandler());
		return;
	}

	engine->postParse();

	if(!setState(engine, Parsed)) {
		stop(engine);
	}
}


//Most sophisicated load concept allows groups of engines to share a common subset of synched events
//and an engine-specific "patch" list of events.  Need to keep track of what events are loaded.
//Simplier version only allows patching when the event lists are the same length and the times all all the same.
//Then only a few of the values need to change.

//Some devices can be loaded on multiple engines simultaneously 
//(e.g., an FPGA device with two independent memory mapped regions)

void LocalEventEngineManager::load(const EngineInstance& engineInstance, const EngineCallbackHandler_ptr& loadCallBack)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))
		return;		//can't find engine

	if(!engine->inState(Parsed))
		return;		//must be parsed to load; in particular, if it's already loaded it will not load again...

	if(!setState(engine, RequestingLoad))		//Loading state also indicates a device trying to load; it may not be allowed to load yet if the engine is locked.
		return;

	while( engine->inState(RequestingLoad) && !requestLoad(engineInstance.id, engine) ) {}

	if(!engine->inState(PreparingToLoad)) {
		stop(engine);
		return;	//Load request was aborted or failed
	}

	try {

		if(!setState(engine, Loading)) {
			stop(engine);
			return;
		}

		engine->preLoad();

		engine->load(engineInstance.parseTimestamp, loadCallBack);

		engine->postLoad();
	} 
	catch(const EngineTimestampException&) {
		clear(engineInstance.id, loadCallBack);
	}
	catch(const EngineException&) {
		stop(engine);
	}

	if( !setState(engine, Loaded) ) {
		//Error: load failed for some reason
		if(!engine->leaveState(Loading)) {
			stop(engine);
		}
	}

	if(loadCallBack != 0) {
		loadCallBack->callback(engine->getState());
	}
}


bool LocalEventEngineManager::requestLoad(const EngineID& engineID, EventEngine_ptr& engine)
{
	//Get state lock or timeout => no engine can change state
	boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
		boost::get_system_time() + boost::posix_time::seconds(timeout_s));

	if( !stateLock.owns_lock() ) 
		return false;	//timed out

	bool allowed = true;
	EventEngine_ptr otherEngine;
	EngineIDSet engineIDs;
	engines.getKeys(engineIDs);

	//Check policy for loading this engine when other engines are playing or loaded.
	//Unload other engines if the policy requires it.
	for(EngineIDSet::iterator id = engineIDs.begin(); (allowed && id != engineIDs.end()); ++id) {
		if(isPlaying(*id)) {
			allowed &= loadPolicy->loadWhilePlayingAllowed(engineID, *id);
		}
		else if(isLoading(*id)) {
			allowed = false;	//only one can load at a time
		}
		else if(getEngine(*id, otherEngine) &&
			otherEngine->inState(Loaded) &&
			!loadPolicy->loadWhileLoadedAllowed(engineID, *id)) 
		{
			//Found a loaded 'otherEngine' and the LoadPolicy does not allow 
			//shared loading between otherEngine and the requesting engineID.
			//Unload otherEngine.

//			otherEngine->setState(Unloading);
//			otherEngine->unload();

			otherEngine->setState(Parsed);

			allowed = !otherEngine->inState(Loaded);	//make sure the otherEngine is not Loaded
		}
	}

	if(allowed) {
		return engine->setState(PreparingToLoad);	//Bypasss local version of setState() we already have the lock
//		return setState(engine, PreparingToLoad);
	}
	return false;
}
bool LocalEventEngineManager::isLoading(const EngineID& engineID)
{
	EventEngine_ptr engine;
	if( !getEngine(engineID, engine) )
		return false;

	return loadStates.count( engine->getState() ) == 1;
}
bool LocalEventEngineManager::waitForTrigger(const EngineInstance& engineInstance, EventEngine_ptr& engine, const EngineCallbackHandler_ptr& callBack)
{
	if(!setState(engine, WaitingForTrigger)) {
		stop(engine);
		return false;
	}

	engine->waitForTrigger(callBack);



	//if( localTrigger != 0 && localTrigger->isMasterTrigger() ) {
	//	
	//	armLocalTrigger(engineInstance, engine);
	//}
	//else {
	//	//Enter a wait state. A call to trigger() will release this wait.

	//	//move this into engine
	//	engine->waitForTrigger();

	//	boost::unique_lock< boost::shared_mutex > triggerLock(triggerMutex);
	//	boost::system_time wakeTime;

	//	while( engine->inState(WaitingForTrigger) && !triggerReceived)
	//	{
	//		wakeTime = boost::get_system_time() + boost::posix_time::seconds( static_cast<long>(triggerTimeout_s) );

	//		triggerCondition.timed_wait(triggerLock, wakeTime);		//timed wait so it doesn't hang here if something goes wrong.
	//	}
	//	triggerReceived = false;	//reset so next time we can catch an early trigger

	//}

	//Should be in state Triggered
	if(!setState(engine, Playing)) {
		stop(engine);
		return false;
	}
	
	return true;
}
//
//void LocalEventEngineManager::armLocalTrigger(const EngineInstance& engineInstance, EventEngine_ptr& engine)
//{
//	//Implementation of waitForTrigger may optionally wait for all other engines to reach WaitingForTrigger
//	if( localTrigger != 0 && !localTrigger->waitForTrigger(engineInstance.id) ) {
//		stop(engine);
//		localTrigger->stopAll(engineInstance.id);
//		return;
//	}
//
//	//The local Trigger received a trigger pulse
//	if(!setState(engine, Triggered)) {
//		stop(engine);
//		if(localTrigger != 0) {
//			localTrigger->stopAll(engineInstance.id);
//		}
//		return;
//	}
//
//	//trigger() all other engines
//	if(localTrigger != 0)
//		localTrigger->triggerAll(engineInstance);
//}
//
//void LocalEventEngineManager::resetLocalTrigger()
//{
//	boost::unique_lock< boost::shared_mutex > triggerLock(triggerMutex);
//	if(localTrigger != 0) {
//		localTrigger->setIsMasterTrigger(false);
//	}
//	triggerReceived = false;
//}

//If delegating, server only sends trigger to one event engine manager, using this function
void LocalEventEngineManager::trigger(const EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))	
		return;		//can't find engine


	engine->trigger(delegatedTrigger);

	//Install the MasterTrigger object in the local trigger.
	//This indicates that the MasterTrigger has been delegated to this device for this EngineInstance.

//	if(localTrigger != 0) {
//		localTrigger->setIsMasterTrigger(true);
//		localTrigger->installMasterTrigger(delegatedTrigger);	//sets isMasterTrigger = true;
//	}
//
////	waitForLocalTrigger(engine);
//	
//	armLocalTrigger(engineInstance, engine);

	//if( !engine->inState(Triggered) ) {
	//	stop(engine);
	//	return;
	//}

	////Trigger the rest of the devices
	//localTrigger->triggerAll();



	//Release the local engine from waitForTrigger().  This might happen automatically if the MasterTrigger has all the reference...
//	trigger(engine);
}


void LocalEventEngineManager::trigger(const EngineInstance& engineInstance)
{	
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))	
		return;		//can't find engine

	engine->trigger();
//	trigger(engine);
}

//void LocalEventEngineManager::trigger(EventEngine_ptr& engine)
//{
//	////This makes sure the engine doesn't wait if the trigger comes before the engine calls waitForTrigger.
//	//{
//	//	boost::unique_lock< boost::shared_mutex > triggerLock(triggerMutex);
//	//	triggerReceived = true;
//	//}
//	
//	//if(!setState(engine, Triggered)) {
//	//	stop(engine);
//	//	return;
//	//}
//
//	engine->trigger();
//
////	triggerCondition.notify_one();
//}

//no, segments need to be run inside repeats.
//repeats should mean the entire sequence repeats, not just one segment
//segments best done using STIEvents like Pause or Wait events
//If a device gets a conditional wait event, it is given a ConditionalTrigger
//which it uses to check the conditional and retrigger the waiting system when the 
//condition is met. All other devices receive a Wait. Dependent devices with events are triggered
//immediately by the conditional trigger (?)


//Play control flow:
//[Patch Sequences, Repeats], Segments, Pause-Resumes
//
//Patch load targets get installed during parse. They associate with
//events in another engine and get loaded by the reload call in the other engine.
//Patched sequences need to run using the repeat loop. Could require an ordered list of engines sent to play()...
//Generic sequences run with a single repeat and use the (traditional) server flow control.

void LocalEventEngineManager::play(const EngineInstance& engineInstance, const PlayOptions_ptr& playOptions, 
								   const DocumentationOptions_ptr& docOptions, 
								   const MeasurementResultsHandler_ptr& resultsHander, 
								   const EngineCallbackHandler_ptr& playCallBack)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))	
		return;		//can't find engine

	if(engine->inState(Paused))
		return resume(engineInstance, playCallBack);

	if(!engine->inState(Loaded))
		return;		//must be loaded to play; multiple engines can be loaded at a time, depending on LoadPolicy details

	if(!setState(engine, RequestingPlay))
		return;

	//Only one engine can play at a time.		
	while( engine->inState(RequestingPlay) && !requestPlay(engineInstance.id, engine) ) {}

	if(!engine->inState(PreparingToPlay))	
		return;	//Play request was aborted or failed

	try {

		engine->prePlay(engineInstance.parseTimestamp, engineInstance.playTimestamp, 
			playOptions, docOptions, resultsHander, playCallBack);

		engine->preTrigger(playOptions->startTime, playOptions->endTime);	//same for all repeats (for single segment) so can happen outside repeat loop

		//Reset device trigger (removes any old delegated triggers)
	//	device.getTrigger().reset();
	//	resetLocalTrigger();	//perhaps also reset triggerReceived = false;

		bool continuous = (playOptions->repeats == -1);
		int cycles = playOptions->repeats + 1;

		EngineTimestamp playTimestamp = engineInstance.playTimestamp;
		playTimestamp.repeatID = 0;

		do {
			if(!waitForTrigger(engineInstance, engine, playCallBack)) {
				break;
			}
			engine->play(engineInstance.parseTimestamp, playTimestamp, playOptions, docOptions, resultsHander, playCallBack);

			playTimestamp.repeatID += 1;
			cycles--;
		} while( (cycles > 0 || continuous) && engine->inState(Playing) );

		engine->postPlay();

	}
	catch(const EngineTimestampException&) {
		clear(engineInstance.id, playCallBack);
	}
	catch(const EngineException&) {
		stop(engine);
	}

	if(!setState(engine, Stopping)) {
	}
	if(!setState(engine, Loaded)) {
	}

	if(playCallBack != 0) {
		playCallBack->callback(engine->getState());
	}
}


bool LocalEventEngineManager::requestPlay(const EngineID& engineID, EventEngine_ptr& engine)
{
	//Get state lock or timeout => no engine can change state
	boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
		boost::get_system_time() + boost::posix_time::seconds(timeout_s));

	if( !stateLock.owns_lock() ) 
		return false;	//timed out

	bool playing = false;
	EngineIDSet engineIDs;
	engines.getKeys(engineIDs);

	for(EngineIDSet::iterator id = engineIDs.begin(); (!playing && id != engineIDs.end()); ++id) {
		playing |= isPlaying(*id);	//only one can play at a time
	}

	if(!playing) {
		return engine->setState(PreparingToPlay);	//Bypass local version of setState() because we already have the lock
//		return setState(engine, PreparingToPlay);
	}
	return false;
}
bool LocalEventEngineManager::isPlaying(const EngineID& engineID)
{
	EventEngine_ptr engine;
	if( !getEngine(engineID, engine) )
		return false;

	return playStates.count( engine->getState() ) == 1;
}

void LocalEventEngineManager::stop(const EngineID& engineID)
{
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))
		return;		//can't find engine

	stop(engine);
}

void LocalEventEngineManager::stop(EventEngine_ptr& engine)
{
	if(!setState(engine, Stopping))
		return;

	engine->stop();

	engine->postStop();

	engine->leaveState(Stopping);
}

void LocalEventEngineManager::pause(const EngineID& engineID)
{
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))
		return;		//can't find engine

	if( setState(engine, Pausing) ) {
		engine->pause();
	}
}

void LocalEventEngineManager::resume(const EngineInstance& engineInstance, const EngineCallbackHandler_ptr& resumeCallBack)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))
		return;		//can't find engine

	if( setState(engine, PreparingToResume) ) {
		engine->preResume();

		waitForTrigger(engineInstance, engine, resumeCallBack);

		engine->resume(resumeCallBack);
	}
}

bool LocalEventEngineManager::setState(const EngineID& engineID, EventEngineState newState)
{
	EventEngine_ptr engine;
	if(!getEngine(engineID, engine))
		return false;	//engine not found

	return setState(engine, newState);
}

bool LocalEventEngineManager::setState(EventEngine_ptr& engine, EventEngineState newState)
{
	//get lock or timeout
	boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
		boost::get_system_time() + boost::posix_time::seconds(timeout_s));

	if( !stateLock.owns_lock() ) 
		return false;	//timed out

	return engine->setState(newState);
}



void LocalEventEngineManager::publishData(const EngineInstance& engineInstance, 
										  const MeasurementResultsHandler_ptr& resultsHander, 
										  const DocumentationOptions_ptr& documentation)
{
	EventEngine_ptr engine;
	if(!getEngine(engineInstance.id, engine))
		return;		//can't find engine

	if(!setState(engine, RequestingPublish))
		return;
	
	if(!setState(engine, Publishing))
		return;

	engine->prePublishData();

	TimingMeasurementGroup_ptr data;

	bool success = engine->publishData(engineInstance.playTimestamp, data, resultsHander, documentation);

//	if(success && data != 0 ) {
	if(success && data != 0) {
		resultsHander->handleNewData(engineInstance, data);
		engine->postPublishData();
	}

	stop(engine);
}

