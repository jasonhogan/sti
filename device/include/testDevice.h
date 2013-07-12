
#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include "STI_Device.h"
#include <string>


class testDevice : public STI::Device::STI_Device
{
public:
	
	testDevice(std::string DeviceName, std::string Address, unsigned short ModuleNumber) : 
	STI_Device(DeviceName, Address, ModuleNumber) {};
	~testDevice() {};

	// Device main()
	bool deviceMain(int argc, char** argv);

	// Device Attributes
	void defineAttributes();
	void refreshAttributes() {};
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();
	bool readChannel(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut);
	bool writeChannel(unsigned short channel, const STI::Utils::MixedValue& commandIn);
	
	// Device Command line interface setup
	std::string execute(int argc, char** argv);
	void definePartnerDevices() 
	{
//		addPartnerDevice("test", "128.12.174.77", 1, "testDevice");
//		this->
	};
	
	// Device-specific event parsing
	void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut) { parseDeviceEventsDefault(eventsIn, eventsOut); }


};

#endif

