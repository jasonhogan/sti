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
#include <ORBManager.h>
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
#include <set>

using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;

typedef map<string, RemoteDevice> RemoteDeviceMap;

bool STI_Server::eventTransferLock = false;

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
	controlServant = new Control_i(this);
	expSequenceServant = new ExpSequence_i();
	modeHandlerServant = new ModeHandler_i();
	parserServant = new Parser_i(this);
	serverConfigureServant = new ServerConfigure_i(this);
	deviceConfigureServant = new DeviceConfigure_i(this);
	streamingDataTransferServant = new StreamingDataTransfer_i(this);

	//Inter-servant communication
	parserServant->add_ExpSequence(expSequenceServant);
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

	//transferEvents
	eventTransferLock = false;

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
//	string x;
//	cin >> x;		//cin interferes with python initialization
	// python waits for cin to return before it initializes

//	system("pause");
//	transferEvents();



//	expSequenceServant->printExpSequence();


	//registeredDevices.begin()->second.printChannels();


	//registeredDevices.begin()->second.setAttribute("BiasVoltage",x);

	//attributeMap const * test = registeredDevices.begin()->second.getAttributes();

	//cerr << test->begin()->first << " = "<< test->begin()->second.value() << endl;
	//test->begin()->second.printAllowedValues();

	//string device1 = CORBA::string_dup((*deviceConfigureServant->devices())[0].deviceID);

	//cerr << "Device: " << device1 << endl;
	//cerr << "Device Ch: " << (*deviceConfigureServant->getDeviceChannels(device1.c_str()))[0].channel << endl;

	return false;
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

std::string STI_Server::generateDeviceID(const STI_Server_Device::TDevice& device) const
{
	stringstream device_id;

	// context example: STI/Device/192_54_22_1/module_1/DigitalOut/
	device_id << CORBA::string_dup(device.address) << "/" 
		<< "module_" << device.moduleNum << "/" << device.deviceName << "/";

	return device_id.str();
}

bool STI_Server::registerDevice(STI_Server_Device::TDevice& device)
{
	bool deviceRegistered = false;
	string deviceIDstring = generateDeviceID(device);

	string deviceContextString = removeForbiddenChars(deviceIDstring);
	deviceContextString.insert(0,"STI/Device/");


	if(isUnique(deviceIDstring))
	{
		device.deviceContext = deviceContextString.c_str();
		device.deviceID      = deviceIDstring.c_str();

		registeredDevices[deviceIDstring] = RemoteDevice(this, device);
		deviceRegistered = true;
	}
	else
	{
		// registration failed -- this deviceID is already registered
		deviceRegistered = false;

		// Check that this Device is still working and remove it if not
		deviceStatus(deviceIDstring);
	}

	cerr << "Registered Device ID: " << deviceIDstring << " ok? " << deviceRegistered << endl;

	return deviceRegistered;
}

void STI_Server::refreshDevices()
{
	//checks the status of all registered devices, automatically removing dead devices

	std::map<std::string, RemoteDevice>::iterator iter = registeredDevices.begin();

	while(iter != registeredDevices.end())
	{
		if( deviceStatus(iter->first) )
			iter++;		// device is active; go to next device
		else
			iter = registeredDevices.begin();	//removed a dead device; start over
	}
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


void STI_Server::refreshPartnersDevices()
{
	// first confirm that all registered devices are alive
	refreshDevices();
	
	cerr << "refreshPartnersDevices()" << endl;

	bool success = true;
	unsigned i;
	RemoteDeviceMap::iterator device, partner;

	for(device = registeredDevices.begin(); device != registeredDevices.end(); device++)
	{
		//refreshing registered device 'device'
		for(i = 0; i < device->second.getRequiredPartners().size(); i++)
		{
			// try to find this required partner in registeredDevices
			partner = registeredDevices.find( device->second.getRequiredPartners()[i] );
			
			if( partner	!= registeredDevices.end() )
			{
				//found this deviceID; (re-)registering this partner
				success &= device->second.registerPartner(partner->first, partner->second.CommandLineRef);
			}
			else
			{
				//not found; unregistering this partner
				success &= device->second.unregisterPartner( device->second.getRequiredPartners()[i] );
			}
		}
	}
	
	// Registration should only fail if a device has died; in this case
	// we should refresh again to eliminate the dead device.

	if( !success )
		refreshPartnersDevices();
}


void STI_Server::divideEventList()
{
	using STI_Client_Server::TEventSeq;
	using STI_Client_Server::TEventSeq_var;
	using STI_Server_Device::TDeviceEvent;
	using STI_Server_Device::TDeviceEvent_var;

	const STI_Client_Server::TEventSeq& parsedEvents = parserServant->getParsedEvents();
	STI_Client_Server::TChannelSeq& parsedChannels = parserServant->getParsedChannels();

	events.clear();
	string deviceID = "";
	unsigned short channel = 0;

	for(unsigned i = 0; i < parsedEvents.length(); i++)
	{
		if( parsedEvents[i].channel < parsedChannels.length() )
		{
			channel = parsedChannels[parsedEvents[i].channel].channel;
			deviceID = generateDeviceID(parsedChannels[parsedEvents[i].channel].device);
		}
		else
		{
			deviceID = "Unknown";
		}

		events[deviceID].push_back( new TDeviceEvent );

		events[deviceID].back()->channel = channel;
		events[deviceID].back()->time = parsedEvents[i].time;
		events[deviceID].back()->value = parsedEvents[i].value;
	}
}


void STI_Server::transferEventsWrapper(void* object)
{
	STI_Server* thisObject = (STI_Server*) object;
	
	// Make local copy of STI_Server::currentDevice (a deviceID)
	string threadDeviceInstance = thisObject->currentDevice;
	eventTransferLock = false;		//release lock

//	thisObject->registeredDevices[threadDeviceInstance].
//		transferEvents(thisObject->events[threadDeviceInstance]);
}

void STI_Server::transferEvents()
{
	RemoteDeviceMap::iterator iter;
	
	// Transfer events in parallel: spawn a new event transfer thread for each device
	for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)
	{
		while(eventTransferLock) {}		//spin lock while the new thead makes a local copy of currentDevice
		eventTransferLock = true;
		currentDevice = iter->first;		//deviceID

		omni_thread::create(transferEventsWrapper, (void*)this, omni_thread::PRIORITY_LOW);
	}
}

