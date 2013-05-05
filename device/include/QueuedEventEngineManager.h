#ifndef STI_TIMINGENGINE_QUEUEDEVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_QUEUEDEVENTENGINEMANAGER_H

#include "TimingEngineTypes.h"
#include <EventEngineManager.h>
#include "QueuedEventHandler.h"
#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

class QueuedEventEngineManager : public EventEngineManager
{
//	using STI::Utils::QueuedEventHandler::QueuedEvent;
public:

	QueuedEventEngineManager(EventEngineManager& manager, unsigned threadPoolSize);
	QueuedEventEngineManager(EventEngineManager& manager, 
		const STI::Utils::QueuedEventHandler_ptr& sharedEventHandler);	//for shared thread pool among mulitple managers

	bool addEventEngine(const EngineID& engineID, EventEngine_ptr& engine);
	bool hasEngine(const EngineID& engineID) const;
	void removeAllEngines();
	void getEngineIDs(EngineIDSet& ids) const;
	
	EventEngineState getState(const EngineID& engineID) const;
	bool inState(const EngineID& engineID, EventEngineState state) const;

	void clear(const EngineID& engineID);
	void parse(const EngineInstance& engineInstance, 
		const TimingEventVector_ptr& eventsIn, 
		const ParsingResultsHandler_ptr& results);
	void load(const EngineInstance& engineInstance);
	void play(const EngineInstance& engineInstance, double startTime, double endTime, short repeats, 
		const DocumentationOptions_ptr& docOptions);
	void trigger(const EngineInstance& engineInstance);
	void trigger(const EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger);
	void pause(const EngineID& engineID);
	void resume(const EngineInstance& engineInstance);
	void stop(const EngineID& engineID) ;
	void publishData(const EngineInstance& engineInstance, const MeasurementResultsHandler_ptr& resultsHander);	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).


private:

	EventEngineManager& engineManager;

	STI::Utils::QueuedEventHandler_ptr eventHandler;

	//Event classes
	class TimingEngineEvent : public STI::Utils::QueuedEvent
	{
	public:
		TimingEngineEvent(EventEngineManager& manager, const EngineID& engineID)
			: manager_l(manager), engineInstance_l(engineID) {}
		TimingEngineEvent(EventEngineManager& manager, const EngineInstance& engineInstance)
			: manager_l(manager), engineInstance_l(engineInstance) {}

	protected:
		EventEngineManager& manager_l;
		const EngineInstance engineInstance_l;
	};

	class ClearEvent : public TimingEngineEvent
	{
	public:
		ClearEvent(EventEngineManager& manager, const EngineID& engineID);
		void run();
	};
	typedef boost::shared_ptr<ClearEvent> ClearEvent_ptr;

	class ParseEvent : public TimingEngineEvent
	{
	public:
		ParseEvent(EventEngineManager& manager, const EngineInstance& engineInstance,
			  const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results);
		void run();
	private:
		TimingEventVector_ptr eventsIn_l;
		ParsingResultsHandler_ptr results_l;
	};
	typedef boost::shared_ptr<ParseEvent> ParseEvent_ptr;

	class LoadEvent : public TimingEngineEvent
	{
	public:
		LoadEvent(EventEngineManager& manager, const EngineInstance& engineInstance);
		void run();
	};
	typedef boost::shared_ptr<LoadEvent> LoadEvent_ptr;
	
	class PlayEvent : public TimingEngineEvent
	{
	public:
		PlayEvent(EventEngineManager& manager, const EngineInstance& engineInstance, 
			double startTime, double endTime, short repeats, 
			const DocumentationOptions_ptr& docOptions);
		void run();
	
	private:
		double startTime_l;
		double endTime_l; 
		short repeats_l;
		DocumentationOptions_ptr docOptions_l;

	};
	typedef boost::shared_ptr<PlayEvent> PlayEvent_ptr;
	
	class TriggerEvent : public TimingEngineEvent
	{
	public:
		TriggerEvent(EventEngineManager& manager, const EngineInstance& engineInstance);
		TriggerEvent(EventEngineManager& manager, const EngineInstance& engineInstance, 
			const MasterTrigger_ptr& delegatedTrigger);
		void run();
	private:
		MasterTrigger_ptr delegatedTrigger_l;
	};
	typedef boost::shared_ptr<TriggerEvent> TriggerEvent_ptr;

	class PauseEvent : public TimingEngineEvent
	{
	public:
		PauseEvent(EventEngineManager& manager, const EngineID& engineID);
		void run();
	};
	typedef boost::shared_ptr<PauseEvent> PauseEvent_ptr;
	
	class ResumeEvent : public TimingEngineEvent
	{
	public:
		ResumeEvent(EventEngineManager& manager, const EngineInstance& engineInstance);
		void run();
	};
	typedef boost::shared_ptr<ResumeEvent> ResumeEvent_ptr;

	class StopEvent : public TimingEngineEvent
	{
	public:
		StopEvent(EventEngineManager& manager, const EngineID& engineID);
		void run();
	};
	typedef boost::shared_ptr<StopEvent> StopEvent_ptr;

	class PublishEvent : public TimingEngineEvent
	{
	public:
		PublishEvent(EventEngineManager& manager, const EngineInstance& engineInstance, 
			const MeasurementResultsHandler_ptr& resultsHander);
		void run();
	private:
		MeasurementResultsHandler_ptr resultsHander_l;
	};
	typedef boost::shared_ptr<PublishEvent> PublishEvent_ptr;

};


}
}



#endif
