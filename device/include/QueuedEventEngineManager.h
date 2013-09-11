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

	QueuedEventEngineManager(const EventEngineManager_ptr& manager, unsigned threadPoolSize);
	QueuedEventEngineManager(const EventEngineManager_ptr& manager, 
		const STI::Utils::QueuedEventHandler_ptr& sharedEventHandler);	//for shared thread pool among mulitple managers

	bool addEventEngine(const EngineID& engineID, EventEngine_ptr& engine);
	bool hasEngine(const EngineID& engineID) const;
	void removeAllEngines();
	void getEngineIDs(EngineIDSet& ids) const;
	
	EventEngineState getState(const EngineID& engineID) const;
	bool inState(const EngineID& engineID, EventEngineState state) const;

	void clear(const EngineID& engineID, const EngineCallbackHandler_ptr& clearCallback);
	void parse(const EngineInstance& engineInstance, 
		const TimingEventVector_ptr& eventsIn, 
		const ParsingResultsHandler_ptr& results);
	void load(const EngineInstance& engineInstance, const EngineCallbackHandler_ptr& loadCallback);
	void play(const EngineInstance& engineInstance, const PlayOptions_ptr& playOptions, 
		const DocumentationOptions_ptr& docOptions, const MeasurementResultsHandler_ptr& resultsHander, 
		const EngineCallbackHandler_ptr& callBack);
	void trigger(const EngineInstance& engineInstance);
	void trigger(const EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger);
	void pause(const EngineID& engineID);
	void resume(const EngineInstance& engineInstance, const EngineCallbackHandler_ptr& resumeCallBack);
	void stop(const EngineID& engineID) ;
	void publishData(const EngineInstance& engineInstance, const MeasurementResultsHandler_ptr& resultsHander, const DocumentationOptions_ptr& documentation);	//false if the data doesn't exist because the EngineInstance didn't run (or is no longer in the buffer).


private:

	EventEngineManager_ptr engineManager;

	STI::Utils::QueuedEventHandler_ptr eventHandler;

	//Event classes
	class TimingEngineEvent : public STI::Utils::QueuedEvent
	{
	public:
		TimingEngineEvent(const EventEngineManager_ptr& manager, const EngineID& engineID)
			: manager_l(manager), engineInstance_l(engineID) {}
		TimingEngineEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance)
			: manager_l(manager), engineInstance_l(engineInstance) {}
		virtual ~TimingEngineEvent() {}

	protected:
		EventEngineManager_ptr manager_l;
		const EngineInstance engineInstance_l;
	};

	class ClearEvent : public TimingEngineEvent
	{
	public:
		ClearEvent(const EventEngineManager_ptr& manager, const EngineID& engineID, 
			const EngineCallbackHandler_ptr& clearCallback);
		void run();
	private:
		EngineCallbackHandler_ptr clearCallback_l;
	};
	typedef boost::shared_ptr<ClearEvent> ClearEvent_ptr;

	class ParseEvent : public TimingEngineEvent
	{
	public:
		ParseEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance,
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
		LoadEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
			const EngineCallbackHandler_ptr& loadCallBack);
		void run();
	private:
		EngineCallbackHandler_ptr loadCallBack_l;
	};
	typedef boost::shared_ptr<LoadEvent> LoadEvent_ptr;
	
	class PlayEvent : public TimingEngineEvent
	{
	public:
		PlayEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
			const PlayOptions_ptr& playOptions, 
			const DocumentationOptions_ptr& docOptions, 
			const MeasurementResultsHandler_ptr& resultsHander, 
			const EngineCallbackHandler_ptr& playCallBack);
		void run();
	
	private:
		PlayOptions_ptr playOptions_l;
		DocumentationOptions_ptr docOptions_l;
		MeasurementResultsHandler_ptr resultsHander_l;
		EngineCallbackHandler_ptr playCallBack_l;
	};
	typedef boost::shared_ptr<PlayEvent> PlayEvent_ptr;
	
	class TriggerEvent : public TimingEngineEvent
	{
	public:
		TriggerEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance);
		TriggerEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
			const MasterTrigger_ptr& delegatedTrigger);
		void run();
	private:
		MasterTrigger_ptr delegatedTrigger_l;
	};
	typedef boost::shared_ptr<TriggerEvent> TriggerEvent_ptr;

	class PauseEvent : public TimingEngineEvent
	{
	public:
		PauseEvent(const EventEngineManager_ptr& manager, const EngineID& engineID);
		void run();
	};
	typedef boost::shared_ptr<PauseEvent> PauseEvent_ptr;
	
	class ResumeEvent : public TimingEngineEvent
	{
	public:
		ResumeEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
			const EngineCallbackHandler_ptr& resumeCallBack);
		void run();
	private:
		EngineCallbackHandler_ptr resumeCallBack_l;
	};
	typedef boost::shared_ptr<ResumeEvent> ResumeEvent_ptr;

	class StopEvent : public TimingEngineEvent
	{
	public:
		StopEvent(const EventEngineManager_ptr& manager, const EngineID& engineID);
		void run();
	};
	typedef boost::shared_ptr<StopEvent> StopEvent_ptr;

	class PublishEvent : public TimingEngineEvent
	{
	public:
		PublishEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
			const MeasurementResultsHandler_ptr& resultsHander, const DocumentationOptions_ptr& documentation);
		void run();
	private:
		MeasurementResultsHandler_ptr resultsHander_l;
		DocumentationOptions_ptr documentation_l;
	};
	typedef boost::shared_ptr<PublishEvent> PublishEvent_ptr;

};


}
}



#endif
