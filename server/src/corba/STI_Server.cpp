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
#define _CRTDBG_MAP_ALLOC



#include "STI_Server.h"
#include <ORBManager.h>
#include "ServerTimingSeqControl_i.h"
#include "ExpSequence_i.h"
#include "ModeHandler_i.h"
#include "Parser_i.h"
#include "ServerConfigure_i.h"
#include "RegisteredDevices_i.h"
#include "ServerCommandLine_i.h"
#include "RemoteDevice.h"
#include "DocumentationSettings_i.h"
#include <ClientBootstrap_i.h>
#include <ServerEventPusher_i.h>
#include <DeviceEventHandler_i.h>
#include <utils.h>

#include <COSBindingNode.h>

#include <sstream>
#include <string>
#include <map>
#include <set>

using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;

#include <stdlib.h>
#include <crtdbg.h>



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
	delete serverCommandLineServant;
	delete documentationSettingsServant;
}

//*********** Server setup functions ****************//


void STI_Server::reregisterActiveDevices()
{
	cout << "Scanning for active devices..." << endl << endl;

	CosNaming::NamingContext_var namingContext( orbManager->getNamingContext("STI/Device") );
	COSBindingNode devicesNode("Device", namingContext);

	cout << "Current binding tree:" << endl;
	devicesNode.printTree();
	cout << "----------------------------" << endl;
	//cout << "Pruning tree:" << endl;
	//devicesNode.prune();
	
	
	unsigned i,j,k,m;

	std::stringstream deviceContext;

	bool active;
	STI::Server_Device::DeviceBootstrap_ptr BootstrapRef;
	CORBA::Object_var obj;

	std::string ipAddress, module, deviceName;

	for(i = 0; i < devicesNode.branches(); i++)
	{
		if(devicesNode[i].hasBranches())
		{
			//found an IP address
			ipAddress = devicesNode[i].getName();
			for(j = 0; j < devicesNode[i].branches(); j++)
			{
				//found a module
				module = devicesNode[i][j].getName();
				for(k = 0; k < devicesNode[i][j].branches(); k++)
				{
					//found a device name
					deviceName = devicesNode[i][j][k].getName();
					for(m = 0; m < devicesNode[i][j][k].branches(); m++)
					{
						//probably a DeviceBootstrap
						if(devicesNode[i][j][k][m].getName().compare("DeviceBootstrap.Object") == 0)
						{
							deviceContext.str("");
							deviceContext << "STI/Device/" 
								<< ipAddress << "/" 
								<< module << "/" 
								<< deviceName 
								<< "/DeviceBootstrap.Object";

//							cout << devicesNode[i][j][k][m].getName() << " " << devicesNode[i][j][k][m].isDead() << endl;
							if( !devicesNode[i][j][k][m].isDead() )
							{
								obj = orbManager->getObjectReference(deviceContext.str());
								BootstrapRef = STI::Server_Device::DeviceBootstrap::_narrow(obj);

								active = false;

								try {
									active = BootstrapRef->ping();
									if(active)
									{
										BootstrapRef->getDeviceConfigure()->reRegisterWithServer();
									}
								} catch(CORBA::TRANSIENT&) {
								}
								catch(CORBA::SystemException&) {
								}
								catch(CORBA::Exception&) {
								}
								catch(...) {
								}
							}
						}
					}
				}
			}
		}
	}

}

