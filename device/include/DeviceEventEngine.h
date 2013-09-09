#ifndef STI_TIMINGENGINE_DEVICEEVENTENGINE_H
#define STI_TIMINGENGINE_DEVICEEVENTENGINE_H

#include "TimingEngineTypes.h"

#include "EventEngine.h"
#include "EventEngineStateMachine.h"

#include "EngineID.h"
#include "Clock.h"
#include "MixedValue.h"

#include "DeviceTimingEngineInterface.h"

#include <boost/thread.hpp>

#include <sstream>

namespace STI
{
namespace TimingEngine
{


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


//class LocalEventEngine : public EventEngine

class DeviceEventEngine : public EventEngine
{
public:

	DeviceEventEngine(STI::Device::DeviceTimingEngineInterface& deviceInterface);
	
//	virtual const EventEngineStatus& getStatus() const = 0;

	EventEngineState getState() const { return stateMachine.getState(); }
	bool setState(EventEngineState newState) { return stateMachine.setState(newState); }
	bool leaveState(EventEngineState state) { return stateMachine.leaveState(state); }
	bool inState(EventEngineState state) const { return stateMachine.inState(state); }
	
	virtual void preClear() {};
	void clear(const EngineCallbackHandler_ptr& clearCallback);
	virtual void postClear() {};


	//Patches come from sequence parses where setvars (iterator?) have only been used to change the value field
	//of an event. This produces a (small) list of changes that can be reparsed quicky.
	//Only devices that need to be patched are asked to parse.
	//User space code parseDeviceEvents must provide some functionality to connect the new value field
	//to the SynchEvent.
	//Timining event vectors can be shared naturally between engines since they are reference counted pointers.
	virtual void preParse() {}
	void parse(const EngineTimestamp& parseTimeStamp, const TimingEventVector_ptr& rawEvents, const ParsingResultsHandler_ptr& results);
	virtual void postParse() {}

	virtual void preLoad() {}
	void load(const EngineTimestamp& parseTimeStamp, const EngineCallbackHandler_ptr& loadCallback);
	virtual void postLoad() {}

	virtual void prePlay(const EngineTimestamp& parseTimeStamp, 
		const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions,
		const DocumentationOptions_ptr& docOptions, 
		const MeasurementResultsHandler_ptr& resultsHander,
		const EngineCallbackHandler_ptr& callBack) {}
	void preTrigger(double startTime, double endTime);
	
	void waitForTrigger(const EngineCallbackHandler_ptr& triggerCallBack);
	void trigger();
	void trigger(const MasterTrigger_ptr& delegatedTrigger);

	void play(const EngineTimestamp& parseTimeStamp, const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions, 
		const DocumentationOptions_ptr& docOptions, 
		const MeasurementResultsHandler_ptr& resultsHander,
		const EngineCallbackHandler_ptr& callBack);
	virtual void postPlay() {}


	void pause();
	//virtual void pauseAt(double time) = 0;	//adds an event if not playing?
	//
	virtual void preResume() {}
	void resume(const EngineCallbackHandler_ptr& callBack);
	void resumeAt(double newTime);


	void stop()
	{
		playCondition.notify_one();
	}
	virtual void postStop() {}

	virtual void prePublishData() {}
	
	bool publishData(
		const EngineTimestamp& timestamp, TimingMeasurementGroup_ptr& data,
		const MeasurementResultsHandler_ptr& resultsHander, 
		const DocumentationOptions_ptr& documentation);

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

protected:

	//Storage
	TimingEventGroupMap rawEvents;			//map<time, TimingEventGroup>;  Raw events grouped by time.
	SynchronousEventVector synchedEvents;


	virtual void waitUntilEvent(const STI::TimingEngine::SynchronousEvent& evt, STI::TimingEngine::EventEngineState stateCondition);


private:

	bool createNewMeasurementGroup(const STI::TimingEngine::MeasurementResultsHandler_ptr& resultsHander, TimingMeasurementGroup_ptr& measurementGroup);

	unsigned getFirstEvent(double startTime);

	void waitUntil(double time_ns, STI::TimingEngine::EventEngineState stateCondition);
	void waitUntil(boost::unique_lock< boost::shared_mutex >& lock,
		double time_ns, STI::TimingEngine::EventEngineState stateCondition);

	//Parsing
	bool addRawEvent(const TimingEvent_ptr& eventsIn, unsigned& errorCount, unsigned maxErrors);
	bool parseDeviceEvents();

	void armTrigger();
	void armTrigger(boost::unique_lock< boost::shared_mutex >& triggerLock);
	bool triggerReceived;
	long triggerTimeout_s;

	EventEngineStateMachine stateMachine;

	//Playing
	STI::Utils::Clock localClock;		//for event playback
	unsigned firstEventToPlay;
	unsigned lastEventToPlay;
	unsigned eventCounter;

//	Trigger_ptr localTrigger;

	EngineTimestamp lastParseTimeStamp;
	EngineTimestamp currentPlayTimeStamp;

	const double pauseTimeout_ns;
	const double measurementsTimeout_ns;
	unsigned measurementBufferSize;

	
	TimingEventVector_ptr partnerEventsOut;
	PartnerEventTarget_ptr partnerEventTarget;
	
	TimingMeasurementGroupMap measurements;		//Measurements groups by play time stamp
	ScheduledMeasurementVector scheduledMeasurements;

	mutable boost::timed_mutex resourceMutex;
	mutable boost::shared_mutex playMutex;
	mutable boost::shared_mutex triggerMutex;
	mutable boost::condition_variable_any playCondition;
	mutable boost::timed_mutex measurementsMutex;


	std::stringstream evtTransferErr;
	STI::Device::DeviceTimingEngineInterface& device;

	//temp
//	std::string getDeviceName();
//	ChannelMap channels;
//	EngineTimestamp timeStamp;
//	void parseDeviceEvents(const TimingEventGroupMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);
	std::set<unsigned> conflictingEvents;
	std::set<unsigned> unparseableEvents;


	////Initialize on engine construction? 
	//const STI::Device::DeviceID& deviceID; 
	//const ChannelMap& channels;
	////No: needs access to partners too. Should have STI_Device reference

	//STI_Device& device;



	//get them by timestamp and then repackage into TMeasurementSeq (flat?)
	//getMeasurements(EngineTimestamp first, EngineTimestamp last)
	//getMeasurements(EngineTimestamp time)
	//getAllMeasurements()

};



//class STI_Device
//{
//public:
//	
//	const STI::Device::DeviceID& getDeviceID() const;
//	const ChannelMap& getChannels() const;
//
//	PartnerDevice& partnerDevice(std::string partnerName);	//usage: partnerDevice("lock").execute("--e1");
//
//};



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
