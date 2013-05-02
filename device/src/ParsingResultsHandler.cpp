
#include "ParsingResultsHandler.h"
#include "ParsingResultsTarget.h"

using STI::TimingEngine::ParsingResultsHandler;
using STI::TimingEngine::ParsingResultsTarget;

ParsingResultsHandler::ParsingResultsHandler(
	const STI::Device::DeviceID& deviceID, 
	const STI::TimingEngine::EngineInstance& engineInstance,
	ParsingResultsTarget& resultsTarget)
: deviceID_l(deviceID), engineInstance_l(engineInstance), target(resultsTarget), success_l(false)
{
}


bool ParsingResultsHandler::parseSucceeded()
{
	return success_l;
}

void ParsingResultsHandler::returnResults(bool success, const std::string& errors, 
	const STI::TimingEngine::DeviceTimingEventsMap& eventsOut)
{
	success_l = success;
	target.handleParsingResults(deviceID_l, engineInstance_l, success, errors, eventsOut);
}