void STI_Server::init()
{
	//Servants
	controlServant = new ServerTimingSeqControl_i(this);
	expSequenceServant = new ExpSequence_i(this);
	modeHandlerServant = new ModeHandler_i();
	parserServant = new Parser_i(this);
	serverConfigureServant = new ServerConfigure_i(this);
	deviceConfigureServant = new RegisteredDevices_i(this);
	serverCommandLineServant = new ServerCommandLine_i(this);
	documentationSettingsServant = new DocumentationSettings_i("STIdocumentation.ini");
	clientBootstrapServant = new ClientBootstrap_i(this);

	localServerEventPusher = new ServerEventPusher_i(orbManager);
	deviceEventHandlerServant = new DeviceEventHandler_i(localServerEventPusher, this);

	registeredDevicesMutex = new omni_mutex();

	refreshMutex = new omni_mutex();

	//Inter-servant communication
	parserServant->add_ExpSequence(expSequenceServant);
	controlServant->add_Parser(parserServant);
	controlServant->add_ModeHandler(modeHandlerServant);
	controlServant->add_ExpSequence(expSequenceServant);

	//Register Servants
	//orbManager->registerServant(controlServant, 
	//	"STI/Client/Control.Object");
	//orbManager->registerServant(expSequenceServant, 
	//	"STI/Client/ExpSequence.Object");
	//orbManager->registerServant(modeHandlerServant, 
	//	"STI/Client/ModeHandler.Object");
	//orbManager->registerServant(parserServant, 
	//	"STI/Client/Parser.Object");
	orbManager->registerServant(serverConfigureServant, 
		"STI/Device/ServerConfigure.Object");
	//orbManager->registerServant(deviceConfigureServant, 
	//	"STI/Client/DeviceConfigure.Object");
	//orbManager->registerServant(serverCommandLineServant, 
	//	"STI/Client/ServerCommandLine.Object");
	
	orbManager->registerServant(clientBootstrapServant, 
		"STI/Client/ClientBootstrap.Object");
	orbManager->registerServant(documentationSettingsServant, 
		"STI/Client/DocumentationSettings.Object");

	registeredDevicesMutex->lock();
	{
		registeredDevices.clear();
	}
	registeredDevicesMutex->unlock();


	attributes.clear();
	devicesWithEvents.clear();

	defineAttributes();

	//transferEvents
	eventTransferLock = false;
	
	serverStopped = true;
	PausedByDevice = false;

	serverPauseMutex = new omni_mutex();
	serverPauseCondition = new omni_condition(serverPauseMutex);

	serverStateMutex = new omni_mutex();

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
	reregisterActiveDevices();

	cout << endl << "STI Server ready: " << endl << endl;
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


//*********** Client and Device bootstrap functions ****************//
bool STI_Server::addNewClient(STI::Pusher::ServerEventHandler_ptr eventHandler)
{
	STI::Pusher::TStatusEvent statusEvt;

	serverStateMutex->lock();
	{
		statusEvt.state = serverStatus;
		localServerEventPusher->addNewClient(eventHandler, statusEvt);
	}
	serverStateMutex->unlock();

	return true;
}


STI::Client_Server::ModeHandler_ptr STI_Server::getModeHandler()
{
	return modeHandlerServant->_this();
}

STI::Client_Server::Parser_ptr STI_Server::getParser()
{
	return parserServant->_this();
}
STI::Client_Server::ExpSequence_ptr STI_Server::getExpSequence()
{
	return expSequenceServant->_this();
}
STI::Client_Server::ServerTimingSeqControl_ptr STI_Server::getServerTimingSeqControl()
{
	return controlServant->_this();
}
STI::Client_Server::RegisteredDevices_ptr STI_Server::getRegisteredDevicesRef()
{
	return deviceConfigureServant->_this();
}
STI::Client_Server::ServerCommandLine_ptr STI_Server::getServerCommandLine()
{
	return serverCommandLineServant->_this();
}




STI::Pusher::DeviceEventHandler_ptr STI_Server::getDeviceEventHandler()
{
	return deviceEventHandlerServant->_this();
}

//*********** Device functions ****************//
bool STI_Server::activateDevice(string deviceID)
{
	bool success = false;
	
	registeredDevicesMutex->lock();
	{
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
	}
	registeredDevicesMutex->unlock();

	return success;
}

bool STI_Server::registerDevice(STI::Types::TDevice& device, STI::Server_Device::DeviceBootstrap_ptr bootstrap)
{
	refreshDevices();

	bool deviceRegistered = false;
	bool unique = false;

	//Make the deviceID and context
	string deviceIDstring = generateDeviceID(device);
	string deviceContextString = removeForbiddenChars(deviceIDstring);
	deviceContextString.insert(0,"STI/Device/");

	registeredDevicesMutex->lock();
	{
		// Look for this deviceID in the map of known RemoteDevices
		RemoteDeviceMap::iterator dev = registeredDevices.find(deviceIDstring);
		unique = (dev == registeredDevices.end());	//true if not found in the list

		if( unique )
		{
			device.deviceContext = deviceContextString.c_str();
			device.deviceID      = deviceIDstring.c_str();

			RemoteDevice* newDevice = new RemoteDevice(this, device, bootstrap);
			deviceRegistered = newDevice->activate();

			if( deviceRegistered )
			{
				//The device bootstrap servant is working; this device is alive. 
				registeredDevices.insert( deviceIDstring, newDevice );
			}
			else
			{
				//dead device
				delete newDevice;
			}
		}
	}
	registeredDevicesMutex->unlock();

	if( !unique )
	{
		// registration failed -- this deviceID is already registered
		deviceRegistered = false;

		// Check that this Device is still working and remove it if not
		getDeviceStatus(deviceIDstring);
	}

	if(deviceRegistered)
	{
		//push refresh to clients
		STI::Pusher::TDeviceRefreshEvent refreshEvent;
		refreshEvent.type = STI::Pusher::RefreshDeviceList;
		sendEvent(refreshEvent);

		cout << "Registered Device ID: " << deviceIDstring << endl;
	}

	return deviceRegistered;
}


bool STI_Server::removeDevice(string deviceID)
{
	bool removed = false;
		
	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);

		if(it != registeredDevices.end())
		{
			(it->second)->deactivate();	//RemoteDevice::deactivate()

			registeredDevices.erase(it);

			//try 
			//{
			//	registeredDevices.erase(it);
			//}
			//catch(CORBA::TRANSIENT&) {
			//	cerr << "CORBA::TRANSIENT" << endl;
			//}
			//catch(CORBA::SystemException& ex) {
			//	cerr << "CORBA::" << ex._name() << endl;
			//}
			//catch(CORBA::Exception&) {
			//}
			//catch(...) {
			//}

			removed = true;
		}
		else
		{
			// Device not found in registeredDevices
			removed = false;
		}
	}
	registeredDevicesMutex->unlock();

	//mandatory refresh
	STI::Pusher::TDeviceRefreshEvent refreshEvent;
	refreshEvent.type = STI::Pusher::RefreshDeviceList;
	refreshEvent.deviceID = CORBA::string_dup("");
	refreshEvent.errorMessage = CORBA::string_dup("");

	sendEvent(refreshEvent);

	refreshPartnersDevices();

	return removed;
}


bool STI_Server::setChannels(std::string deviceID, const STI::Types::TDeviceChannelSeq& channels)
{
	bool success = true;
	unsigned i;

	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);

		if( device != registeredDevices.end() )
		{
			for(i = 0; i < channels.length(); i++)
			{
				success &= (device->second)->addChannel( channels[i] );
			}
		}
		else
		{
			success = false;		//deviceID not found
		}
	}
	registeredDevicesMutex->unlock();

	return success;
}


bool STI_Server::getDeviceStatus(string deviceID)
{
	bool deviceActive = false;
	bool found = false;

	RemoteDeviceMap::iterator device;
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		found = (device != registeredDevices.end());
		
		if(found)
		{
			deviceActive = device->second->isActive();
		}
	}
	registeredDevicesMutex->unlock();

	// Remove the device if it's found but not active
	if(found && !deviceActive)
	{
		// Servants cannot be accessed -- this Device either took too long
		// to activate or is not accessible anymore and will be removed 
		// from the Server.
		removeDevice(deviceID);
		cout << "Removed: " << deviceID << endl;
	}

	return deviceActive;
}


//bool STI_Server::isUnique(string deviceID)
//{
//	// Look for this deviceID string in the map of known RemoteDevices
//	RemoteDeviceMap::iterator it = registeredDevices.find(deviceID);
//
//	if(it == registeredDevices.end())
//		return true;	// not found
//
//	return false;
//}


