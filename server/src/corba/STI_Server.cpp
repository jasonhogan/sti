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
#include "ServerCommandLine_i.h"
#include "RemoteDevice.h"
#include <utils.h>

#include <sstream>
#include <string>
#include <map>
#include <set>

using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;


bool STI_Server::eventTransferLock = false;

STI_Server::STI_Server(ORBManager* orb_manager) : 
orbManager(orb_manager)
{
	init();
}

STI_Server::STI_Server(std::string serverName, ORBManager* orb_manager) : 
orbManager(orb_manager), serverName_(serverName)
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
	delete serverCommandLineServant;
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
	serverCommandLineServant = new ServerCommandLine_i(this);

	refreshMutex = new omni_mutex();

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
	orbManager->registerServant(serverCommandLineServant, 
		"STI/Client/ServerCommandLine.Object");

	registeredDevices.clear();
	attributes.clear();
	devicesWithEvents.clear();

	defineAttributes();

	//transferEvents
	eventTransferLock = false;
	
	serverStopped = true;
	PausedByDevice = false;

	serverPauseMutex = new omni_mutex();
	serverPauseCondition = new omni_condition(serverPauseMutex);

	collectMeasurementsMutex = new omni_mutex();
	collectMeasurementsCondition = new omni_condition(collectMeasurementsMutex);

	serverStatus = EventsEmpty;
	changeStatus(EventsEmpty);

	//server main loop
	omni_thread::create(serverMainWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}


void STI_Server::serverMainWrapper(void* object)
{
	STI_Server* thisObject = static_cast<STI_Server*>(object);
	while(thisObject->serverMain()) {};
}


bool STI_Server::serverMain()
{
	cout << "STI Server ready: " << endl << endl;
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


void STI_Server::setSeverName(std::string serverName)
{
	serverName_ = serverName;
}


void STI_Server::defineAttributes()
{
}

bool STI_Server::activateDevice(string deviceID)
{
	bool success = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		success = (it->second)->activate();	//RemoteDevice::activate()
	}
	else
	{
		// Device not found in registeredDevices
		success = false;
	}
	return success;
}

bool STI_Server::registerDevice(STI::Types::TDevice& device)
{
	refreshDevices();

	bool deviceRegistered = false;
	string deviceIDstring = generateDeviceID(device);

	string deviceContextString = removeForbiddenChars(deviceIDstring);
	deviceContextString.insert(0,"STI/Device/");

	if( isUnique(deviceIDstring) )
	{
		device.deviceContext = deviceContextString.c_str();
		device.deviceID      = deviceIDstring.c_str();

		registeredDevices.insert( deviceIDstring, new RemoteDevice(this, device) );
		deviceRegistered = true;
	}
	else
	{
		// registration failed -- this deviceID is already registered
		deviceRegistered = false;

		// Check that this Device is still working and remove it if not
		getDeviceStatus(deviceIDstring);
	}

	cout << "Registered Device ID: " << deviceIDstring << " ok? " << deviceRegistered << endl;

	return deviceRegistered;
}


bool STI_Server::removeDevice(string deviceID)
{
	bool removed = false;
	
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it != registeredDevices.end())
	{
		(it->second)->deactivate();	//RemoteDevice::deactivate()
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


bool STI_Server::setChannels(std::string deviceID, const STI::Types::TDeviceChannelSeq& channels)
{
	bool success = true;
	unsigned i;

	RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);

	if( device != registeredDevices.end() )
	{
		for(i = 0; i < channels.length(); i++)
		{
			success &= (device->second)->addChannel( channels[i] );
		}
	}
	else
		success = false;		//deviceID not found

	return success;
}


bool STI_Server::getDeviceStatus(string deviceID)
{
	bool deviceActive = false;

	if( isUnique(deviceID) )
	{
		// device is not registered
		deviceActive = false;
	}
	else
	{
		// found deviceID

		// Check that this registered Device still has alive servants
		// or that it hasn't timed out.  (Devices get a brief timeout period
		// after initial registration but before activation when they are
		// considered active.)
		deviceActive = registeredDevices[deviceID].isActive();

		// Remove the device if it's not active
		if( !deviceActive )
		{
			// Servants cannot be accessed -- this Device either took too long
			// to activate or is not accessible anymore and will be removed 
			// from the Server.
			removeDevice(deviceID);
			cerr << "Removed: " << deviceID << endl;
		}
	}

	return deviceActive;
}


