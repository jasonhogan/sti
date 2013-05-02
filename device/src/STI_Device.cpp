

#include <STI_Device.h>
//#include <PartnerDevice.h>

#include "TimingEngineTypes.h"
#include <EngineID.h>
#include <DeviceEventEngine.h>

#include <string>

using STI::Device::STI_Device;
using STI::Device::DeviceID;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::ChannelMap;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::EventEngine_ptr;

STI_Device::STI_Device(std::string DeviceName, 
					   std::string IPAddress, unsigned short ModuleNumber) 
: deviceID(DeviceName, IPAddress, ModuleNumber)
{
	init();
}

STI_Device::~STI_Device()
{
}

void STI_Device::init()
{
	//add main engine for testing
	EngineID mainEngine(1, "Main");
	EventEngine_ptr engine = EventEngine_ptr(new DeviceEventEngine(*this));
	eventEngineManager.addEventEngine(mainEngine, engine);

//	dummyPartner = new PartnerDevice(true);
}


void STI_Device::setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& eventTarget)
{
	partnerEventTarget = eventTarget;
}

//Partner::event(...)
//{
//	if(partnerEventTarget) {
//		partnerEventTarget->addEvent(getDevice().getID(), evt);
//	}
//}

const DeviceID& STI_Device::getDeviceID() const
{
	return deviceID;
}

const ChannelMap& STI_Device::getChannels() const
{
	return channels;
}