void STI_Server::refreshDevices()
{
	//checks the status of all registered devices, automatically removing dead devices
	
	serverStopped = false;

	vector<string> devicesToCheck;
	RemoteDeviceMap::iterator device;
	bool success = true;

	//Make a list of the current devices
	registeredDevicesMutex->lock();
	{
		for(device = registeredDevices.begin(); device != registeredDevices.end(); device++)
		{
			devicesToCheck.push_back(device->first);
		}
	}
	registeredDevicesMutex->unlock();

	//Since we give up the mutex lock, the list "devicesToCheck" might get out of date.
	//Still, we check all the deviceID values in the list. getDeviceStatus() is robust
	//against missing devices.

	refreshMutex->lock();
	{
		for(unsigned i = 0; i < devicesToCheck.size(); i++)
		{
			success &= getDeviceStatus(devicesToCheck.at(i));
		}
	}
	refreshMutex->unlock();

	if(!success)
	{
		//A dead device was found and removed, or the list was out of date.
		refreshDevices();
	}
}


void STI_Server::refreshPartnersDevices()
{
	// first confirm that all registered devices are alive
	refreshDevices();

	serverStopped = false;

	bool success = true;
	bool unregisterSuccess = true;
	unsigned i, j;
	RemoteDeviceMap::iterator device, partner;



	vector<string>* registerdPartners;

	registeredDevicesMutex->lock();
//	refreshMutex->lock();
	{
		for(device = registeredDevices.begin(); device != registeredDevices.end() 
			&& !serverStopped; device++)
		{
			//refreshing registered device 'device'

			//First look for any of this device's requiredPartners that might be on
			//the server in registeredDevices.
			for(i = 0; i < (device->second)->getRequiredPartners().size() && !serverStopped; i++)
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
					unregisterSuccess &= (device->second)->unregisterPartner( (device->second)->getRequiredPartners().at(i) );
					
					if( !unregisterSuccess )
					{
						//This should never happen.
						cerr << "Error: Failed to unregister partner '" << (device->second)->getRequiredPartners().at(i) 
							<< "' on device '" << (device->second)->getDevice().deviceName << "'." << endl;
						break;
					}
				}
			}
			

			// Now refresh the registeredPartnerDevices on this device
			registerdPartners = &( (device->second)->getRegisteredPartners() );

			for(j = 0; j < registerdPartners->size() && !serverStopped; j++)
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
//	refreshMutex->unlock();
	registeredDevicesMutex->unlock();
	// Registration should only fail if a device has died; in this case
	// we should refresh again to eliminate the dead device.

	if( !success && !serverStopped)
		refreshPartnersDevices();
}

void STI_Server::sendMessageToClient(STI::Pusher::MessageType type, std::string message, bool clearFirst,  unsigned int linesBack, unsigned int charsBack)
{
	STI::Pusher::TMessageEvent messageEvt;
	messageEvt.type = type;
	messageEvt.message = CORBA::string_dup(message.c_str());
	messageEvt.clearFirst = clearFirst;
	messageEvt.linesBack = linesBack;
	messageEvt.charsBack = charsBack;

//	cout << "* " << message << endl;

	sendEvent( messageEvt );
}



void STI_Server::handleDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event)
{
	if(event.type == STI::Pusher::RefreshDevice || event.type == STI::Pusher::RefreshDeviceList)
	{
		refreshPartnersDevices();
	}

	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator device = registeredDevices.find( std::string(event.deviceID) );
		bool success = ( device != registeredDevices.end() );

		if( success && device->second != NULL)
		{	
			device->second->handleDeviceRefreshEvent(event);
		}
		else
		{
			//re-register?
			cout << "An unregistered device (" << event.deviceID << ")" 
				<< endl << "attempted to contact the server." << endl;
		}
	}
	registeredDevicesMutex->unlock();

}

//*********** Timing event functions ****************//
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

	registeredDevicesMutex->lock();
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
					if(device->first.compare( (eventPartner->second)->getEventPartners().at(j) ) == 0)
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
				//If this device doesn't have any events, remove it from dependent partner list. Otherwise, eventPartner will hang waiting for device to "finish".
				//Do this AFTER checking for circular dependencies because we want to enfore NO circular dependency rule in all situations.
//				if( !hasEvents(device->first) )	
				if(STI::Utils::isUniqueString(device->first, devicesWithEvents))
				{
					//device->first has no events
					eventPartner->second->removePartnerDependency( device->first );
				}
			}
		}
	}
	refreshMutex->unlock();
	registeredDevicesMutex->unlock();

	return error;
}