bool STI_Server::isUnique(string deviceID)
{
	// Look for this deviceID string in the map of known RemoteDevices
	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

	if(it == registeredDevices.end())
		return true;	// not found

	return false;
}


void STI_Server::refreshDevices()
{
	//checks the status of all registered devices, automatically removing dead devices
	
	serverStopped = false;

	refreshMutex->lock();
	{
		RemoteDeviceMap::iterator iter = registeredDevices.begin();

		while(iter != registeredDevices.end() && !serverStopped)
		{
			if( getDeviceStatus(iter->first) )
				iter++;		// device is active; go to next device
			else
				iter = registeredDevices.begin();	//removed a dead device; start over
		}
	}
	refreshMutex->unlock();
}


void STI_Server::refreshPartnersDevices()
{
	// first confirm that all registered devices are alive
	refreshDevices();

	serverStopped = false;

	bool success = true;
	unsigned i, j;
	RemoteDeviceMap::iterator device, partner;

	vector<string>* registerdPartners;

	refreshMutex->lock();
	{
		for(device = registeredDevices.begin(); device != registeredDevices.end(); device++)
		{
			//refreshing registered device 'device'

			//First look for any of this device's requiredPartners that might be on
			//the server in registeredDevices.
			for(i = 0; i < (device->second)->getRequiredPartners().size(); i++)
			{
				// try to find this requiredPartner in registeredDevices
				partner = registeredDevices.find( (device->second)->getRequiredPartners().at(i) );
			
				if( partner	!= registeredDevices.end() )
				{
					//found this deviceID; (re-)registering this partner
					success &= (device->second)->registerPartner(partner->first, (partner->second)->getCommandLineRef());
				}
				else
				{
					//not found in registeredDevices; unregistering this partner
					success &= (device->second)->unregisterPartner( (device->second)->getRequiredPartners().at(i) );
				}
			}
			
			// Now refresh the registeredPartnerDevices on this device
			registerdPartners = &( (device->second)->getRegisteredPartners() );

			for(j = 0; j < registerdPartners->size(); j++)
			{
				// try to find this partner in the server's registeredDevices
				partner = registeredDevices.find( registerdPartners->at(j) );
				
				if( partner	== registeredDevices.end() )
				{
					//not found on server; remove it
					success &= (device->second)->unregisterPartner( registerdPartners->at(j) );
				}
			}
		}
	}
	refreshMutex->unlock();
	// Registration should only fail if a device has died; in this case
	// we should refresh again to eliminate the dead device.

	if( !success && !serverStopped)
		refreshPartnersDevices();
}

bool STI_Server::sendMessageToClient(STI::Client_Server::Messenger_ptr clientCallback, std::string message)
{
	bool success = false;

	try {
		clientCallback->sendMessage( message.c_str() );
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cout << "Caught exception CORBA::" << ex._name() 
			<< " at STI_Server::sendMessageToClient()" << endl;
	}
	catch(CORBA::SystemException& ex) {	
		cout << "Caught exception CORBA::" << ex._name() 
			<< " at STI_Server::sendMessageToClient()" << endl;
	}
	return success;
}

bool STI_Server::hasEvents(std::string deviceID)
{
	return ( events.find( deviceID ) != events.end() );
}

