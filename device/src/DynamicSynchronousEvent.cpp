
#include "DynamicSynchronousEvent.h"
#include "TimingEvent.h"
#include "DynamicValue.h"


using STI::TimingEngine::DynamicSynchronousEvent;
using STI::TimingEngine::SynchronousEvent;
using STI::TimingEngine::TimingEventVector_ptr;
using STI::TimingEngine::DynamicValueEvent;


DynamicSynchronousEvent::DynamicSynchronousEvent(double time, const TimingEventVector_ptr& sourceEvents) : 
SynchronousEvent(time)
{
	addSourceEvents(sourceEvents);
}

void DynamicSynchronousEvent::addSourceEvents(const TimingEventVector_ptr& sourceEvents)
{
	sourceEvents_l = sourceEvents;

	DynamicValue_ptr dynamicValue;

	for(unsigned i = 0; i < sourceEvents_l->size(); i++) {
		
		if(sourceEvents_l->at(i)->getDynamicValue(dynamicValue)) {
			dynamicValue->addLink(this);
			dynamicValues.push_back(dynamicValue);
		}
	}
}

DynamicSynchronousEvent::~DynamicSynchronousEvent()
{
	for(unsigned i = 0; i < dynamicValues.size(); i++) {
		if(dynamicValues.at(i) != 0) {
			dynamicValues.at(i)->unLink(this);
		}
	}
}

void DynamicSynchronousEvent::refresh(const DynamicValueEvent& evt)
{
	if(sourceEvents_l != 0) {
		updateValue(sourceEvents_l);
	}

	load();
}

