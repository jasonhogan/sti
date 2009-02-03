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

#include <Attribute.h>
#include "RemoteDevice.h"

#include <string>
#include <sstream>
#include <map>
#include <boost/ptr_container/ptr_map.hpp>

class Attribute;
class ORBManager;
class ServerConfigure_i;
class Control_i;
class ExpSequence_i;
class ModeHandler_i;
class Parser_i;
class ServerConfigure_i;
class DeviceConfigure_i;
class StreamingDataTransfer_i;
class RemoteDevice;

typedef std::map<std::string, Attribute> AttributeMap;
typedef boost::ptr_map<std::string, RemoteDevice> RemoteDeviceMap;
typedef std::map<std::string, std::vector<STI_Server_Device::TDeviceEvent_var> > EventMap;

class STI_Server
{
public:

	STI_Server(ORBManager* orb_manager);
	STI_Server(std::string serverName, ORBManager* orb_manager);
	virtual ~STI_Server();

	virtual bool serverMain();
	virtual void defineAttributes();
	
	void transferEvents();
	void loadEvents();
	void playEvents();
	void stopAllDevices();
	bool eventsParsed();
	bool checkChannelAvailability(std::stringstream& message);
	void divideEventList();

	// Client control handling (ModeHandler)

	// STI_Device communication
	bool activateDevice(std::string deviceID);
	bool registerDevice(STI_Server_Device::TDevice& device);
	bool removeDevice(std::string deviceID);
	bool getDeviceStatus(std::string deviceID);
	std::string generateDeviceID(const STI_Server_Device::TDevice& device) const;
	void refreshDevices();
	void refreshPartnersDevices();

	// Server attributes
//	bool setAttribute(std::string key, std::string value);
	void setSeverName(std::string serverName);
	
	ORBManager* getORBManager() const;
	const AttributeMap& getAttributes() const;
	std::string getServerName() const;
	std::string getErrorMsg()const;
	std::string getTransferErrLog(std::string deviceID) const;

	RemoteDeviceMap registeredDevices;	// DeviceID => RemoteDevice

protected:

	// Servants
	Control_i* controlServant;
	ExpSequence_i* expSequenceServant;
	ModeHandler_i* modeHandlerServant;
	Parser_i* parserServant;
	ServerConfigure_i* serverConfigureServant;
	DeviceConfigure_i* deviceConfigureServant;
	StreamingDataTransfer_i* streamingDataTransferServant;

	// Containers
	EventMap events;
	AttributeMap attributes;	//server attributes
	
	bool isUnique(std::string deviceID);

private:

	void init();
	
	std::string removeForbiddenChars(std::string input) const;

	static void serverMainWrapper(void* object);
	static void transferEventsWrapper(void* object);

	ORBManager* orbManager;

	static bool eventTransferLock;
	std::string currentDevice;

	std::stringstream errStream;
	std::string serverName_;

	omni_mutex* refreshMutex;
};

#endif
