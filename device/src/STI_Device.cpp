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

#include <cassert>
#include <sstream>
#include <string>
#include <map>
using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;


STI_Device::STI_Device(
		ORBManager *   orb_manager, 
		std::string    DeviceName, 
		std::string    IPAddress, 
		unsigned short ModuleNumber,
		uInt32 FPGA_BufferAddress
		) : orbManager(orb_manager), deviceName(DeviceName)
{
	isFPGADevice = true;
	this->FPGA_BufferAddress = FPGA_BufferAddress;

	init(IPAddress, ModuleNumber);
}

STI_Device::STI_Device(
		ORBManager *   orb_manager, 
		std::string    DeviceName, 
		std::string    IPAddress, 
		unsigned short ModuleNumber
		) : orbManager(orb_manager), deviceName(DeviceName)
{
	isFPGADevice = false;

	init(IPAddress, ModuleNumber);
}

void STI_Device::init(std::string IPAddress, unsigned short ModuleNumber)
{

	// servant names -- the STI_Server must look for these same names
	configureObjectName    = "Configure.Object";
	dataTransferObjectName = "DataTransfer.Object";
	commandLineObjectName = "CommandLine.Object";
	deviceControlObjectName = "DeviceControl.Object";

	//servants
	configureServant = new Configure_i(this);
	dataTransferServant = new DataTransfer_i(this);
	commandLineServant = new CommandLine_i(this);
	deviceControlServant = new DeviceControl_i(this);

	dummyPartner = new PartnerDevice();

	//	measurements = new STI_Server_Device::TMeasurementSeqSeq();

	//TDevice
	tDevice = new STI_Server_Device::TDevice;	//_var variable does not need to be deleted
	tDevice->deviceName = getDeviceName().c_str();
	tDevice->address = IPAddress.c_str();
	tDevice->moduleNum = ModuleNumber;

	serverConfigureFound = false;
	registedWithServer = false;
	registrationAttempts = 0;

	mainLoopMutex = new omni_mutex();

	// Aquire a reference to ServerConfigure from the NameService.
	// When found, register this Device with the server and acquire 
	// a unique deviceID.
	omni_thread::create(acquireServerReferenceWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	// Hold until serverConfigureFound and registedWithServer.
	// Register servants with the Name Service, then activate the Device
	// using ServerConfigure::activateDevice(deviceID)
	omni_thread::create(initServerWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

//reaquireServerReference()

STI_Device::~STI_Device()
{
	//remove this Device from the Server
	ServerConfigureRef->removeDevice(tDevice->deviceID);

	delete configureServant;
	delete dataTransferServant;
	delete commandLineServant;
	delete deviceControlServant;

	delete dummyPartner;
	delete mainLoopMutex;
}

void STI_Device::deviceMainWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;

	//while(thisObject->deviceMain(					//pure virtual
	//	thisObject->orbManager->getArgc(), 
	//	thisObject->orbManager->getArgv() )) {};

	bool run = true;
	while(run)
	{
		//mutex ensures that execute(argc, argv) is not running
//		thisObject->mainLoopMutex->lock();			
		{
			run = thisObject->deviceMain(			//pure virtual
			thisObject->orbManager->getArgc(), 
			thisObject->orbManager->getArgv() );
		}
//		thisObject->mainLoopMutex->unlock();

	};  
}


void STI_Device::initServerWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->initServer();
}

void STI_Device::initServer()
{
	// Wait until the ServerConfigure Object is found and the DeviceID
	// has been acquired.
	while(!serverConfigureFound) {}
	while(!registedWithServer) {}	// Have deviceID

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

	// setup the channels and send them to the server
	try {
		setChannels();
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

	// setup partner devices before activation
	definePartnerDevices();			//pure virtual


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
	
	//deviceMain loop
	mainThread = omni_thread::create(deviceMainWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}


void  STI_Device::setChannels()
{
	using STI_Server_Device::TDeviceChannel;
	using STI_Server_Device::TDeviceChannelSeq;
	using STI_Server_Device::TDeviceChannelSeq_var;

	measurements.clear();
	channels.clear();

	defineChannels();	//pure virtual

	unsigned i;
	channelMap::iterator it;

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
	if( ! ServerConfigureRef->setChannels(tDevice->deviceID, channelSeq))
	{
		cerr << "Error when sending channels to the server:" 
			<< " channels are invalid." << endl;
	}
}


void STI_Device::acquireServerReferenceWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->acquireServerReference();
}


void STI_Device::acquireServerReference()
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


const ParsedMeasurementMap& STI_Device::getMeasurements() const
{
	return measurements;
}

std::string STI_Device::dataTransferErrorMsg()
{
	return dataTransferError.str();
}


void STI_Device::initializeAttributes()
{
	bool success = true;

	attributeMap::iterator it;

	if(attributes.empty())
	{
		defineAttributes();	// pure virtual

		for(it = attributes.begin(); it != attributes.end(); it++)
		{
			success &= setAttribute(it->first, it->second.value());
		}
	}

	if(!success)
	{
		cerr << "Error initializing attributes." << endl;
	}
}

attributeMap const * STI_Device::getAttributes()
{
	// Initialize to defaults the first time this is called
	initializeAttributes();

	return &attributes;
}


bool STI_Device::setAttribute(string key, string value)
{
	// Initialize to defaults the first time this is called
	initializeAttributes();

	if( attributes.empty() )
		return false;	//There are no defined attributes

	attributeMap::iterator attrib = attributes.find(key);

	if( attrib == attributes.end() )
		return false;	// Attribute not found

	if( !attrib->second.isAllowed(value) )
		return false;	//attribute not in list of allowed values

	//Pass the 'value' string by reference updateStreamAttribute.
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
	if( updateAttribute(key, newValue) )	//pure virtual
	{
		attrib->second.setValue(newValue);
		return true;
	}
	return false;
}

void STI_Device::enableStreaming(unsigned short Channel, 
								 string         SamplePeriod, 
								 string         BufferDepth)
{
	// Setup other attributes the first time this is called.
	// This ensures that defineAttributes() gets called.
	initializeAttributes();

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
		streamingBuffers[Channel] = new StreamingBuffer(this, Channel, false);
		streamingBuffers[Channel]->thread->id();

		attributes[attrib + "_SamplePeriod"] = Attribute(SamplePeriod);
		updateStreamAttribute(attrib + "_SamplePeriod", SamplePeriod);

		attributes[attrib + "_BufferDepth"] = Attribute(BufferDepth);
		updateStreamAttribute(attrib + "_BufferDepth", BufferDepth);

		attributes[attrib + "_InputStream" ] = Attribute("Enabled", "Enabled, Disabled");
		string streamState = "Enabled";
		updateStreamAttribute(attrib + "_InputStream", streamState);
	}
}


bool STI_Device::isStreamAttribute(string key)
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

bool STI_Device::updateStreamAttribute(string key, string & value)
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
			streamingBuffers[Channel]->setStreamingStatus(true);
		}
		if(value.compare("Disabled") == 0)
		{
			streamingBuffers[Channel]->setStreamingStatus(false);
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
		return streamingBuffers[Channel]->setSamplePeriod(samplePeriod);
	}
	if(key.compare(chNum.str() + "_BufferDepth") == 0)
	{
		unsigned int bufferDepth;
		if( !stringToValue(value, bufferDepth) )
			return false;
		value = valueToString(bufferDepth);	//use the exact result of the conversion

		cerr << "Buffer depth: " << value << " = " << bufferDepth << endl;

		return streamingBuffers[Channel]->setBufferDepth(bufferDepth);
	}

	return false;	//Not a stream attribute
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

