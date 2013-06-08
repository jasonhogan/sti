

#include "QueuedEventEngineManager.h"
#include "QueuedEventHandler.h"

using STI::TimingEngine::QueuedEventEngineManager;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EngineInstance;
using STI::TimingEngine::EngineIDSet;
using STI::TimingEngine::EventEngine_ptr;
using STI::TimingEngine::EventEngineState;
using STI::TimingEngine::TimingEventVector_ptr;
using STI::TimingEngine::ParsingResultsHandler_ptr;
using STI::Utils::QueuedEvent_ptr;
using STI::Utils::QueuedEventHandler;
using STI::Utils::QueuedEventHandler_ptr;
using STI::TimingEngine::MasterTrigger_ptr;
using STI::TimingEngine::MeasurementResultsHandler_ptr;
using STI::TimingEngine::DocumentationOptions_ptr;
using STI::TimingEngine::PlayOptions_ptr;
using STI::TimingEngine::EventEngineManager_ptr;

//using STI::TimingEngine::QueuedEventEngineManager::ClearEvent;
//using STI::TimingEngine::QueuedEventEngineManager::ParseEvent;

QueuedEventEngineManager::QueuedEventEngineManager(const EventEngineManager_ptr& manager, unsigned threadPoolSize)
: engineManager(manager), eventHandler( new QueuedEventHandler(threadPoolSize) )
{
}

QueuedEventEngineManager::QueuedEventEngineManager(const EventEngineManager_ptr& manager, 
												   const QueuedEventHandler_ptr& sharedEventHandler)
: engineManager(manager)
{
	//for shared thread pool among multiple managers
	if(eventHandler != 0) {
		eventHandler = sharedEventHandler;
	}
	else {
		//Default; ensures an event handler exists in case of null pointer
		eventHandler = QueuedEventHandler_ptr( new QueuedEventHandler(2) );
	}
}

bool QueuedEventEngineManager::addEventEngine(const EngineID& engineID, EventEngine_ptr& engine)
{
	return engineManager->addEventEngine(engineID, engine);
}

bool QueuedEventEngineManager::hasEngine(const EngineID& engineID) const
{
	return engineManager->hasEngine(engineID);
}

void QueuedEventEngineManager::removeAllEngines()
{
	return engineManager->removeAllEngines();
}

void QueuedEventEngineManager::getEngineIDs(EngineIDSet& ids) const
{
	return engineManager->getEngineIDs(ids);
}

EventEngineState QueuedEventEngineManager::getState(const EngineID& engineID) const
{
	return engineManager->getState(engineID);
}

bool QueuedEventEngineManager::inState(const EngineID& engineID, EventEngineState state) const
{
	return engineManager->inState(engineID, state);
}



void QueuedEventEngineManager::clear(const EngineID& engineID)
{
	QueuedEvent_ptr clearEvt = QueuedEvent_ptr( new ClearEvent(engineManager, engineID) );
	eventHandler->addEvent(clearEvt);
}

void QueuedEventEngineManager::parse(const EngineInstance& engineInstance, 
									 const TimingEventVector_ptr& eventsIn, 
									 const ParsingResultsHandler_ptr& results)
{
	QueuedEvent_ptr parseEvt = QueuedEvent_ptr( new ParseEvent(engineManager, engineInstance, eventsIn, results) );
	eventHandler->addEvent(parseEvt);
}
void QueuedEventEngineManager::load(const EngineInstance& engineInstance)
{
	QueuedEvent_ptr loadEvt = QueuedEvent_ptr( new LoadEvent(engineManager, engineInstance) );
	eventHandler->addEvent(loadEvt);
}
void QueuedEventEngineManager::play(const EngineInstance& engineInstance, const PlayOptions_ptr& playOptions, 
	const STI::TimingEngine::DocumentationOptions_ptr& docOptions)
{
	QueuedEvent_ptr playEvt = QueuedEvent_ptr( new PlayEvent(engineManager, engineInstance, playOptions, docOptions) );
	eventHandler->addEvent(playEvt);
}

void QueuedEventEngineManager::trigger(const STI::TimingEngine::EngineInstance& engineInstance)
{
	QueuedEvent_ptr triggerEvt = QueuedEvent_ptr( new TriggerEvent(engineManager, engineInstance) );
	eventHandler->addEvent(triggerEvt);
}
void QueuedEventEngineManager::trigger(const STI::TimingEngine::EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger)
{
	QueuedEvent_ptr triggerEvt = QueuedEvent_ptr( new TriggerEvent(engineManager, engineInstance, delegatedTrigger) );
	eventHandler->addEvent(triggerEvt);
}

