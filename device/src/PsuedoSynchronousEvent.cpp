
#include "PsuedoSynchronousEvent.h"
#include "STI_Device.h"
#include "TimingMeasurement.h"
#include "TimingEventGroup.h"

using STI::TimingEngine::PsuedoSynchronousEvent;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::TimingEventVector;
using STI::TimingEngine::SynchronousEvent;
using STI::Device::STI_Device;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingEventVector_ptr;

PsuedoSynchronousEvent::PsuedoSynchronousEvent(EventTime time, const TimingEventVector_ptr& events, STI_Device* device) 
: SynchronousEvent(time), events_l(events), device_l(device)
{
}


PsuedoSynchronousEvent::PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy)
: SynchronousEvent(copy), events_l(copy.events_l), device_l(copy.device_l)
{
}

void PsuedoSynchronousEvent::playEvent()
{
	for(unsigned i = 0; i < events_l->size(); i++) {
		device_l->write( events_l->at(i)->channel().channelNum(), events_l->at(i)->value());
	}
}

void PsuedoSynchronousEvent::collectMeasurements(TimingMeasurementVector& measurementsOut)
{
	unsigned k;
	for(unsigned i = 0; i < measurementsOut.size(); i++) 
	{
		if( getEventIndex(measurementsOut.at(i)->eventNum(), k) ) {
			
			if(device_l->read(
				measurementsOut.at(i)->channel().channelNum(), 
				events_l->at(k)->value(),				//command
				measurementsOut.at(i)->value())) {		//measured value
			}
		}
	}
	
//	for(unsigned i = 0; i < events_l.size(); i++)
//	{
//		device_l->read(events_l.at(i));
		//if( events_l.at(i).isMeasurementEvent() && !events_l.at(i).getMeasurement()->isMeasured() )
		//{
		//}
//	}
}
void PsuedoSynchronousEvent::publishMeasurements(const TimingMeasurementVector& measurements)
{
}

bool PsuedoSynchronousEvent::getEventIndex(unsigned eventNum, unsigned& k)
{
	for(unsigned i = 0; i < events_l->size(); i++) {
		if(events_l->at(i)->eventNum() == eventNum) {
			k = i;
			return true;
		}
	}
	return false;

}