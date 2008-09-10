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

#include "STI_Server.h"
#include "ORBManager.h"
#include "Control_i.h"
#include "ExpSequence_i.h"
#include "ModeHandler_i.h"
#include "Parser_i.h"
#include "ServerConfigure_i.h"
#include "DeviceConfigure_i.h"
#include "StreamingDataTransfer_i.h"
#include "RemoteDevice.h"

#include <sstream>
#include <string>
#include <map>
using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;

typedef map<string, RemoteDevice> RemoteDeviceMap;


STI_Server::STI_Server(ORBManager* orb_manager) : 
orbManager(orb_manager)
{
	init();
}

STI_Server::STI_Server(std::string name, ORBManager* orb_manager) : 
orbManager(orb_manager), serverName_l(name)
{
	init();
}

STI_Server::~STI_Server()
{
	delete controlServant;
	delete expSequenceServant;
	delete modeHandlerServant;
	delete parserServant;
	delete serverConfigureServant;
	delete deviceConfigureServant;
	delete streamingDataTransferServant;
}

void STI_Server::init()
{
	//Servants
	controlServant = new Control_i();
	expSequenceServant = new ExpSequence_i();
	modeHandlerServant = new ModeHandler_i();
	parserServant = new Parser_i();
	serverConfigureServant = new ServerConfigure_i(this);
	deviceConfigureServant = new DeviceConfigure_i(this);
	streamingDataTransferServant = new StreamingDataTransfer_i(this);

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

	orbManager->registerServant(deviceConfigureServant, 
		"STI/Client/DeviceConfigure.Object");

	orbManager->registerServant(streamingDataTransferServant, 
		"STI/Client/StreamingDataTransfer.Object");

	registeredDevices.clear();
	       
	//server main loop
	omni_thread::create(serverMainWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}


void STI_Server::serverMainWrapper(void* object)
{
	STI_Server* thisObject = (STI_Server*) object;
	while(thisObject->serverMain()) {};
}


bool STI_Server::serverMain()
{
	cerr << "Server Main ready: " << endl;
	string x;
	cin >> x;

	registeredDevices.begin()->second.printChannels();


	registeredDevices.begin()->second.setAttribute("BiasVoltage",x);

	attributeMap const * test = registeredDevices.begin()->second.getAttributes();

	cerr << test->begin()->first << " = "<< test->begin()->second.value() << endl;
	test->begin()->second.printAllowedValues();

	string device1 = CORBA::string_dup((*deviceConfigureServant->devices())[0].deviceID);

	cerr << "Device: " << device1 << endl;
	cerr << "Device Ch: " << (*deviceConfigureServant->getDeviceChannels(device1.c_str()))[0].channel << endl;

	return true;
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

	return &attributes;
}


void STI_Server::defineAttributes()
{
}

bool STI_Server::activateDevice(const char* deviceID)
{

	bool found = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		it->second.activate();	//RemoteDevice::activate()
		found = true;
	}
	else
	{
		// Device not found in registeredDevices
		found = false;
	}
	return found;
}


bool STI_Server::removeDevice(const char* deviceID)
{
	bool removed = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		it->second.deactivate();	//RemoteDevice::deactivate()
		registeredDevices.erase(it);
		removed = true;
	}
	else
	{
		// Device not found in registeredDevices
		removed = true;
	}
	return removed;
}



string STI_Server::removeForbiddenChars(string input)
{
	string output = input;
	string::size_type loc = 0;

	// replace "." with "_"
	while(loc != string::npos)
	{
		loc = output.find(".", 0);
		if(loc != string::npos)
			output.replace(loc, 1, "_");
	}

	return output;
}

bool STI_Server::registerDevice(STI_Server_Device::TDevice& device)
{
	bool deviceRegistered = false;
	stringstream device_id;
	string deviceIDstring;
	string deviceContextString;

	// context example: STI/Device/192_54_22_1/module_1/DigitalOut/
	device_id << CORBA::string_dup(device.address) << "/" 
		<< "module_" << device.moduleNum << "/";
	
	deviceIDstring      = device_id.str().c_str();
	deviceContextString = removeForbiddenChars(deviceIDstring);
	deviceContextString.insert(0,"STI/Device/");


	if(isUnique(device_id.str()))
	{
		device.deviceContext = deviceContextString.c_str();
		device.deviceID      = deviceIDstring.c_str();

		registeredDevices[deviceIDstring] = RemoteDevice(orbManager, device);
		deviceRegistered = true;
	}
	else
	{
		// registration failed -- this deviceID is already registered
		deviceRegistered = false;

		// Check that this Device is still working and remove it if not
		deviceStatus(deviceIDstring);
	}

	cerr << "Registered Device ID: " << device_id.str() << " ok? " << deviceRegistered << endl;

	return deviceRegistered;
}


bool STI_Server::deviceStatus(string deviceID)
{
	bool deviceActive;

	if(isUnique(deviceID))
	{
		// device is not registered
		deviceActive = false;
	}
	else
	{
		// found deviceID
		// Check that this Device registered still has alive servants
		deviceActive = registeredDevices[deviceID].isActive();

		if(!deviceActive)
		{
			// Servants cannot be accessed -- this Device is not a working
			// and will be removed from the Server
			removeDevice(deviceID.c_str());
			cerr << "Removed: " << deviceID << endl;
		}
	}

	return deviceActive;
}


bool STI_Server::isUnique(string device_id)
{
	// Look for this device id string in the map of known RemoteDevices
	RemoteDeviceMap::iterator it = registeredDevices.find(device_id);

	if(it == registeredDevices.end())
		return true;	// not found

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
