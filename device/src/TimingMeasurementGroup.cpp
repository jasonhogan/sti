

#include "TimingMeasurementGroup.h"
#include "TimingMeasurementResult.h"

using STI::TimingEngine::TimingMeasurementGroup;
using STI::TimingEngine::EngineTimestamp;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingMeasurementResultVector;

TimingMeasurementGroup::TimingMeasurementGroup(const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement) 
: timeStamp_l(timeStamp), numScheduled(numberScheduledMeasurement) 
{
}

const EngineTimestamp TimingMeasurementGroup::getTimeStamp() const 
{
	return timeStamp_l;
}

unsigned TimingMeasurementGroup::numberOfScheduledMeasurements() const
{
	return numScheduled;
}
unsigned TimingMeasurementGroup::numberOfRecordedMeasurements() const
{
	boost::unique_lock<boost::mutex> lock(mutex);
	return timingMeasurements.size();
}

void TimingMeasurementGroup::appendResultsToGroup(TimingMeasurementResultVector& measurementsIn)
{
	boost::unique_lock<boost::mutex> lock(mutex);
	timingMeasurements.insert(timingMeasurements.end(), 
		measurementsIn.begin(), measurementsIn.end());
}
void TimingMeasurementGroup::appendResultsToGroup(TimingMeasurementVector& measurementsIn)
{
	boost::unique_lock<boost::mutex> lock(mutex);
	timingMeasurements.insert(timingMeasurements.end(), measurementsIn.begin(), measurementsIn.end());
}

void TimingMeasurementGroup::copyResultsTo(TimingMeasurementVector& measurementsOut, unsigned firstMeasurement)
{
	boost::unique_lock<boost::mutex> lock(mutex);

	if(firstMeasurement < timingMeasurements.size()) {
		measurementsOut.insert(measurementsOut.end(), 
			timingMeasurements.begin() + firstMeasurement, timingMeasurements.end());
	}
}