//virtual void measureChannel(unsigned short Channel, TDataMixed & data)=0;   Actually how a measurement is made using hardware

//void STI_Device::addPartnerDevice(std::string deviceName)


void STI_Device::addPartnerDevice(string partnerName, string IP, short module, string deviceName)
{
	string deviceID;
	
	STI_Server_Device::TDevice partnerTDevice;
	partnerTDevice.address    = CORBA::string_dup( IP.c_str() );
	partnerTDevice.moduleNum  = module;
	partnerTDevice.deviceName = CORBA::string_dup( deviceName.c_str() );

	try {
		deviceID = ServerConfigureRef->generateDeviceID(partnerTDevice);
		addPartnerDevice(partnerName, deviceID);
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to add a partner device: " 
			<< endl << "--> addPartnerDevice(" << partnerName << ", " 
			<< IP << ", " << module << ", " << deviceName << ")" << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " when trying to add a partner device: " 
			<< endl << "--> addPartnerDevice(" << partnerName << ", " 
			<< IP << ", " << module << ", " << deviceName << ")" << endl;
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


PartnerDevice& STI_Device::partnerDevice(std::string partnerName)
{
	// requiredPartners:     partnerName => deviceID
	// registeredPartners:   deviceID => PartnerDevice

	map<string, string>::iterator partner = requiredPartners.find(partnerName);

	if(partner == requiredPartners.end())	// invalid partnerName
	{
		cerr << "Error: The partner '" << partnerName 
			<< "' is not a partner of this device. " << endl
			<< "All partners must be added in definePartnerDevices()." << endl;
		return *dummyPartner;
	}
	else
	{
		// search to see if the partner has been registered

		map<string, PartnerDevice> & partnerMap = 
			commandLineServant->registeredPartners;

		std::map<std::string, PartnerDevice>::iterator it = 
			partnerMap.find(partner->second);	//deviceID

		if(it == partnerMap.end())	// this partner has not been registered
			return *dummyPartner;
		else
			return it->second;
	}
}

string STI_Device::execute(string args)
{
	std::string result = "";
	vector<string> arguments;
	// Using the C++ main(int argc, char** argv) convention: the 
	// first argument is the program's name (in this case the device's name).
	arguments.push_back(deviceName);

	splitString(args, " ", arguments);

	char **argv = new char*[arguments.size()];
	unsigned i;

	for(i = 0; i < arguments.size(); i++)
	{
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


void STI_Device::addInputChannel(unsigned short Channel, TData InputType)
{
	// Each input channel gets its own measurment vector
	measurements[Channel].clear();

	addChannel(Channel, Input, InputType, ValueMeas);
}


void STI_Device::addOutputChannel(unsigned short Channel, TValue OutputType)
{
	addChannel(Channel, Output, DataNone, OutputType);
}


bool STI_Device::addChannel(
		unsigned short		Channel, 
		TChannelType		Type, 
		TData				InputType, 
		TValue				OutputType)
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
	channelMap::iterator duplicate = channels.find(Channel);

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


const channelMap& STI_Device::getChannels() const
{
	return channels;
}


const std::map<std::string, std::string> * STI_Device::getRequiredPartners() const
{
	return &requiredPartners;
}

void STI_Device::splitString(string inString, string delimiter, vector<string> & outVector)
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

const STI_Server_Device::TDevice & STI_Device::getTDevice() const
{
	return tDevice;
}

bool STI_Device::loadEvents() // unnecessary if transferEvents() calls loadDeviceEvents()
{
	if(isFPGADevice)
	{
		ParsedEventMap::iterator iter;
		for(iter = parsedEvents.begin(); iter != parsedEvents.end(); iter++)
		{
		//	iter->second
		}
	}
	else	//non-FPGA device; nothing to do
		return true;
}

bool STI_Device::transferEvents(const STI_Server_Device::TDeviceEventSeq &events)
{
	unsigned i,j;
	ParsedEventMap::iterator badEvent;
	STI_Server_Device::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	numMeasurementEvents = 0;
	parsedEvents.clear();

	for(i = 0; i < events.length(); i++)
	{
		//add event
		parsedEvents[events[i].time].push_back( ParsedEvent(events[i], i) );

		//check for multiple events on the same channel at the same time
		for(j = 0; j < parsedEvents[events[i].time].size() - 1; j++)
		{
			//Is the current event's channel already being set?
			if(events[i].channel == parsedEvents[events[i].time][j].channel())
			{
				success = false;

				//Error: Multiple events scheduled on channel #24 at time 2.56:
				evtTransferErr << "Error: Multiple events scheduled on channel #" 
					<< events[i].channel << " at time " 
					<< events[i].time << ":" << endl
					<< "       " << parsedEvents[events[i].time][j].print() << endl
					<< "       " << parsedEvents[events[i].time].back().print() << endl;
			}
		}
		
		//look for the newest event's channel number on this device
		channelMap::iterator channel = 
			channels.find(parsedEvents[events[i].time].back().channel());
		
		//check that newest event's channel is defined
		if(channel == channels.end())
		{
			success = false;

			//Error: Channel #24 is not defined on this device. Event trace:
			evtTransferErr << "Error: Channel #" 
				<< parsedEvents[events[i].time].back().channel()
				<< " is not defined on this device. Event trace:" << endl
				<< "       " << parsedEvents[events[i].time].back().print() << endl;
		}
		//check that the newest event is of the correct type for its channel
		else if(parsedEvents[events[i].time].back().type() != channel->second.outputType)
		{
			success = false;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr << "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< ParsedEvent::TValueToStr(channel->second.outputType) << "'. Event trace:" << endl
				<< "       " << parsedEvents[events[i].time].back().print() << endl;
		}
		if(success && parsedEvents[events[i].time].back().type() == ValueMeas)	//measurement event
		{
			numMeasurementEvents++;

			measurement.time = parsedEvents[events[i].time].back().time();
			measurement.channel = parsedEvents[events[i].time].back().channel();
			measurement.data._d( channel->second.inputType );

			measurements[measurement.channel].push_back( ParsedMeasurement(measurement, i) );
		}
	}

	if(success)		//all events were added successfully.  Now check for conflicts and errors while loading.
	{
		errors = false;
		do {
			try {
//				errors = !parseDeviceEvents(parsedEvents);
			}
			catch(EventConflictException &eventConflict)
			{
				success = false;
				errors = true;
				//Error: Event conflict. <Device Specific Message>
				//       Event trace:
				evtTransferErr << "Error: Event conflict. "
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
				badEvent = parsedEvents.find( eventConflict.lastTime() );

				//remove all previous events from the map
				if(badEvent != parsedEvents.end())
					parsedEvents.erase( parsedEvents.begin(), badEvent );
				else	//this should never happen
					errors = false;		//break the error loop immediately
			}
			catch(EventParsingException &eventParsing)
			{
				success = false;
				errors = true;
				//Error: Event parsing error. <Device Specific Message>
				//       Event trace:
				evtTransferErr << "Error: Event parsing error. "
					<< eventParsing.printMessage() << endl
					<< "       Event trace:" << endl
					<< "       " << eventParsing.Event.print() << endl;
				
				//Add to list of unparseable events; this will be sent to the client
				unparseableEvents.insert(eventParsing.Event.eventNum());

				//find the event associated with this exception
				badEvent = parsedEvents.find( eventParsing.Event.time() );

				//remove all previous events from the map
				if(badEvent != parsedEvents.end())
					parsedEvents.erase( parsedEvents.begin(), badEvent );
				else	//this should never happen
					errors = false;		//break the error loop immediately
			}
			catch(...)	//generic conflict or error
			{
				success = false;
				//Error: Event error or conflict detected. Debug info not available.
				evtTransferErr << "Error: Event error or conflict detected. " 
					<< "Debug info not available." << endl;

				errors = false;		//break the error loop immediately
			}
		} while(errors);
	}

	return success;
}

std::string STI_Device::eventTransferErr()
{
	return evtTransferErr.str();
}
