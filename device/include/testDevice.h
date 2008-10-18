
#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include "STI_Device.h"
#include <string>


class testDevice : public STI_Device
{
public:
	
	testDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber) : 
	STI_Device(orb_manager,DeviceName,Address,ModuleNumber) {};

	~testDevice() {};

	virtual void defineAttributes();
//	virtual bool updateAttribute(std::string key, std::string & value);
	virtual void defineChannels();
	virtual bool deviceMain();

	bool writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event);
	bool readChannel(STI_Server_Device::TMeasurement & Measurement);

	std::string executeArgs(std::string args);
	std::string commandLineDeviceName() {return "test2";};

	void definePartnerDevices() {addPartnerDevice("lock");};

	void refreshAttributes() {};
	virtual bool updateAttribute(std::string key, std::string value) { return false;};

};

#endif

