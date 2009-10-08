/*! \file STI_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class STI_Device
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

//#include <deviceVersion.h>

#include <STI_Device.h>
#include <ORBManager.h>
#include <Configure_i.h>
#include <DataTransfer_i.h>
#include <CommandLine_i.h>
#include <DeviceControl_i.h>
#include <Attribute.h>
#include <device.h>
#include <EventConflictException.h>
#include <EventParsingException.h>

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <map>

using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;


STI_Device::STI_Device(ORBManager* orb_manager, std::string DeviceName, 
					   std::string IPAddress, unsigned short ModuleNumber) : 
orbManager(orb_manager), deviceName(DeviceName)
{
	init(IPAddress, ModuleNumber);
}

void STI_Device::init(std::string IPAddress, unsigned short ModuleNumber)
{

	// servant names -- the STI_Server must look for these same names
	configureObjectName     = "Configure.Object";
	dataTransferObjectName  = "DataTransfer.Object";
	commandLineObjectName   = "CommandLine.Object";
	deviceControlObjectName = "DeviceControl.Object";

	//servants
	configureServant = new Configure_i(this);
	dataTransferServant = new DataTransfer_i(this);
	commandLineServant = new CommandLine_i(this, configureServant);
	deviceControlServant = new DeviceControl_i(this);

	dummyPartner = new PartnerDevice();

	//TDevice
	tDevice = new STI_Server_Device::TDevice;	//_var variable does not need to be deleted
	tDevice->deviceName = getDeviceName().c_str();
	tDevice->address = IPAddress.c_str();
	tDevice->moduleNum = ModuleNumber;

	serverConfigureFound = false;
	registedWithServer = false;
	registrationAttempts = 0;

	deviceStatus = EventsEmpty;
	updateState();

	addedPartners.clear();
	attributes.clear();
	channels.clear();
	requiredPartners.clear();
	mutualPartners.clear();
	measurements.clear();

	mainLoopMutex = new omni_mutex();
	
	playEventsMutex = new omni_mutex();
	playEventsTimer = new omni_condition(playEventsMutex);
	
	loadEventsThread = 0;
	playEventsThread = 0;
	
	// Automatically connect to the STI server and transfer 
	// channels, attributes, partners, etc.
	omni_thread::create(connectToServerWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	//deviceMain loop
	mainThread = omni_thread::create(deviceMainWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

STI_Device::~STI_Device()
{
	cerr << "Destructor: " << tDevice->deviceID << endl;
	//remove this Device from the Server
	if (orbManager->isRunning() )
	{
		ServerConfigureRef->removeDevice(tDevice->deviceID);
	}

	//if(configureServant != 0)
	//	delete configureServant;
	//if(dataTransferServant != 0)
	//	delete dataTransferServant;
	//if(commandLineServant != 0)
	//	delete commandLineServant;
	//if(deviceControlServant != 0)
	//	delete deviceControlServant;

	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
		delete attributeUpdaters.at(i);

	delete dummyPartner;
	delete mainLoopMutex;
	delete playEventsMutex;
	delete playEventsTimer;
}



void STI_Device::deviceShutdown()
{
	ServerConfigureRef->removeDevice(tDevice->deviceID);
	orbManager->ORBshutdown();

}

void STI_Device::deviceMainWrapper(void* object)
{
	cout << endl;
	STI_Device* thisObject = static_cast<STI_Device*>(object);

	bool run = true;

	thisObject->orbManager->waitForRun();	//this ensure that STI_Device has finished its constructor

	while(run)
	{
		//mutex ensures that execute(argc, argv) is not running
//		thisObject->mainLoopMutex->lock();			
		{
			run = thisObject->deviceMain(			//pure virtual
			thisObject->orbManager->getArgc(), 
			thisObject->orbManager->getArgv() );
		}
	//	thisObject->sti_err << 45;
//		thisObject->mainLoopMutex->unlock();

	};  
}





//*********** Network communication functions ****************//
void STI_Device::connectToServerWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	thisObject->connectToServer();
}

void STI_Device::connectToServer()
{
	orbManager->waitForRun();

	// Aquire a reference to ServerConfigure from the NameService.
	// When found, register with the server and acquire a unique deviceID.
	registerDevice();
	
	if(registedWithServer)
	{
		// Register this device's servants with the Name Service
		registerServants();

		// Activate the Device using ServerConfigure::activateDevice(deviceID)
		activateDevice();
	}
}


