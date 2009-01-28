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
	commandLineServant = new CommandLine_i(this);
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

	attributes.clear();
	channels.clear();
	requiredPartners.clear();
	measurements.clear();

	mainLoopMutex = new omni_mutex();
	
	playEventsMutex = new omni_mutex();
	playEventsTimer = new omni_condition(playEventsMutex);
	
	loadEventsThread = 0;
	playEventsThread = 0;

	// Aquire a reference to ServerConfigure from the NameService.
	// When found, register this Device with the server and acquire 
	// a unique deviceID.
	omni_thread::create(registerDeviceWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	// Hold until serverConfigureFound and registedWithServer.
	// Register servants with the Name Service, then activate the Device
	// using ServerConfigure::activateDevice(deviceID)
	omni_thread::create(activateDeviceWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	//deviceMain loop
	mainThread = omni_thread::create(deviceMainWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

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


void STI_Device::activateDeviceWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->activateDevice();
}

void STI_Device::registerDeviceWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->registerDevice();
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
	while(!serverConfigureFound) {}   //Wait for ServerConfigure obj reference
	while(!registedWithServer) {}     //Wait for deviceID string
	
	//Register this device's servants with the Name Service
	registerServants();

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
	definePartnerDevices();			//pure virtual
	
	// activate device
	try {
		cout << "Activating: " << tDevice->deviceID << endl;
		ServerConfigureRef->activateDevice(tDevice->deviceID);
		cout << "Activated!!!!!!!!!!!: " << tDevice->deviceID << endl;
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

	for(it = attributes.begin(); it != attributes.end(); it++)
	{
		success &= setAttribute(it->first, it->second.value());
	}

	if(!success)
	{
		cerr << "Error initializing attributes." << endl;
	}
}


bool STI_Device::setAttribute(string key, string value)
{
	if( attributes.empty() )
		return false;	//There are no defined attributes

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
	if( updateAttribute(key, newValue) )	//pure virtual
	{
		attrib->second.setValue(newValue);
		return true;
	}
	return false;
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

//separate measurement buffer for each instance of DataTransfer
//streamBuffer buffers measuremnts for each (enabled) streaming channel
//buffer for each streaming channel is a circular queue
//buffer for each non-streaming channel is a vector
//streaming channels must default to vector buffer behavior if timing critical
//consumer threads block the circular queue while reading buffer, preventing overwritting of the first element
//However it only prevents queue overwrites -- data is still acquired with high priority.
//The measuremnt thread must regain overwrite control before another consumer thread does or else the queue will grow indefinitely
//Streaming data may be lossy, but it is time-stamped.

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
	measurements[Channel].clear();		//adds a new vector and clears it

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


const AttributeMap& STI_Device::getAttributes() const
{
	return attributes;
}


const ChannelMap& STI_Device::getChannels() const
{
	return channels;
}


const ParsedMeasurementMap& STI_Device::getMeasurements() const
{
	return measurements;
}

const std::map<std::string, std::string>& STI_Device::getRequiredPartners() const
{
	return requiredPartners;
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


// load status: loading, sleeping, all loaded
//	loadEventsThread->exit();
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
	loadEventsThread = omni_thread::create(loadDeviceEventsWrapper, (void*)this, 
		omni_thread::PRIORITY_HIGH);

	//change load status to loading
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
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->playDeviceEvents();
}

void STI_Device::playDeviceEvents()
{
	if( !changeStatus(Running) )
		return;

	time.reset();

	cout << "playEvent() " << getTDevice().deviceName << " start time: " << time.getCurrentTime() << endl;

	unsigned long wait_s;
	unsigned long wait_ns;
	Int64 wait;

	for(unsigned i = 0; i < synchedEvents.size(); i++)
	{
		wait = static_cast<Int64>( 
			synchedEvents.at(i).getTime() - time.getCurrentTime() );

		if(wait > 0)
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
	//	while(time < t_goal && !stopPlayback) {};		//busy wait;  TODO: put thread sleep.

		if(stopPlayback)
			break;

		synchedEvents.at(i).playEvent();
		
		cout << "playEvent() " << getTDevice().deviceName << ": " << synchedEvents.at(i).getTime() << " c=" << time.getCurrentTime() << endl;
	}

	//set play status to Finished
	changeStatus(EventsLoaded);
}

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
		break;
	case EventsLoading:
		stopPlayback = true;
		break;
	case EventsLoaded:
		stopPlayback = true;
		break;
	case Running:
		stopPlayback = false;
		break;
	}
}

bool STI_Device::transferEvents(const STI_Server_Device::TDeviceEventSeq& events)
{
	unsigned i,j;
	RawEventMap::iterator badEvent;
	STI_Server_Device::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	numMeasurementEvents = 0;
	rawEvents.clear();
	synchedEvents.clear();

	//Move the events from TDeviceEventSeq 'events' (provided by server) to
	//the raw event list 'rawEvents'.  Check for general event errors.
	for(i = 0; i < events.length(); i++)
	{
		//add event
		rawEvents[events[i].time].push_back( RawEvent(events[i], i) );

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
				<< " is not defined on this device. Event trace:" << endl
				<< "       " << rawEvents[events[i].time].back().print() << endl;
		}
		//check that the newest event is of the correct type for its channel
		else if(rawEvents[events[i].time].back().type() != channel->second.outputType)
		{
			success = false;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr << "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< RawEvent::TValueToStr(channel->second.outputType) << "'. Event trace:" << endl
				<< "       " << rawEvents[events[i].time].back().print() << endl;
		}
		if(success && rawEvents[events[i].time].back().type() == ValueMeas)	//measurement event
		{
			numMeasurementEvents++;

			measurement.time = rawEvents[events[i].time].back().time();
			measurement.channel = rawEvents[events[i].time].back().channel();
			measurement.data._d( channel->second.inputType );

			measurements[measurement.channel].push_back( ParsedMeasurement(measurement, i) );
		}
	}

	if( !success )
		return false;

	//All events were added successfully.  
	//Now check for device-specific conflicts and errors while parsing.

	errors = false;
	do {
		try {
			parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
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
			badEvent = rawEvents.find( eventConflict.lastTime() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
				rawEvents.erase( rawEvents.begin(), badEvent );
			else	//this should never happen
				return false;		//break the error loop immediately
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
			badEvent = rawEvents.find( eventParsing.Event.time() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
				rawEvents.erase( rawEvents.begin(), badEvent );
			else	//this should never happen
				return false;		//break the error loop immediately
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

	//sort in time order
	synchedEvents.sort();

	int synchedEventsLength = static_cast<int>( synchedEvents.size() );
	
	//check that synchedEvents has only one entry for each time
	for(int k = 0; k < synchedEventsLength - 1; k++)
	{
		if( synchedEvents.at(k) == synchedEvents.at(k + 1) )
		{
			evtTransferErr << "Error: Multiple parsed events are scheduled " << endl
			<< "to occur at the same time on device '" << getDeviceName() << "'." << endl
			<< "Events that occur on multiple channels at the same time must be grouped" << endl
			<< "into a single SynchonousEvent object during STI_Device::parseDeviceEvents(...)." << endl
			<< "Only one SynchonousEvent is allowed at any time." << endl;

			success = false;
			break;
		}
	}

	cout << "Event Transfer Error:" << endl << evtTransferErr.str() << endl;

	return success;
}

void STI_Device::loadDeviceEventsWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->loadDeviceEvents();
}

void STI_Device::loadDeviceEvents()
{
	if( !changeStatus(EventsLoading) )
		return;

	uInt32 waitTime = 0;
	for(unsigned i=0; i < synchedEvents.size(); i++)
	{
		do {
			waitTime = synchedEvents.at(i).loadEvent();
			
			cout << "loadEvent() " << getTDevice().deviceID << ": " << synchedEvents.at(i).getTime() << endl;

			//sleep for waitTime
			if(waitTime > 0)
			{
				changeStatus(EventsLoaded);		//change load status
				loadEventsThread->sleep(static_cast<unsigned long>(waitTime));	//waitTime in seconds
			}

		} while(waitTime > 0);
	}

	changeStatus(EventsLoaded);
}


std::string STI_Device::dataTransferErrorMsg() const
{
	return dataTransferError.str();
}


std::string STI_Device::eventTransferErr() const
{
	return evtTransferErr.str();
}


void STI_Device::stopEventPlayback()
{
}


void STI_Device::PsuedoSynchronousEvent::playEvent()
{
	for(unsigned i = 0; i < events_.size(); i++)
		device_->writeChannel( events_.at(i) );
}

//This event parser works well for non time critical devices (i.e. non-FPGA devices) and devices that
//are fundamentally serial. Events scheduled for the same time will instead be played sequentially
//using calls to STI_Device::writeChannel(...).
void STI_Device::parseDeviceEventsDefault(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
{
	RawEventMap::const_iterator iter;

	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		eventsOut.push_back( 
			new STI_Device::PsuedoSynchronousEvent(iter->first, iter->second, this) );
	}
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
