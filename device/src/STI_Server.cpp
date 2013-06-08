
#include "STI_Server.h"
#include "LocalEventEngineManager.h"
#include "QueuedEventEngineManager.h"
#include "EngineID.h"
#include "ServerEventEngine.h"
#include "DeviceEventEngine.h"
#include "LocalCollector.h"

using STI::Server::STI_Server;
using STI::TimingEngine::LocalEventEngineManager;
using STI::TimingEngine::QueuedEventEngineManager;
using STI::TimingEngine::QueuedEventEngineManager_ptr;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::EventEngine_ptr;
using STI::Device::DeviceCollector_ptr;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::ServerEventEngine;

STI_Server::STI_Server(STI::Device::DeviceID& serverID)
: localEngineManager(new LocalEventEngineManager()), localDevice(serverID), 
registeredDevices(new STI::Device::LocalDeviceCollector())
{
	unsigned threadPoolSize = 3;
	queuedEngineManager = QueuedEventEngineManager_ptr(
		new QueuedEventEngineManager(localEngineManager, threadPoolSize));

//	localDevice = ServerLocalDevice_ptr(new ServerLocalDevice(serverID) );

	initEngines();
}


void STI_Server::initEngines()
{
	//Local engine
	EventEngine_ptr localEngine = EventEngine_ptr(new DeviceEventEngine(localDevice));
	
	//add main engine for testing
	EngineID mainEngine(1, "Main");
	EventEngine_ptr engine = EventEngine_ptr(new ServerEventEngine(mainEngine, localEngine, registeredDevices));
	localEngineManager->addEventEngine(mainEngine, engine);
}