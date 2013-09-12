#ifndef STI_SERVER_STI_SERVER_H
#define STI_SERVER_STI_SERVER_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "DeviceTimingEngineInterface.h"
#include "MixedValue.h"
#include "DeviceID.h"
#include "ServerInterface.h"

namespace STI
{
namespace Server
{

//class ServerLocalDevice;
//typedef boost::shared_ptr<ServerLocalDevice> ServerLocalDevice_ptr;

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

//class ServerLocalDevice : public STI::Device::DeviceTimingEngineInterface
//{
//public:
//	ServerLocalDevice(const STI::Device::DeviceID& id) : deviceID(id) {}	//
//
//	void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
//		STI::TimingEngine::SynchronousEventVector& eventsOut) throw(std::exception) {}	// Device-specific event parsing
//	const STI::TimingEngine::ChannelMap& getChannels() const { return channels; }
//	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget) {}
//	const STI::Device::DeviceID& getDeviceID() const { return deviceID; }
//	bool waitForTrigger(const STI::TimingEngine::MasterTrigger_ptr& masterTrigger) { return true; }
//
//private:
//	STI::Device::DeviceID deviceID;
//	STI::TimingEngine::ChannelMap channels;
//};


class STI_Server : public STI::Device::DeviceTimingEngineInterface, public STI::Server::ServerInterface
{
public:
//	STI_Server(const std::string& name, const std::string& address, unsigned short module);
	STI_Server(STI::Device::DeviceID& serverID);

	bool addDevice(const STI::Device::DeviceInterface_ptr& device);
	bool removeDevice(const STI::Device::DeviceID& deviceID);

	bool getDeviceEventHandler(STI::Server::DeviceEventHandler_ptr& handler);

	STI::TimingEngine::EventEngineManager_ptr localEngineManager;
	STI::TimingEngine::QueuedEventEngineManager_ptr queuedEngineManager;
	STI::Device::DeviceCollector_ptr registeredDevices;
	STI::Device::DeviceDistributer deviceDistributer;

	STI::Server::DeviceEventHandler_ptr deviceEventHandler;
	
	void initEngines();
	
	const STI::Device::DeviceID& getDeviceID() const { return serverID; }


private:




//	ServerLocalDevice localDevice;


	//**********DeviceTimingEngineInterface***********
public:

	void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut) throw(std::exception) {}	// Device-specific event parsing
	const STI::TimingEngine::ChannelMap& getChannels() const { return channels; }
	void setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& partnerEventTarget) {}
	bool waitForTrigger(const STI::TimingEngine::MasterTrigger_ptr& masterTrigger) { return true; }

private:
	STI::Device::DeviceID serverID;
	STI::TimingEngine::ChannelMap channels;

};





}
}

#endif

