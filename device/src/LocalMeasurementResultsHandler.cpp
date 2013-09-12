

#include "LocalMeasurementResultsHandler.h"
#include "MeasurementResultsTarget.h"
#include "TimingMeasurement.h"
#include "TimingMeasurementGroup.h"
#include "EngineID.h"
#include "LocalTimingMeasurementGroup.h"

using STI::TimingEngine::LocalMeasurementResultsHandler;
using STI::TimingEngine::MeasurementResultsTarget;
using STI::TimingEngine::MeasurementResultsTarget_ptr;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingMeasurementGroup_ptr;
using STI::TimingEngine::EngineInstance;
using STI::TimingEngine::EngineTimestamp;
using STI::TimingEngine::LocalTimingMeasurementGroup;


LocalMeasurementResultsHandler::LocalMeasurementResultsHandler(const MeasurementResultsTarget_ptr& target, unsigned firstResult)
	: target_l(target), firstResult_l(firstResult)
{
}

void LocalMeasurementResultsHandler::handleNewData(const EngineInstance& engineInstance, const TimingMeasurementGroup_ptr& data)
{
	//lightweight list of pointers to subset of the measurements
	TimingMeasurementVector newData;
	//Copy data pointers starting from point firstResult
	data->copyResultsTo(newData, firstResult_l);

	target_l->appendData(engineInstance, newData, data->numberOfScheduledMeasurements());
}

bool LocalMeasurementResultsHandler::createNewMeasurementGroup(TimingMeasurementGroup_ptr& measurementGroup, 
		const EngineTimestamp& timeStamp, unsigned numberScheduledMeasurement)
{
	measurementGroup = TimingMeasurementGroup_ptr(new LocalTimingMeasurementGroup(timeStamp, numberScheduledMeasurement));

	return (measurementGroup != 0);
}
