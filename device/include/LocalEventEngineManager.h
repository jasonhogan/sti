#ifndef STI_TIMINGENGINE_LOCALEVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_LOCALEVENTENGINEMANAGER_H

#include "EventEngineManager.h"
#include "SynchronizedMap.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


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

	LocalEventEngineManager()
	{
		setupStateLists();
	}


	LoadPolicy& getLoadPolicy() { return policy; }

	bool addEventEngine(const EngineID& engineID, boost::shared_ptr<EventEngine>& engine)
	{
//		boost::shared_ptr<DeviceEventEngine> engine(newEngine);
		return engines.add(engineID, engine);
	}

	//load policy should be function of the manager, since it depends on interactions between the engines
	bool lock(const EngineID& engineID)
	{
		//trylock should be used here, or maybe short timeout
		unique_lock policyLock;	//mutex lock for policy
		if(policy not locked)
			return false;

		stateReadLock;
		bool lockAllowed = true;

		for(all enginesWithLock) {
			lockAllowed &= policy.isSharedAccessAllowed(engineID, id) );

			if (!lockAllowed) {
				break;
			}
		}

		if(lockAllowed) {
			return true;
		}

		try {
			if(isPlaying())
				return false;
		} catch(StateException& ex) {
			return false;
		}

		//Lock denied, but no engine is playing. Check for lock timeout

		if(!lockTimeoutTriggered) {
			lastLockRequestTime = boost::get_system_time();		//starts timeout clock on first denial
			lockTimeoutTriggered = true;
			return false;
		}

		if(lastLockRequestTime + lockTimeout < boost::get_system_time()) {
			//Last lock(s) timed out:
			//  *Lock request is accepted.
			//  *Unlock all conflicting engines.
			//  *Refresh enginesWithLock list

			lockTimeoutTriggered = false;	//reset

			enginesWithLock.clear();	//rebuild the enginesWithLock list
			enginesWithLock.insert(engineID);
			
			for(all ids) {
				if( hasLock(id) ) {
					if( policy.isSharedAccessAllowed(engineID, id) ) {
						enginesWithLock.insert(id);	//no conflict; this engine stays locked
					} else {
						unlock(id);	//must succeed
					}
				}
			}
		}

		return false;

		
		//lock request denied. Another engine is loaded and policy doesn't premit shared loading.  
		//If it's playing, it keeps the lock, but if it's only loaded, it can lose the lock after a timeout now that the lock is in demand.

	}

	bool isPlaying()
	{
		boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
			boost::get_system_time() + boost::posix_time::seconds(timeout));

		if( !stateLock.owns_lock() ) {
			throw StateException();	//timed out
		}

		bool playing = false;

		for(all engines) {
			try {
				playing |= isPlaying(engine);
			} catch(EngineException& ex) {
				//engine not found; ignore
				//clean up lockHolders?
				//playing = isPlaying();
				//break; ?
			}
		}

		return playing;
	}

	
	bool isPlaying(const EngineID& engineID)
	{
		boost::shared_ptr<EventEngine> engine;
	
		if( !getEngine(engineID, engine) )
			return false;

		return playStates.count( engine->getState() ) == 1;
	}

	bool hasLock(const EngineID& engineID)
	{
		boost::shared_ptr<EventEngine> engine;
	
		if( !getEngine(engineID, engine) )
			return false;

		return lockStates.count( engine->getState() ) == 1;
	}
	
	void unlock(const EngineID& engineID)
	{
		stop(engineID);

		leaveState(engineID, Stopping);

		if(!setState(engineID, Parsed)) {

			//Error; this should not occur
			//Attempt to clear.
			cerr << "Error during unlock." << endl;

			clear(engineID);

			if(!engine->inState(Empty))
				stop(engineID);
		}
	}

	void clear(const EngineID& engineID)
	{
		boost::shared_ptr<EventEngine> engine;
		if(!getEngine(engineID, engine))	
			return;		//can't find engine
			
		setState(engine, Clearing);
		
		engine->preClear();
		engine->clear();
		engine->postClear();

		setState(engine, Empty);
	}


