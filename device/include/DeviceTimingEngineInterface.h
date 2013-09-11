#ifndef STI_DEVICE_DEVICETIMINGENGINEINTERFACE_H
#define STI_DEVICE_DEVICETIMINGENGINEINTERFACE_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include <string>

namespace STI
{
namespace Device
{

class DeviceTimingEngineInterface
{
public:

	virtual ~DeviceTimingEngineInterface() {}

	virtual void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut) throw(std::exception) = 0;	// Device-specific event parsing

	virtual const STI::TimingEngine::ChannelMap& getChannels() const = 0;

	virtual void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget) = 0;

	virtual const DeviceID& getDeviceID() const = 0;

	virtual bool waitForTrigger(const STI::TimingEngine::MasterTrigger_ptr& masterTrigger) = 0;
};

}
}

#endif