bool STI_Server::calculatePartnerDependencies(std::stringstream& message)
{
	serverStopped = false;

	bool error = false;

	RemoteDeviceMap::iterator device, otherDevice, eventPartner;
	unsigned i,j;

	refreshMutex->lock();
	{
		for(device = registeredDevices.begin(); device != registeredDevices.end(); device++)
		{
			if(error)
				break;

			//check all the Event Partners for this device to see if they are present.
			for(i = 0; i < (device->second)->getEventPartners().size(); i++)
			{
				// try to find this eventPartner in the registeredDevices
				eventPartner = registeredDevices.find( (device->second)->getEventPartners().at(i) );

				if(eventPartner == registeredDevices.end())
				{
					//an event partner is not registered on the server
					if(hasEvents(device->first)) //only a problem if this device has events
					{
						message << "Missing event partner '" << (device->second)->getEventPartners().at(i) << "'" << endl;
						message << "that is required by: " << endl;
						message << "    " << device->second->printDeviceIndentiy() << endl;
						error = true;
					}
					else
					{
						error = false;
					}
					break;
				}
				


				//add this device to the eventPartner's list of dependencies (this device must parse first,
				//so the eventPartner depends on it)
				eventPartner->second->addPartnerDependency( device->first );

				//check for a circular partner dependency
				for(j = 0; j < eventPartner->second->getEventPartners().size(); j++)
				{
					if(device->first.compare( (eventPartner->second)->getEventPartners().at(i) ) == 0)
					{
						message << "Error: Circular partner dependency detected! " << endl;
						message << "    " << device->second->printDeviceIndentiy() << endl;
						message << "    " << eventPartner->second->printDeviceIndentiy() << endl;
						message << "These partner devices are BOTH allowed to generate partner " 
							<< "events on the opposite partner. " << endl 
							<< "This is forbidden to avoid a possible infinite loop." << endl;
						error = true;
						break;
					}
				}
				//If this device doesn't have any events, remove all it from dependent partner list. Otherwise, eventPartner will hang waiting for device to "finish".
				//Do this AFTER checking for circular dependencies because we want to enfore NO circular dependency rule in all situations.
				if( !hasEvents(device->first) )	
				{
					eventPartner->second->removePartnerDependency( device->first );
				}
			}
		}
	}
	refreshMutex->unlock();

	return error;
}

