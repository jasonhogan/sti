

#ifndef STI_DEVICE_STI_DEVICE_H
#define STI_DEVICE_STI_DEVICE_H

#include "TimingEngineTypes.h"

#include "DeviceTimingEngineInterface.h"
#include "DeviceID.h"
#include "LocalEventEngineManager.h"

#include "Channel.h"
#include "MixedValue.h"
#include <string>

namespace STI
{
namespace Device
{

class STI_Device : public DeviceTimingEngineInterface
{

public:

//	STI_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);
//	STI_Device(ORBManager* orb_manager, std::string DeviceName, 
///		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".");
	STI_Device(std::string DeviceName, std::string IPAddress, unsigned short ModuleNumber);
	virtual ~STI_Device();

private:

	virtual void defineAttributes() = 0;
	virtual void defineChannels() = 0;
	virtual void definePartnerDevices() = 0;

	// Device Channels
	virtual bool readChannel(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut) = 0;
	virtual bool writeChannel(unsigned short channel, const STI::Utils::MixedValue& commandIn) = 0;

	// Device Attributes
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

public:

	virtual bool deviceMain(int argc, char* argv[]) = 0;		// Device main()
	virtual std::string execute(int argc, char* argv[]) = 0;	// Device Command line interface


private:

	//DeviceTimingEngineInterface Partial Implementation
	const STI::TimingEngine::ChannelMap& getChannels() const;
	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget);
	const DeviceID& getDeviceID() const;


	void init();



	STI::TimingEngine::ChannelMap channels;

	DeviceID deviceID;
	STI::TimingEngine::LocalEventEngineManager eventEngineManager;
	STI::TimingEngine::PartnerEventTarget_ptr partnerEventTarget;

};

}
}

#endif
