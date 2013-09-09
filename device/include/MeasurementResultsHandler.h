#ifndef STI_TIMINGENGINE_MEASUREMENTRESULTSHANDLER_H
#define STI_TIMINGENGINE_MEASUREMENTRESULTSHANDLER_H

#include "TimingEngineTypes.h"


namespace STI
{
namespace TimingEngine
{

class MeasurementResultsHandler
{
public:
	
	virtual ~MeasurementResultsHandler() {}

	virtual void handleNewData(const EngineInstance& engineInstance, const TimingMeasurementGroup_ptr& data) = 0;

	virtual bool createNewMeasurementGroup(TimingMeasurementGroup_ptr& measurementGroup, 
		const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement) = 0;

};


}
}

#endif

