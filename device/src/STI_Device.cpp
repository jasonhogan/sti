

#include <STI_Device.h>
//#include <PartnerDevice.h>

#include "TimingEngineTypes.h"
#include "TimingEvent.h"
#include "Channel.h"
#include <EngineID.h>
#include <DeviceEventEngine.h>
#include "TimingEventGroup.h"
#include "PsuedoSynchronousEvent.h"

#include <string>
#include <iostream>
using namespace std;

using STI::Device::STI_Device;
using STI::Device::DeviceID;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::Channel;
using STI::TimingEngine::Channel_ptr;
using STI::TimingEngine::ChannelMap;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::EventEngine_ptr;
using STI::TimingEngine::TimingEvent_ptr;
using STI::Utils::MixedValue;
using STI::TimingEngine::TimingEventGroupMap;
using STI::TimingEngine::PsuedoSynchronousEvent;
using STI::TimingEngine::PsuedoSynchronousEvent_ptr;


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
	
	usingDefaultEventParsing = false;


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

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	addChannel(channel, STI::TimingEngine::Input, inputType, outputType, defaultName);
}

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType)
{
	addInputChannel(channel, inputType, STI::Utils::Empty, "");
}

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, std::string defaultName)
{
	addInputChannel(channel, inputType, STI::Utils::Empty, defaultName);
}

void STI_Device::addOutputChannel(unsigned short channel, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	addChannel(channel, STI::TimingEngine::Output, STI::Utils::Empty, outputType, defaultName);
}

bool STI_Device::addChannel(unsigned short channel, STI::TimingEngine::ChannelType type, 
							STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	bool valid = true;
//	STI::TimingEngine::Channel tChannel;

	//if(Type == Input && OutputType != ValueNone)
	//{
	//	valid = false;
	//}
	if(type == STI::TimingEngine::Output && inputType != STI::Utils::Empty)
	{
		valid = false;
	}

	//check for duplicate channel number
	ChannelMap::iterator duplicate = channels.find(channel);

	if(duplicate != channels.end())
	{
		valid = false;

		std::cerr << "Error: Duplicate channel number in device " 
			<< getDeviceID().getName() << endl;
	}

	if(valid)
	{
		Channel_ptr tChannel = Channel_ptr(
			new Channel(getDeviceID(), channel, type, defaultName, inputType, outputType));
		channels[channel] = tChannel;
	}
	else
	{
		cerr << "Error: Invalid channel specification in device " 
			<< getDeviceID().getName() << endl;
	}

	return valid;
}


void STI_Device::parseDeviceEventsDefault(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut)
{
	//This event parser works well for non time critical devices (i.e. non-FPGA devices) and devices that
	//are fundamentally serial. Events scheduled for the same time will instead be played sequentially
	//using calls to STI_Device::writeChannel(...).

	usingDefaultEventParsing = true;

	TimingEventGroupMap::const_iterator iter;
	unsigned i;

	PsuedoSynchronousEvent_ptr evt;

	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		evt = PsuedoSynchronousEvent_ptr( 
			new PsuedoSynchronousEvent(iter->first, iter->second, this) );

		eventsOut.push_back( evt );

		// register all measurement events
		for(i = 0; i < iter->second->numberOfEvents(); i++)
		{
			if( iter->second->at(i)->isMeasurementEvent() )	// measurement event
				evt->addMeasurement( iter->second->at(i) );
		}
	}
}


//bool STI_Device::read(const TimingEvent_ptr& measurementEvent)
//{
//	MixedValue_ptr data;
//	measurementEvent->getMeasurement(
//	if(readChannel(measurementEvent->channelNum(), measurementEvent->value(), data))
//	{
//		measurementEvent.getMeasurement()->setData(data);
//		return true;
//	}
//	else
//		return false;
//}

bool STI_Device::read(unsigned short channel, 
	const MixedValue& commandIn, MixedValue& measurementOut)
{
	return readChannel(channel, commandIn, measurementOut);
}

bool STI_Device::write(unsigned short channel, const MixedValue& value)
{
	return writeChannel(channel, value);
}







