#ifndef STI_DEVICE_DEVICE_H
#define STI_DEVICE_DEVICE_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace Device
{

class Device;

class Device
{
public:

	virtual ~Device() {}
	
	virtual const STI::Device::DeviceID& getDeviceID() const = 0;
	virtual bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager) = 0;
	virtual bool getPartnerCollector(STI::Device::DeviceCollector_ptr& deviceCollector) = 0;

	virtual bool read(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut) = 0;
	virtual bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn) = 0;


};

}
}

#endif

