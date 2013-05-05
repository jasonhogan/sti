
#include "SynchronousEvent.h"

#include "EventTime.h"
#include "TimingMeasurementResult.h"
#include "ScheduledMeasurement.h"
#include "TimingEvent.h"
#include "TimingMeasurementGroup.h"

using STI::TimingEngine::EventTime;
using STI::TimingEngine::SynchronousEvent;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingEvent_ptr;
using STI::TimingEngine::TimingMeasurementGroup_ptr;
using STI::TimingEngine::SynchronousEvent_ptr;

bool STI::TimingEngine::compareSynchronousEventPtrs(SynchronousEvent_ptr l,SynchronousEvent_ptr r) 
{ 
	return (*(l.get()) <  *(r.get()));
}


SynchronousEvent::SynchronousEvent(EventTime time)
{
	setTime(time);
}


void SynchronousEvent::reload()
{
	setupEvent();	//pure virtual
	reloadEvent();	//pure virtual
}
void SynchronousEvent::reloadEvent()
{
	loadEvent(); //pure virtual
}

void SynchronousEvent::collectData(const TimingMeasurementGroup_ptr& measurements)
{
	//The results vector is generated each time this event is played.
	//It is based on the ScheduledMeasurementVector for this event, 
	//which is setup during parsing and is then static.
	TimingMeasurementResultVector results;
	
	for(ScheduledMeasurementVector::iterator scheduledMeas = scheduledMeasurements.begin(); 
		scheduledMeas != scheduledMeasurements.end(); ++scheduledMeas)
	{
		results.push_back(
			TimingMeasurementResult_ptr(new TimingMeasurementResult(
			*(*scheduledMeas)
			)));
	}
	//Fill the results vector using the derived-class-specific implementation.
	//Sends the entire vector so that the derived class can properly handle cases
	//with multiple measurements.
	collectMeasurements( results );

	//Append the results vector to the measurement list for this play instance.
//	measurements.insert(measurements.end(), results.begin(), results.end());
	measurements->appendResultsToGroup(results);
}

void SynchronousEvent::addMeasurement(const TimingEvent_ptr& measurementEvent)
{
	if(measurementEvent->isMeasurementEvent()) {
		ScheduledMeasurement_ptr measurement;
		if(measurementEvent->getMeasurement(measurement) && measurement != 0) {
			measurement->setTime(getTime());
			measurement->setScheduleStatus(true);
			scheduledMeasurements.push_back(measurement);
		}
	}
}


void SynchronousEvent::load()
{
	setupEvent();	//pure virtual
	loadEvent();	//pure virtual
}

void SynchronousEvent::play()
{
	playEvent();	//pure virtual
}

