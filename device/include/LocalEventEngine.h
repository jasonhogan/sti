#ifndef STI_TIMINGENGINE_DEVICEEVENTENGINE_H
#define STI_TIMINGENGINE_DEVICEEVENTENGINE_H

#include "EventEngine.h"
#include "EventEngineStateMachine.h"

namespace STI
{
namespace TimingEngine
{

class LocalEventEngine : public EventEngine
{
public:

	EventEngineStateMachine stateMachine;
	
	virtual const EventEngineStatus& getStatus() const = 0;

	const EventEngineState getState() const { return stateMachine.getState(); }
	bool setState(EventEngineState newState) { return stateMachine.setState(newState); }
	bool leaveState(EventEngineState state) { return stateMachine.leaveState(state); }
	bool inState(EventEngineState state) const { return stateMachine.inState(state); }

	virtual void preParse() {}
	virtual void parseEvents(const std::vector<TimingEvent>& eventsIn, ParsingResultsHandler& results);
	virtual void postParse() {}

	virtual void preLoad() {}
	void load() 
	{
		for(unsigned i = 0; i < synchedEvents.size(); i++) {
			synchedEvents.at(i).load();	//needs to call old "setup" phase first...
		}
	}
	virtual void postLoad() {}


	void preTrigger(double startTime, double endTime)
	{
		setStop(false);

		//find first event
		for(unsigned j = 0; j < synchedEvents.size(); j++) {
			if( synchedEvents.at(j).getTime() > startTime ) {
				firstEventToPlay = j;
				break;
			}
		}
		//find last event, (start looking from the end)
		unsigned lastIndex = synchedEvents.size() - 1;
		lastEventToPlay = lastIndex;
		for(unsigned j = 0; j < synchedEvents.size(); j++) {
			if( synchedEvents.at(lastIndex - j).getTime() < endTime ) {
				lastEventToPlay = lastIndex - j;
				break;
			}
		}
	}

	void play(double startTime, double endTime) 
	{
		time.preset(startTime);

		for(unsigned i = firstEventToPlay; i < lastEventToPlay; i++) {
			
			waitUntil( synchedEvents.at(i).getTime() );	//returns at time event should play

			if( checkStop() ) {
				time.pause();
				throw EngineStopException();
			}
			if( checkPause() ) {
				time.pause();
				throw EnginePauseException();
			}

			synchedEvents.at(i).play();

			//data is ready
			collectionCondition->notify_one();	//wake up the collection thread?

		}
		
		//this may be unneccessary -- in repeat mode at least we really prefer the devices wait for the trigger
		if(repeats == 0)
			waitUntil(endTime);
	}

	

//PreparingToCollectData, CollectingData,
//perhaps PreparingToCollectData, CollectingData are not states if they can happen asynchronously or after multiple trial
//just need a mutex on the data storage during Collection, and EngineInstance tagging to make sure it's the right data
//What about calls to event->collectData() on each device?

	//maybe this is called "background" work, and it collects and reloads if needed. minder(), cleanup(), postPlay(), 
	void postPlay()
//	void collectData(bool reload)	//harvestData(), saveData(), measure(), measureData()
	{
		unsigned i = 0;
		for(unsigned i = 0; i < synchedEvents.size(); i++) 
		{
			if( checkStop() ) {
				throw EngineStopException();

			if( synchedEvents.at(i).isPlayed() ) {		//won't go to sleep unless no events are ready to collect data
				
				synchedEvents.at(i).collectData();

				//pushback the Measurement to the buffer
				//The Measurement should be a shared_ptr<Measurement> so it sticks around even when synchedEvents gets cleared.

				if(reload) {
					synchedEvents.at(i).load();
				}
			} else {
				collectionCondition->timedWait(timeout);	//gets one wakeup call per event played
			}
		}
	}
	}
	void waitUntil(double time)
	{
		boostcondition playCondition->timedWait(time);
	}

	void stop()
	{
		setStop(true);
		playCondition->notify_one();	//broadcast() ?
	}
	void setStop(bool stop)
	{
		uniquelock stopLock(playMutex);
		stopFlag = true;
	}
	bool checkStop()
	{
		sharedlock readLock(playMutex);
		return stopFlag;
	
		//or eliminate the extra flag entirely and just check state==Stopping
		return (getState() == Stopping);

		return (isState(Stopping));
	}
	bool checkPause()
	{
		return (getState() == Pausing);
	}

	
private:

	TimingEventVector rawEvents;
	SynchronizedEventVector synchedEvents;

	typedef vector<DataMeasurement> DataMeasurementVector;
	typedef map<EngineTimestamp, DataMeasurementVector> DataMeasurementMap;

	DataMeasurementMap measurements;	
	
	//get them by timestamp and then repackage into TMeasurementSeq (flat?)
	//getMeasurements(EngineTimestamp first, EngineTimestamp last)
	//getMeasurements(EngineTimestamp time)
	//getAllMeasurements()

};


//class FPGAEventEngine : public LocalEventEngine
//{
//	void preLoad() {
//		SynchronousEventVector& events = getSynchronousEvents();
//		numberOfEvents = static_cast<uInt32>(events.size());
//	}
//	void postLoad() {
//		writeRAM_Parameters();
//	}
//
//};



}
}


#endif
