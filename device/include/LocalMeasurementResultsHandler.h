#ifndef STI_TIMINGENGINE_LOCALMEASUREMENTRESULTSHANDLER_H
#define STI_TIMINGENGINE_LOCALMEASUREMENTRESULTSHANDLER_H

#include "TimingEngineTypes.h"
#include "MeasurementResultsHandler.h"


namespace STI
{
namespace TimingEngine
{


class LocalMeasurementResultsHandler : public MeasurementResultsHandler
{
public:
	LocalMeasurementResultsHandler(const MeasurementResultsTarget_ptr& target, unsigned firstResult);

	void handleNewData(const EngineInstance& engineInstance, const TimingMeasurementGroup_ptr& data);

	bool createNewMeasurementGroup(TimingMeasurementGroup_ptr& measurementGroup, 
		const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement);

private:
	MeasurementResultsTarget_ptr target_l;
	unsigned firstResult_l;
};


}
}

#endif

