

#include <STI_Device.h>
//#include <PartnerDevice.h>

#include "TimingEngineTypes.h"
#include <EngineID.h>
#include <DeviceEventEngine.h>

#include <string>

using STI::Device::STI_Device;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::DeviceEventEngine_ptr;

STI_Device::STI_Device(std::string DeviceName, 
					   std::string IPAddress, unsigned short ModuleNumber) 
: deviceID(DeviceName, IPAddress, ModuleNumber)
{
	init();
}

void STI_Device::init()
{
	//add main engine for testing
	EngineID mainEngine(1, "Main");
	DeviceEventEngine_ptr engine(*this);
	eventEngineManager.addEventEngine(mainEngine, engine);

//	dummyPartner = new PartnerDevice(true);
}