bool STI_Server::setupEventsOnDevices(STI::Client_Server::Messenger_ptr parserCallback)
{
	std::string eventPartnerDeviceID;
	STI::Types::TPartnerDeviceEventSeq_var partnerEvents;

	serverStopped = false;

	if( !changeStatus(PreparingEvents) )
		return true; //error

	unsigned i;
	bool error = false;
	std::stringstream errors, messenges;
//	RemoteDeviceMap::iterator device;

	resetDeviceEvents();
	
	errors.str("");

	sendMessageToClient( parserCallback, "Checking channels...\n" );
	if( checkChannelAvailability(errors) )
	{
		error = true;
		sendMessageToClient( parserCallback, errors.str().c_str() );
	}

	errors.str("");
	
	if( !error )
	{
		divideEventList();
	}

	if( !error && calculatePartnerDependencies(errors) )
	{
		error = true;
		sendMessageToClient( parserCallback, errors.str().c_str() );
	}

	if( !error )
	{
		sendMessageToClient( parserCallback, "Transferring events to devices...\n" );
		
		transferEvents();

		vector<string> devicesTransfering( devicesWithEvents );

		//Continuously poll all the devices, checking to see which are done transfering.
		while( devicesTransfering.size() > 0 && !serverStopped && !error )
		{
			for(i = 0; i < devicesTransfering.size() && !serverStopped && !error; i++)
			{
				if( registeredDevices[devicesTransfering.at(i)].finishedEventsTransferAttempt() )
				{
					messenges.str("");
					messenges << "    " << registeredDevices[devicesTransfering.at(i)].printDeviceIndentiy() << "...";
					sendMessageToClient( parserCallback, messenges.str() );

					//Send transfer status (or errors) to client
					if( registeredDevices[devicesTransfering.at(i)].eventsTransferSuccessful() )
					{
						sendMessageToClient( parserCallback, "success\n" );
					}
					else
					{
						error = true;
						messenges.str("");
						messenges << endl << registeredDevices[devicesTransfering.at(i)].getTransferErrLog() << endl;
						sendMessageToClient( parserCallback, messenges.str() );
					}
					//if this device has dependent devices, get partner events and add them to the transfer lists
					for(unsigned k = 0; k < registeredDevices[devicesTransfering.at(i)].getEventPartners().size(); k++)
					{
						eventPartnerDeviceID = registeredDevices[devicesTransfering.at(i)].getEventPartners().at(k);
						partnerEvents = registeredDevices[devicesTransfering.at(i)].getPartnerEvents(eventPartnerDeviceID);
						
						//add each event requested by this device to its Event Partner's event list and to the list of parsed events (for sending to the client)
						for(unsigned m = 0; m < partnerEvents->length(); m++)
						{
							push_backEvent(eventPartnerDeviceID, partnerEvents[m].time, partnerEvents[m].channel, partnerEvents[m].value, 
								events[devicesTransfering.at(i)].at( partnerEvents[m].eventNum ).getTEvent());
							
							//add to the list of parsed events that get passed to the client
							parserServant->addDeviceGeneratedEvent(partnerEvents[m], events[devicesTransfering.at(i)].at( partnerEvents[m].eventNum ).getTEvent(), 
								registeredDevices[eventPartnerDeviceID]);
						}
					}

					//remove the device that just finished transfering from the dependency lists of all other devices
					for(unsigned j = 0; j < devicesTransfering.size(); j++)
					{
						registeredDevices[devicesTransfering.at(j)].
							removePartnerDependency( devicesTransfering.at(i) );
					}

					//this device is done transfering; stop polling it
					devicesTransfering.erase(devicesTransfering.begin() + i);
					break;
				}
				else
				{
					registeredDevices[devicesTransfering.at(i)].checkDependencies();
				}
			}
		}

		
/*		
		for(i = 0; i < devicesWithEvents.size() && !serverStopped; i++)
		{

			messenges.str("");
			messenges << "    " << registeredDevices[devicesWithEvents.at(i)].printDeviceIndentiy() << "...";
			sendMessageToClient( parserCallback, messenges.str() );
			
			//spin lock waiting for each device to finish receiving events
			while( !registeredDevices[devicesWithEvents.at(i)].finishedEventsTransferAttempt() 
				&& !serverStopped) {}

			if( registeredDevices[devicesWithEvents.at(i)].eventsTransferSuccessful() )
			{
				sendMessageToClient( parserCallback, "success\n" );
			}
			else
			{
				error = true;
				messenges.str("");
				messenges << endl << registeredDevices[devicesWithEvents.at(i)].getTransferErrLog() << endl;
				sendMessageToClient( parserCallback, messenges.str() );
			}
		}
*/


	}

	if( !error )
	{
		sendMessageToClient(parserCallback, "Loading events on devices...\n");
		loadEvents();

//		for(device = registeredDevices.begin(); device != registeredDevices.end() && !serverStopped; device++)
		for(i = 0; i < devicesWithEvents.size() && !serverStopped && !serverStopped; i++)
		{
			messenges.str("");
			messenges << "    " << registeredDevices[devicesWithEvents.at(i)].printDeviceIndentiy() << "...";
			sendMessageToClient( parserCallback, messenges.str() );
			
			//spin lock waiting for each device to finish loading events
			while( !registeredDevices[devicesWithEvents.at(i)].eventsLoaded() && !serverStopped) {}
			sendMessageToClient( parserCallback, "done\n" );
		}
	}

	
	if( serverStopped )
	{
		sendMessageToClient( parserCallback, "\n**Parsing Interrupted**." );
		error = true;
	}
	else if( !error )
	{
		sendMessageToClient( parserCallback, "\nFinished. Ready to play." );
	}
	else
	{
		sendMessageToClient( parserCallback, "\nFinished. There are errors." );
	}

	if(error)
		changeStatus(EventsEmpty);
	else
		error = !changeStatus(EventsReady);

	return error;
}


void STI_Server::resetDeviceEvents()
{
	RemoteDeviceMap::iterator iter;
	
	for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)
	{
		iter->second->reset();
	}
}

void STI_Server::divideEventList()
{
	using STI::Types::TEventSeq;
	using STI::Types::TEventSeq_var;
	using STI::Types::TDeviceEvent;
	using STI::Types::TDeviceEvent_var;

	const STI::Types::TEventSeq& parsedEvents = parserServant->getParsedEvents();
	STI::Types::TChannelSeq& parsedChannels = parserServant->getParsedChannels();

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

		push_backEvent(deviceID, parsedEvents[i].time, channel, parsedEvents[i].value, parsedEvents[i]);
	
	//	events[deviceID].push_back( new TDeviceEvent );

	//	events[deviceID].back()->channel = channel;
	//	events[deviceID].back()->time = parsedEvents[i].time;
	//	events[deviceID].back()->value = parsedEvents[i].value;
	}
}