bool STI_Server::setupEventsOnDevices()
{
	std::string eventPartnerDeviceID;
	STI::Types::TDeviceEventSeq_var partnerEvents;

	serverStopped = false;

	if( !changeStatus(PreparingEvents) )
	{
		sendMessageToClient( STI::Pusher::ParsingMessage, "\nError: Server failed to change to PreparingEvents state." );
		return true; //error
	}

	unsigned i;
	bool error = false;
	std::stringstream errors, messenges;
	RemoteDeviceMap::iterator device, otherDevice, eventPartner;

	resetDeviceEvents();
	
	errors.str("");

	if( !error )
	{
		divideEventList();
	}

	refreshDevices();

	//Get the list of devices that have explicit events and must be registered.
	//These are "primary" devices that have explicit event(...) commands referencing them in this timing file.
	determineWhichDevicesHaveExplicitEvents();

	errors.str("");

	sendMessageToClient( STI::Pusher::ParsingMessage, "Checking channels...\n" );
	if( checkChannelAvailability(errors) )
	{
		error = true;
		sendMessageToClient( STI::Pusher::ParsingMessage, errors.str().c_str() );
	}

	errors.str("");

	determineWhichDevicesHaveEvents();

	if( !error && calculatePartnerDependencies(errors) )
	{
		error = true;
		sendMessageToClient( STI::Pusher::ParsingMessage, errors.str().c_str() );
	}

	if( !error )
	{
		sendMessageToClient( STI::Pusher::ParsingMessage, "Transferring events to devices...\n" );

		registeredDevicesMutex->lock();
		{
			transferEvents();	//Makes a new thread for each device so events are transfered in parallel

			vector<string> devicesTransfering( devicesWithEvents );

			//Continuously poll all the devices, checking to see which are done transfering.
			while( devicesTransfering.size() > 0 && !serverStopped && !error )
			{
				for(i = 0; i < devicesTransfering.size() && !serverStopped && !error; i++)
				{
					device = registeredDevices.find(devicesTransfering.at(i));

					if( device == registeredDevices.end() )
					{
						//This device isn't registered; this should never happen.
						messenges.str("");
						messenges << "Error: " << registeredDevices[devicesTransfering.at(i)].printDeviceIndentiy() << " not found.";
						sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );

						error = true;
						break;
					}
					if( device->second->finishedEventsTransferAttempt() )
					{
						messenges.str("");
						messenges << "    " << device->second->printDeviceIndentiy() << "...";
						sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );

						//Send transfer status (or errors) to client
						if( device->second->eventsTransferSuccessful() )
						{
							sendMessageToClient( STI::Pusher::ParsingMessage, "success\n" );
						}
						else
						{
							error = true;
							messenges.str("");
							messenges << endl << device->second->getTransferErrLog() << endl;
							sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );
						}
						//if this device has dependent devices, get partner events and add them to the transfer lists
						for(unsigned k = 0; k < device->second->getEventPartners().size(); k++)
						{
							eventPartnerDeviceID = device->second->getEventPartners().at(k);
							partnerEvents = device->second->getPartnerEvents(eventPartnerDeviceID);
							eventPartner = registeredDevices.find(eventPartnerDeviceID);

							if(eventPartner == registeredDevices.end())
							{
								//This should never happen (if true, it should have failed earlier).
								error = true;
								messenges.str("");
								messenges << "Error: Event partner " << eventPartnerDeviceID << " is not registered." << endl;
								sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );
								break;
							}

							//add each event requested by this device to its Event Partner's event list and to the list of parsed events (for sending to the client)
							for(unsigned m = 0; m < partnerEvents->length() && !error; m++)
							{
								push_backPartnerEvent(eventPartnerDeviceID, partnerEvents[m].time, partnerEvents[m].channel, partnerEvents[m].value, 
									events[devicesTransfering.at(i)].at( partnerEvents[m].eventNum ).getTEvent(), 
									partnerEvents[m].isMeasurementEvent, STI::Utils::valueToString(partnerEvents[m].description), partnerEvents[m]);

								//add to the list of parsed events that get passed to the client
								parserServant->addDeviceGeneratedEvent(partnerEvents[m], 
									events[devicesTransfering.at(i)].at( partnerEvents[m].eventNum ).getTEvent(), 
									*eventPartner->second);
							}
						}

						//remove the device that just finished transfering from the dependency lists of all other devices
						for(unsigned j = 0; j < devicesTransfering.size(); j++)
						{
							otherDevice = registeredDevices.find(devicesTransfering.at(j));
							if(otherDevice != registeredDevices.end())
							{
								otherDevice->second->removePartnerDependency( devicesTransfering.at(i) );
							}								
						}

						//this device is done transfering; stop polling it
						devicesTransfering.erase(devicesTransfering.begin() + i);
						break;
					}
					else
					{
						device->second->checkDependencies();
					}
				}
			}
		}
		registeredDevicesMutex->unlock();

	}

	if( !error )
	{
		sendMessageToClient(STI::Pusher::ParsingMessage, "Loading events on devices...\n");
		loadEvents();	//start the loading threads on all devices

		registeredDevicesMutex->lock();
		{
			map<int, string> devicesLoading;
			map<int, string>::iterator loadingDevice;

			for(i = 0; i < devicesWithEvents.size() && !serverStopped; i++)
			{
				device = registeredDevices.find( devicesWithEvents.at(i) );

				if(device != registeredDevices.end())
				{
					messenges.str("");
					messenges << "    " << device->second->printDeviceIndentiy() << "..." << endl;
					sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );
				}

				devicesLoading[i] = devicesWithEvents.at(i);
			}

			//wait for each device to finish loading asynchronously
			while(devicesLoading.size() > 0 && !serverStopped && !error)
			{
				for(loadingDevice = devicesLoading.begin(); loadingDevice != devicesLoading.end() && !serverStopped; loadingDevice++)
				{
					device = registeredDevices.find( loadingDevice->second );

					if(device != registeredDevices.end())
					{
						if(device->second->eventsLoaded())
						{
							//this device finished loading
							sendMessageToClient( STI::Pusher::ParsingMessage, "done", false, 
								(devicesWithEvents.size() - loadingDevice->first));		//print "done" on the appropriate line

							devicesLoading.erase(loadingDevice);
							break;		//the iterator loadingDevice is no longer valid
						}
					}
					else
					{
						//this should never happen
						error = true;
						messenges.str("");
						messenges << "Loading Error: Device " << loadingDevice->second << " is not registered." << endl;
						sendMessageToClient( STI::Pusher::ParsingMessage, messenges.str() );
						break;
					}
				}
			}
		}
		registeredDevicesMutex->unlock();
	}

	
	if( serverStopped )
	{
		sendMessageToClient( STI::Pusher::ParsingMessage, "\n**Parsing Interrupted**." );
		error = true;
	}
	else if( !error )
	{
		sendMessageToClient( STI::Pusher::ParsingMessage, "\nFinished. Ready to play." );
	}
	else
	{
		sendMessageToClient( STI::Pusher::ParsingMessage, "\nFinished. There are errors." );
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

	registeredDevicesMutex->lock();
	{
		for(iter = registeredDevices.begin(); iter != registeredDevices.end() && !serverStopped; iter++)
		{
			iter->second->reset();
		}
	}
	registeredDevicesMutex->unlock();
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

		push_backEvent(deviceID, parsedEvents[i].time, channel, parsedEvents[i].value, parsedEvents[i], parsedEvents[i].isMeasurementEvent, STI::Utils::valueToString(parsedEvents[i].description));
//   	push_backEvent(eventPartnerDeviceID, partnerEvents[m].time, partnerEvents[m].channel, partnerEvents[m].value, events[devicesTransfering.at(i)].at( partnerEvents[m].eventNum ).getTEvent(), partnerEvents[m].isMeasurementEvent, STI::Utils::valueToString(partnerEvents[m].description));
	
	//	events[deviceID].push_back( new TDeviceEvent );

	//	events[deviceID].back()->channel = channel;
	//	events[deviceID].back()->time = parsedEvents[i].time;
	//	events[deviceID].back()->value = parsedEvents[i].value;
	}
}