void STI_Device::registerDevice()
{
	CORBA::Object_var obj;

	// Try to acquire ServerConfigure Object
	while(!serverConfigureFound || !registedWithServer)
	{
		obj = orbManager->getObjectReference(
			"STI/Device/ServerConfigure.Object");
		
		ServerConfigureRef = STI_Server_Device::ServerConfigure::_narrow(obj);
		
		if( !CORBA::is_nil(ServerConfigureRef) )
		{
			// Object reference was found on the NameService
			serverConfigureFound = true;
			
			// Attempt to register this device with the server
			try {
				serverName = ServerConfigureRef->serverName();

				registedWithServer = ServerConfigureRef->registerDevice(tDevice);
			
				registrationAttempts++;
			}
			catch(CORBA::TRANSIENT& ex) {
				cerr << "Caught system exception CORBA::" 
					<< ex._name() << " -- unable to contact the "
					<< "STI Server." << endl
					<< "Make sure the server is running and that omniORB is "
					<< "configured correctly." << endl;
			}
			catch(CORBA::SystemException& ex) {
				cerr << "Caught a CORBA::" << ex._name()
					<< " while trying to contact the STI Server." << endl;
			}
		}
		else
		{
			// Reference is nil
			cerr << "ServerConfigure Object was not found." << endl;
		}
	}
}

void STI_Device::activateDevice()
{
	// setup the device's attributes
	initializeAttributes();
	
	// setup the device's channels and send them to the server
	try {
		initializeChannels();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to send channels to the STI Server." << endl;
	}
	
	// setup the device's partner devices
	initializePartnerDevices();
	
	// activate device
	try {
		ServerConfigureRef->activateDevice(tDevice->deviceID);
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}
}


void STI_Device::reRegisterDevice()
{
	serverConfigureFound = false;
	registedWithServer = false;

	connectToServer();
}

void STI_Device::registerServants()
{
	STI_Server_Device::Configure_var ConfigureRef;

	string contextName = string(tDevice->deviceContext);

	// Loop until this STI_Device succesfully registers its 
	// servants with the Name Service
	do {
		orbManager->registerServant(configureServant, 
			contextName + configureObjectName);
		
		orbManager->registerServant(dataTransferServant, 
			contextName + dataTransferObjectName);
	
		orbManager->registerServant(commandLineServant, 
			contextName + commandLineObjectName);

		orbManager->registerServant(deviceControlServant, 
			contextName + deviceControlObjectName);

		// Try to resolve one of the servants as a test
		CORBA::Object_var obj = orbManager->getObjectReference(
			contextName + configureObjectName);
		ConfigureRef = STI_Server_Device::Configure::_narrow(obj);

	} while(CORBA::is_nil(ConfigureRef));  
	// CAREFULL: This doesn't mean the servants are live, just that their 
	// is a resolvable reference on the Name Service. Add another check for this.
}


STI_Server_Device::CommandLine_var STI_Device::generateCommandLineReference()
{
	//This is needed so this device can register itself with mutual partners

	CORBA::Object_var obj = orbManager->getObjectReference(
			string(tDevice->deviceContext) + commandLineObjectName);
	STI_Server_Device::CommandLine_var CommandRef = STI_Server_Device::CommandLine::_narrow(obj);

	return CommandRef;
}

void STI_Device::initializeChannels()
{
	using STI_Server_Device::TDeviceChannel;
	using STI_Server_Device::TDeviceChannelSeq;
	using STI_Server_Device::TDeviceChannelSeq_var;

	measurements.clear();
	channels.clear();

	defineChannels();	//pure virtual

	unsigned i;
	ChannelMap::iterator it;

	//build the TDeviceChannel sequence using the stored vector<TDeviceChannel>
	TDeviceChannelSeq_var channelSeq( new TDeviceChannelSeq() );
	channelSeq->length(channels.size());

	for(it = channels.begin(), i = 0; it != channels.end(); it++, i++)
	{
		channelSeq[i].channel    = it->second.channel;
		channelSeq[i].type       = it->second.type;
		channelSeq[i].inputType  = it->second.inputType;
		channelSeq[i].outputType = it->second.outputType;
	}

	//set channels on the server for this device
	if( !ServerConfigureRef->setChannels(tDevice->deviceID, channelSeq))
	{
		cerr << "Error when sending channels to the server:" 
			<< " channels are invalid." << endl;
	}
}


void STI_Device::initializeAttributes()
{
	bool success = true;
	AttributeMap::iterator it;

	attributes.clear();

	defineAttributes();	// pure virtual

	for(unsigned i = 0; i < attributeUpdaters.size(); i++)
	{
		attributeUpdaters.at(i)->defineAttributes();
	}

	for(it = attributes.begin(); it != attributes.end(); it++)
	{
		success &= setAttribute(it->first, it->second.value());
	}

	if(!success)
	{
		cerr << "Error initializing attributes." << endl;
	}
}


void STI_Device::initializePartnerDevices()
{
	requiredPartners.clear();

	definePartnerDevices();			//pure virtual

	string deviceID;
	TDeviceMap::iterator partner;

	try {
		for(partner = addedPartners.begin(); partner != addedPartners.end(); partner++)
		{
			deviceID = ServerConfigureRef->generateDeviceID( partner->second );
			addPartnerDevice(partner->first, deviceID);
		}
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to initializr partner devices." << endl; 
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to initializr partner devices." << endl; 
	}
}