void STI_Server::push_backEvent(std::string deviceID, double time, unsigned short channel, STI::Types::TValMixed value, const STI::Types::TEvent& originalTEvent)
{
//	events[deviceID].push_back( new STI::Types::TDeviceEvent );
	events[deviceID].push_back( CompositeEvent(originalTEvent)  );

	events[deviceID].back().getTDeviceEvent().channel = channel;
	events[deviceID].back().getTDeviceEvent().time = time;
	events[deviceID].back().getTDeviceEvent().value = value;

//	STI::Types::TEvent newEvent;

//	CompositeEvent temp(new STI::Types::TDeviceEvent, newEvent);
//	compositeEvents.push_back( temp  );


}

void STI_Server::transferEventsWrapper(void* object)
{
	STI_Server* thisObject = static_cast<STI_Server*>(object);
	
	// Make local copy of STI_Server::currentDevice (a deviceID)
	string threadDeviceInstance = thisObject->currentDevice;
	eventTransferLock = false;		//release lock

	thisObject->registeredDevices[threadDeviceInstance].
		waitForDependencies();

	thisObject->registeredDevices[threadDeviceInstance].
		transferEvents(thisObject->events[threadDeviceInstance]);
}



void STI_Server::transferEvents()		//transfer events from the server to the devices
{
	unsigned i;

	serverStopped = false;

	RemoteDeviceMap::iterator iter;
	eventTransferLock = false;
	
	devicesWithEvents.clear();

	//determine which devices have events
	for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)
	{
		if( events.find( iter->first ) != events.end() ) 
		{
			//this device has events
			if( STI::Utils::isUniqueString(iter->first, devicesWithEvents) )
			{
				devicesWithEvents.push_back( iter->first );
			}

			//add all the dependent partners of this device
			for(i = 0; i < iter->second->getEventPartners().size(); i++)
			{
				if( STI::Utils::isUniqueString(iter->second->getEventPartners().at(i), devicesWithEvents) )
				{
					devicesWithEvents.push_back( iter->second->getEventPartners().at(i) );
				}
			}
		}
	}


	// Transfer events in parallel: make a new event transfer thread for each device that has events
//	for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)
	for(i = 0; i < devicesWithEvents.size() && !serverStopped; i++)
	{
		while(eventTransferLock && !serverStopped) {}		//spin lock while the new thead makes a local copy of currentDevice
		eventTransferLock = true;
		currentDevice = devicesWithEvents.at(i);		//deviceID

		omni_thread::create(transferEventsWrapper, (void*)this, omni_thread::PRIORITY_HIGH);

//		if( events.find(currentDevice) != events.end() ) //Only transfer to devices that have events
//		{
////			devicesWithEvents.push_back(currentDevice);
//		}
//		else
//		{
//			eventTransferLock = false;
//		}
	}
}

void STI_Server::loadEvents()
{
	serverStopped = false;
//	RemoteDeviceMap::iterator iter;
//	for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)

	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		cout << "loadEvents() " << devicesWithEvents.at(i) << endl;
		//(iter->second)->loadEvents();
		registeredDevices[devicesWithEvents.at(i)].loadEvents();
	}
}


bool STI_Server::requestPlay()
{
	if( !changeStatus(RequestingPlay) )
		return false;
	
	bool success = true;

	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		success &= registeredDevices[devicesWithEvents.at(i)].prepareToPlay();
	}

	return success;
}

void STI_Server::playEvents()
{
	if( !changeStatus(PlayingEvents) )
		return;

	serverStopped = false;
//	RemoteDeviceMap::iterator iter;
//	for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)
	
	playAllDevices();				//does not block; devices return promptly
	collectDeviceMeasurements();	//starts a new thread

	waitForEventsToFinish();		//blocks until all devices are done
	waitForMeasurementCollection();	//blocks until all measurements have been received

	if( !changeStatus(EventsReady) )
		changeStatus(EventsEmpty);
}