void STI_Server::push_backPartnerEvent(std::string deviceID, double time, unsigned short channel, STI::Types::TValMixed value, 
									   const STI::Types::TEvent& originalTEvent, bool isMeasurement, 
									   std::string description, const STI::Types::TDeviceEvent& originalTDeviceEvent)
{
	CompositeEvent& lastEvent = push_backEvent(deviceID, time, channel, value, originalTEvent, isMeasurement, description);

	lastEvent.getTDeviceEvent().hasDynamicValue = originalTDeviceEvent.hasDynamicValue;
	if(originalTDeviceEvent.hasDynamicValue) {
		lastEvent.getTDeviceEvent().dynamicValueRef = STI::Server_Device::DynamicValueLink::_duplicate(originalTDeviceEvent.dynamicValueRef);
	}
	lastEvent.getTDeviceEvent().useCallback = originalTDeviceEvent.useCallback;
	if(originalTDeviceEvent.useCallback) {
		lastEvent.getTDeviceEvent().callbackRef = STI::Server_Device::TMeasurementCallback::_duplicate(originalTDeviceEvent.callbackRef);
	}
}

CompositeEvent& STI_Server::push_backEvent(std::string deviceID, double time, unsigned short channel, STI::Types::TValMixed value, 
										   const STI::Types::TEvent& originalTEvent, bool isMeasurement, std::string description)
{
//	events[deviceID].push_back( new STI::Types::TDeviceEvent );
	events[deviceID].push_back( CompositeEvent(originalTEvent)  );

	events[deviceID].back().getTDeviceEvent().channel = channel;
	events[deviceID].back().getTDeviceEvent().time = time;
	events[deviceID].back().getTDeviceEvent().value = value;
	
	if( parserServant->getTimingFiles().size() > originalTEvent.pos.file )
	{
		events[deviceID].back().getTDeviceEvent().pos.file =
			CORBA::string_dup( parserServant->getTimingFiles().at( originalTEvent.pos.file ).c_str() );
	}
	else
	{
		events[deviceID].back().getTDeviceEvent().pos.file = CORBA::string_dup("???");
	}
	events[deviceID].back().getTDeviceEvent().pos.line = originalTEvent.pos.line;

	events[deviceID].back().getTDeviceEvent().isMeasurementEvent = isMeasurement;
	events[deviceID].back().getTDeviceEvent().description = CORBA::string_dup(description.c_str());

	events[deviceID].back().getTDeviceEvent().hasDynamicValue = false;
	events[deviceID].back().getTDeviceEvent().useCallback = false;

//	STI::Types::TEvent newEvent;

//	CompositeEvent temp(new STI::Types::TDeviceEvent, newEvent);
//	compositeEvents.push_back( temp  );

	return events[deviceID].back();

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





void STI_Server::determineWhichDevicesHaveExplicitEvents()
{
	devicesWithExplicitEvents.clear();
	EventMap::iterator dev;
	
	//The "events" map is sorted by device.  The keys correspond to the primary 
	//devices (those that have explicit events in the timing file).
	for(dev = events.begin(); dev != events.end(); dev++)
	{
		devicesWithExplicitEvents.insert(dev->first);	//adds the deviceID of the device to the set
	}
}

void STI_Server::determineWhichDevicesHaveEvents()
{
	RemoteDeviceMap::iterator iter;

	registeredDevicesMutex->lock();
	{
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
				addDependentPartners(*(iter->second), devicesWithEvents);
			}
		}
	}
	registeredDevicesMutex->unlock();

}

void STI_Server::transferEvents()		//transfer events from the server to the devices
{
	unsigned i;

	serverStopped = false;

	eventTransferLock = false;

	// Transfer events in parallel: make a new event transfer thread for each device that has events
	for(i = 0; i < devicesWithEvents.size() && !serverStopped; i++)
	{
		while(eventTransferLock && !serverStopped) {}	//spin lock while the new thead makes a local copy of currentDevice
		eventTransferLock = true;
		currentDevice = devicesWithEvents.at(i);		//deviceID

		omni_thread::create(transferEventsWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
	}
}

void STI_Server::addDependentPartners(RemoteDevice& device, std::vector<std::string>& dependencies)
{
	//Add all the dependent partners of this device.

	//Warning: This function is inherently unsafe with respect to thread synchronization
	//         of the registeredDevices map.  Since it is recursive, it cannot internally lock
	//         the registeredDevices mutex.  This function should ONLY be called from within
	//         a registeredDevicesMutex locked segment to prevent critical access violations.
	for(unsigned i = 0; i < device.getEventPartners().size(); i++)
	{
		//add this partner
		if( STI::Utils::isUniqueString(device.getEventPartners().at(i), dependencies) )
		{
			dependencies.push_back( device.getEventPartners().at(i) );
		}

		//add this partner's dependencies
		RemoteDeviceMap::iterator dev = registeredDevices.find(device.getEventPartners().at(i));
		if(dev != registeredDevices.end())
		{
			//this partner is a registered device
			addDependentPartners(*dev->second, dependencies);
		}
	}
}

void STI_Server::loadEvents()
{
	serverStopped = false;
	RemoteDeviceMap::iterator device;

	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			device = registeredDevices.find(devicesWithEvents.at(i));
			
			if(device != registeredDevices.end())
			{
				device->second->loadEvents();	//returns promptly by spawning a device-side loading thread
			}
		}
	}
	registeredDevicesMutex->unlock();
}