//	void parseBypass(const EngineID& engineID, SynchronousEvents events) {}
	void parse(const EngineID& engineID, STI::Types::TEventSeq eventsIn, ParsingResultsHandler results)
	{
		//clear(engineID);		this should be only called by the server when need to erase all devices. Generally not all devices will need to clear or reparse.

		boost::shared_ptr<EventEngine> engine;

		if(!getEngine(engineID, engine))	
			return;		//can't find engine



		//Do this work in separate thread


		//only one can parse at a time; doesn't need to lock the engines though (can be playing or loading on other engines)
		
		//timed try lock, followed by check for Stop, repeat
		boost::unique_lock< boost::shared_mutex > parserLock(parserMutex);


		try {
			unlock(engineID);

			setState(engine, Empty);	//Important! Establishes that the last static state is Empty. 
										//In case of an stop/abort during Parsing, the engine will revert 
										//to the last static state).
			setState(engine, Parsing);
			
			engine->preParse();
			engine->parseEvents(eventsIn, results);
			engine->postParse();

			setState(engine, Parsed);

		} catch(EngineException e) {
		} catch(StateException e) {
		}
	}

	void load(const EngineInstance& engineInstance)
	{
		//Most sophisicated load concept allows groups of engines to share a common subset of synched events
		//and an engine-specific "patch" list of events.  Need to keep track of what events are loaded.
		//Simplier version only allows patching when the event lists are the same length and the times all all the same.
		//Then only a few of the values need to change.

		//Some devices can be loaded on multiple engines simultaneously 
		//(e.g., an FPGA device with two independent memory mapped regions)

		boost::shared_ptr<EventEngine> engine;

		if(!getEngine(engineInstance.id, engine))	
			return;		//can't find engine

		if(!engine->inState(Parsed))	
			return;		//must be parsed to load; in particular, if it's already loaded it will not load again...

		try {
	
			if(!setState(engine, PreparingToLoad))		//Loading state also indicates a device trying to load; it may not be allowed to load yet if the engine is locked.
				return;

			//Needs lock to load; attempt to get lock
			while(!lock(id) && engine->inState(PreparingToLoad)) {}

			if(!hasLock(id)) {	//load aborted before lock aquired
				//go through stopping first!  stop()? abort?
				leaveState(engine, Loading);
				return;
			}

			if(!setState(engine, Loading))		//Loading state also indicates a device trying to load; it may not be allowed to load yet if the engine is locked.
				return;

			//Once we have lock, we are good to go; no need to change any other engine since there load state reflects the current policy.
			////this engine has lock; Now change all other (unlocked) engines state to reflect their loss of load state (if applicable)

			//Here is where we could try to use a shortcut and only load a "patch", if another engine shares 
			//this load space and it has loaded similar events.

			engine->preLoad();
			engine->load(engineInstance.timestamp);
			engine->postLoad();

			if( !setState(engine, Loaded) ) 
				unlock(id);		//Error: load failed for some reason; unlock the engine.

		} catch(EngineException& e) {
			stop(id);
		} catch(StateException& e) {
			stop(id);
		}
	}

	//which one gets called determines if the trigger has been delegated to this engine instance
	//avoids the need for tagged local storage
	//Only one manager can be delegated a trigger per play instance
	void trigger(const EngineInstance& engineInstance, Trigger delegatedTrigger)
	void trigger(const EngineInstance& engineInstance)
	{
		boost::shared_ptr<EventEngine> engine;
		boost::shared_ptr<Trigger> theTrigger;

		if(!getEngine(engineInstance.id, engine))	
			return;		//can't find engine

		if(!engine->inState(Triggering)) {	
			return;
		}

		//In case this device is responsible for triggering the system (Remote Trigger)
		if(isDelegatedTrigger(engineID)) {
			
			device->getTrigger()->waitBeforePlayTrigger();	//use the locally installed trigger to wait


			engine->remoteTrigger()->trigger();
			getRemoteTrigger(engineID)->trigger();			//tells the server to start the rest of the system


			//Most recent attempt....

			if( engine->getDelegatedTrigger(theTrigger) ) {

				theTrigger->waitBeforeTrigger();

				if(!setState(id, Triggered)) {
					theTrigger->abort();
					stop(id);
					return;
				}

				theTrigger->trigger();
			}
		}	//some issues above with storage of the RemoteTrigger reference for this EngineInstance...

		//Where does this mutex live? Can it be shared by all event engines? EventEngine class doesn't expose any mutex accessors.
		boost::shared_lock< boost::shared_mutex > triggerLock(engine->stateMutex);

		//release hold of the local device engine
		triggerReceived = true;		//this makes sure the engine doesn't wait if the trigger comes before the engine calls waitForTrigger

		//maybe:  setState(engine, Triggered);  could make triggerReceived unneeded?

		triggerCondition->notify_one();
		//might be cool to get the time on the EventEngine clock when the trigger arrives...
		//engine->getTime();

	}
	void waitForTrigger(const EngineInstance& engineInstance, const EventEngine& engine)
	{
		boost::shared_lock< boost::shared_mutex > triggerLock(engine->stateMutex);

		if(!triggerReceived) {
			setState(engine, WaitingForTrigger);
			triggerConditional->wait( triggerLock );
		}
		triggerReceived = false;	//reset so next time we can catch an early trigger




	////////////////////////////////////////
		//attempt 2
		
		//this doesn't work; need a dedicated trigger mutex because need to change the state while protecting the trigger logic...
		boost::shared_lock< boost::shared_mutex > triggerLock(stateMutex);
		
		if( engine->inState(PreparingToPlay))
			if(!setState(engine, WaitingForTrigger))
				return;
		else
			return;

		triggerConditional->wait( triggerLock );

		if(!setState(engine, Triggered))
			setState(engine, Stopping);

	}


	
	void play(const EngineInstance& engineInstance, double startTime, double endTime, short repeats, double repeatTime, STI::Types::TDocumentationOptions docOptions)
	{
		if(!hasLock(id))	
			return;		//needs lock to play

		boost::shared_ptr<EventEngine> engine;

		if(!getEngine(engineInstance.id, engine))	
			return;		//can't find engine

		if(!engine->inState(Loaded))	
			return;		//must be loaded to play; multiple engines can be loaded at a time, depending on LoadPolicy details

		try {
			//Only one engine can play at a time.
			//In the RequestingPlay state, the engine waits for an available slot to play.
			setState(engine, RequestingPlay);

			while(engine->inState(RequestingPlay)) {
				if( !isPlaying() )	//Only one engine can be in PreparingToPlay at a time.
					setState(engine, PreparingToPlay);
			}

			if(!engine->inState(PreparingToPlay))	
				return;	//Play request was aborted or failed

			engine->prePlay();

			//no, segments need to be run inside repeats.
			//repeats should mean the entire sequence repeats, not just one segment
			//segments best done using STIEvents like Pause or Wait events
			//If a device gets a conditional wait event, it is given a ConditionalTrigger
			//which it uses to check the conditional and retrigger the waiting system when the 
			//condition is met. All other devices receive a Wait. Dependent devices with events are triggered
			//immediately by the conditional trigger (?)

			engine->preTrigger(startTime, endTime);

			do {	
				
				//for(all timing segments)	//timing file broken into segments by conditional pauses 
				
				//what happens for continuous? Does the external trigger get to resynch the start time, 
				//or does it just follow the FPGA clock?
				//block, waiting for trigger, with interrupt option
				
				waitFotTrigger(engineInstance, engine);
				//Trigger received; begin playing
				
				if(!setState(engine, Playing)) {
					stop();
					return;
				}

					//rename repeats -> cycles
				engine->play(startTime, endTime, repeats, repeatTime, docOptions);
				cycleNumber++;
				repeat = !conditionMet(???) || (repeats != cycleNumber) || (repeats == -1);

				//during conditional wait, engines without events should pause
				//consider adding an STIPause command to the TimingEvent list which gets pulled out

			} while( repeat && engine->inState(Playing) )

			engine->postPlay();

			leaveState(engine, Playing);
			unlock(engineInstance.id);

		} catch(EngineStopException e) {
			setState(engine, Stopped);
		} catch(EngineException e) {
			if( e.stop() ) {
				setState(engine, Stopped);
			}
		} catch(StateException e) {
		}

		//collectData() thread here?
		//The collectData thread has to also be the "reload" thread in the case of a repeat.
		if(repeats > 0) {
			//reload after collecting measurement data
			//needs to know when each event is played?!? State machine in event too? Or just played, saved flags.
			//The reload feature should occur right after aquiring the data for that event (without delay or another function call)
			setState(engine, Loaded);
		}
	}

	void stop(const EngineID& engineID)
	{
		boost::shared_ptr<EventEngine> engine;

		if(!getEngine(engineID, engine))
			return;		//can't find engine

		//In case stop itself is hung
		if(engine->inState(Stopping)) {
//			engine->stop();	//abort() ?  Or nothing so it returns fast?
			engine->leaveState(Stopping);
		}

		//Do this work in separate thread
		try {
			setState(engine, Stopping);
		
			engine->stop();
			engine->postStop();
			
			engine->leaveState(Stopping);
			setState(engine, engine->getLastStaticState() );

		} catch(EngineException e) {
		} catch(StateException e) {
		}
		
		leaveState(engine, Stopping );
	}
	
	//state setting and isAllowedTransition could go to the Engine itself
	bool setState(const EngineID& engineID, EngineState newState)
	{
		boost::shared_ptr<EventEngine> engine;

		if(!getEngine(engineID, engine))
			return false;	//engine not found

		return setState(engine, newState);
	}
	
	bool setState(boost::shared_ptr<EventEngine>& engine, EngineState newState)
	{
		//get lock or timeout
		boost::unique_lock<boost::timed_mutex> stateLock(stateMutex, 
			boost::get_system_time() + boost::posix_time::seconds(timeout));

		if( !stateLock.owns_lock() ) 
			return false;	//timed out

		return engine->setState(newState);
	}


	

	
	
	
	//global state mutex, all engine share it; only one engine can change state at a time
	//boost::unique_lock<boost::timed_mutex> trylockEx(stateMutex, boost::try_to_lock); ;

	//if (!stateLock.owns_lock())
	//	stateLock.timed_lock(boost::get_system_time() + boost::posix_time::seconds(timeout));

	//getAbsTimeout();


