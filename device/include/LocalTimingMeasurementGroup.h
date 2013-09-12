#ifndef STI_TIMINGENGINE_LOCALTIMINGMEASUREMENTGROUP_H
#define STI_TIMINGENGINE_LOCALTIMINGMEASUREMENTGROUP_H

#include "TimingMeasurementGroup.h"
#include <TimingEngineTypes.h>
#include "EngineID.h"


#include <boost/thread/mutex.hpp>
//#include <boost/thread.hpp>
//#include <boost/thread/locks.hpp>


namespace STI
{
namespace TimingEngine
{


class LocalTimingMeasurementGroup : public TimingMeasurementGroup
{
public:
	LocalTimingMeasurementGroup(const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement);
	~LocalTimingMeasurementGroup();

	const EngineTimestamp getTimeStamp() const;
//	TimingMeasurementVector& measurements() { return timingMeasurements; }
	
	unsigned numberOfScheduledMeasurements() const;
	unsigned numberOfRecordedMeasurements() const;

//	void addMeasurementResult(const TimingMeasurement_ptr& measurement, TimingMeasurement_ptr& result) const;
	void createMeasurementResult(const ScheduledMeasurement_ptr& measurement, TimingMeasurement_ptr& result) const;

	void appendResultsToGroup(const TimingMeasurementVector& measurementsIn);
	void copyResultsTo(TimingMeasurementVector& measurementsOut, unsigned firstMeasurement);
	
	bool getResults(TimingMeasurementVector_ptr& measurementsOut);

private:
	
	const EngineTimestamp timeStamp_l;
	TimingMeasurementVector_ptr timingMeasurements;
	unsigned numScheduled;
	
	mutable boost::mutex mutex;
};

}
}

#endif
