
#include "ParsingResultsHandler.h"
#include "ParsingResultsTarget.h"

using STI::TimingEngine::ParsingResultsHandler;
using STI::TimingEngine::ParsingResultsTarget;
using STI::TimingEngine::ParsingResultsTarget_ptr;

ParsingResultsHandler::ParsingResultsHandler(
	const STI::Device::DeviceID& deviceID, 
	const STI::TimingEngine::EngineInstance& engineInstance,
	const ParsingResultsTarget_ptr& resultsTarget)
: deviceID_l(deviceID), engineInstance_l(engineInstance), target(resultsTarget), success_l(false)
{
}


bool ParsingResultsHandler::parseSucceeded()
{
	return success_l;
}

void ParsingResultsHandler::returnResults(bool success, const std::string& errors, 
	const STI::TimingEngine::TimingEventVector_ptr& eventsOut)
{
	success_l = success;
	target->handleParsingResults(deviceID_l, engineInstance_l, success, errors, eventsOut);
}
