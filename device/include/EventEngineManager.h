#ifndef STI_TIMINGENGINE_EVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_EVENTENGINEMANAGER_H

#include "TimingEngineTypes.h"

#include <set>

namespace STI
{
namespace TimingEngine
{

class EventEngineManager //ABC
{
public:

	//Engine lock
	//parse, lock, load, play, unlock, ..., collect
//	bool lock(const EngineID& engineID) = 0;
//	void unlock(const EngineID& engineID) = 0;
//	void unlockAll() = 0;

	//Trigger delegation
	//Device-level trigger support (e.g., for external triggering instead of software triggering by the server)
	//Nothing starts to play until the Trigger initiates play. Server is the Trigger by default.
//	void delegateTrigger(const EngineID& engineID, Trigger& trigger) = 0;	//at most one device engine per cycle
//	void removeTrigger(const EngineID& engineID) = 0;

	//Engine list management
	virtual bool addEventEngine(const STI::TimingEngine::EngineID& engineID) = 0;
	virtual bool hasEngine(const STI::TimingEngine::EngineID& engineID) const = 0;
	virtual void removeAllEngines() = 0;
	virtual const std::set<const STI::TimingEngine::EngineID> getEngineIDs() const = 0;

	//Engine status
//	STI::Types::TEventEngineStatus getStatus(const EngineID& engineID) const = 0;
//	STI::Types::TEventEngineState getState(const EngineID& engineID) const = 0;

	//Engine behavior
	virtual void clear(const STI::TimingEngine::EngineID& engineID) = 0;	//deletes events, removes the Trigger
	//boost::shared_ptr< std::vector<TimingEvents> > eventsIn
	
//	void parse(const EngineID& engineID, SynchronousEvents baseEvents, TimingEventsPatches patchesIn, ParsingResultsHandler& results) = 0;
	virtual void parse(const STI::TimingEngine::EngineID& engineID, 
		const STI::TimingEngine::TimingEventVector& eventsIn, 
		ParsingResultsHandler_ptr& results) = 0;
	virtual void load(const STI::TimingEngine::EngineID& engineID) = 0;

	virtual void play(STI::TimingEngine::EngineInstance engineInstance, double startTime, double endTime, short repeats, STI::TimingEngine::DocumentationOptions_ptr& docOptions) = 0;  //repeats=-1 => infinity?
//	void playAll(EngineInstance engineInstance, STI::Types::TDocumentationOptions docOptions) = 0;	//plays one complete cycle once
	virtual void trigger(STI::TimingEngine::EngineInstance engineInstance) = 0;

	virtual void pause(const STI::TimingEngine::EngineID& engineID) = 0;
	virtual void pauseAt(const STI::TimingEngine::EngineID& engineID, double time) = 0;

	virtual void resume(const STI::TimingEngine::EngineID& engineID) = 0;
	virtual void resumeAt(const STI::TimingEngine::EngineID& engineID, double startTime, double endTime) = 0;

	virtual void stop(const STI::TimingEngine::EngineID& engineID) = 0;

	//possibly should be called getData();  Usage: collecting vs saving...
	//bool collectData(in STI::Types::TEngineInstance engineInstance, out STI::Types::TMeasurementSeq data) = 0;	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).
//	virtual bool publishData(STI::TimingEngine::EngineInstance engineInstance, STI::Types::TMeasurementSeq data) = 0;	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).

};

}
}


#endif
