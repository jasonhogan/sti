#ifndef STI_TIMINGENGINE_EVENTENGINE_H
#define STI_TIMINGENGINE_EVENTENGINE_H

#include <TimingEngineTypes.h>
#include <EventEngineStateMachine.h>


namespace STI
{
namespace TimingEngine
{

class EventEngine
{
public:

//	virtual const double getTime() const = 0;	//gets the time on the clock
//	virtual const EngineTimestamp getCurrentTimestamp() = 0;	//engine timestamp of the most recently loaded data


//	virtual const EventEngineStatus& getStatus() const = 0;
//	virtual const EventEngineStatus& getStatus(Time timestamp) const = 0;

	virtual ~EventEngine() {}

	virtual EventEngineState getState() const = 0;
	virtual bool setState(EventEngineState newState) = 0;
	virtual bool leaveState(EventEngineState state) = 0;
	virtual bool inState(EventEngineState state) const = 0;

	virtual void preClear() = 0;
	virtual void clear(const EngineCallbackHandler_ptr& clearCallback) = 0;
	virtual void postClear() = 0;

	virtual void preParse() = 0;
	virtual void parse(const EngineTimestamp& parseTimeStamp, 
		const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results) = 0;
	virtual void postParse() = 0;

	virtual void preLoad() = 0;
	virtual void load(const EngineTimestamp& parseTimeStamp, const EngineCallbackHandler_ptr& loadCallback) = 0;
	virtual void postLoad() = 0;

	virtual void prePlay(
		const EngineTimestamp& parseTimeStamp, 
		const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions,
		const DocumentationOptions_ptr& docOptions, const EngineCallbackHandler_ptr& callBack) = 0;

	virtual void preTrigger(double startTime, double endTime) = 0;
	virtual void waitForTrigger(const EngineCallbackHandler_ptr& triggerCallBack) = 0;
	virtual void trigger() = 0;
	virtual void trigger(const MasterTrigger_ptr& delegatedTrigger) = 0;

	virtual void play(const EngineTimestamp& parseTimeStamp, 
		const EngineTimestamp& playTimeStamp, 
		const PlayOptions_ptr& playOptions, 
		const DocumentationOptions_ptr& docOptions, const EngineCallbackHandler_ptr& callBack) = 0;
//	virtual void play(double startTime, double endTime, short repeats, double repeatTime, DocumentationOptions docOptions) = 0;
//	virtual void playAll(DocumentationOptions docOptions) = 0;
	virtual void postPlay() = 0;

	virtual void pause() = 0;
//	virtual void pauseAt(double time) = 0;	//adds an event if not playing?
	
	virtual void preResume() = 0;
	virtual void resume(const EngineCallbackHandler_ptr& callBack) = 0;
	virtual void resumeAt(double newTime) = 0;
	
	virtual void stop() = 0;
	virtual void postStop() = 0;

	//Collection means get it as soon as data is available and store it in some local structure (e.g., std::vector<measurement>)
	//Collection happens in quasi-real time, chasing the play() function.
	//This is the "minder c-program" funtionality needed for hard-timing devices that produce data and store it in a temporary cache that must be rapidly reset.
//	virtual void preCollectData() = 0;
//	virtual void collectData() = 0;
//	virtual void postCollectData() = 0;
	//Likely that the above are not needed as discrete engine hooks, since they are called by play() anyway.


	//Publication means move to permanent storage of the data, and clear the local storage.
	//Permanent publication can happen asynchronously.
	//Classically this means the server gets the data and writes to XML,
	//but it might also involve writing to a local disk or output stream, etc.
	virtual void prePublishData() = 0;
	virtual bool publishData(const EngineTimestamp& timestamp, TimingMeasurementGroup_ptr& data, const DocumentationOptions_ptr& documentation) = 0;
	virtual void postPublishData() = 0;

	//retreiveData() ??
};

}
}


#endif
