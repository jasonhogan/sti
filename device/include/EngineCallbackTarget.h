#ifndef STI_TIMINGENGINE_ENGINECALLBACKTARGET_H
#define STI_TIMINGENGINE_ENGINECALLBACKTARGET_H

#include "TimingEngineTypes.h"
#include "DeviceID.h"
#include "EngineID.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

class EngineCallbackTarget	//inplemented by ServerEventEngine
{
public:

	virtual ~EngineCallbackTarget() {}

	virtual void handleCallback(const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		const STI::TimingEngine::EventEngineState& state) = 0;
};

class NullEngineCallbackTarget : public EngineCallbackTarget
{
public:
	void handleCallback(const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		const STI::TimingEngine::EventEngineState& state) {}
};

}
}

#endif

