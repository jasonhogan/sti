
#ifndef STI_DEVICE_STI_DEVICE_H
#define STI_DEVICE_STI_DEVICE_H

#include "TimingEngineTypes.h"

#include "DeviceTimingEngineInterface.h"
#include "DeviceID.h"
#include "EngineID.h"
#include "LocalEventEngineManager.h"
#include "PartnerDevice.h"

#include "Channel.h"
#include "MixedValue.h"
#include <string>

#include "DeviceInterface.h"
#include "TimingEventGroup.h"


#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>

#include "ConfigFile.h"

namespace STI
{
namespace Device
{

class STI_Device : public DeviceTimingEngineInterface, public DeviceInterface
{

public:

//	STI_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);
//	STI_Device(ORBManager* orb_manager, std::string DeviceName, 
///		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".");
	
	STI_Device(const std::string& deviceName, const std::string& deviceConfigFilename);
	STI_Device(const std::string& deviceConfigFilename);
	STI_Device(const STI::Utils::ConfigFile& deviceConfigFile);
	STI_Device(const std::string& DeviceName, const std::string& IPAddress, unsigned short ModuleNumber);
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

	//virtual void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
	//	STI::TimingEngine::SynchronousEventVector& eventsOut) throw(std::exception) = 0;	// Device-specific event parsing

	virtual bool deviceMain(int argc, char* argv[]) = 0;		// Device main()
	virtual std::string execute(int argc, char* argv[]) = 0;	// Device Command line interface


public:

	//**********DeviceInterface Implementation*******************//

	void connect(const STI::Server::ServerInterface& serverRef);
	void start() { initializeDevice(); }

	bool read(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut);
	bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn);

	bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager);
	bool getEventPartners(STI::Device::DeviceIDSet_ptr& eventPartners) { eventPartners = requiredEventPartners; return (eventPartners != 0); }
	bool getPartnerCollector(STI::Device::DeviceCollector_ptr& deviceCollector) { deviceCollector = partnerCollector; return (deviceCollector != 0); }


	//***********DeviceTimingEngineInterface Implementation******//
	
	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget);

	const STI::TimingEngine::ChannelMap& getChannels() const;
	const DeviceID& getDeviceID() const;
	
	virtual bool waitForTrigger(const STI::TimingEngine::MasterTrigger_ptr& masterTrigger);

public:
//protected:
	bool readChannelDefault(unsigned short channel, const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut, double minimumStartTime_ns=10000);
	bool writeChannelDefault(unsigned short channel, const STI::Utils::MixedValue& commandIn, double minimumStartTime_ns=10000);
	virtual bool playSingleEventDefault(const STI::TimingEngine::TimingEvent_ptr& event, STI::TimingEngine::TimingMeasurement_ptr& measurement);

protected:

	//***************Device setup helper functions******************//

	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType);
	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, 
		STI::Utils::MixedValueType OutputType, std::string defaultName = "");
	void addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, std::string defaultName);
	
	void addOutputChannel(unsigned short channel, STI::Utils::MixedValueType outputType, std::string defaultName = "");
	
	bool addChannel(unsigned short channel, STI::TimingEngine::ChannelType type, 
                    STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType 
					outputType, std::string defaultName);
	
	bool addPartnerDevice(std::string partnerAlias, std::string IP, short module, std::string deviceName);
	
	void parseDeviceEventsDefault(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut);
	
	STI::Device::PartnerDevice& partnerDevice(std::string partnerAlias);	//usage: partnerDevice("lock").execute("--e1");

	//Attributes
	template<class T>
	void addAttribute(const std::string& key, T initialValue, const std::string& allowedValues = "")
	{
		addAttribute(key, STI::Utils::valueToString(initialValue), allowedValues);
	}
	void addAttribute(const std::string& key, const std::string& initialValue, const std::string& allowedValues = "");

	template<class T> 
	bool setAttribute(const std::string& key, T value)
	{ 
		return setAttribute(key, STI::Utils::valueToString(value));
	}
	bool setAttribute(const std::string& key, const std::string& value);
	bool getAttribute(const std::string& key, std::string& value) const;
	void refreshDeviceAttributes();

	
	friend class AttributeUpdater;
	
	void addAttributeUpdater(const AttributeUpdater_ptr& updater);

	virtual void makeNewDeviceEventEngine(const STI::TimingEngine::EngineID& engineID, STI::TimingEngine::EventEngine_ptr& newEngine);


private:

	//***************Device initialization******************//

	void init();
	bool initializeUsingConfigFile(const STI::Utils::ConfigFile& deviceConfigFile, bool nameInitialized);

	void initializeDevice();	
	void initializeChannels();
	void initializePartnerDevices();
//	void initializeAttributes();

	bool requiredPartnersRegistered();
	void checkForNewPartners();
	void waitForRequiredPartners();

private:

	class DefaultPartnerPolicy : public STI::Utils::SynchronizedMapPolicy<STI::Device::DeviceID>
	{
	public:
		DefaultPartnerPolicy(STI_Device* device) : device(device) {}
		bool include(const STI::Device::DeviceID& key) const { return device->includePartner(key); }
		bool replace(const STI::Device::DeviceID& oldKey, const STI::Device::DeviceID& newKey) const { return true; }
		STI_Device* device;
	};
	typedef boost::shared_ptr<DefaultPartnerPolicy> DefaultPartnerPolicy_ptr;

	bool includePartner(STI::Device::DeviceID deviceID);


protected:

	STI::TimingEngine::LocalEventEngineManager_ptr eventEngineManager;

private:

	DeviceID deviceID;

	STI::TimingEngine::EngineID localServerEngineID;

	const STI::Server::ServerInterface* server;

	STI::Device::AttributeMap attributes;
	STI::Device::AttributeUpdaterVector attributeUpdaters;

	STI::TimingEngine::ChannelMap channels;
	STI::TimingEngine::QueuedEventEngineManager_ptr queuedEventEngineManager;

	STI::TimingEngine::PartnerEventHandler_ptr partnerEventHandler;
//	STI::TimingEngine::PartnerEventTarget_ptr partnerEventTarget;

	STI::Device::DeviceCollector_ptr partnerCollector;
	STI::Device::PartnerDeviceMap partnerDevices;
	STI::Device::DeviceIDSet_ptr requiredEventPartners;
	STI::Device::DeviceIDSet_ptr partnerIDs;

	bool channelsInitialized;
	bool attributesInitialized;
	bool partnerDevicesInitialized;

	bool usingDefaultEventParsing;

	mutable boost::shared_mutex initializationMutex;
	boost::condition_variable_any initializationCondition;


};

}
}

#endif
