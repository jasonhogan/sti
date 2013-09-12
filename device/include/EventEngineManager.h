#ifndef STI_TIMINGENGINE_EVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_EVENTENGINEMANAGER_H

#include "TimingEngineTypes.h"
#include "EventEngineState.h"

#include <set>

namespace STI
{
namespace TimingEngine
{

class EventEngineManager //ABC
{
public:

	virtual ~EventEngineManager() {}
	//Engine lock
	//parse, lock, load, play, unlock, ..., collect
//	bool lock(const EngineID& engineID) = 0;
//	void unlock(const EngineID& engineID) = 0;
//	void unlockAll() = 0;

//	void lock(const EngineInstance& engineInstance);
//	void unlock(const EngineInstance& engineInstance);


	//Trigger delegation
	//Device-level trigger support (e.g., for external triggering instead of software triggering by the server)
	//Nothing starts to play until the Trigger initiates play. Server is the Trigger by default.
//	void delegateTrigger(const EngineID& engineID, Trigger& trigger) = 0;	//at most one device engine per cycle
//	void removeTrigger(const EngineID& engineID) = 0;

	//Engine list management
	virtual bool addEventEngine(const STI::TimingEngine::EngineID& engineID, EventEngine_ptr& engine) = 0;
	virtual bool hasEngine(const STI::TimingEngine::EngineID& engineID) const = 0;
	virtual void removeAllEngines() = 0;
	virtual void getEngineIDs(EngineIDSet& ids) const = 0;

	//Engine status
	virtual EventEngineState getState(const EngineID& engineID) const = 0;
	virtual bool inState(const EngineID& engineID, EventEngineState state) const = 0;

//	STI::Types::TEventEngineStatus getStatus(const EngineID& engineID) const = 0;
//	STI::Types::TEventEngineState getState(const EngineID& engineID) const = 0;

	//Engine behavior
	virtual void clear(const STI::TimingEngine::EngineID& engineID, const EngineCallbackHandler_ptr& clearCallback) = 0;
	
//	virtual void parsePatch(const EngineInstance& oldInstance, const EngineInstance& newInstance, 
//		const TimingEventVector& addedEvents, const TimingEventVector& subtractedEvents, 
//		const ParsingResultsHandler_ptr& results) = 0;
//	void parse(const EngineID& engineID, SynchronousEvents baseEvents, TimingEventsPatches patchesIn, ParsingResultsHandler& results) = 0;
	virtual void parse(const STI::TimingEngine::EngineInstance& engineInstance, 
		const STI::TimingEngine::TimingEventVector_ptr& eventsIn, 
		const ParsingResultsHandler_ptr& results) = 0;
	virtual void load(const STI::TimingEngine::EngineInstance& engineInstance, const EngineCallbackHandler_ptr& loadCallBack) = 0;

	virtual void play(const STI::TimingEngine::EngineInstance& engineInstance, const PlayOptions_ptr& playOptions, 
		const STI::TimingEngine::DocumentationOptions_ptr& docOptions, const MeasurementResultsHandler_ptr& resultsHander, const EngineCallbackHandler_ptr& playCallBack) = 0;  //repeats=-1 => infinity?
//	void playAll(EngineInstance engineInstance, STI::Types::TDocumentationOptions docOptions) = 0;	//plays one complete cycle once
	virtual void trigger(const STI::TimingEngine::EngineInstance& engineInstance) = 0;
	virtual void trigger(const STI::TimingEngine::EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger) = 0;

	virtual void pause(const STI::TimingEngine::EngineID& engineID) = 0;
//	virtual void pauseAt(const STI::TimingEngine::EngineID& engineID, double time) = 0;

	virtual void resume(const EngineInstance& engineInstance, const EngineCallbackHandler_ptr& resumeCallBack) = 0;
//	virtual void resumeAt(const STI::TimingEngine::EngineID& engineID, double startTime, double endTime) = 0;

	virtual void stop(const STI::TimingEngine::EngineID& engineID) = 0;

	virtual void publishData(const STI::TimingEngine::EngineInstance& engineInstance, 
		const MeasurementResultsHandler_ptr& resultsHander, const DocumentationOptions_ptr& documentation) = 0;	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).

};

}
}


#endif
