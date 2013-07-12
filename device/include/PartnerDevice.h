#ifndef STI_DEVICE_PARTNERDEVICE_H
#define STI_DEVICE_PARTNERDEVICE_H

#include "DeviceTypes.h"
#include "TimingEngineTypes.h"
#include "DeviceID.h"

#include <string>

namespace STI
{
namespace Device
{

class PartnerDevice
{
public:


	PartnerDevice(std::string partnerAlias, const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::PartnerEventHandler_ptr& eventHandler);

	static const STI::Device::PartnerDevice_ptr& getNullPartner();

	const STI::Device::DeviceID& getDeviceID() const;

	void enablePartnerEvents();
	bool getPartnerEventsSetting() const;

	//Adding partner events
	template<typename T>
	void event(double time, unsigned short channel, const T& value, 
		const STI::TimingEngine::TimingEvent_ptr& referenceEvent, std::string description="") throw(std::exception)
	{
		event(time, channel, STI::Utils::MixedValue(value), referenceEvent, description);
	}

	template<typename T>
	void meas(double time, unsigned short channel, const T& value, 
		const STI::TimingEngine::TimingEvent_ptr& referenceEvent, std::string description="") throw(std::exception)
	{
		meas(time, channel, MixedValue(value), referenceEvent, description);
	}

	void event(double time, unsigned short channel, const STI::Utils::MixedValue& value, 
		const STI::TimingEngine::TimingEvent_ptr& referenceEvent, std::string description="") throw(std::exception);
	
	void meas(double time, unsigned short channel, const STI::Utils::MixedValue& value, 
		const STI::TimingEngine::TimingEvent_ptr& referenceEvent, std::string description="") throw(std::exception);
	void meas(double time, unsigned short channel, const STI::TimingEngine::TimingEvent_ptr& referenceEvent, 
		std::string description="") throw(std::exception);

private:

	bool checkPartnerStatus(const STI::TimingEngine::TimingEvent_ptr& referenceEvent) throw(std::exception);

	PartnerDevice();
	static STI::Device::PartnerDevice_ptr nullPartner;
	bool isNull;

	STI::Device::DeviceID partnerDeviceID;
	std::string partnerAlias_l;
	
	bool partnerEventsEnabled;

	STI::TimingEngine::PartnerEventHandler_ptr partnerEventHandler;

};

}
}

#endif

