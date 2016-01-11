
#include "PartnerDevice.h"
#include "PartnerEventHandler.h"
#include "RawEvent.h"
#include "TimingEvent.h"
#include "Channel.h"
#include "LocalTimingEvent.h"
#include "EventParsingException.h"
#include "EngineException.h"

using STI::Device::PartnerDevice;
using STI::TimingEngine::PartnerEventHandler_ptr;
using STI::TimingEngine::RawEvent;
using STI::TimingEngine::TimingEvent;
using STI::TimingEngine::TimingEvent_ptr;

STI::Device::PartnerDevice_ptr PartnerDevice::nullPartner = STI::Device::PartnerDevice_ptr(new PartnerDevice());



PartnerDevice::PartnerDevice(std::string partnerAlias, 
							const STI::Device::DeviceID& deviceID, 
							const STI::TimingEngine::PartnerEventHandler_ptr& eventHandler) : 
partnerDeviceID(deviceID), partnerAlias_l(partnerAlias), partnerEventHandler(eventHandler), isNull(false), partnerEventsEnabled(false)
{
}

//For constucting the null partner
PartnerDevice::PartnerDevice() : 
partnerDeviceID("", "", 0), partnerAlias_l(""), isNull(true), partnerEventsEnabled(false)
{
}

const STI::Device::PartnerDevice_ptr& PartnerDevice::getNullPartner()
{
	return nullPartner;
}

const STI::Device::DeviceID& PartnerDevice::getDeviceID() const
{
	return partnerDeviceID;
}

void PartnerDevice::enablePartnerEvents()
{
	partnerEventsEnabled = true;
}

bool PartnerDevice::getPartnerEventsSetting() const
{
	return partnerEventsEnabled;
}


void PartnerDevice::event(double time, unsigned short channel, const STI::Utils::MixedValue& value, 
						  const RawEvent& referenceEvent, std::string description) throw(std::exception)
{
	if(!checkPartnerStatus(referenceEvent)) {
		return;
	}

	STI::TimingEngine::Channel chan(partnerDeviceID, channel, STI::TimingEngine::Output, "", STI::Utils::Unknown, STI::Utils::Unknown);

	TimingEvent_ptr evt(
		new STI::TimingEngine::LocalTimingEvent(time, chan, value, referenceEvent.eventNum(), referenceEvent.position(), description, false) );

	if(partnerEventHandler != 0) {
		partnerEventHandler->addEvent(evt);
	}
	else {
	//	TimingEvent_ptr timingEvt = referenceEvent.getTimingEvent();
		
		throw STI::TimingEngine::EventParsingException(referenceEvent.getTimingEvent(),
			"Error:  An event was requested on a partner device ('" + partnerAlias_l + "')\n"
			+ "but the PartnerEventHandler reference for this Device is Null.");
	}

}
bool PartnerDevice::checkPartnerStatus(const RawEvent& referenceEvent) throw(std::exception)
{
	//TimingEvent_ptr timingRefEvent;

	//referenceEvent.getTimingEvent();

	//if(!referenceEvent.getTimingEvent(timingRefEvent)) {
	//	throw STI::TimingEngine::EngineException(
	//		"An event was requested on partner '" + partnerAlias_l + "' but the RawEvent\n"
	//		+"was not properly created by the engine (TimingEvent_ptr was null!).\n");
	//	return false;
	//}

	return checkPartnerStatus(referenceEvent.getTimingEvent());
}

bool PartnerDevice::checkPartnerStatus(const TimingEvent_ptr& referenceEvent) throw(std::exception)
{
	if(isNull) {
		throw STI::TimingEngine::EventParsingException(referenceEvent,
			"An event was requested on partner alias '" + partnerAlias_l + "' but this partner\n"
			+"has not been defined. \n"
			+ "Partners must be defined inside ::definePartnerDevices() using the expression\n" 
			+ "    addPartnerDevice(\""  + partnerAlias_l + "\", <IP>, <Module>, <DeviceName>);\n" );
		return false;
	}

	if(partnerEventsEnabled) {
		throw STI::TimingEngine::EventParsingException(referenceEvent,
			"An event was requested on partner '" + partnerAlias_l + "' but partner events\n"
			+"are not enabled on this partner.  \n"
			+ "Partner events must first be enabled inside ::definePartnerDevices() using the expression\n" 
			+ "    partnerDevice("  + partnerAlias_l + ").enablePartnerEvents();\n" );
		return false;
	}

	return true;
}

void PartnerDevice::meas(double time, unsigned short channel, const STI::Utils::MixedValue& value, 
						 const RawEvent& referenceEvent, std::string description) throw(std::exception)
{
	if(!checkPartnerStatus(referenceEvent)) {
		return;
	}

	STI::TimingEngine::Channel chan(partnerDeviceID, channel, STI::TimingEngine::Input,  "", STI::Utils::Unknown, STI::Utils::Unknown);

	TimingEvent_ptr evt(
		new STI::TimingEngine::LocalTimingEvent(time, chan, value, referenceEvent.eventNum(), referenceEvent.position(), description, true) );

	if(partnerEventHandler != 0) {
		partnerEventHandler->addEvent(evt);
	}
}

void PartnerDevice::meas(double time, unsigned short channel, const STI::TimingEngine::RawEvent& referenceEvent, 
		  std::string description) throw(std::exception)
{
	meas(time, channel, STI::Utils::MixedValue(), referenceEvent, description);
}

