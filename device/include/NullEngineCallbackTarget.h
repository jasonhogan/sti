#ifndef STI_TIMINGENGINE_NULLENGINECALLBACKTARGET_H
#define STI_TIMINGENGINE_NULLENGINECALLBACKTARGET_H

#include "TimingEngineTypes.h"
#include "EngineCallbackTarget.h"

namespace STI
{
namespace TimingEngine
{

class NullEngineCallbackTarget : public EngineCallbackTarget
{
public:
	void handleCallback(const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		const STI::TimingEngine::EventEngineState& state) {}

	static EngineCallbackTarget_ptr createNullTarget()
	{
		EngineCallbackTarget_ptr target = EngineCallbackTarget_ptr(new NullEngineCallbackTarget());
		return target;
	}
};

}
}

#endif

