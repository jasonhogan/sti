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

#include "ORBManager.h"
#include "Attribute.h"
#include "RemoteDevice.h"

#include <string>
#include <sstream>
#include <map>

class Attribute;
class ORBManager;
class ServerConfigure_i;
class Control_i;
class ExpSequence_i;
class ModeHandler_i;
class Parser_i;
class ServerConfigure_i;
class DeviceConfigure_i;
class RemoteDevice;

typedef std::map<std::string, Attribute> attributeMap;

class STI_Server
{
public:

	STI_Server(ORBManager* orb_manager);
	STI_Server(std::string name, ORBManager* orb_manager);
	virtual ~STI_Server();

	void init();

	void setSeverName(std::string name);
	std::string serverName() const;
	
	void defineAttributes();
	attributeMap const * getAttributes();
//	bool setAttribute(std::string key, std::string value);
	
	std::string errorMsg();
	
	ORBManager* orbManager;
	std::map<std::string, RemoteDevice> registeredDevices;
	std::map<std::string, int> registeredDevices2;

	// servants
	Control_i* controlServant;
	ExpSequence_i* expSequenceServant;
	ModeHandler_i* modeHandlerServant;
	Parser_i* parserServant;
	ServerConfigure_i* serverConfigureServant;
	DeviceConfigure_i* deviceConfigureServant;

	bool registerDevice(const char* deviceName, 
								  STI_Server_Device::TDevice& device);


	bool activateDevice(const char* deviceID);
	bool removeDevice(const char* deviceID);

	bool deviceStatus(std::string deviceID);

protected:

	attributeMap attributes;	//server attributes
	std::stringstream errStream;

private:
	
	static void serverMainWrapper(void* object);
	bool serverMain();

	bool isUnique(std::string device_id);
	std::string removeForbiddenChars(std::string input);
	std::string serverName_l;

};

#endif
