#ifndef STI_SERVER_STI_SERVER_H
#define STI_SERVER_STI_SERVER_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "DeviceTimingEngineInterface.h"
#include "MixedValue.h"
#include "DeviceID.h"

namespace STI
{
namespace Server
{

class ServerLocalDevice;
typedef boost::shared_ptr<ServerLocalDevice> ServerLocalDevice_ptr;

//class ServerLocalDevice :  public STI::Device::DeviceInterface
//{
//public:
//	
//	ServerLocalDevice(const STI::Device::DeviceID& id) : deviceID(id) {}	//
//
//	const STI::Device::DeviceID& getDeviceID() const { return deviceID; }
//	bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager) { return false; }
//	bool getPartnerCollector(STI::Device::DeviceCollector_ptr& deviceCollector) { return false;}
//
//	bool read(unsigned short channel, 
//		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut)  { return false; }
//	bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn) { return false; }
//
//private:
//	STI::Device::DeviceID deviceID;
//};

class ServerLocalDevice : public STI::Device::DeviceTimingEngineInterface
{
public:
	ServerLocalDevice(const STI::Device::DeviceID& id) : deviceID(id) {}	//

	void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut) throw(std::exception) {}	// Device-specific event parsing

	const STI::TimingEngine::ChannelMap& getChannels() const { return channels; }

	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget) {}

	const STI::Device::DeviceID& getDeviceID() const { return deviceID; }

	bool waitForTrigger(const STI::TimingEngine::MasterTrigger_ptr& masterTrigger) { return true; }

private:
	STI::Device::DeviceID deviceID;
	STI::TimingEngine::ChannelMap channels;
};


class STI_Server
{
public:
	STI_Server(STI::Device::DeviceID& serverID);

	STI::TimingEngine::QueuedEventEngineManager_ptr queuedEngineManager;
	STI::Device::DeviceCollector_ptr registeredDevices;
	STI::Device::DeviceDistributer deviceDistributer;

private:
	void initEngines();

	STI::TimingEngine::LocalEventEngineManager_ptr localEngineManager;

	ServerLocalDevice localDevice;

};





}
}

#endif

