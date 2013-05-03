
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
	void init();

public:

	//DeviceTimingEngineInterface Partial Implementation
	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget);

	const STI::TimingEngine::ChannelMap& getChannels() const;
	const DeviceID& getDeviceID() const;

private:

	DeviceID deviceID;

	STI::TimingEngine::ChannelMap channels;
	STI::TimingEngine::LocalEventEngineManager eventEngineManager;
	STI::TimingEngine::PartnerEventTarget_ptr partnerEventTarget;

protected:
	bool usingDefaultEventParsing;
	void parseDeviceEventsDefault(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut);
	
	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType);
	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, 
		STI::Utils::MixedValueType OutputType, std::string defaultName = "");
	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, std::string defaultName);
	void addOutputChannel(unsigned short channel, STI::Utils::MixedValueType outputType, std::string defaultName = "");
	bool addChannel(unsigned short channel, STI::TimingEngine::ChannelType type, 
                    STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType 
					outputType, std::string defaultName);


public:
//	bool read(const TimingEvent_ptr& measurementEvent);
//	bool write(const TimingEvent_ptr& event);

	bool read(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut);
	bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn);

	STI::TimingEngine::LocalEventEngineManager& getEventEngineManager() { return eventEngineManager; }

	void start()
	{
		defineChannels();
	}

};

}
}

#endif