bool STI_Server::requestPlay(bool devicesOnly)
{
	if( !(devicesOnly && serverStatus == PlayingEvents) )
	{
		// Only skip this step if the server is PlayingEvents and wants to prepareToPlay() the devices (e.g., in continuous mode)
		if( !changeStatus(RequestingPlay) )
			return false;
	}
	
	bool success = true;
	RemoteDeviceMap::iterator device;
	
	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			device = registeredDevices.find(devicesWithEvents.at(i));

			if(device != registeredDevices.end())
			{
				success &= device->second->prepareToPlay();
			}
			else
			{
				success = false;
			}
		}
	}
	registeredDevicesMutex->unlock();

	return success;
}

void STI_Server::playEvents(bool playContinuous)
{

	serverStopped = false;
	bool requestDevicesOnly = false;
	
	do {

	if( !requestPlay( requestDevicesOnly ) )
		return;

	if( !changeStatus(PlayingEvents) )
		return;

	playAllDevices();				//does not block; devices return promptly
	collectDeviceMeasurements();	//starts a new thread

	waitForEventsToFinish();		//blocks until all devices are done
	waitForMeasurementCollection();	//blocks until all measurements have been received

	requestDevicesOnly = playContinuous;

	} while(playContinuous && !serverStopped);

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

	RemoteDeviceMap::iterator device;

	registeredDevicesMutex->lock();
	{
		for(unsigned i = 0; i < devicesWithEvents.size(); i++)
		{
			device = registeredDevices.find(devicesWithEvents.at(i));
			
			if(device != registeredDevices.end())
			{
				device->second->resetMeasurements();
			}
		}
	}
	registeredDevicesMutex->unlock();


	bool measurementsRemaining = true;

	while(measurementsRemaining && !serverStopped)
	{
		measurementsRemaining = false;
		
		registeredDevicesMutex->lock();
		{
			for(unsigned i = 0; i < devicesWithEvents.size(); i++)
			{
				device = registeredDevices.find(devicesWithEvents.at(i));

				if(device != registeredDevices.end() 
					&& device->second->hasMeasurementsRemaining())
				{
					measurementsRemaining = true;
					device->second->getNewMeasurementsFromServer();
				}
			}
		}
		registeredDevicesMutex->unlock();

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
	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator iter;
		for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)
		{
			(iter->second)->stop();
		}
	}
	registeredDevicesMutex->unlock();
}


void STI_Server::pauseAllDevices()
{
	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			//(iter->second)->loadEvents();
			registeredDevices[devicesWithEvents.at(i)].pause();
		}
	}
	registeredDevicesMutex->unlock();
}

void STI_Server::playAllDevices()
{
	unsigned i;
	
	registeredDevicesMutex->lock();
	{
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			registeredDevices[devicesWithEvents.at(i)].playEvents();
		}
	}
	registeredDevicesMutex->unlock();
}

void STI_Server::playEventsOnDevice(std::string deviceID)
{
	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			//(iter->second)->loadEvents();
			if(deviceID.compare(devicesWithEvents.at(i)) == 0)
				registeredDevices[devicesWithEvents.at(i)].playEvents();
		}
	}
	registeredDevicesMutex->unlock();
}


void STI_Server::pauseAllDevicesExcept(std::string deviceID)	//pauses all devices except device deviceID
{
	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			if( deviceID.compare(registeredDevices[devicesWithEvents.at(i)].getDevice().deviceID) != 0 )
				registeredDevices[devicesWithEvents.at(i)].pause();
		}

		unpausedDeviceID = deviceID;
	}
	registeredDevicesMutex->unlock();
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
		registeredDevicesMutex->lock();
		{
			finished = true;
			for(i = 0; i < devicesWithEvents.size() && !serverStopped && !serverPaused; i++)
			{
				finished &= registeredDevices[devicesWithEvents.at(i)].eventsPlayed();
			}
		}
		registeredDevicesMutex->unlock();

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

//	_CrtDumpMemoryLeaks();
}

//*********** State machine functions ****************//
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


bool STI_Server::changeStatus(STI::Pusher::ServerState newStatus)
{
	bool allowedTransition = false;	

	serverStateMutex->lock();
	{
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
	}
	serverStateMutex->unlock();

	if(allowedTransition)
	{
		serverStateMutex->lock();
		{
			serverStatus = newStatus;
		}
		serverStateMutex->unlock();
	
		updateState();
	}
	return allowedTransition;
}

void STI_Server::updateState()
{
	serverStateMutex->lock();
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

		STI::Pusher::TStatusEvent statusEvt;
		statusEvt.state = serverStatus;
		sendEvent( statusEvt );
//		cout << "Status: " << serverStatus << endl;
	}
	serverStateMutex->unlock();
}


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
	std::set<std::string>::iterator deviceWithExplicitEvents;

	STI::Types::TDevice tDevice;
	string deviceID;

	registeredDevicesMutex->lock();
	{
		for(unsigned i = 0; i < channels.length(); i++)
		{
			tDevice.address    = channels[i].device.address;
			tDevice.moduleNum  = channels[i].device.moduleNum;
			tDevice.deviceName = channels[i].device.deviceName;

			deviceID = generateDeviceID(tDevice);
			//device = explicitEventDevices.find( deviceID );
			device = registeredDevices.find( deviceID );

			deviceWithExplicitEvents = devicesWithExplicitEvents.find( deviceID );

			if(deviceWithExplicitEvents == devicesWithExplicitEvents.end() )
			{
				//This device does not have any explicit events so we don't check that it's registered here, 
				//even though it's a defined channel (i.e., it has a dev(...) command in the timing file).
				//If it gets partner events, it's registration will be verified later.
			}
			else if( device != registeredDevices.end() )		//found this device
			{
				channels[i].device.deviceID = CORBA::string_dup( device->second->getDevice().deviceID );
				channels[i].device.deviceContext = CORBA::string_dup( device->second->getDevice().deviceContext );

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
					//Fill in missing channel information.  This info comes from the Device,
					//so it is not known during python parsing and must be filled in here.
					channels[i].type = channelIter->type;
					channels[i].inputType = channelIter->inputType;
					channels[i].outputType = channelIter->outputType;
					channels[i].channelName = channelIter->channelName;
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
	}
	registeredDevicesMutex->unlock();

	return missingChannels;
}


bool STI_Server::eventsParsed()
{
//check that all devices have parsed their events and are ready to proceed
	bool allParsed = true;
//	RemoteDeviceMap::iterator iter;

//	for(iter = registeredDevices.begin(); iter != registeredDevices.end(); iter++)

	registeredDevicesMutex->lock();
	{
		unsigned i;
		for(i = 0; i < devicesWithEvents.size(); i++)
		{
			allParsed &= registeredDevices[devicesWithEvents.at(i)].eventsParsed();
		}
	}
	registeredDevicesMutex->unlock();


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




//*********** Helper functions ****************//

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



//*********** Getter functions ****************//
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
	std::string result = "";

	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			result = (device->second)->getTransferErrLog();
		}
	}
	registeredDevicesMutex->unlock();

	return result;
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
	bool found = false;
	string device_id(deviceID);
	RemoteDeviceMap::iterator device;
	std::string result = "";
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(device_id);
		found = (device != registeredDevices.end());

		if(found)
		{
			result = (device->second)->execute(args);
		}
	}
	registeredDevicesMutex->unlock();

	if( !found )
	{
		refreshDevices();

		//now try again...

		registeredDevicesMutex->lock();
		{
			device = registeredDevices.find(device_id);
			found = (device != registeredDevices.end());

			if(found)
			{
				result = (device->second)->execute(args);
			}
		}
		registeredDevicesMutex->unlock();
	}

	return result;
}

