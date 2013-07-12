#ifndef STI_TIMINGENGINE_ENGINECALLBACKHANDLER_H
#define STI_TIMINGENGINE_ENGINECALLBACKHANDLER_H

#include "TimingEngineTypes.h"
#include "DeviceID.h"
#include "EngineID.h"
#include "EventEngineState.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

class EngineCallbackHandler
{
public:

	EngineCallbackHandler(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		const EngineCallbackTarget_ptr& callbackTarget);

	void callback(const STI::TimingEngine::EventEngineState& state);

private:

	STI::Device::DeviceID deviceID_l;
	STI::TimingEngine::EngineInstance engineInstance_l;
	
	EngineCallbackTarget_ptr target;

};


}
}

#endif