void STI_Server::collectDeviceMeasurements()
{
	collectMeasurementsMutex->lock();
	{
		collectingMeasurements = true;
	}
	collectMeasurementsMutex->unlock();

	omni_thread::create(collectMeasurementsLoopWrapper, (void*)this, omni_thread::PRIORITY_NORMAL);
}

void STI_Server::collectMeasurementsLoopWrapper(void* object)
{
	STI_Server* thisObject = static_cast<STI_Server*>(object);
	thisObject->collectMeasurementsLoop();
}

void STI_Server::waitForMeasurementCollection()
{
	collectMeasurementsMutex->lock();
	{
		if(collectingMeasurements)
			collectMeasurementsCondition->wait();
	}
	collectMeasurementsMutex->unlock();
}

void STI_Server::collectMeasurementsLoop()
{
	unsigned long secs, nsecs;
	unsigned long sleepTimeSeconds = 1;

	for(unsigned i = 0; i < devicesWithEvents.size(); i++)
	{
		registeredDevices[devicesWithEvents.at(i)].resetMeasurements();
	}

	bool measurementsRemaining = true;

	while(measurementsRemaining && !serverStopped)
	{
		measurementsRemaining = false;
		
		for(unsigned i = 0; i < devicesWithEvents.size(); i++)
		{
			if(registeredDevices[devicesWithEvents.at(i)].hasMeasurementsRemaining())
			{
				measurementsRemaining = true;
				registeredDevices[devicesWithEvents.at(i)].getNewMeasurementsFromServer();
			}
		}

		if( !measurementsRemaining )
			break;

		collectMeasurementsMutex->lock();
		{
			omni_thread::get_time(&secs, &nsecs, sleepTimeSeconds, 0);
			collectMeasurementsCondition->timedwait(secs, nsecs);
		}
		collectMeasurementsMutex->unlock();

		
	}

	collectMeasurementsMutex->lock();
	{
		collectingMeasurements = false;
		collectMeasurementsCondition->signal();
	}
	collectMeasurementsMutex->unlock();

}

void STI_Server::stopAllDevices()
{
	RemoteDeviceMap::iterator iter;
	for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)
	{
		(iter->second)->stop();
	}
}


void STI_Server::pauseAllDevices()
{
	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		//(iter->second)->loadEvents();
		registeredDevices[devicesWithEvents.at(i)].pause();
	}
}

void STI_Server::playAllDevices()
{
	unsigned i;
	
	//bool success = true;
	////first prepare all devices to play
	//for(i = 0; i < devicesWithEvents.size(); i++)
	//{
	//	success &= registeredDevices[devicesWithEvents.at(i)].prepareToPlay();
	//}

	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		registeredDevices[devicesWithEvents.at(i)].playEvents();
	}

	//if(success)
	//{
	//}
	//else
	//{
	//	stopServer();
	//	stopAllDevices();
	//}
}

void STI_Server::playEventsOnDevice(std::string deviceID)
{
	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		//(iter->second)->loadEvents();
		if(deviceID.compare(devicesWithEvents.at(i)) == 0)
			registeredDevices[devicesWithEvents.at(i)].playEvents();
	}
}


void STI_Server::pauseAllDevicesExcept(std::string deviceID)	//pauses all devices except device deviceID
{
	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		if( deviceID.compare(registeredDevices[devicesWithEvents.at(i)].getDevice().deviceID) != 0 )
			registeredDevices[devicesWithEvents.at(i)].pause();
	}

	unpausedDeviceID = deviceID;
}

void STI_Server::waitForEventsToFinish()
{
	//This is where the server's play thread spends all its time during a play sequence.
	//This occupies the server while events are playing so the client knows when the 
	//events are done.

	unsigned i;
	bool finished = false;
	
	while(!serverStopped && !finished) 
	{
		finished = true;
		for(i = 0; i < devicesWithEvents.size() && !serverStopped && !serverPaused; i++)
		{
			finished &= registeredDevices[devicesWithEvents.at(i)].eventsPlayed();
		}

		if(serverPaused)
		{
			serverPauseMutex->lock();
			{
				serverPauseCondition->wait();
			}
			serverPauseMutex->unlock();

			finished = false;
		}

	}
}