bool STI_Server::writeChannelDevice(std::string deviceID, unsigned short channel, const MixedValue& value)
{
	RemoteDeviceMap::iterator device;
	bool result = false;
	bool found = false;
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		found = (device != registeredDevices.end());
		
		if(found)
		{
			result = (device->second)->write(channel, value);
		}
	}
	registeredDevicesMutex->unlock();

	if( !found )
	{
		refreshDevices();

		//now try again...

		registeredDevicesMutex->lock();
		{
			device = registeredDevices.find(deviceID);
			found = (device != registeredDevices.end());
			
			if(found)
			{
				result = (device->second)->write(channel, value);
			}
		}
		registeredDevicesMutex->unlock();
	}

	return result;
}

bool STI_Server::readChannelDevice(std::string deviceID, unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	RemoteDeviceMap::iterator device;
	bool result = false;
	bool found = false;
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		found = (device != registeredDevices.end());
		
		if(found)
		{
			result = (device->second)->read(channel, valueIn, dataOut);
		}
	}
	registeredDevicesMutex->unlock();
	
	if( !found )
	{
		refreshDevices();

		//now try again...

		registeredDevicesMutex->lock();
		{
			device = registeredDevices.find(deviceID);
			found = (device != registeredDevices.end());
			
			if(found)
			{
				result = (device->second)->read(channel, valueIn, dataOut);
			}
		}
		registeredDevicesMutex->unlock();
	}

	return result;

}
void STI_Server::getRequiredPartners(std::string deviceID, std::vector<std::string>& partners)
{
	RemoteDeviceMap::iterator device;
	bool found = false;
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		found = (device != registeredDevices.end());
		
		if(found)
		{
			partners = (device->second)->getRequiredPartners();
		}
	}
	registeredDevicesMutex->unlock();
	
	if( !found )
	{
		refreshDevices();

		//now try again...

		registeredDevicesMutex->lock();
		{
			device = registeredDevices.find(deviceID);
			found = (device != registeredDevices.end());
			
			if(found)
			{
				partners = (device->second)->getRequiredPartners();
			}
		}
		registeredDevicesMutex->unlock();
	}
}

void STI_Server::getRegisteredPartners(std::string deviceID, std::vector<std::string>& partners)
{
	RemoteDeviceMap::iterator device;
	bool found = false;
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		found = (device != registeredDevices.end());
		
		if(found)
		{
			partners = (device->second)->getRegisteredPartners();
		}
	}
	registeredDevicesMutex->unlock();
	
	if( !found )
	{
		refreshDevices();

		//now try again...

		registeredDevicesMutex->lock();
		{
			device = registeredDevices.find(deviceID);
			found = (device != registeredDevices.end());
			
			if(found)
			{
				partners = (device->second)->getRegisteredPartners();
			}
		}
		registeredDevicesMutex->unlock();
	}
}

bool STI_Server::setDeviceAttribute(std::string deviceID, std::string key, std::string value)
{
	bool success = false;

	if(getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		registeredDevicesMutex->lock();
		{
			RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);
			
			if(device != registeredDevices.end())
			{
				success = device->second->setAttribute(key, value);
			}
		}
		registeredDevicesMutex->unlock();
	}

	return success;
}

STI::Types::TAttributeSeq* STI_Server::getDeviceAttributes(std::string deviceID)
{
	using STI::Types::TAttributeSeq;

	RemoteDeviceMap::iterator device;

	STI::Types::TAttributeSeq_var attribSeq( new TAttributeSeq );
	
	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			unsigned i,j;
			std::map<std::string, Attribute>::const_iterator it;
			const vector<string>* allowedValues = NULL;

			const AttributeMap& attribs = device->second->getAttributes();
			attribSeq->length(attribs.size());
			
			for(it = attribs.begin(), i = 0; it != attribs.end() && i < attribs.size(); it++, i++)
			{
				attribSeq[i].key = CORBA::string_dup(it->first.c_str());
				attribSeq[i].value = CORBA::string_dup(it->second.value().c_str());

				allowedValues = it->second.valuelist();	// Attribute::valuelist()

				attribSeq[i].values.length(allowedValues->size());

				//get allowed values for this attribute
				for(j = 0; j < allowedValues->size(); j++)
				{
					attribSeq[i].values[j] = 
						CORBA::string_dup( allowedValues->at(j).c_str() );
				}
			}
		}
		else
		{
			attribSeq->length(0);
		}
	}
	registeredDevicesMutex->unlock();

	return attribSeq._retn();
}


