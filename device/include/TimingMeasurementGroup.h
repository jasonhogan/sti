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
	TimingMeasurementGroup(const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement);

	const EngineTimestamp getTimeStamp() const;
//	TimingMeasurementVector& measurements() { return timingMeasurements; }
	
	unsigned numberOfScheduledMeasurements() const;
	unsigned numberOfRecordedMeasurements() const;

	void appendResultsToGroup(TimingMeasurementResultVector& measurementsIn);
	void appendResultsToGroup(TimingMeasurementVector& measurementsIn);
	void copyResultsTo(TimingMeasurementVector& measurementsOut, unsigned firstMeasurement);

private:
	
	const EngineTimestamp timeStamp_l;
	TimingMeasurementVector timingMeasurements;
	unsigned numScheduled;
	
	mutable boost::mutex mutex;
};

}
}

#endif
