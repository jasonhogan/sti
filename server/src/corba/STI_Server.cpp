/*! \file STI_Server.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class STI_Server
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
#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif

#include "STI_Server.h"
#include "ORBManager.h"
#include "Control_i.h"
#include "ExpSequence_i.h"
#include "ModeHandler_i.h"
#include "Parser_i.h"
#include "ServerConfigure_i.h"

#include <string>
#include <map>
using std::string;
using std::map;


#include <iostream>
using namespace std;

typedef map<string, RemoteDevice> RemoteDeviceMap;


STI_Server::STI_Server(ORBManager* orb_manager) : orbManager(orb_manager)
{
	init();
}

STI_Server::STI_Server(std::string name, ORBManager* orb_manager) : serverName_l(name), orbManager(orb_manager)
{
	init();
}

STI_Server::~STI_Server()
{
}

void STI_Server::init()
{
	attributes_ptr = &attributes;

	//Servants
	controlServant = new Control_i();
	expSequenceServant = new ExpSequence_i();
	modeHandlerServant = new ModeHandler_i();
	parserServant = new Parser_i();
	serverConfigureServant = new ServerConfigure_i(this);

	//Inter-servant communication
	parserServant->add_ModeHandler(modeHandlerServant);
	controlServant->add_Parser(parserServant);
	controlServant->add_ModeHandler(modeHandlerServant);
	controlServant->add_ExpSequence(expSequenceServant);

	//Register Servants
	orbManager->registerServant(controlServant, 
		"STI/Client/Control.Object");

	orbManager->registerServant(expSequenceServant, 
		"STI/Client/ExpSequence.Object");

	orbManager->registerServant(modeHandlerServant, 
		"STI/Client/ModeHandler.Object");

	orbManager->registerServant(parserServant, 
		"STI/Client/Parser.Object");
	
	orbManager->registerServant(serverConfigureServant, 
		"STI/Device/ServerConfigure.Object");

}

void STI_Server::setSeverName(std::string name)
{
	serverName_l = name;
}

std::string STI_Server::serverName() const
{
	return serverName_l;
}

std::string STI_Server::errorMsg()
{
	return errStream.str();
}

attributeMap const * STI_Server::getAttributes()
{
	// Initialize to defaults the first time this is called
	if(attributes.empty())
		defineAttributes();

	return attributes_ptr;
}

void STI_Server::defineAttributes()
{
}

//addRemoteDevice() should be merged with registerDevice()
STI_Server_Device::TDeviceID* 
STI_Server::addRemoteDevice(string								deviceName, 
							const STI_Server_Device::TDevice &	tDevice, 
							STI_Server_Device::TDeviceID* tDeviceID)
{
	string deviceIDstring = CORBA::string_dup(tDeviceID->deviceID);
	tDeviceID->registered = true;
	tDeviceID->deviceContext = 
		CORBA::string_dup(
		removeForbiddenChars(
		CORBA::string_dup(tDeviceID->deviceID)).c_str());

	// Add a new RemoteDevice to the list of registeredDevices

	registeredDevices[deviceIDstring] = RemoteDevice(orbManager, deviceName, tDevice, tDeviceID);

//	registeredDevices[CORBA::string_dup(tDeviceID.deviceID)] = 
//		RemoteDevice(orbManager, deviceName, tDevice, tDeviceID);

	cerr << "Registered: "<< CORBA::string_dup(tDeviceID->deviceID) << endl;

//	return registeredDevices[deviceIDstring].deviceID();

	return tDeviceID;
}

string STI_Server::removeForbiddenChars(string input)
{
	string output = input;
//	input.find()
	return output;
}



STI_Server_Device::TDeviceID* 
STI_Server::registerDevice(const char* deviceName, 
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
		return addRemoteDevice(deviceName, device, tDeviceID);
	}
	else
	{
		// FUTURE:  touch it to look if there still is a device with this ID
		// then unmount if the device is dead.
	}

	return tDeviceID;
}




bool STI_Server::isUnique(string device_id)
{
	RemoteDeviceMap& deviceMap = (registeredDevices);
	
	// Look for this device id string in the map of known RemoteDevices
	RemoteDeviceMap::iterator it = deviceMap.find(device_id);

	if(it == deviceMap.end())
		return true;	// not found


	cerr << "Not Unique!!" << endl;
	return false;
}





/*
bool STI_Server::setAttribute(string key, string value)
{
	// Initialize to defaults the first time this is called
	if(attributes.empty())
		defineAttributes();

	if(attributes.empty())
		return false;	//There are no defined attributes

	attributeMap::iterator attrib = attributes.find(key);

	if(attrib == attributes.end())
	{
		return false;	// Attribute not found
	}
	else
	{
		// set the attribute
		attrib->second.setValue(value);
		return true;
	}
}
*/