bool STI_Server::checkChannelAvailability(std::stringstream &message)
{
	bool missingChannels = false;

	const std::vector<STI_Server_Device::TDeviceChannel> *deviceChannels;
	std::vector<STI_Server_Device::TDeviceChannel>::const_iterator channelIter;

	//This channel list is the result of the python parsing.
	//It does not contain information about the channel type since
	//this comes from each device.  This information will be added now
	//if the channel is found on the server.
	STI_Client_Server::TChannelSeq &channels = parserServant->getParsedChannels();

	set<string> missingDevices;
	set<string>::iterator missingDevice;

	RemoteDeviceMap::iterator device;

	STI_Server_Device::TDevice tDevice;
	string deviceID;

	for(unsigned i = 0; i < channels.length(); i++)
	{
		tDevice.address    = channels[i].device.address;
		tDevice.moduleNum  = channels[i].device.moduleNum;
		tDevice.deviceName = channels[i].device.deviceName;

		deviceID = generateDeviceID(tDevice);
		device = registeredDevices.find( deviceID );
			
		if( device != registeredDevices.end() )		//found this device
		{
			deviceChannels = &(device->second.getChannels());	//pointer to this device's vector of channels

			//Find the channel
			for(channelIter = deviceChannels->begin(); 
				channelIter != deviceChannels->end(); channelIter++)
			{
				if( channelIter->channel == channels[i].channel )
					break;	//found
			}

			if(channelIter != deviceChannels->end())	//found this channel
			{
				channels[i].type = channelIter->type;
				channels[i].inputType = channelIter->inputType;
				channels[i].outputType = channelIter->outputType;
			}
			else
			{
				missingChannels = true;

				message << "Missing channel: " 
					<< channels[i].channel << " on dev("
					<< channels[i].device.deviceName << ", "
					<< channels[i].device.address << ", "
					<< channels[i].device.moduleNum << ")" << endl;
			}
		}
		else
		{
			missingChannels = true;

			missingDevice = missingDevices.find( deviceID );
			
			if( missingDevice == missingDevices.end() )	//only display the message once
			{
				message << "Missing device: dev(" 
					<< channels[i].device.deviceName << ", "
					<< channels[i].device.address << ", "
					<< channels[i].device.moduleNum << ")" << endl;

				missingDevices.insert(deviceID);
			}
		}
	}

	return missingChannels;
}

//check that all devices have parsed their events and are ready to proceed
bool STI_Server::eventsParsed()
{
	bool allParsed = true;
	RemoteDeviceMap::iterator iter;

	for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)
	{
		allParsed &= iter->second.eventsParsed();
	}

	return allParsed;
}

std::string STI_Server::getTransferErrLog(std::string deviceID)
{
	std::map<std::string, RemoteDevice>::iterator device;

	device = registeredDevices.find(deviceID);

	if(device == registeredDevices.end())	//not found
		return "";
	else
		return device->second.getTransferErrLog();
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
