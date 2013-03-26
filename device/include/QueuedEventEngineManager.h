#ifndef STI_TIMINGENGINE_QUEUEDEVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_QUEUEDEVENTENGINEMANAGER_H

#include <EventEngineManager.h>

namespace STI
{
namespace TimingEngine
{


class QueuedEventEngineManager : public EventEngineManager
{
	using STI::Utils::QueuedEventHandler::QueuedEvent;

	QueuedEventEngineManager();

	STI::Types::TEventEngineState getState(const EngineID& engineID) const
	{
		return engineManager->getState(engineID);
	}

	//Engine behavior
	void clear(const EngineID& engineID)
	{
		boost::shared_ptr<ClearEvent> clearEvt;
		clearEvt = boost::shared_ptr<ClearEvent>( new ClearEvent(engineManager, engineID) );
		eventHandler.addEvent(clearEvt);
	}

	void parse(const EngineID& engineID, STI::Types::TEventSeq eventsIn, ParsingResultsHandler results)
	{
		boost::shared_ptr<ClearEvent> parseEvt;
		parseEvt = boost::shared_ptr<ParseEvent>( new ParseEvent(engineManager, engineID, eventsIn, results) );
		eventHandler.addEvent(parseEvt);
	}


private:

	QueuedEventHandler eventHandler;
	QueuedEventHandler highPriorityEventHandler;

	boost::shared_ptr<EventEngineManager> engineManager;

	class TimingEngineEvent : public QueuedEvent
	{
	public:
		TimingEngineEvent(boost::shared_ptr<EventEngineManager>& managerRef, const EngineID& id)
			: manager(managerRef), engineID(id) {}

	protected:
		boost::shared_ptr<DeviceEventEngineManager> manager;
		EngineID engineID;
	};

	class ClearEvent : public TimingEngineEvent
	{
	public:
		ClearEvent(boost::shared_ptr<EventEngineManager>& manager, const EngineID& engineID)
			: TimingEngineEvent(manager, engineID {}

		void run()
		{
			manager->clear(engineID);
		}
	};
	
	class ParseEvent : public TimingEngineEvent
	{
	public:
		ParseEvent(boost::shared_ptr<EventEngineManager>& manager, const EngineID& engineID,
			  TimingEvents& eventsIn, ParsingResultsHandler& results)
			: TimingEngineEvent(manager, engineID {}

		void run()
		{
			manager->parse(engineID, eventsIn, results);
		}
	};

};

};

}
}



#endif
