#ifndef STI_TIMINGENGINE_MEASUREMENTRESULTSTARGET_H
#define STI_TIMINGENGINE_MEASUREMENTRESULTSTARGET_H

#include "TimingEngineTypes.h"
#include "EngineID.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

class MeasurementResultsHandler
{
public:
	MeasurementResultsHandler(MeasurementResultsTarget& target, unsigned firstResult);

	void handleNewData(const EngineInstance& engineInstance, const TimingMeasurementGroup_ptr& data);

private:
	MeasurementResultsTarget& target_l;
	unsigned firstResult_l;
};


class MeasurementResultsTarget	//abc
{
public:
	virtual ~MeasurementResultsTarget() {}

	virtual void appendData(const EngineInstance& engineInstance, const TimingMeasurementVector& newData, unsigned numberScheduledMeasurements) = 0;
};

}
}

#endif

