
#ifndef TESTDEVICE_H
#define TESTDEVICE_H

#include "STI_Device.h"
#include <string>


class testDevice : public STI_Device
{
public:
	
	testDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string DeviceType, std::string Address, 
		unsigned short ModuleNumber) : 
	STI_Device(orb_manager,DeviceName, DeviceType,Address,ModuleNumber) {};

	~testDevice() {};

	virtual std::string deviceType();
	virtual void defineAttributes();

};

#endif