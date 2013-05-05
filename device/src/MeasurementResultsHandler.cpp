

#include "MeasurementResultsHandler.h"
#include "TimingMeasurement.h"
#include "TimingMeasurementGroup.h"
#include "EngineID.h"

using STI::TimingEngine::MeasurementResultsHandler;
using STI::TimingEngine::MeasurementResultsTarget;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingMeasurementGroup_ptr;
using STI::TimingEngine::EngineInstance;

MeasurementResultsHandler::MeasurementResultsHandler(MeasurementResultsTarget& target, unsigned firstResult)
	: target_l(target), firstResult_l(firstResult)
{
}

void MeasurementResultsHandler::handleNewData(const EngineInstance& engineInstance, const TimingMeasurementGroup_ptr& data)
{
	//lightweight list of pointers to subset of the measurements
	TimingMeasurementVector newData;
	//Copy data pointers starting from point firstResult
	data->copyResultsTo(newData, firstResult_l);

	target_l.appendData(engineInstance, newData, data->numberOfScheduledMeasurements());
}

