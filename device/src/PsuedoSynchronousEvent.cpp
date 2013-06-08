
#include "PsuedoSynchronousEvent.h"
#include "STI_Device.h"
#include "TimingMeasurementResult.h"
#include "TimingEventGroup.h"

using STI::TimingEngine::PsuedoSynchronousEvent;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::TimingEventVector;
using STI::TimingEngine::SynchronousEvent;
using STI::Device::STI_Device;
using STI::TimingEngine::TimingMeasurementResultVector;
using STI::TimingEngine::TimingEventGroup_ptr;

PsuedoSynchronousEvent::PsuedoSynchronousEvent(EventTime time, const TimingEventGroup_ptr& events, STI_Device* device) 
: SynchronousEvent(time), events_l(events), device_l(device)
{
}


PsuedoSynchronousEvent::PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy)
: SynchronousEvent(copy), events_l(copy.events_l), device_l(copy.device_l)
{
}

void PsuedoSynchronousEvent::playEvent()
{
	for(unsigned i = 0; i < events_l->numberOfEvents(); i++) {
		device_l->write( events_l->at(i)->channel().channelNum(), events_l->at(i)->value());
	}
}

void PsuedoSynchronousEvent::collectMeasurements(TimingMeasurementResultVector& measurementsOut)
{
	unsigned k;
	for(unsigned i = 0; i < measurementsOut.size(); i++) 
	{
		if( getEventIndex(measurementsOut.at(i)->eventNum(), k) ) {
			
			device_l->read(
				measurementsOut.at(i)->channel().channelNum(), 
				events_l->at(k)->value(),				//command
				measurementsOut.at(i)->value());		//measured value
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

bool PsuedoSynchronousEvent::getEventIndex(unsigned eventNum, unsigned& k)
{
	for(unsigned i = 0; i < events_l->numberOfEvents(); i++) {
		if(events_l->at(i)->eventNum() == eventNum) {
			k = i;
			return true;
		}
	}
	return false;

}