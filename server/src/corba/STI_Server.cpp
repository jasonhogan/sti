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
	delete controlServant;
	delete expSequenceServant;
	delete modeHandlerServant;
	delete parserServant;
	delete serverConfigureServant;

	delete nullDeviceID;
}

void STI_Server::init()
{
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

	nullDeviceID = new STI_Server_Device::TDeviceID;
	nullDeviceID->deviceID =		"NULL";
	nullDeviceID->deviceContext =	"NULL";
	nullDeviceID->registered =		false;

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
	int x;
	cin >> x;

	registeredDevices.begin()->second.printChannels();

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
	bool Mounted = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		it->second.activate();
		Mounted = true;
	}
	else
	{
		// Device not found in registeredDevices
		Mounted = false;
	}
	return Mounted;
}


bool STI_Server::removeDevice(const char* deviceID)
{
	bool unmounted = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		it->second.deactivate();
		registeredDevices.erase(it);
		unmounted = true;
	}
	else
	{
		// Device not found in registeredDevices
		unmounted = true;
	}
	return unmounted;
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


//This function really needs to be cleaned up.  Also, the RemoteDevice
//constructor can be simplified by just sending the device_id string
STI_Server_Device::TDeviceID* 
STI_Server::registerDevice(const char* deviceName, 
								  const STI_Server_Device::TDevice& device)
{
	using STI_Server_Device::TDeviceID;

	TDeviceID* tDeviceID = 0;
	TDeviceID tDeviceIDtemp;	//This is silly; just send the device_id!!

	stringstream device_id;

	// context example: STI/Device/192_54_22_1/module_1/DigitalOut/
	device_id << CORBA::string_dup(device.address) << "/" 
		<< "module_" << device.moduleNum << "/" << deviceName << "/";
	
	string deviceIDstring = device_id.str().c_str();

	cerr << "*** Device ID: " << device_id.str() << endl;

	tDeviceIDtemp.registered = true;
	tDeviceIDtemp.deviceContext = removeForbiddenChars(deviceIDstring).c_str();
	tDeviceIDtemp.deviceID = deviceIDstring.c_str();

	cerr << "**** Device Context: " << CORBA::string_dup(tDeviceIDtemp.deviceContext) << endl;

	if(isUnique(device_id.str()))
	{
		registeredDevices[deviceIDstring] = RemoteDevice(orbManager, deviceName, device, tDeviceIDtemp);
		tDeviceID = registeredDevices[deviceIDstring].deviceID();
	}
	else
	{
		// registration failed -- this device is already registered
		tDeviceID = nullDeviceID;

		//check that the Device registered with this ID still has alive servants
		if( !registeredDevices[deviceIDstring].isActive()) 
		{
			//servants cannot be accessed -- this Devive is not a working
			//and will be removed from the Server
			removeDevice(deviceIDstring.c_str());
		}
	}

	return tDeviceID;
}


bool STI_Server::isUnique(string device_id)
{
	// Look for this device id string in the map of known RemoteDevices
	RemoteDeviceMap::iterator it = registeredDevices.find(device_id);

	if(it == registeredDevices.end())
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
