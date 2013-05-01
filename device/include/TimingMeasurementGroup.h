#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENTGROUP_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENTGROUP_H

#include <TimingEngineTypes.h>
#include "EngineID.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

namespace STI
{
namespace TimingEngine
{

class TimingMeasurementGroup
{
public:
	TimingMeasurementGroup(const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement) 
		: timeStamp_l(timeStamp), numScheduled(numberScheduledMeasurement) {}

	const EngineTimestamp getTimeStamp() const { return timeStamp_l; }
//	TimingMeasurementVector& measurements() { return timingMeasurements; }
	
	unsigned numberOfScheduledMeasurements() const { return numScheduled; }
	unsigned numberOfRecordedMeasurements() const {
		boost::unique_lock<boost::mutex> lock(mutex);
		return timingMeasurements.size();
	}

	void appendResultsToGroup(TimingMeasurementResultVector& measurementsIn)
	{
		boost::unique_lock<boost::mutex> lock(mutex);
		timingMeasurements.insert(timingMeasurements.end(), measurementsIn.begin(), measurementsIn.end());
	}
	//void appendGroupTo(TimingMeasurementVector& measurementsOut)
	//{
	//	//lock
	//	unsigned initial = measurementsOut.size();
	//	measurementsOut.insert(measurementsOut.end(), timingMeasurements.begin(), timingMeasurements.end());
	//	unsigned final = measurementsOut.size();

	//	//final - initial;
	//}

private:
	
	const EngineTimestamp timeStamp_l;
	TimingMeasurementResultVector timingMeasurements;
	unsigned numScheduled;
	
	mutable boost::mutex mutex;
};

}
}

#endif
