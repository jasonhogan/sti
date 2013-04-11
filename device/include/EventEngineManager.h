#ifndef STI_TIMINGENGINE_EVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_EVENTENGINEMANAGER_H

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
	void delegateTrigger(const EngineID& engineID, Trigger& trigger) = 0;	//at most one device engine per cycle
	void removeTrigger(const EngineID& engineID) = 0;

	//Engine list management
	bool addEventEngine(const EngineID& engineID) = 0;
	bool hasEngine(const EngineID& engineID) const = 0;
	void removeAllEngines() = 0;
	const std::set<const EngineID> getEngineIDs() const = 0;

	//Engine status
	STI::Types::TEventEngineStatus getStatus(const EngineID& engineID) const = 0;
	STI::Types::TEventEngineState getState(const EngineID& engineID) const = 0;

	//Engine behavior
	void clear(const EngineID& engineID) = 0;	//deletes events, removes the Trigger
	//boost::shared_ptr< std::vector<TimingEvents> > eventsIn
	
//	void parse(const EngineID& engineID, SynchronousEvents baseEvents, TimingEventsPatches patchesIn, ParsingResultsHandler& results) = 0;
	void parse(const EngineID& engineID, TimingEvents eventsIn, ParsingResultsHandler& results) = 0;
	void load(const EngineID& engineID) = 0;

	void play(EngineInstance engineInstance, double startTime, double endTime, short repeats, double repeatTime, STI::Types::TDocumentationOptions docOptions) = 0;  //repeats=-1 => infinity?
	void playAll(EngineInstance engineInstance, STI::Types::TDocumentationOptions docOptions) = 0;	//plays one complete cycle once
	void trigger(EngineInstance engineInstance) = 0;

	void pause(const EngineID& engineID) = 0;
	void pauseAt(const EngineID& engineID, in double time) = 0;

	void resume(const EngineID& engineID) = 0;
	void resumeAt(const EngineID& engineID, in double newTime) = 0;

	void stop(const EngineID& engineID) = 0;

	//possibly should be called getData();  Usage: collecting vs saving...
	//bool collectData(in STI::Types::TEngineInstance engineInstance, out STI::Types::TMeasurementSeq data) = 0;	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).
	bool publishData(in STI::Types::TEngineInstance engineInstance, out STI::Types::TMeasurementSeq data) = 0;	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).

};

}
}


#endif