void STI_Server::stopServer()
{
	serverStopped = true;
	PausedByDevice = false;
	
	if( !changeStatus(EventsReady) )
		changeStatus(EventsEmpty);
}

void STI_Server::pauseServer(bool pausedByDevice)
{
	if(pausedByDevice)
		changeStatus(Waiting);
	else
		changeStatus(Paused);

	if( serverPaused && !pausedByDevice )
		pauseAllDevices();


	//if (!PausedByDevice)
	//	PausedByDevice = pausedByDevice;

	//changeStatus(Paused);
}


void STI_Server::unpauseServer(bool unpausedByDevice)
{

//	playAllDevices();	//unpause everything
	if( serverPaused )
	{
		if( !changeStatus(PlayingEvents) )
			if(!changeStatus(EventsReady) )
				changeStatus(EventsEmpty);
	
		if( !serverPaused )
			playAllDevices();

	}

	//if(PausedByDevice && unpausedByDevice)
	//{
	//	playAllDevices();	//unpause everything
	//	PausedByDevice = false;
	//	changeStatus(PlayingEvents);
	//}
	//else if(PausedByDevice && !unpausedByDevice)
	//{
	//	// the client paused while waiting for a pausing device to unpause the server
	//	playEventsOnDevice(unpausedDeviceID);	//unpause only the device that originally paused the server
	//	changeStatus(PlayingEvents);
	//	pauseServer(true);		//repause the server
	//}
	//else if(!PausedByDevice)
	//{
	//	playAllDevices();	//unpause everything
	//	changeStatus(PlayingEvents);
	//}
}


bool STI_Server::changeStatus(ServerStatus newStatus)
{
	bool allowedTransition = false;	

	switch(serverStatus) 
	{
	case EventsEmpty:
		allowedTransition = 
			(newStatus == PreparingEvents);
		break;
	case PreparingEvents:
		allowedTransition = 
			(newStatus == EventsEmpty) ||
			(newStatus == EventsReady);
		break;
	case EventsReady:
		allowedTransition =
			(newStatus == EventsEmpty) ||
			(newStatus == PreparingEvents) ||
			(newStatus == RequestingPlay);
		break;
	case RequestingPlay:
		allowedTransition =
			(newStatus == EventsReady) ||
			(newStatus == PlayingEvents);
		break;
	case PlayingEvents:
		allowedTransition = 
			(newStatus == Paused) ||
			(newStatus == EventsReady) ||
			(newStatus == Waiting) || 
			(newStatus == EventsEmpty);
		break;
	case Paused:
		allowedTransition = 
			(newStatus == PlayingEvents) ||
			(newStatus == EventsReady) || 
			(newStatus == Waiting) || 
			(newStatus == EventsEmpty);
		break;
	case Waiting:
		allowedTransition = 
			(newStatus == PlayingEvents) ||
			(newStatus == EventsReady) || 
			(newStatus == Paused) ||
			(newStatus == EventsEmpty);
		break;
	default:
		break;
	}

	allowedTransition |= (newStatus == serverStatus); //same state is allowed

	if(allowedTransition)
	{
		serverStatus = newStatus;
		updateState();
	}
	return allowedTransition;
}

void STI_Server::updateState()
{
	switch(serverStatus) 
	{
	case EventsEmpty:
		serverPaused = false;
		PausedByDevice = false;
		serverStopped = true;
		stopAllDevices();
		resetDeviceEvents();

		serverPauseMutex->lock();
		{
			serverPauseCondition->broadcast();
		}
		serverPauseMutex->unlock();
		break;
	case PreparingEvents:
		serverPaused = false;
		PausedByDevice = false;
		serverPauseMutex->lock();
		{
			serverPauseCondition->broadcast();
		}
		serverPauseMutex->unlock();
		break;
	case EventsReady:
		serverPaused = false;
		PausedByDevice = false;
		serverPauseMutex->lock();
		{
			serverPauseCondition->broadcast();
		}
		serverPauseMutex->unlock();
		break;
	case RequestingPlay:
		serverPaused = false;
		PausedByDevice = false;
		serverPauseMutex->lock();
		{
			serverPauseCondition->broadcast();
		}
		serverPauseMutex->unlock();
		break;
	case PlayingEvents:
		serverPaused = false;
		serverPauseMutex->lock();
		{
			serverPauseCondition->broadcast();
		}
		serverPauseMutex->unlock();
		break;
	case Paused:
		serverPaused = true;
		break;
	case Waiting:
		serverPaused = true;
		break;
	}
}




