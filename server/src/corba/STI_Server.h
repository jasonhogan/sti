/*! \file STI_Server.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_Server
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */


// Interface for server communication with STI_Device objects using CORBA

#ifndef STI_SERVER_H
#define STI_SERVER_H

#include "device.h"
#include "client.h"

#include <Attribute.h>
#include "RemoteDevice.h"
#include <CompositeEvent.h>
#include <ServerEventPusher_i.h>

#include <MixedValue.h>
#include <MixedData.h>

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <boost/ptr_container/ptr_map.hpp>

class Attribute;
class ORBManager;
class ServerConfigure_i;
class ServerTimingSeqControl_i;
class ExpSequence_i;
class ModeHandler_i;
class Parser_i;
class ServerConfigure_i;
class RegisteredDevices_i;
class ServerCommandLine_i;
class RemoteDevice;
class DocumentationSettings_i;
class ClientBootstrap_i;
class DeviceEventHandler_i;

typedef boost::ptr_map<std::string, RemoteDevice> RemoteDeviceMap;
typedef std::map<std::string, std::vector<CompositeEvent> > EventMap;

using STI::Pusher::EventsEmpty;
using STI::Pusher::PreparingEvents;
using STI::Pusher::EventsReady;
using STI::Pusher::RequestingPlay;
using STI::Pusher::PlayingEvents;
using STI::Pusher::Paused;
using STI::Pusher::Waiting;

namespace STI 
{
namespace Server
{


//Device-side
class Trigger	//abc
{
	virtual bool waitBeforePlayTrigger() = 0;
	virtual bool waitBeforeResumeTrigger() = 0;
};

class DefaultTrigger : public Trigger
{
	bool waitBeforePlayTrigger() { return true; }
	bool waitBeforeResumeTrigger() { return true; }
};

//could be nested in Trigger_Device class
class FPGATrigger : public Trigger
{
	bool waitBeforePlayTrigger()
	{
		while( triggerDevice->waitingForExternal() ) {}

		return true;
	}
	bool waitBeforeResumeTrigger() { return true; }
};



class ServerEventEngineManager
{
	
	void parse()
	{
		//parse

		//attempt to load; this may work even in other devices are playing
//		forall devices
		if(getDeviceEngineManager(deviceID, engineManager))
		{
			engineManager->lock(engineID);
		}

	}

	void playAll(EngineID engineID)
	{
		//lock and load...

		//Lock logic: 
		//Is the system playing right now? (are any of the needed devices playing right now?)
			//Yes: return, report error
			//No: Are any of the required DeviceEventEngineManagers locked on a different engineID?
				//No: Lock them all on this engineID and proceed to load
				//Yes: Are any of those devices playing?
					//No: Unlock them then lock them to this engineID and proceed to load
					//Yes: Probably single line timing file. Attempt Timed try lock. Are the locks aquired?
						//Yes: Proceed to load
						//No: return, report error (probably hung device)

		//load; engines only load if they are not loaded
		//
	}
}

//decided issues!
//1. only managers change the state machine of the engines; engines just store them
//2. SynchronousEvents should be tagged with the RawEvent of origin. This allows for error checking during slicing for conditional waits. Not mandatory (for backward compatibility and allowing for things like initialization events)
//3. collectData() must get called before load() because load() overwrittes measurements.
//4. collectData() should be distinct form saveData (which occurs during XML writting, perhaps much later). So
//the cameras will have to use some kind of NetworkFile structure to save to disk. Provide a hook to devices
//that saves file types to a particular location if desired, called at the time of saving.
//5. All measurement events get saved in the engine's buffer and tagged with the engineInstance
//6. Analog in has to save it's old 'value' field and reload into the register as they are saved (during collect measurementData()?); kind of a hack, but it's because we hack the memory req's in the FPGA this way
//7. Measurement system needs to use shared_ptr's

//device engines can call checkStop() at any point as an interrupt.  Also checkPause() ?
void checkStop()
{
	readLock;
	if(stopReceived)
		throw StopException();
}

class ServerEventEngine
{
	//map<EngineID, EventEngine>
	//access to registeredDevices list to access all the RemoteEventEngineManagers
	//needs to provide trylock access (with timeouts) for EventEngine access.


	//Documentation reqs:
	//raw event and channel list
	//set vars
	//overwritten variables
	//timing files	(first one is main file)
	//measurements
	//timestamp

};



//Another idea, for deployed mode "parsing", etc.  DeployedServer
//Run a lightweight deployed server on the etrax that can accept the parsed timing events from the real server.
//The events can be stored locally optionally.
//The deployed server can command a reparse of the events it has stored. It can also call load(), play() etc. 
//The Trigger can still be delegated in the normal way.
//The deployed server may connect to a deployed client that is actuated through the etrax SMAs, or maybe a command line client.
//Possibly would benefit from a nameserver running on the etrax.
//Deployed server basically has a ServerEventManager and a DeviceManager, but no parser or client communication infrastructure.

class STI_Server
{
public:

	STI_Server(ORBManager* orb_manager);
	~STI_Server();

	void init();
	void run();
	void reregisterActiveDevices();
	
	ServerEventEngineManager* serverEventEngineManager;
	
	//server plumbing
	controlServant->addEventEngineManager(serverEventEngine);	//consider abstracting controlServant functionality (ClientControl: play, pause, resume, stop)

	//Documentation
	DocumentationSettings_i* getDocumentationSettings() {return documentationSettingsServant;}


	//Client communication
	void sendMessageToClient(STI::Pusher::MessageType type, std::string message, bool clearFirst=false, unsigned int linesBack=0, unsigned int charsBack=0);
	template<class T> void sendEvent(const T& event) {
		localServerEventPusher->pushEvent(event);
	}
	RemoteDeviceMap& getRegisteredDevices() { return registeredDevices; }// DeviceID => RemoteDevice



private:
	ORBManager* orbManager;

public:



	STI::Types::TAttributeSeq* getDeviceAttributes(std::string deviceID);
	STI::Types::TChannelSeq* getDeviceChannels(std::string deviceID);
	STI::Types::TPartnerSeq* getDevicePartners(std::string deviceID);
	STI::Types::TLabeledData* getLabledData(std::string deviceID, std::string label);
	STI::Types::TDeviceSeq* getDevices();
	

	


	//Server Event Engine

	EventMap events;
	omni_mutex* serverStateMutex;
	omni_mutex* serverPauseMutex;
	omni_condition* serverPauseCondition;
	omni_mutex* collectMeasurementsMutex;
	omni_condition* collectMeasurementsCondition;

	bool collectingMeasurements;
	bool serverStopped;
	bool serverPaused;
	bool PausedByDevice;

	STI::Pusher::ServerState serverStatus;

	void updateState();
	bool changeStatus(STI::Pusher::ServerState newStatus);


	void resetDeviceEvents();
	bool setupEventsOnDevices();
	void transferEvents();
	void push_backEvent(std::string deviceID, double time, unsigned short channel, STI::Types::TValMixed value, const STI::Types::TEvent& originalTEvent, bool isMeasurement=false, std::string description="");
	std::string getTransferErrLog(std::string deviceID) const;
	bool eventsParsed();
	bool checkChannelAvailability(std::stringstream& message);
	void divideEventList();
	bool calculatePartnerDependencies(std::stringstream& message);
	void addDependentPartners(RemoteDevice& device, std::vector<std::string>& dependencies);
	void determineWhichDevicesHaveExplicitEvents();
	void determineWhichDevicesHaveEvents();
	bool hasEvents(std::string deviceID);
	
	void loadEvents();
	
	bool requestPlay(bool devicesOnly = false);
	void playEvents(bool playContinuous = false);
	void playEventsOnDevice(std::string deviceID);
	void playAllDevices();
	void waitForEventsToFinish();
	
	void stopAllDevices();
	void stopServer();
	void pauseAllDevices();
	void pauseServer(bool pausedByDevice);
	void unpauseServer(bool unpausedByDevice);
	void pauseAllDevicesExcept(std::string deviceID);	//pauses all devices except device deviceID
	bool isPausedByDevice() {return (PausedByDevice && serverPaused);}

	void collectMeasurementsLoop();
	void collectDeviceMeasurements();
	void waitForMeasurementCollection();

	std::string unpausedDeviceID;
	// Client control handling (ModeHandler)
private:
	std::vector<std::string> devicesWithEvents;	// DeviceID's of devices with events
	std::set<std::string> devicesWithExplicitEvents;	// deviceID's of devices that have explicit event(...) calls in the timing file and must be defined therein


public:
	// STI_Device communication

	omni_mutex* registeredDevicesMutex;
	omni_mutex* refreshMutex;

	bool registerDevice(STI::Types::TDevice& device, STI::Server_Device::DeviceBootstrap_ptr bootstrap);
	bool setChannels(std::string deviceID, const STI::Types::TDeviceChannelSeq& channels);
	bool removeDevice(std::string deviceID);
	bool getDeviceStatus(std::string deviceID);
	std::string generateDeviceID(const STI::Types::TDevice& device) const;
	void refreshDevices();
	void refreshPartnersDevices();
	std::string executeArgs(const char* deviceID, const char* args);
	bool writeChannelDevice(std::string deviceID, unsigned short channel, const MixedValue& value);
	bool readChannelDevice(std::string deviceID, unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	void getRegisteredPartners(std::string deviceID, std::vector<std::string>& partners);
	void getRequiredPartners(std::string deviceID, std::vector<std::string>& partners);
	void handleDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event);
	bool setDeviceChannelName(std::string deviceID, short channel, std::string name);
	bool setDeviceAttribute(std::string deviceID, std::string key, std::string value);
	void stopDevice(std::string deviceID);
	void killDevice(std::string deviceID);
	long devicePing(std::string deviceID);

	const std::vector<std::string>& getDevicesWithEvents() const { return devicesWithEvents; }	// DeviceID's of devices with events
	
	static void transferEventsWrapper(void* object);
	static void collectMeasurementsLoopWrapper(void* object);

	static bool eventTransferLock;
	std::string currentDevice;

private:
	RemoteDeviceMap registeredDevices;	// DeviceID => RemoteDevice

public:

	STI::Client_Server::ModeHandler_ptr getModeHandler();
	STI::Client_Server::Parser_ptr getParser();
    STI::Client_Server::ExpSequence_ptr getExpSequence();
    STI::Client_Server::ServerTimingSeqControl_ptr getServerTimingSeqControl();
    STI::Client_Server::RegisteredDevices_ptr getRegisteredDevicesRef();
    STI::Client_Server::ServerCommandLine_ptr getServerCommandLine();
	STI::Pusher::DeviceEventHandler_ptr getDeviceEventHandler();

	bool addNewClient(STI::Pusher::ServerEventHandler_ptr eventHandler);

protected:

	// Servants
	ClientBootstrap_i* clientBootstrapServant;
	ServerTimingSeqControl_i* controlServant;
	ExpSequence_i* expSequenceServant;
	ModeHandler_i* modeHandlerServant;
	Parser_i* parserServant;
	RegisteredDevices_i* deviceConfigureServant;
	ServerCommandLine_i* serverCommandLineServant;
	ServerEventPusher_i* localServerEventPusher;
	DocumentationSettings_i* documentationSettingsServant;

	ServerConfigure_i* serverConfigureServant;
	DeviceEventHandler_i* deviceEventHandlerServant;


};

}
}

#endif