PartnerDevice& STI_Device::partnerDevice(std::string partnerName)
{
	// requiredPartners:     partnerName => deviceID
	// registeredPartners:   deviceID => PartnerDevice


	//if(partner == requiredPartners.end())	// invalid partnerName
	//{
	//	cerr << "Error: The partner '" << partnerName 
	//		<< "' is not a partner of this device. " << endl
	//		<< "All partners must be added in definePartnerDevices()." << endl;
	//	return *dummyPartner;
	//}
//	else
	
	PartnerDeviceMap& partnerMap = commandLineServant->getRegisteredPartners();
		
	// Search to see if the partnerName is a requiredPartner
	map<string, string>::iterator partner = requiredPartners.find( partnerName );

	// Search to see if partnerName IS a registered deviceID.
	// This happens (for example) if the partner is adds itself and/or is not specified in 
	// this device's definePartners() function, or if the partner is 
	// added using addLocalPartnerDevice().
	PartnerDeviceMap::iterator it = partnerMap.find( partnerName );	

	if( partner != requiredPartners.end() )		//partnerName is a required partner
		it = partnerMap.find( partner->second );	//lookup deviceID using partnerName

	if( it == partnerMap.end() )	// this partner has not been registered
	{
		cerr << "Error: The partner '" << partnerName 
			<< "' is not a registered partner of this device." << endl;

		return *dummyPartner;
	}
	else
		return *(it->second);
}

string STI_Device::execute(string args)
{
	std::string result = "";
	vector<string> arguments;
	// Using the C++ main(int argc, char** argv) convention: the 
	// first argument is the program's name (in this case the device's name).
	arguments.push_back(deviceName);

	splitString(args, " ", arguments);

	char** argv = new char*[ arguments.size() ];

//	char** argv;
//	argv = new (char*)[ arguments.size() ];
	unsigned i;

	for(i = 0; i < arguments.size(); i++)
	{
		cerr << "arg: " << arguments[i] << endl;
		argv[i] = new char[arguments[i].size() + 1];
		strcpy(argv[i], arguments[i].c_str());
	}

	mainLoopMutex->lock();		// Prevents deviceMain() from running again
	{
		result = execute(arguments.size(), argv);
	}
	mainLoopMutex->unlock();	// Allow deviceMain() to proceed
	
	for(i = 0; i < arguments.size(); i++)
		delete[] argv[i];
	delete[] argv;

	return result;
}



void STI_Device::reportMessage(STI_Server_Device::TMessageType type, string message)
{
	if(serverConfigureFound)
	{
		try {
			ServerConfigureRef->reportMessage( tDevice->deviceID, type, message.c_str() );
		}
		catch(CORBA::TRANSIENT& ex) {
			cerr << "Caught system exception CORBA::" 
				<< ex._name() << " -- unable to contact the "
				<< "STI Server." << endl
				<< "Make sure the server is running and that omniORB is "
				<< "configured correctly." << endl;
		}
		catch(CORBA::SystemException& ex) {
			cerr << "Caught a CORBA::" << ex._name()
				<< " while trying to contact the STI Server." << endl;
		}
	}
}






//*********** Device attributes functions ****************//
bool STI_Device::setAttribute(string key, string value)
{

//****************//
setAttribClock.reset();
//****************//

	AttributeMap::iterator attrib = attributes.find(key);

	if( attrib == attributes.end() )
		return false;	// Attribute not found

	if( !attrib->second.isAllowed(value) )
		return false;	//attribute not in list of allowed values

	//Pass the 'value' string by reference to updateStreamAttribute.
	//Allows the update functions to modify the newValue string.
	string newValue = value;

	if(isStreamAttribute(key))
	{
		if( updateStreamAttribute(key, newValue) )
		{
			attrib->second.setValue(newValue);
			return true;
		}
		else	// failed to update stream attribute -- invalid value or not found
			return false;
	}

	// Derived classes may add attributeUpdaters that implement
	// STI_Device::AttributeUpdater::updateAttributes.
	// This allows for attribute updates without implementing 
	// STI_Device::updateAttributes so that the derived class can act as
	// another abstract base without having to change the name 
	// of the interface function hooks.

	bool success = false;
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
	{
		success |= attributeUpdaters.at(i)->updateAttributes(key, newValue);

		if(success)
			attrib->second.setValue(newValue);
	}

//****************//
updateAttributeClock.reset();
//****************//

	if( updateAttribute(key, newValue) )	//pure virtual
	{
		success = true;
		attrib->second.setValue(newValue);
	}
cout << "STI_Device::updateAttribute time = " << updateAttributeClock.getCurrentTime() << endl;


cout << "STI_Device::setAttribute time = " << setAttribClock.getCurrentTime() << endl;

	return success;
}
void STI_Device::refreshDeviceAttributes()
{
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
		attributeUpdaters.at(i)->refreshAttributes();

	refreshAttributes();		//pure virtual
}


bool STI_Device::isStreamAttribute(string key) const
{
	unsigned short Channel;
	stringstream chNum;

	string::size_type Ch_Pos = key.find_first_of("Ch", 0);
	string::size_type Ch_EndPos = key.find_first_of("_", 0);

	if(Ch_Pos != 0 
		|| Ch_Pos == string::npos 
		|| Ch_EndPos == string::npos)	//Not a stream attribute
		return false;

	if( !stringToValue(key.substr(2, Ch_EndPos), Channel) )
		return false;    //error converting Channel

	chNum << "Ch" << Channel;

	if(key.compare(chNum.str() + "_InputStream") == 0 ||
		key.compare(chNum.str() + "_SamplePeriod") == 0 ||
		key.compare(chNum.str() + "_BufferDepth") == 0)
		return true;
	else
		return false;
}

