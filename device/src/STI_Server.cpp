
#include "STI_Server.h"
#include "LocalEventEngineManager.h"
#include "QueuedEventEngineManager.h"
#include "EngineID.h"
#include "ServerEventEngine.h"
#include "DeviceEventEngine.h"
#include "LocalCollector.h"
#include "DeviceInterface.h"
#include "DeviceEventHandler.h"



using STI::Server::STI_Server;
using STI::TimingEngine::LocalEventEngineManager;
using STI::TimingEngine::QueuedEventEngineManager;
using STI::TimingEngine::QueuedEventEngineManager_ptr;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EventEngine_ptr;
using STI::Device::DeviceCollector_ptr;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::ServerEventEngine;

//STI_Server::STI_Server(const std::string& name, const std::string& address, unsigned short module)
//: 


//STI_Server::STI_Server(STI::Device::DeviceID& serverID)
//: localEngineManager(new LocalEventEngineManager()), localDevice(serverID), 
//registeredDevices(new STI::Device::LocalDeviceCollector()), deviceDistributer(registeredDevices)
STI_Server::STI_Server(STI::Device::DeviceID& serverID) : 
localEngineManager(new LocalEventEngineManager()), 
registeredDevices(new STI::Device::LocalDeviceCollector()), 
deviceDistributer(registeredDevices),
serverID(serverID)
{
	unsigned threadPoolSize = 3;
	queuedEngineManager = QueuedEventEngineManager_ptr(
		new QueuedEventEngineManager(localEngineManager, threadPoolSize));

//	localDevice = ServerLocalDevice_ptr(new ServerLocalDevice(serverID) );

//	initEngines();
}


void STI_Server::initEngines()
{
	//Local engine
//	EventEngine_ptr localEngine = EventEngine_ptr(new DeviceEventEngine(localDevice));
	EventEngine_ptr localEngine = EventEngine_ptr(new DeviceEventEngine(*this));
	
	//add main engine for testing
	EngineID mainEngine(1, "Main");
	EventEngine_ptr engine = EventEngine_ptr(
		new ServerEventEngine(mainEngine, localEngine, registeredDevices, getDeviceID()) );

	localEngineManager->addEventEngine(mainEngine, engine);
}

bool STI_Server::addDevice(const STI::Device::DeviceInterface_ptr& device)
{
	if(device == 0)
		return false;

	bool added = deviceDistributer.addNode(device->getDeviceID(), device);

	STI::Device::DeviceCollector_ptr collector;
	if(device->getPartnerCollector(collector) && collector != 0)
		deviceDistributer.addCollector(device->getDeviceID(), collector);

	if(added)
		device->connect(*this);

	return added;

}

bool STI_Server::removeDevice(const STI::Device::DeviceID& deviceID)
{
	deviceDistributer.removeCollector(deviceID);
	deviceDistributer.removeNode(deviceID);

	return !registeredDevices->contains(deviceID);
}

bool STI_Server::getDeviceEventHandler(STI::Server::DeviceEventHandler_ptr& handler)
{
	handler = deviceEventHandler;

	return (handler != 0);
}


