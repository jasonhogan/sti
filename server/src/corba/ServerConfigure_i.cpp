/*! \file ServerConfigure_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ServerConfigure_i
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

#include "device.h"
#include "ServerConfigure_i.h"
#include "STI_Server.h"
#include "RemoteDevice.h"

#include <sstream>
#include <string>
#include <map>
using std::map;
using std::stringstream;
using std::string;

#include <iostream>
using namespace std;

typedef map<string, RemoteDevice> RemoteDeviceMap;


ServerConfigure_i::ServerConfigure_i(STI_Server* server) : sti_Server(server)
{
	attributeSeq = NULL;
}

ServerConfigure_i::~ServerConfigure_i()
{
}

STI_Server_Device::TDeviceID* 
ServerConfigure_i::registerDevice(const char* deviceName, 
								  const STI_Server_Device::TDevice& device)
{
	using STI_Server_Device::TDeviceID;

	TDeviceID* tDeviceID = new TDeviceID;
	stringstream device_id;

	// context example: STI/Device/192_54_22_1/module_1/DigitalOut/
	device_id << CORBA::string_dup(device.address) << "/" 
		<< "module_" << device.moduleNum << "/" << deviceName << "/";

	
	tDeviceID->deviceID = device_id.str().c_str();
	tDeviceID->registered = false;

	if(isUnique(device_id.str()))
	{
		addRemoteDevice(deviceName, device, *tDeviceID);
	}
	else
	{
		// FUTURE:  touch it to look if there still is a device with this ID
		// then unmount if the device is dead.
	}

	return tDeviceID;
}

void ServerConfigure_i::addRemoteDevice(string deviceName, 
										const STI_Server_Device::TDevice& device, 
										STI_Server_Device::TDeviceID& tDeviceID)
{
	tDeviceID.registered = true;
	tDeviceID.deviceContext = 
		CORBA::string_dup(
		removeForbiddenChars(
		CORBA::string_dup(tDeviceID.deviceID)).c_str());

	// Add a new RemoteDevice to the list of registeredDevices
	typedef map<string, RemoteDevice> RemoteDeviceMap;
	RemoteDeviceMap& deviceMap = (sti_Server->registeredDevices);
	deviceMap[CORBA::string_dup(tDeviceID.deviceID)] = 
		RemoteDevice(sti_Server->orbManager, deviceName, device, tDeviceID);

	cerr << "Registered: "<< CORBA::string_dup(tDeviceID.deviceID) << endl;
}


string ServerConfigure_i::removeForbiddenChars(string input)
{
	string output = input;
//	input.find()
	return output;
}


bool ServerConfigure_i::isUnique(string device_id)
{
	RemoteDeviceMap& deviceMap = (sti_Server->registeredDevices);
	
	// Look for this device id string in the map of known RemoteDevices
	RemoteDeviceMap::iterator it = deviceMap.find(device_id);

	if(it == deviceMap.end())
		return true;	// not found


	cerr << "Not Unique!!" << endl;
	return false;
}


::CORBA::Boolean 
ServerConfigure_i::setChannels(const char* deviceID, 
							   const STI_Server_Device::TDeviceChannelSeq& channels)
{
	return true;
}


::CORBA::Boolean ServerConfigure_i::mountDevice(const char* deviceID)
{
	// Look for this device id string in the map of known RemoteDevices
	RemoteDeviceMap& deviceMap = (sti_Server->registeredDevices);
	RemoteDeviceMap::iterator it = deviceMap.find(deviceID);

	if(it != deviceMap.end())
	{
		it->second.mount();
		return true;
	}
	else
	{
		// Device not found in registeredDevices
		return false;
	}
}

::CORBA::Boolean ServerConfigure_i::unmountDevice(const char* deviceID)
{
	return true;
}

STI_Server_Device::TAttributeSeq* ServerConfigure_i::attributes()
{
	STI_Server_Device::TAttributeSeq* dummy = 0;
	return dummy;
}

char* ServerConfigure_i::serverName()
{
	return CORBA::string_dup(sti_Server->serverName().c_str());
}


/*
::CORBA::Boolean ServerConfigure_i::setAttribute(const char* deviceID, 
const char* key, const char* value)
{
	return sti_Server->setAttribute(deviceID, key, value);
}

char* ServerConfigure_i::getAttribute(const char* deviceID, const char* key)
{
	return CORBA::string_dup(
		sti_Server->getAttributes()->find(key)->second.value().c_str());
}
*/
