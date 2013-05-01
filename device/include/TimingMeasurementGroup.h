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
	TimingMeasurementGroup(const EngineTimestamp& timeStamp) 
		: timeStamp_l(timeStamp) {}

	const EngineTimestamp getTimeStamp() const { return timeStamp_l; }
//	TimingMeasurementVector& measurements() { return timingMeasurements; }
	
	unsigned numberOfScheduledMeasurements() const { return numScheduled; }
//	unsigned numberOfRecordedMeasurements() const { return timingMeasurements.size(); } //not thread safe!

	void appendToGroup(TimingMeasurementVector& measurementsIn)
	{
		//lock
		timingMeasurements.insert(timingMeasurements.end(), measurementsIn.begin(), measurementsIn.end());
	}
	void appendGroupTo(TimingMeasurementVector& measurementsOut)
	{
		//lock
		unsigned initial = measurementsOut.size();
		measurementsOut.insert(measurementsOut.end(), timingMeasurements.begin(), timingMeasurements.end());
		unsigned final = measurementsOut.size();

		//final - initial;
	}

private:
	
	const EngineTimestamp timeStamp_l;
	TimingMeasurementVector timingMeasurements;

};

}
}

#endif
