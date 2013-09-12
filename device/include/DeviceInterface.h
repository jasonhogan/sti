#ifndef STI_DEVICE_DEVICEINTERFACE_H
#define STI_DEVICE_DEVICEINTERFACE_H

#include "TimingEngineTypes.h"
#include "ServerTypes.h"
#include "DeviceTypes.h"
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
//	virtual bool addEventEngine(const STI::TimingEngine::EngineID& engineID) = 0;
	
	virtual bool getPartnerCollector(STI::Device::DeviceCollector_ptr& deviceCollector) = 0;
	virtual bool getEventPartners(STI::Device::DeviceIDSet_ptr& eventPartners) = 0;
	virtual bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager) = 0;

	virtual bool read(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut) = 0;
	virtual bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn) = 0;

//	virtual void start() = 0;
	virtual void connect(const STI::Server::ServerInterface& server) = 0;


};

}
}

#endif

