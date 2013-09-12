#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENTGROUP_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENTGROUP_H

#include <TimingEngineTypes.h>

namespace STI
{
namespace TimingEngine
{

class TimingMeasurementGroup
{
public:

	virtual ~TimingMeasurementGroup() {}

	virtual const EngineTimestamp getTimeStamp() const = 0;
	virtual unsigned numberOfScheduledMeasurements() const = 0;
	virtual unsigned numberOfRecordedMeasurements() const = 0;

	virtual void createMeasurementResult(const ScheduledMeasurement_ptr& measurement, TimingMeasurement_ptr& result) const = 0;
	//correctly pushes back to the local list (may be a TMeasurementSeq)
	
	virtual void appendResultsToGroup(const TimingMeasurementVector& measurementsIn) = 0;
	virtual void copyResultsTo(TimingMeasurementVector& measurementsOut, unsigned firstMeasurement) = 0;

	virtual bool getResults(TimingMeasurementVector_ptr& measurementsOut) = 0;

};

}
}

#endif