private:
	
	LoadPolicy policy;

	boost::posix_time::seconds timeout;
	bool lockTimeoutTriggered;
	
	mutable boost::timed_mutex stateMutex;

	STI::Utils::SynchronizedMap<EngineID, boost::shared_ptr<EventEngine> > engines;
	std::set<EngineID> enginesWithLock;
		
	std::set<EventEngineState> lockStates;
	std::set<EventEngineState> playStates;

	void setupStateLists()
	{
		playStates.insert(PreparingToPlay);
		playStates.insert(WaitingForTrigger);
		playStates.insert(Triggered);
		playStates.insert(Playing);
		playStates.insert(Pausing);
		playStates.insert(Resuming);

		lockStates.insert(PreparingToPlay);
		lockStates.insert(WaitingForTrigger);
		lockStates.insert(Triggered);
		lockStates.insert(Playing);
		lockStates.insert(Pausing);
		lockStates.insert(Resuming);
	}
	
	
	


	//struct EngineWithPolicy
	//{
	//	EngineWithPolicy(boost::shared_ptr<EventEngine> Engine,	boost::shared_ptr<LoadPolicy> Policy) 
	//		: engine(Engine), policy(Policy)

	//	boost::shared_ptr<EventEngine> engine;
	//	boost::shared_ptr<LoadPolicy> policy;
	//};

	//STI::Utils::SynchronizedMap<EngineID, EngineWithPolicy> engines;

};


//testing...


//class EventEngineOperator
//class EventEngineController
//class EventEngineManagerGroup



class EventEngineServer
{

	void prepare(EngineID& id, TimingEvents events)
	void parse(EngineID& id, TimingEvents events)
	{
		createEngineIDs(id);		//if !engineManager.hasEngine(engineID), addEventEngine(engineID)

		for(all independent engine managers)
			manager->parse(id, events[managerID], resultHandlerRef);

		waitForParsers();

		manager->load(id);
	}
	
	void play(EngineInstance& id, double startTime, double endTime, short repeats)
	{
		if( !loaded(id) ) {
			for(all managers)
				manager->load(id);
		}

		if( !loaded(id) )
			return;

		delegateTrigger(id);

		trigger(id);

		//play each segment of the sequence if interrupted by waits
		//this function gets control again at a pause
		play(id);
	}
	void stop(EngineID& id);

};



}
}


#endif
