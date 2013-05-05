#ifndef STI_TIMINGENGINE_LOCALEVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_LOCALEVENTENGINEMANAGER_H

#include "TimingEngineTypes.h"

#include "EngineID.h"
#include "EventEngineManager.h"
//#include "EventEngineStateMachine.h"
#include "EventEngineState.h"
#include "SynchronizedMap.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>


namespace STI
{
namespace TimingEngine

{

//This engine manager may be general enough to be the server and the device engine manager.
//The decoupling between the functionality of the manager and the thread spawning of the associated
//queue allows flexibility. The manager can be used directly (with no thread spawning) for testing purposes.
//The queued manager calls the same functions using a FIFO based event driven model which useful for ensuring
//prompt return of the calling thread as well as multiple execution threads.

//Device-side

//delegatedEngineManger->delegateTrigger(engineID, trigger);



class LocalEventEngineManager : public EventEngineManager
{
public:

	LocalEventEngineManager();
	~LocalEventEngineManager();

	//load policy should be function of the manager, since it depends on interactions between the engines
//	LoadPolicy& getLoadPolicy() { return policy; }

//	void lock(const EngineInstance& engineInstance);
//	void unlock(const EngineInstance& engineInstance);

	bool addEventEngine(const STI::TimingEngine::EngineID& engineID, EventEngine_ptr& engine);
	void removeAllEngines();
	bool hasEngine(const STI::TimingEngine::EngineID& engineID) const;
	void getEngineIDs(EngineIDSet& ids) const;

	EventEngineState getState(const EngineID& engineID) const;
	bool inState(const EngineID& engineID, EventEngineState state) const;


	void clear(const STI::TimingEngine::EngineID& engineID);

//	void parseBypass(const EngineID& engineID, SynchronousEvents events) {}
	void parse(const STI::TimingEngine::EngineInstance& engineInstance, const STI::TimingEngine::TimingEventVector_ptr& eventsIn, 
		const ParsingResultsHandler_ptr& results);

	void load(const STI::TimingEngine::EngineInstance& engineInstance);

	//which one gets called determines if the trigger has been delegated to this engine instance
	//avoids the need for tagged local storage
	//Only one manager can be delegated a trigger per play instance
	void trigger(const STI::TimingEngine::EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger);
	void trigger(const STI::TimingEngine::EngineInstance& engineInstance);

	//Patching needs to associate a RawEvent with each synched event. 
	//When new patch RawEvent comes it, it generates a new set of synched events 
	//that can be used to override the old ones.
	//single engine play.  Add a multiengine play for patched sequences?
	void play(const STI::TimingEngine::EngineInstance& engineInstance, double startTime, double endTime, 
		short repeats, const DocumentationOptions_ptr& docOptions);

	void stop(const STI::TimingEngine::EngineID& engineID);
	void pause(const STI::TimingEngine::EngineID& engineID);
	void resume(const STI::TimingEngine::EngineInstance& engineInstance);
	
	void publishData(const STI::TimingEngine::EngineInstance& engineInstance, const MeasurementResultsHandler_ptr& resultsHander);	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).

private:
	
	bool getEngine(const STI::TimingEngine::EngineID& engineID, EventEngine_ptr& engine) const;

	bool setState(const STI::TimingEngine::EngineID& engineID, EventEngineState newState);
	bool setState(EventEngine_ptr& engine, EventEngineState newState);

	bool waitForTrigger(const STI::TimingEngine::EngineInstance& engineInstance, EventEngine_ptr& engine);
	void armLocalTrigger(const EngineInstance& engineInstance, EventEngine_ptr& engine);
	void trigger(EventEngine_ptr& engine);
	void resetLocalTrigger();


	void stop(EventEngine_ptr& engine);
	
	bool requestLoad(const STI::TimingEngine::EngineID& engineID, EventEngine_ptr& engine);
	bool requestPlay(const STI::TimingEngine::EngineID& engineID, EventEngine_ptr& engine);
	bool isLoading(const STI::TimingEngine::EngineID& engineID);
	bool isPlaying(const STI::TimingEngine::EngineID& engineID);

	void setupStateLists();

	STI::Utils::SynchronizedMap<const EngineID, EventEngine_ptr> engines;

	Trigger_ptr localTrigger;

	LoadAccessPolicy_ptr loadPolicy;

	std::set<EventEngineState> playStates;
	std::set<EventEngineState> loadStates;

	long triggerTimeout_s;
	long timeout_s;
	bool triggerReceived;
	
	mutable boost::timed_mutex parserMutex;
	mutable boost::timed_mutex stateMutex;
	mutable boost::shared_mutex triggerMutex;
	
	boost::condition_variable_any triggerCondition;

	
//	bool lockTimeoutTriggered;
//	std::set<EngineID> enginesWithLock;
	
};	
	


