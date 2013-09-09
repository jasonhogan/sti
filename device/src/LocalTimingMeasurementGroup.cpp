

#include "LocalTimingMeasurementGroup.h"
#include "LocalTimingMeasurement.h"
#include "TimingEvent.h"
#include "ScheduledMeasurement.h"
//#include "TimingMeasurementResult.h"

using STI::TimingEngine::LocalTimingMeasurementGroup;
using STI::TimingEngine::EngineTimestamp;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingMeasurementVector_ptr;
using STI::TimingEngine::ScheduledMeasurement_ptr;
using STI::TimingEngine::TimingMeasurement_ptr;
using STI::TimingEngine::LocalTimingMeasurement;
using STI::TimingEngine::TimingEvent_ptr;
//using STI::TimingEngine::TimingMeasurementResultVector;
//using STI::TimingEngine::TimingMeasurement_ptr;
//using STI::TimingEngine::TimingMeasurementResult;
//using STI::TimingEngine::TimingMeasurementResult_ptr;

LocalTimingMeasurementGroup::LocalTimingMeasurementGroup(const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement) :
timeStamp_l(timeStamp), numScheduled(numberScheduledMeasurement) 
{
	timingMeasurements = TimingMeasurementVector_ptr(new TimingMeasurementVector());
}

LocalTimingMeasurementGroup::~LocalTimingMeasurementGroup()
{
}

const EngineTimestamp LocalTimingMeasurementGroup::getTimeStamp() const 
{
	return timeStamp_l;
}

unsigned LocalTimingMeasurementGroup::numberOfScheduledMeasurements() const
{
	return numScheduled;
}
unsigned LocalTimingMeasurementGroup::numberOfRecordedMeasurements() const
{
	boost::unique_lock<boost::mutex> lock(mutex);
	return timingMeasurements->size();
}

//void LocalTimingMeasurementGroup::addMeasurementResult(const TimingMeasurement_ptr& measurement, TimingMeasurementResult_ptr& result) const
void LocalTimingMeasurementGroup::createMeasurementResult(const ScheduledMeasurement_ptr& measurement, TimingMeasurement_ptr& result) const
{
	boost::unique_lock<boost::mutex> lock(mutex);

	TimingEvent_ptr rawEvent;
	if(measurement->getMeasurementEvent(rawEvent) && rawEvent != 0) {
		result = TimingMeasurement_ptr(
			new LocalTimingMeasurement(measurement->scheduledTime(), rawEvent->channel(), rawEvent->eventNum(), measurement->scheduledDescription()));
	}
}


void LocalTimingMeasurementGroup::appendResultsToGroup(const TimingMeasurementVector& measurementsIn)
{
	boost::unique_lock<boost::mutex> lock(mutex);
	timingMeasurements->insert(timingMeasurements->end(), 
		measurementsIn.begin(), measurementsIn.end());
}


void LocalTimingMeasurementGroup::copyResultsTo(TimingMeasurementVector& measurementsOut, unsigned firstMeasurement)
{
	boost::unique_lock<boost::mutex> lock(mutex);

	if(firstMeasurement < timingMeasurements->size()) {
		measurementsOut.insert(measurementsOut.end(), 
			timingMeasurements->begin() + firstMeasurement, timingMeasurements->end());
	}
}

bool LocalTimingMeasurementGroup::getResults(TimingMeasurementVector_ptr& measurementsOut)
{
	measurementsOut = timingMeasurements;

	return (measurementsOut != 0);
}