bool STI_Server::setDeviceChannelName(std::string deviceID, short channel, std::string name)
{
	RemoteDeviceMap::iterator device;
	bool success = false;

	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			success = device->second->setDeviceChannelName(channel, name);
		}
	}
	registeredDevicesMutex->unlock();

	if(success)
	{
		//push refresh to clients
		STI::Pusher::TDeviceRefreshEvent refreshEvent;
		refreshEvent.type = STI::Pusher::RefreshChannels;
		refreshEvent.deviceID = CORBA::string_dup(deviceID.c_str());
		sendEvent(refreshEvent);
	}


	return success;
}

STI::Types::TChannelSeq* STI_Server::getDeviceChannels(std::string deviceID)
{
	using STI::Types::TChannelSeq;
	RemoteDeviceMap::iterator device;

	unsigned i;
	STI::Types::TChannelSeq_var channelSeq( new TChannelSeq );

	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			const vector<STI::Types::TDeviceChannel>& channels = device->second->getChannels();
			
			channelSeq->length(channels.size());
			
			const STI::Types::TDevice& tDevice = device->second->getDevice();
			
			for(i = 0; i < channels.size(); i++)
			{
				channelSeq[i].device.deviceName    = CORBA::string_dup(tDevice.deviceName);
				channelSeq[i].device.address       = CORBA::string_dup(tDevice.address);
				channelSeq[i].device.moduleNum     = tDevice.moduleNum;
				channelSeq[i].device.deviceID      = CORBA::string_dup(tDevice.deviceID);
				channelSeq[i].device.deviceContext = CORBA::string_dup(tDevice.deviceContext);

				channelSeq[i].channel    = channels.at(i).channel;
				channelSeq[i].type       = channels.at(i).type;
				channelSeq[i].inputType  = channels.at(i).inputType;
				channelSeq[i].outputType = channels.at(i).outputType;
				channelSeq[i].channelName = channels.at(i).channelName;
			}
		}
		else
		{
			channelSeq->length(0);
		}
	}
	registeredDevicesMutex->unlock();

	return channelSeq._retn();
}

STI::Types::TPartnerSeq* STI_Server::getDevicePartners(std::string deviceID)
{
	using STI::Types::TPartnerSeq;
	
	unsigned i;
	RemoteDeviceMap::iterator device;
	STI::Types::TPartnerSeq_var partnerSeq( new TPartnerSeq );

	registeredDevicesMutex->lock();
	{
		device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			const vector<STI::Types::TPartner>& partners = device->second->getPartners();

			partnerSeq->length(partners.size());

			for(i = 0; i < partners.size(); i++)
			{
				partnerSeq[i].partnerDeviceID = CORBA::string_dup( partners.at(i).partnerDeviceID );
				
				partnerSeq[i].deviceName = CORBA::string_dup( partners.at(i).deviceName );
				partnerSeq[i].ipAddress  = CORBA::string_dup( partners.at(i).ipAddress );
				partnerSeq[i].moduleNum  = partners.at(i).moduleNum;
				
				partnerSeq[i].isRequired    = partners.at(i).isRequired;
				partnerSeq[i].isEventTarget = partners.at(i).isEventTarget;
				partnerSeq[i].isMutual      = partners.at(i).isMutual;
				partnerSeq[i].isRegistered  = partners.at(i).isRegistered;
			}
		}
		else
		{
			partnerSeq->length(0);
		}
	}
	registeredDevicesMutex->unlock();

	return partnerSeq._retn();
}



STI::Types::TDeviceSeq* STI_Server::getDevices()
{
	refreshDevices();

	using STI::Types::TDeviceSeq;
		
	STI::Types::TDeviceSeq_var deviceSeq( new TDeviceSeq );

	registeredDevicesMutex->lock();
	{
		int i;
		RemoteDeviceMap::iterator device;

		deviceSeq->length( registeredDevices.size() );

		for(device = registeredDevices.begin(), i = 0; device != registeredDevices.end(); device++, i++)
		{
			deviceSeq[i].deviceName    = CORBA::string_dup( (device->second)->getDevice().deviceName );
			deviceSeq[i].address       = CORBA::string_dup( (device->second)->getDevice().address );
			deviceSeq[i].moduleNum     = (device->second)->getDevice().moduleNum;
			deviceSeq[i].deviceID      = CORBA::string_dup( (device->second)->getDevice().deviceID );
			deviceSeq[i].deviceContext = CORBA::string_dup( (device->second)->getDevice().deviceContext );
		}
	}
	registeredDevicesMutex->unlock();

	return deviceSeq._retn();
}

STI::Types::TLabeledData* STI_Server::getLabledData(std::string deviceID, std::string label)
{
	STI::Types::TLabeledData_var labeledData;

	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);
		if(device != registeredDevices.end())
		{
			labeledData = device->second->getLabedData(label);
		}
		else
		{
			labeledData = new STI::Types::TLabeledData();
			labeledData->label = CORBA::string_dup("");
		}
	}
	registeredDevicesMutex->unlock();

	return labeledData._retn();

}

void STI_Server::stopDevice(std::string deviceID)
{
	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);
		if(device != registeredDevices.end())
		{
			device->second->stop();
		}
	}
	registeredDevicesMutex->unlock();

}
void STI_Server::killDevice(std::string deviceID)
{
	if(getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		registeredDevicesMutex->lock();
		{
			RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);
			if(device != registeredDevices.end())
			{
				device->second->killDevice();
			}
		}
		registeredDevicesMutex->unlock();

		removeDevice(deviceID);
	}
	else
	{
		refreshDevices();
	}
}

long STI_Server::devicePing(std::string deviceID)
{
	long ping;

	registeredDevicesMutex->lock();
	{
		RemoteDeviceMap::iterator device = registeredDevices.find(deviceID);
		
		if(device != registeredDevices.end())
		{
			ping = device->second->pingDevice();
		}
		else
		{
			ping = -2;
		}
	}
	registeredDevicesMutex->unlock();

	return ping;

}