void QueuedEventEngineManager::pause(const STI::TimingEngine::EngineID& engineID)
{
	QueuedEvent_ptr pauseEvt = QueuedEvent_ptr( new PauseEvent(engineManager, engineID) );
	eventHandler->addEvent(pauseEvt);
}
void QueuedEventEngineManager::resume(const EngineInstance& engineInstance)
{
	QueuedEvent_ptr resumeEvt = QueuedEvent_ptr( new ResumeEvent(engineManager, engineInstance) );
	eventHandler->addEvent(resumeEvt);
}
void QueuedEventEngineManager::stop(const STI::TimingEngine::EngineID& engineID)
{
	QueuedEvent_ptr stopEvt = QueuedEvent_ptr( new StopEvent(engineManager, engineID) );
	eventHandler->addEventHighPriority(stopEvt);
}
void QueuedEventEngineManager::publishData(const STI::TimingEngine::EngineInstance& engineInstance, 
										   const MeasurementResultsHandler_ptr& resultsHander,
										   const DocumentationOptions_ptr& documentation)
{
	QueuedEvent_ptr publishEvt = QueuedEvent_ptr( new PublishEvent(engineManager, engineInstance, resultsHander, documentation) );
	eventHandler->addEvent(publishEvt);
}

//ClearEvent
QueuedEventEngineManager::ClearEvent::ClearEvent(const EventEngineManager_ptr& manager, const EngineID& engineID)
: TimingEngineEvent(manager, engineID) 
{
}
void QueuedEventEngineManager::ClearEvent::run()
{
	if(manager_l)
		manager_l->clear(engineInstance_l.id);
}
//ParseEvent
QueuedEventEngineManager::ParseEvent::ParseEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance,
			  const TimingEventVector_ptr& eventsIn, const ParsingResultsHandler_ptr& results)
: TimingEngineEvent(manager, engineInstance), eventsIn_l(eventsIn), results_l(results)
{
}
void QueuedEventEngineManager::ParseEvent::run()
{
	if(manager_l)
		manager_l->parse(engineInstance_l, eventsIn_l, results_l);
}
//LoadEvent
QueuedEventEngineManager::LoadEvent::LoadEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance)
: TimingEngineEvent(manager, engineInstance)
{
}
void QueuedEventEngineManager::LoadEvent::run()
{
	if(manager_l)
		manager_l->load(engineInstance_l);
}

//PlayEvent
QueuedEventEngineManager::PlayEvent::PlayEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, 
											   const PlayOptions_ptr& playOptions, 
											   const DocumentationOptions_ptr& docOptions)
: TimingEngineEvent(manager, engineInstance), 
playOptions_l(playOptions), docOptions_l(docOptions)
{
}
void QueuedEventEngineManager::PlayEvent::run()
{
	if(manager_l)
		manager_l->play(engineInstance_l, playOptions_l, docOptions_l);
}

//TriggerEvent
QueuedEventEngineManager::TriggerEvent::TriggerEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance)
: TimingEngineEvent(manager, engineInstance)
{
}
QueuedEventEngineManager::TriggerEvent::TriggerEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance, const MasterTrigger_ptr& delegatedTrigger)
: TimingEngineEvent(manager, engineInstance), delegatedTrigger_l(delegatedTrigger)
{
}
void QueuedEventEngineManager::TriggerEvent::run()
{
	if(!manager_l)
		return;
	if(delegatedTrigger_l == 0) {
		manager_l->trigger(engineInstance_l);
	}
	else {
		manager_l->trigger(engineInstance_l, delegatedTrigger_l);
	}
}

//PauseEvent
QueuedEventEngineManager::PauseEvent::PauseEvent(const EventEngineManager_ptr& manager, const EngineID& engineID)
: TimingEngineEvent(manager, engineID)
{
}
void QueuedEventEngineManager::PauseEvent::run()
{
	if(manager_l)
		manager_l->pause(engineInstance_l.id);
}

//ResumeEvent
QueuedEventEngineManager::ResumeEvent::ResumeEvent(const EventEngineManager_ptr& manager, const EngineInstance& engineInstance)
: TimingEngineEvent(manager, engineInstance)
{
}
void QueuedEventEngineManager::ResumeEvent::run()
{
	if(manager_l)
		manager_l->resume(engineInstance_l);
}

//StopEvent
QueuedEventEngineManager::StopEvent::StopEvent(const EventEngineManager_ptr& manager, const EngineID& engineID)
: TimingEngineEvent(manager, engineID)
{
}
void QueuedEventEngineManager::StopEvent::run()
{
	if(manager_l)
		manager_l->stop(engineInstance_l.id);
}

//PublishEvent
QueuedEventEngineManager::PublishEvent::PublishEvent(const EventEngineManager_ptr& manager, 
													 const EngineInstance& engineInstance, 
													 const MeasurementResultsHandler_ptr& resultsHander, 
													 const DocumentationOptions_ptr& documentation)
: TimingEngineEvent(manager, engineInstance), resultsHander_l(resultsHander), documentation_l(documentation)
{
}
void QueuedEventEngineManager::PublishEvent::run()
{
	if(manager_l)
		manager_l->publishData(engineInstance_l, resultsHander_l, documentation_l);
}