//separate measurement buffer for each instance of DataTransfer
//streamBuffer buffers measuremnts for each (enabled) streaming channel
//buffer for each streaming channel is a circular queue
//buffer for each non-streaming channel is a vector
//streaming channels must default to vector buffer behavior if timing critical
//consumer threads block the circular queue while reading buffer, preventing overwritting of the first element
//However it only prevents queue overwrites -- data is still acquired with high priority.
//The measuremnt thread must regain overwrite control before another consumer thread does or else the queue will grow indefinitely
//Streaming data may be lossy, but it is time-stamped.
bool STI_Device::updateStreamAttribute(string key, string& value)
{
	unsigned short Channel;
	stringstream chNum;

	string::size_type Ch_Pos = key.find_first_of("Ch", 0);
	string::size_type Ch_EndPos = key.find_first_of("_", 0);

	if(Ch_Pos != 0 
		|| Ch_Pos == string::npos 
		|| Ch_EndPos == string::npos)	//Not a stream attribute
		return false;

	if( !stringToValue(key.substr(2, Ch_EndPos), Channel) )
		return false;    //error converting Channel

	chNum << "Ch" << Channel;

	if(key.compare(chNum.str() + "_InputStream") == 0)
	{
		if(value.compare("Enabled") == 0)
		{
			streamingBuffers[Channel].setStreamingStatus(true);
		}
		if(value.compare("Disabled") == 0)
		{
			streamingBuffers[Channel].setStreamingStatus(false);
		}
		else
		{
			return false;
		}
	}
	if(key.compare(chNum.str() + "_SamplePeriod") == 0)
	{
		double samplePeriod;
		if( !stringToValue(value, samplePeriod) )
			return false;
		value = valueToString(samplePeriod);	//use the exact result of the conversion
		return streamingBuffers[Channel].setSamplePeriod(samplePeriod);
	}
	if(key.compare(chNum.str() + "_BufferDepth") == 0)
	{
		unsigned int bufferDepth;
		if( !stringToValue(value, bufferDepth) )
			return false;
		value = valueToString(bufferDepth);	//use the exact result of the conversion

		cerr << "Buffer depth: " << value << " = " << bufferDepth << endl;

		return streamingBuffers[Channel].setBufferDepth(bufferDepth);
	}

	return false;	//Not a stream attribute
}







//*********** Timing event functions ****************//
bool STI_Device::playSingleEvent(const RawEvent& Event)
{
	return writeChannel(Event);		//pure virtual
}


bool STI_Device::makeMeasurement(ParsedMeasurement& Measurement)
{
	return readChannel(Measurement);	//pure virtual
}


void STI_Device::setupPartnerEvents()
{
	using STI_Server_Device::TDeviceEventSeq;
	using STI_Server_Device::TDeviceEventSeq_var;
	
	STI_Server_Device::TDeviceEventSeq_var tEventSeq;
//	tEventSeq->length( ? );

}

