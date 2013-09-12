
#include "SynchronousEvent.h"

#include "EventTime.h"
//#include "TimingMeasurementResult.h"
#include "ScheduledMeasurement.h"
#include "TimingEvent.h"
#include "TimingMeasurementGroup.h"

using STI::TimingEngine::EventTime;
using STI::TimingEngine::SynchronousEvent;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingEvent_ptr;
using STI::TimingEngine::TimingMeasurementGroup_ptr;
using STI::TimingEngine::SynchronousEvent_ptr;
using STI::TimingEngine::DocumentationOptions_ptr;

//bool STI::TimingEngine::compareSynchronousEventPtrs(SynchronousEvent_ptr l, SynchronousEvent_ptr r) 
//{ 
//	return (*(l.get()) <  *(r.get()));
//}


SynchronousEvent::SynchronousEvent(const STI::TimingEngine::EventTime& time)
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
	TimingMeasurementVector results;
	
	TimingMeasurement_ptr result;

	for(ScheduledMeasurementVector::iterator scheduledMeas = scheduledMeasurements.begin(); 
		scheduledMeas != scheduledMeasurements.end(); ++scheduledMeas)
	{
		(*scheduledMeas)->setTime(getTime());
		
		measurements->createMeasurementResult(*scheduledMeas, result);
		results.push_back(result);

		//results.push_back(
		//	TimingMeasurementResult_ptr(new TimingMeasurementResult(
		//	*(*scheduledMeas)
		//	)));
	}
	//Fill the results vector using the derived-class-specific implementation.
	//Sends the entire vector so that the derived class can properly handle cases
	//with multiple measurements.
	collectMeasurements( results );

	//Append the results vector to the measurement list for this play instance.
//	measurements.insert(measurements.end(), results.begin(), results.end());
	measurements->appendResultsToGroup(results);
}

void SynchronousEvent::publishData(const TimingMeasurementGroup_ptr& measurements, DocumentationOptions_ptr documentation)
{
	TimingMeasurementVector_ptr results;

	measurements->getResults(results);

	publishMeasurements(*results);
}

void SynchronousEvent::addMeasurement(const TimingEvent_ptr& measurementEvent)
{
	if(measurementEvent->isMeasurementEvent()) {

		if(measurementEvent != 0 && measurementEvent->isMeasurementEvent()) {
//			ScheduledMeasurement_ptr measurement = ScheduledMeasurement_ptr(new ScheduledMeasurement(measurementEvent));

			ScheduledMeasurement_ptr scheduledMeasurement;
			
			if(measurementEvent->getMeasurement(scheduledMeasurement) && scheduledMeasurement != 0) {
				scheduledMeasurement->setScheduleStatus(true);
				scheduledMeasurements.push_back(scheduledMeasurement);
			}
		}

		//if(measurementEvent->getMeasurement(measurement) && measurement != 0) {
		//	measurement->setTime(getTime());
		//	measurement->setScheduleStatus(true);
		//	scheduledMeasurements.push_back(measurement);
		//}
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