//void cancel() {eventTransferLock = false;...}

bool STI_Server::checkChannelAvailability(std::stringstream& message)
{
	bool missingChannels = false;

	const std::vector<STI::Types::TDeviceChannel> *deviceChannels;
	std::vector<STI::Types::TDeviceChannel>::const_iterator channelIter;

	//This channel list is the result of the python parsing.
	//It does not contain information about the channel type since
	//this comes from each device.  This information will be added now
	//if the channel is found on the server.
	STI::Types::TChannelSeq &channels = parserServant->getParsedChannels();

	set<string> missingDevices;
	set<string>::iterator missingDevice;

	RemoteDeviceMap::iterator device;

	STI::Types::TDevice tDevice;
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
			deviceChannels = &( (device->second)->getChannels());	//pointer to this device's vector of channels

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
//	RemoteDeviceMap::iterator iter;

//	for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)

	unsigned i;
	for(i = 0; i < devicesWithEvents.size(); i++)
	{
		allParsed &= registeredDevices[devicesWithEvents.at(i)].eventsParsed();
	}


	return allParsed;
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

std::string STI_Server::generateDeviceID(const STI::Types::TDevice& device) const
{
	stringstream device_id;

	// context example: STI/Device/192_54_22_1/module_1/DigitalOut/
	device_id << CORBA::string_dup(device.address) << "/" 
		<< "module_" << device.moduleNum << "/" << device.deviceName << "/";

	return device_id.str();
}

string STI_Server::removeForbiddenChars(string input) const
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

ORBManager* STI_Server::getORBManager() const
{
	return orbManager;
}

std::string STI_Server::getServerName() const
{
	return serverName_;
}

std::string STI_Server::getTransferErrLog(std::string deviceID) const
{
	RemoteDeviceMap::const_iterator device;

	device = registeredDevices.find(deviceID);

	if(device == registeredDevices.end())	//not found
		return "";
	else
		return (device->second)->getTransferErrLog();
}

std::string STI_Server::getErrorMsg() const
{
	return errStream.str();
}

const AttributeMap& STI_Server::getAttributes() const
{
	return attributes;
}

std::string STI_Server::executeArgs(const char* deviceID, const char* args)
{
	string device_id(deviceID);
	RemoteDeviceMap::iterator device = registeredDevices.find(device_id);

	bool notFound = (device == registeredDevices.end());

	if( notFound )	//not found
	{
		refreshDevices();

		//now try again...

		device = registeredDevices.find(device_id);
		notFound = (device == registeredDevices.end());
	}

	if( notFound )	//still not found
		return "";
	else
		return (device->second)->execute(args);
}

const std::vector<std::string>& STI_Server::getRequiredPartners(std::string deviceID)
{
	RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);

	bool notFound = (device == registeredDevices.end());

	if( notFound )	//not found
	{
		refreshDevices();

		//now try again...

		device = registeredDevices.find(deviceID);
		notFound = (device == registeredDevices.end());
	}

	if( notFound )	//still not found
		return emptyPartnerList;
	else
		return (device->second)->getRequiredPartners();
}

const std::vector<std::string>& STI_Server::getRegisteredPartners(std::string deviceID)
{
	RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);

	bool notFound = (device == registeredDevices.end());

	if( notFound )	//not found
	{
		refreshDevices();

		//now try again...

		device = registeredDevices.find(deviceID);
		notFound = (device == registeredDevices.end());
	}

	if( notFound )	//still not found
		return emptyPartnerList;
	else
		return (device->second)->getRegisteredPartners();
}

