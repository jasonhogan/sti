
#include "EngineCallbackHandler.h"
#include "EngineCallbackTarget.h"

using STI::TimingEngine::EngineCallbackHandler;
using STI::TimingEngine::EngineCallbackTarget;
using STI::TimingEngine::EngineCallbackTarget_ptr;

EngineCallbackHandler::EngineCallbackHandler(
	const STI::Device::DeviceID& deviceID, 
	const STI::TimingEngine::EngineInstance& engineInstance,
	const EngineCallbackTarget_ptr& resultsTarget)
: deviceID_l(deviceID), engineInstance_l(engineInstance), target(resultsTarget)
{
}


void EngineCallbackHandler::callback(const STI::TimingEngine::EventEngineState& state)
{
	target->handleCallback(deviceID_l, engineInstance_l, state);
}

