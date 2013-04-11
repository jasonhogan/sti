#ifndef STI_TIMINGENGINE_DEVICEEVENTENGINE_H
#define STI_TIMINGENGINE_DEVICEEVENTENGINE_H

#include "TimingEngineTypes.h"

#include "EventEngine.h"
#include "EventEngineStateMachine.h"

#include "EngineID.h"
#include "Clock.h"
#include "MixedValue.h"

#include <boost/thread.hpp>

#include <sstream>

namespace STI
{
namespace TimingEngine
{

bool compareSynchronousEventPtrs(SynchronousEvent_ptr l,SynchronousEvent_ptr r) { return (l.get() <  r.get()); }

//void parseDeviceEvents(const RawEvents& eventsIn, SynchronousEventVector& eventsOut)
//{
//	RawEventsOld eventsInOld;
//
//	for(all eventsIn) {
//		for(unsigned j = 0; i < eventsIn.at(i).numberOfEvents(); i++) {
//			eventsInOld[eventsIn.at(i).time()].push_back( RawEventOld( eventsIn.at(i).at(j) ) );
//		}
//	}
//	parseDeviceEventsOld(eventsInOld, eventsOut);
//}



class DeviceEventEngine : public EventEngine
//class LocalEventEngine : public EventEngine
{
public:

	DeviceEventEngine() {};

	EventEngineStateMachine stateMachine;
	
//	virtual const EventEngineStatus& getStatus() const = 0;

	const EventEngineState getState() const { return stateMachine.getState(); }
	bool setState(EventEngineState newState) { return stateMachine.setState(newState); }
	bool leaveState(EventEngineState state) { return stateMachine.leaveState(state); }
	bool inState(EventEngineState state) const { return stateMachine.inState(state); }


	//Patches come from sequence parses where setvars (iterator?) have only been used to change the value field
	//of an event. This produces a (small) list of changes that can be reparsed quicky.
	//Only devices that need to be patched are asked to parse.
	//User space code parseDeviceEvents must provide some functionality to connect the new value field
	//to the SynchEvent.
	//Timining event vectors can be shared naturally between engines since they are reference counted pointers.
	virtual void preParse() {}
	void parseEvents(const TimingEventVector& rawEvents, ParsingResultsHandler& results);
	virtual void postParse() {}

	virtual void preLoad() {}
	void load();
	virtual void postLoad() {}

	void preTrigger(double startTime, double endTime);
	void play(double startTime, double endTime);
	virtual void postPlay() {}
	
	void stop()
	{
		playCondition.notify_one();
	}

	virtual void prePublishData() {}
	void publishData(const EngineTimestamp& timestamp, STI::Utils::MixedValueVector& data);
	virtual void postPublishData() {}

	//Policy options for:
	//Parse during load
	//(re)load during play
	//collect during play


//PreparingToCollectData, CollectingData,
//perhaps PreparingToCollectData, CollectingData are not states if they can happen asynchronously or after multiple trial
//just need a mutex on the data storage during Collection, and EngineInstance tagging to make sure it's the right data
//What about calls to event->collectData() on each device?

	//maybe this is called "background" work, and it collects and reloads if needed. minder(), cleanup(), postPlay(), 
//	void collectData(bool reload)	//harvestData(), saveData(), measure(), measureData()
	//{
	//	unsigned i = 0;
	//	for(unsigned i = 0; i < synchedEvents.size(); i++) 
	//	{
	//		if( checkStop() ) {
	//			throw EngineStopException();

	//		if( synchedEvents.at(i).isPlayed() ) {		//won't go to sleep unless no events are ready to collect data
	//			
	//			synchedEvents.at(i).collectData();

	//			//pushback the Measurement to the buffer
	//			//The Measurement should be a shared_ptr<Measurement> so it sticks around even when synchedEvents gets cleared.

	//			if(reload) {
	//				synchedEvents.at(i).load();
	//			}
	//		} else {
	//			collectionCondition->timedWait(timeout);	//gets one wakeup call per event played
	//		}
	//	}
	//}
	//}

	//void setStop(bool stop)
	//{
	//	uniquelock stopLock(playMutex);
	//	stopFlag = true;
	//}
	//bool checkStop()
	//{
	//	sharedlock readLock(playMutex);
	//	return stopFlag;
	//
	//	//or eliminate the extra flag entirely and just check state==Stopping
	//	return (getState() == Stopping);

	//	return (isState(Stopping));
	//}
	//bool checkPause()
	//{
	//	return (getState() == Pausing);
	//}

	
private:
	

	void waitUntil(double time);

	bool addRawEvent(const boost::shared_ptr<TimingEvent>& eventsIn, unsigned& errorCount, unsigned maxErrors);
	bool parseDeviceEvents();

	TimingEventGroupMap rawEvents;			//map<time, TimingEventGroup>;  Raw events grouped by time.
	SynchronousEventVector synchedEvents;

	DataMeasurementMap measurements;

	mutable boost::shared_mutex playMutex;
	mutable boost::condition_variable_any playCondition;


	//temp
	ChannelMap channels;
	std::stringstream evtTransferErr;
	EngineTimestamp timeStamp;
	void parseDeviceEvents(const TimingEventGroupMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);
	std::set<unsigned> conflictingEvents;
	std::set<unsigned> unparseableEvents;
	std::string getDeviceName();
	unsigned firstEventToPlay;
	unsigned lastEventToPlay;
	Clock time;		//for event playback




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
