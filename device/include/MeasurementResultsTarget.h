#ifndef STI_TIMINGENGINE_MEASUREMENTRESULTSTARGET_H
#define STI_TIMINGENGINE_MEASUREMENTRESULTSTARGET_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class MeasurementResultsTarget	//abc
{
public:
	virtual ~MeasurementResultsTarget() {}

	virtual void appendData(const EngineInstance& engineInstance, const TimingMeasurementVector& newData, unsigned numberScheduledMeasurements) = 0;
};


class NullMeasurementResultsTarget : public MeasurementResultsTarget
{
public:
	NullMeasurementResultsTarget() {}

	virtual void appendData(const EngineInstance& engineInstance, const TimingMeasurementVector& newData, unsigned numberScheduledMeasurements) {}
};


}
}

#endif