	//struct EngineWithPolicy
	//{
	//	EngineWithPolicy(boost::shared_ptr<EventEngine> Engine,	boost::shared_ptr<LoadPolicy> Policy) 
	//		: engine(Engine), policy(Policy)

	//	boost::shared_ptr<EventEngine> engine;
	//	boost::shared_ptr<LoadPolicy> policy;
	//};

	//STI::Utils::SynchronizedMap<EngineID, EngineWithPolicy> engines;


	//enum EventEngineManagerState {
	//AcceptingCommands,
	//PreparingToLoad, 
	//PreparingToPlay, 
	//Playing
	//};

//testing...


//class EventEngineOperator
//class EventEngineController
//class EventEngineManagerGroup



//class EventEngineServer
//{
//
//	void prepare(EngineID& id, TimingEvents events)
//	void parse(EngineID& id, TimingEvents events)
//	{
//		createEngineIDs(id);		//if !engineManager.hasEngine(engineID), addEventEngine(engineID)
//
//		for(all independent engine managers)
//			manager->parse(id, events[managerID], resultHandlerRef);
//
//		waitForParsers();
//
//		manager->load(id);
//	}
//	
//	void play(EngineInstance& id, double startTime, double endTime, short repeats)
//	{
//		if( !loaded(id) ) {
//			for(all managers)
//				manager->load(id);
//		}
//
//		if( !loaded(id) )
//			return;
//
//		delegateTrigger(id);
//
//		trigger(id);
//
//		//play each segment of the sequence if interrupted by waits
//		//this function gets control again at a pause
//		play(id);
//	}
//	void stop(EngineID& id);
//
//};



}
}


	//bool isPlaying()
	//{
	//	boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
	//		boost::get_system_time() + boost::posix_time::seconds(timeout));

	//	if( !stateLock.owns_lock() ) {
	//		throw StateException();	//timed out
	//	}

	//	bool playing = false;

	//	for(all engines) {
	//		try {
	//			playing |= isPlaying(engine);
	//		} catch(EngineException& ex) {
	//			//engine not found; ignore
	//			//clean up lockHolders?
	//			//playing = isPlaying();
	//			//break; ?
	//		}
	//	}

	//	return playing;
	//}





	//bool lock(const EngineID& engineID)
	//{
	//	//trylock should be used here, or maybe short timeout
	//	unique_lock policyLock;	//mutex lock for policy
	//	if(policy not locked)
	//		return false;

	//	stateReadLock;
	//	bool lockAllowed = true;

	//	for(all enginesWithLock) {
	//		lockAllowed &= policy.isSharedAccessAllowed(engineID, id) );

	//		if (!lockAllowed) {
	//			break;
	//		}
	//	}

	//	if(lockAllowed) {
	//		return true;
	//	}

	//	try {
	//		if(isPlaying()) {
	//			//sharedPlay policy??
	//			return false;
	//		}
	//	} catch(StateException& ex) {
	//		return false;
	//	}

	//	//Lock denied, but no engine is playing. Check for lock timeout

	//	if(!lockTimeoutTriggered) {
	//		lastLockRequestTime = boost::get_system_time();		//starts timeout clock on first denial
	//		lockTimeoutTriggered = true;
	//		return false;
	//	}

	//	if(lastLockRequestTime + lockTimeout < boost::get_system_time()) {
	//		//Last lock(s) timed out:
	//		//  *Lock request is accepted.
	//		//  *Unlock all conflicting engines.
	//		//  *Refresh enginesWithLock list

	//		lockTimeoutTriggered = false;	//reset

	//		enginesWithLock.clear();	//rebuild the enginesWithLock list
	//		enginesWithLock.insert(engineID);
	//		
	//		for(all ids) {
	//			if( hasLock(id) ) {
	//				if( policy.isSharedAccessAllowed(engineID, id) ) {
	//					enginesWithLock.insert(id);	//no conflict; this engine stays locked
	//				} else {
	//					unlock(id);	//must succeed
	//				}
	//			}
	//		}
	//	}

	//	return false;

	//	
	//	//lock request denied. Another engine is loaded and policy doesn't premit shared loading.  
	//	//If it's playing, it keeps the lock, but if it's only loaded, it can lose the lock after a timeout now that the lock is in demand.

	//}

	//bool hasLock(const EngineID& engineID)
	//{
	//	boost::shared_ptr<EventEngine> engine;
	//
	//	if( !getEngine(engineID, engine) )
	//		return false;

	//	return lockStates.count( engine->getState() ) == 1;
	//}
	//void unlock(const EngineID& engineID)
	//{
	//	stop(engineID);

	//	leaveState(engineID, Stopping);

	//	if(!setState(engineID, Parsed)) {

	//		//Error; this should not occur
	//		//Attempt to clear.
	//		cerr << "Error during unlock." << endl;

	//		clear(engineID);

	//		if(!engine->inState(Empty))
	//			stop(engineID);
	//	}
	//}




#endif
