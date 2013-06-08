#ifndef STI_DEVICE_DEVICEINTERFACE_H
#define STI_DEVICE_DEVICEINTERFACE_H

#include "TimingEngineTypes.h"
#include "MixedValue.h"

namespace STI
{
namespace Device
{

class DeviceInterface;

class DeviceInterface
{
public:

	virtual ~DeviceInterface() {}
	
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