bool STI_Device::transferEvents(const STI_Server_Device::TDeviceEventSeq& events)
{
	unsigned i,j;
	RawEventMap::iterator badEvent;
	STI_Server_Device::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	measurements.clear();
	rawEvents.clear();
	synchedEvents.clear();

	//This is only zero after clearing this device's parsed events.
	unsigned initialEventNumber = rawEvents.size();

	//Move the events from TDeviceEventSeq 'events' (provided by server) to
	//the raw event list 'rawEvents'.  Check for general event errors.
	for(i = 0; i < events.length(); i++)
	{
		//add event
		rawEvents[events[i].time].push_back( RawEvent(events[i], initialEventNumber + i) );

		//check for multiple events on the same channel at the same time
		for(j = 0; j < rawEvents[events[i].time].size() - 1; j++)
		{
			//Is the current event's channel already being set?
			if(events[i].channel == rawEvents[events[i].time][j].channel())
			{
				success = false;

				//Error: Multiple events scheduled on channel #24 at time 2.56:
				evtTransferErr << "Error: Multiple events scheduled on channel #" 
					<< events[i].channel << " at time " 
					<< events[i].time << ":" << endl
					<< "       " << rawEvents[events[i].time][j].print() << endl
					<< "       " << rawEvents[events[i].time].back().print() << endl;
			}
		}
		
		//look for the newest event's channel number on this device
		ChannelMap::iterator channel = 
			channels.find(rawEvents[events[i].time].back().channel());
		
		//check that newest event's channel is defined
		if(channel == channels.end())
		{
			success = false;

			//Error: Channel #24 is not defined on this device. Event trace:
			evtTransferErr << "Error: Channel #" 
				<< rawEvents[events[i].time].back().channel()
				<< " is not defined on this device. " 
				<< "       Event trace:" << endl
				<< "       " << rawEvents[events[i].time].back().print() << endl;
		}
		//check that the newest event is of the correct type for its channel
		else if(rawEvents[events[i].time].back().type() != channel->second.outputType)
		{
			success = false;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr 
				<< "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< RawEvent::TValueToStr(channel->second.outputType) << "'. " << endl
				<< "       Event trace:" << endl
				<< "       " << rawEvents[events[i].time].back().print() << endl;
		}
		if(success && rawEvents[events[i].time].back().type() == ValueMeas)	//measurement event
		{
			measurement.time = rawEvents[events[i].time].back().time();
			measurement.channel = rawEvents[events[i].time].back().channel();
			measurement.data._d( channel->second.inputType );

			measurements.push_back( new ParsedMeasurement(measurement, i) );

			rawEvents[events[i].time].back().setMeasurement( &( measurements.back() ) );
		}
	}

	if( !success )
		return false;

	//All events were added successfully.  
	//Now check for device-specific conflicts and errors while parsing.

	do {
		errors = false;	//Each time through the loop any offending events 
						//are removed before trying again. This way all events
						//can generate errors messages before returning.
		try {
			parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
		}
		catch(EventConflictException &eventConflict)
		{
			success = false;
			errors = true;
			//Error: Event conflict. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event conflict. "
				<< eventConflict.printMessage() << endl
				<< "       Event trace:" << endl
				<< "       " << eventConflict.Event1.print() << endl;
			
			if(eventConflict.Event1 != eventConflict.Event2)
			{
				evtTransferErr
				<< "       " << eventConflict.Event2.print() << endl;
			}
			
			//Add to list of conflicting events; this will be sent to the client
			conflictingEvents.insert(eventConflict.Event1.eventNum());
			conflictingEvents.insert(eventConflict.Event2.eventNum());

			//find the latest event associated with this exception
			badEvent = rawEvents.find( eventConflict.lastTime() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(EventParsingException &eventParsing)
		{
			success = false;
			errors = true;
			//Error: Event parsing error. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event parsing error. "
				<< eventParsing.printMessage() << endl
				<< "       Event trace:" << endl
				<< "       " << eventParsing.Event.print() << endl;
			
			//Add to list of unparseable events; this will be sent to the client
			unparseableEvents.insert(eventParsing.Event.eventNum());

			//find the event associated with this exception
			badEvent = rawEvents.find( eventParsing.Event.time() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );	
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(...)	//generic conflict or error
		{
			success = false;
			//Error: Event error or conflict detected. Debug info not available.
			evtTransferErr 
				<< "Error: Event error or conflict detected. " << endl
				<< "       Debug info not available." << endl;

			errors = false;		//break the error loop immediately
		}
	} while(errors);

	if( !success )
		return false;

	//check that all measurements are associated with a SynchronousEvent
	for(i = 0; i < measurements.size(); i++)
	{
		if( !measurements.at(i).isScheduled() )
		{
			success = false;
			evtTransferErr << "Error: The following measurement is not associated with a SynchronousEvent."
				<< endl <<    "       Measurement trace: " << endl
				<< endl <<    "       " << measurements.at(i).print() << endl;
		}
	}

	//check that no measurement is scheduled for more than one SynchronousEvent
	unsigned numberScheduled = 0;
	for(i = 0; i < synchedEvents.size(); i++)
	{
		numberScheduled += synchedEvents.at(i).getNumberOfMeasurements();
	}
	
	if(numberScheduled != measurements.size() )
	{
		success = false;
		evtTransferErr << "Error: Measurement scheduling mismatch. Each ParsedMeasurement must be added" << endl
					   << "       to exactly one SynchronousEvent during parseDeviceEvents(...)." << endl
					   << "       Total Number of ParsedMeasurements: <<" << measurements.size() << ">>" << endl
					   << "       Number added to SynchronousEvents:  <<" << numberScheduled  << ">>" << endl;
	}

	//check that all event times are positive
	//shift all event times forward if they are negative? tell server the offset? include ping offset?

	//sort in time order
	synchedEvents.sort();

	int synchedEventsLength = static_cast<int>( synchedEvents.size() );
	
	//check that synchedEvents has only one entry for each time
	for(int k = 0; k < synchedEventsLength - 1; k++)
	{
		if( synchedEvents.at(k).getTime() == synchedEvents.at(k + 1).getTime() )
		{
			evtTransferErr 
			<< "Error: Multiple parsed events are scheduled " << endl
			<< "       to occur at the same time on device '" << getDeviceName() << "'." << endl
			<< "       Events that occur on multiple channels at the same time must be grouped" << endl
			<< "       into a single SynchonousEvent object during STI_Device::parseDeviceEvents(...)." << endl
			<< "       Only one SynchonousEvent is allowed at any time." << endl;

			success = false;
			break;
		}
	}

	//Assign event numbers
	for(i = 0; i < synchedEvents.size(); i++)
	{
		synchedEvents.at(i).setEventNumber( i );
	}

	cout << "Event Transfer Error:" << endl << evtTransferErr.str() << endl;

	return success;
}


void STI_Device::loadEvents()
{
	if(loadEventsThread != 0)
	{
//		loadEventsThread->exit();
//		loadEventsThread = 0;
	}

	//Loading takes place in its own thread to allow for "double buffering".
	//Loading can continue to go on in the background while the first events run.
	//This also allows the calling function on the server to return quickly so
	//that the next device's event's can be loaded.
	omni_thread::create(loadDeviceEventsWrapper, (void*)this, 
		omni_thread::PRIORITY_HIGH);

	//change load status to loading
}

void STI_Device::loadDeviceEventsWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	
	if( !thisObject->changeStatus(EventsLoading) )
		return;

	thisObject->loadDeviceEvents();
}


void STI_Device::loadDeviceEvents()
{
	for(unsigned i = 0; i < synchedEvents.size(); i++)
	{
		synchedEvents.at(i).setupEvent();
		synchedEvents.at(i).loadEvent();
	}
	changeStatus(EventsLoaded);

	//uInt32 waitTime = 0;
	//for(unsigned i = 0; i < synchedEvents.size(); i++)
	//{
	//	do {
	//		waitTime = synchedEvents.at(i).loadEvent();
	//		
	//		cout << "loadEvent() " << getTDevice().deviceID << ": " << synchedEvents.at(i).getTime() << endl;

	//		//sleep for waitTime
	//		if(waitTime > 0)
	//		{
	//			changeStatus(EventsLoaded);		//change load status
	//			loadEventsThread->sleep(static_cast<unsigned long>(waitTime));	//waitTime in seconds
	//		}

	//	} while(waitTime > 0);
	//}
}

void STI_Device::playEvents()
{
	if(playEventsThread != 0)
	{
//		playEventsThread->exit();
//		playEventsThread = 0;
	}

	//Playing takes place in its own thread because playEvents() must return promptly
	//to allow the server to call playEvents() on other devices.  This allows playing
	//to occur in parallel on all devices.
	playEventsThread = omni_thread::create(playDeviceEventsWrapper, (void*)this, 
		omni_thread::PRIORITY_NORMAL);
}

void STI_Device::playDeviceEventsWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	thisObject->playDeviceEvents();
}


void STI_Device::playDeviceEvents()
{

//cerr << "changeStatus=" << changeStatus(Running) << endl;


	if( !changeStatus(Running) )
{
cerr << "changeStatus=BAD" << endl;
		return;
}

	time.reset();
	measuredEventNumber = 0;

//	cout << "playEvent() " << getTDevice().deviceName << " start time: " << time.getCurrentTime() << endl;

	cout << "STI_Device::playDeviceEvents(): " << getDeviceName() << " synchedEvents.size() = " << synchedEvents.size() << endl;
	for(unsigned i = 0; i < synchedEvents.size(); i++)
	{
		waitForEvent(i);
		if(stopPlayback)
			break;
		
		synchedEvents.at(i).playEvent();

//int x=0;
//while(x != 2)
//{		
//cerr << "Played.  Measure?" << endl;
//cin >> x;
//}

		synchedEvents.at(i).collectMeasurementData();
		measuredEventNumber = i;
	//cout << "playEvent() " << getTDevice().deviceName << ": " << synchedEvents.at(i).getTime() << " c=" << time.getCurrentTime() << endl;
		
	}
	
	cout << getDeviceName() << ": Play finished." << endl;

	//set play status to Finished
	changeStatus(EventsLoaded);
}

void STI_Device::waitForEvent(unsigned eventNumber)
{

//	cerr << "STI_Device::waitForEvent()" << endl;


	Int64 wait = static_cast<Int64>( 
			synchedEvents.at(eventNumber).getTime() - time.getCurrentTime() );

	if(wait > 0 && !stopPlayback)
	{
		//calculate absolute time to wake up
		omni_thread::get_time(&wait_s, &wait_ns, 
			Clock::get_s(wait), Clock::get_ns(wait));

		playEventsMutex->lock();
		{
			playEventsTimer->timedwait(wait_s, wait_ns);	//put thread to sleep
		}
		playEventsMutex->unlock();
	}
}


unsigned STI_Device::getMeasuredEventNumber() const
{
	return measuredEventNumber;
}

void STI_Device::parseDeviceEventsDefault(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
{
	//This event parser works well for non time critical devices (i.e. non-FPGA devices) and devices that
	//are fundamentally serial. Events scheduled for the same time will instead be played sequentially
	//using calls to STI_Device::writeChannel(...).
	
	RawEventMap::const_iterator iter;
	unsigned i;

	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		eventsOut.push_back( 
			new STI_Device::PsuedoSynchronousEvent(iter->first, iter->second, this) );

		// register all measurement events
		for(i = 0; i < iter->second.size(); i++)
		{
			if( iter->second.at(i).type() == ValueMeas )	// measurement event
				eventsOut.back().addMeasurement( iter->second.at(i) );
		}
	}
}



//*********** SynchronousEvent subclass functions ****************//
void STI_Device::SynchronousEvent::addMeasurement(const RawEvent& measurementEvent)
{
	ParsedMeasurement* measurement = measurementEvent.getMeasurement();

	if( measurement != 0 )
	{
		eventMeasurements.push_back( measurement );
		measurement->setScheduleStatus(true);
cerr << "addMeasurement Check: " << eventMeasurements.at(0)->time() << endl;
		//eventMeasurements.back()->setScheduleStatus(true);
	}
}


void STI_Device::PsuedoSynchronousEvent::playEvent()
{
	for(unsigned i = 0; i < events_.size(); i++)
		device_->writeChannel( events_.at(i) );
}
void STI_Device::PsuedoSynchronousEvent::collectMeasurementData()
{
	for(unsigned i = 0; i < eventMeasurements.size(); i++)
	{
		if( !eventMeasurements.at(i)->isMeasured() )
		{
			device_->readChannel( *( eventMeasurements.at(i) ) );
		}
	}
}


//*********** State machine functions ****************//
bool STI_Device::changeStatus(DeviceStatus newStatus)
{
	bool allowedTransition = false;
	switch(deviceStatus) 
	{
	case EventsEmpty:
		allowedTransition = 
			(newStatus == EventsLoading);
		break;
	case EventsLoading:
		allowedTransition = 
			(newStatus == EventsEmpty) ||
			(newStatus == EventsLoaded);
		break;
	case EventsLoaded:
		allowedTransition = 
			(newStatus == EventsEmpty) ||
			(newStatus == EventsLoading) ||
			(newStatus == Running);
		break;
	case Running:
		allowedTransition = 
			(newStatus == EventsLoaded);
		break;
	default:
		break;
	}
	if(allowedTransition)
	{
		deviceStatus = newStatus;
		updateState();
	}
	return allowedTransition;
}

void STI_Device::updateState()
{
	switch(deviceStatus) 
	{
	case EventsEmpty:
		stopPlayback = true;
		eventsAreLoaded = false;
		break;
	case EventsLoading:
		stopPlayback = true;
		eventsAreLoaded = false;
		break;
	case EventsLoaded:
		stopPlayback = true;
		eventsAreLoaded = true;
		break;
	case Running:
		stopPlayback = false;
		eventsAreLoaded = true;
		break;
	}
}




bool STI_Device::eventsLoaded()
{
	return eventsAreLoaded;
}

void STI_Device::stop()
{
	switch(deviceStatus) 
	{
	case EventsLoading:
		changeStatus(EventsEmpty);
		break;
	case Running:
		changeStatus(EventsLoaded);
		playEventsTimer->signal();	//wakes up the play thread if sleeping
		stopEventPlayback();	//device-specific stop function
		break;
	default:
		break;
	}
}

//*********** Device setup helper functions ****************//
void STI_Device::addInputChannel(unsigned short Channel, TData InputType)
{
	addChannel(Channel, Input, InputType, ValueMeas);
}


void STI_Device::addOutputChannel(unsigned short Channel, TValue OutputType)
{
	addChannel(Channel, Output, DataNone, OutputType);
}


bool STI_Device::addChannel(unsigned short Channel, TChannelType Type, 
							TData InputType, TValue OutputType)
{
	bool valid = true;
	STI_Server_Device::TDeviceChannel tChannel;

	if(Type == Input && OutputType != ValueMeas)
	{
		valid = false;
	}
	if(Type == Output && InputType != DataNone)
	{
		valid = false;
	}

	//check for duplicate channel number
	ChannelMap::iterator duplicate = channels.find(Channel);

	if(duplicate != channels.end())
	{
		valid = false;

		cerr << "Error: Duplicate channel number in device " 
			<< getDeviceName() << endl;
	}

	if(valid)
	{
		tChannel.channel    = Channel;
		tChannel.type       = Type;
		tChannel.inputType  = InputType;
		tChannel.outputType = OutputType;

		channels[Channel] = tChannel;
	}
	else
	{
		cerr << "Error: Invalid channel specification in device " 
			<< getDeviceName() << endl;
	}

	return valid;
}




void STI_Device::addAttributeUpdater(AttributeUpdater* updater)
{
	attributeUpdaters.push_back(updater);
}


void STI_Device::enableStreaming(unsigned short Channel, string SamplePeriod, 
								 string BufferDepth)
{
	unsigned i;
	bool channelExists = false;
	stringstream chName;
	chName << "Ch" << Channel;
	string attrib = chName.str();

	for(i = 0; i < channels.size(); i++)
	{
		if(channels[i].channel == Channel)
			channelExists = true;
	}

	if(channelExists)
	{
		//add a (sleeping) thread to the streamingThreads vector/map[Channel]
		//each thread calls measureChannel(itsChannel, meas) while itsAlive()
		streamingBuffers[Channel] = StreamingBuffer(this, Channel, false);
		streamingBuffers[Channel].thread->id();

		attributes[attrib + "_SamplePeriod"] = Attribute(SamplePeriod);
		updateStreamAttribute(attrib + "_SamplePeriod", SamplePeriod);

		attributes[attrib + "_BufferDepth"] = Attribute(BufferDepth);
		updateStreamAttribute(attrib + "_BufferDepth", BufferDepth);

		attributes[attrib + "_InputStream" ] = Attribute("Enabled", "Enabled, Disabled");
		string streamState = "Enabled";
		updateStreamAttribute(attrib + "_InputStream", streamState);
	}
}


void STI_Device::addMutualPartnerDevice(string partnerName, string IP, short module, string deviceName)
{
	TDeviceMap::iterator partner = addedPartners.find(partnerName);

	if( partner == addedPartners.end() )	//this is an original partnerName
	{
		addPartnerDevice(partnerName, IP, module, deviceName);
		mutualPartners.push_back(partnerName);
	}
	else
	{
		cerr << "Error adding partner '" << partnerName 
			<< "'." << endl 
			<< "<" << IP << "/" << module << "/" << deviceName << ">" << endl
			<< "This partner name is already in use and will be ignored." << endl;
	}

}

void STI_Device::addPartnerDevice(string partnerName, string IP, short module, string deviceName)
{
	TDeviceMap::iterator partner = addedPartners.find(partnerName);

	if( partner == addedPartners.end() )	//this is an original partnerName
	{
		addedPartners[partnerName].address    = CORBA::string_dup( IP.c_str() );
		addedPartners[partnerName].moduleNum  = module;
		addedPartners[partnerName].deviceName = CORBA::string_dup( deviceName.c_str() );
	}
	else
	{
		cerr << "Error adding partner '" << partnerName 
			<< "'." << endl 
			<< "<" << IP << "/" << module << "/" << deviceName << ">" << endl
			<< "This partner name is already in use and will be ignored." << endl;
	}
}

void STI_Device::addPartnerDevice(string partnerName, string deviceID)
{
	map<string, string>::iterator it = requiredPartners.find(partnerName);
	
	if(it == requiredPartners.end())	//this is an original partnerName
	{
		requiredPartners[partnerName] = deviceID;
	}
	else
	{
		cerr << "Error adding partner '" << partnerName 
			<< "'. This partner name is already in use." << endl;
	}
}

void STI_Device::addLocalPartnerDevice(std::string partnerName, const STI_Device& partnerDevice)
{
	//ment to be called from outside this STI_Device (i.e., in main.cpp )
	
	//overrides addPartnerDevice() if partnerName is duplicated
	TDeviceMap::iterator partner = addedPartners.find( partnerName );

	if( partner != addedPartners.end() )	//this is not an original partnerName
	{
		addedPartners.erase( partner );		//removes the partnerName from the list of required network partners
	}

	commandLineServant->getRegisteredPartners().insert(partnerName, 
		new PartnerDevice(partnerName, partnerDevice.getCommandLineServant()) );

}




void STI_Device::convertArgs(int argc, char** argvInput, std::vector<std::string>& argvOutput) const
{
	for(int i=0; i < argc; i++)
		argvOutput.push_back( std::string( argvInput[i] ) );
}

void STI_Device::splitString(string inString, string delimiter, vector<string>& outVector) const
{
	string::size_type tBegin = 0;
	string::size_type tEnd = 0;

	// splits the sting at every delimiter
	while(tEnd != string::npos)
	{
		tBegin = inString.find_first_not_of(delimiter, tEnd);
		tEnd = inString.find_first_of(delimiter, tBegin);
		
		if(tBegin != string::npos)
			outVector.push_back(inString.substr(tBegin, tEnd - tBegin));
		else
			outVector.push_back("");
	}
}





//*********** Getter functions ****************//
const AttributeMap& STI_Device::getAttributes() const
{
	return attributes;
}

const ChannelMap& STI_Device::getChannels() const
{
	return channels;
}



const std::map<std::string, std::string>& STI_Device::getRequiredPartners() const
{
	return requiredPartners;
}
const std::vector<std::string>& STI_Device::getMutualPartners() const
{
	return mutualPartners;
}
const PartnerDeviceMap& STI_Device::getRegisteredPartners() const
{
	return commandLineServant->getRegisteredPartners();
}

const STI_Server_Device::TDevice& STI_Device::getTDevice() const
{
	return tDevice;
}
std::string STI_Device::getDeviceName() const
{
	return deviceName;
}


std::string STI_Device::getServerName() const
{
	if(serverConfigureFound)
		return serverName;
	else
		return "NOT FOUND";
}
ParsedMeasurementVector& STI_Device::getMeasurements()
{
	return measurements;
}


CommandLine_i* STI_Device::getCommandLineServant() const
{
	return commandLineServant;
}

STI_Device::DeviceStatus STI_Device::getDeviceStatus() const
{
	return deviceStatus;
}

STI_Device::SynchronousEventVector& STI_Device::getSynchronousEvents()
{
	return synchedEvents;
}
std::string STI_Device::dataTransferErrorMsg() const
{
	return dataTransferError.str();
}


std::string STI_Device::eventTransferErr() const
{
	return evtTransferErr.str();
}


