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
#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif

#include "STI_Device.h"
#include "ORBManager.h"
#include "Configure_i.h"
#include "DataTransfer_i.h"
#include "Attribute.h"
#include "device.h"

#include <cassert>
#include <sstream>
#include <string>
#include <map>
using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;


STI_Device::STI_Device(ORBManager *   orb_manager, 
					   std::string    DeviceName, 
					   std::string    DeviceType, 
					   std::string    Address, 
					   unsigned short ModuleNumber)

: orbManager(orb_manager), deviceName(DeviceName)
{

	// servant names -- the STI_Server must look for these same names
	configureObjectName    = "Configure.Object";
	timeCriticalObjectName = "timeCriticalData.Object";
	streamingObjectName    = "streamingData.Object";

	//servants
	configureServant = new Configure_i(this);
	timeCriticalDataServant = new DataTransfer_i(this);
	streamingDataServant = new DataTransfer_i(this);

	//TDevice
	tDevice = new STI_Server_Device::TDevice; 
	tDevice->deviceType = DeviceType.c_str();
	tDevice->address = Address.c_str();
	tDevice->moduleNum = ModuleNumber;

	serverConfigureFound = false;
	registedWithServer = false;
	registrationAttempts = 0;

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
	delete timeCriticalDataServant;
	delete streamingDataServant;
}


void STI_Device::deviceMainWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	while(thisObject->deviceMain()) {};  //pure virtual
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

        //added explicit cast as string
	string contextName = CORBA::string_dup(tDevice->deviceContext);

	// Loop until this STI_Device succesfully registers its 
	// servants with the Name Service
	do {
		orbManager->registerServant(configureServant, 
			contextName + configureObjectName);
		
		orbManager->registerServant(timeCriticalDataServant, 
			contextName + timeCriticalObjectName);

		orbManager->registerServant(streamingDataServant, 
			contextName + streamingObjectName);

		// Try to resolve one of the servants as a test
		CORBA::Object_var obj = orbManager->getObjectReference(
			contextName + configureObjectName);
		ConfigureRef = STI_Server_Device::Configure::_narrow(obj);

	} while(CORBA::is_nil(ConfigureRef));  
	// CAREFULL: This doesn't mean the servants are live, just that their 
	// is a resolvable reference on the Name Service. Add another check for this.

	// setup the channels and then send them to the server
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

	//activateDevice()
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

	cerr << "initServer() done." << endl;
	
	//deviceMain loop
	omni_thread::create(deviceMainWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}


void  STI_Device::setChannels()
{
	using STI_Server_Device::TDeviceChannel;
	using STI_Server_Device::TDeviceChannelSeq;
	using STI_Server_Device::TDeviceChannelSeq_var;

	defineChannels();	//pure virtual

	int i;
	vector<TDeviceChannel>::iterator it;

	//build the TDeviceChannel sequence using the store vector<TDeviceChannel>
	TDeviceChannelSeq_var channelSeq( new TDeviceChannelSeq );
	channelSeq->length(channels.size());

	for(it = channels.begin(), i = 0; it != channels.end(); it++, i++)
	{
		channelSeq[i].channel    = it->channel;
		channelSeq[i].type       = it->type;
		channelSeq[i].inputType  = it->inputType;
		channelSeq[i].outputType = it->outputType;
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

				registedWithServer = ServerConfigureRef->
					registerDevice(getDeviceName().c_str(), tDevice);
				
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


std::string STI_Device::getDeviceName()
{
	return deviceName;
}


std::string STI_Device::getServerName()
{
	if(serverConfigureFound)
		return serverName;
	else
		return "NOT FOUND";
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
	
	if( updateStreamAttribute(key, value) )
	{
		attrib->second.setValue(value);
		return true;
	}
	if( updateAttribute(key, value) )  //pure virtual
	{
		attrib->second.setValue(value);
		return true;
	}

	return false;	//failed to update the atribute
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
	string attrib = "Ch";
	attrib += Channel;

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

		attributes[attrib + "_SamplePeriod"] = Attribute(SamplePeriod);
		updateStreamAttribute(attrib + "_SamplePeriod", SamplePeriod);

		attributes[attrib + "_BufferDepth"  ] = Attribute(BufferDepth);
		updateStreamAttribute(attrib + "_BufferDepth", BufferDepth);
		
		attributes[attrib + "_InputStream" ] = Attribute("Enabled", "Enabled, Disabled");
		updateStreamAttribute(attrib + "_InputStream", "Enabled");
	}
}


bool STI_Device::updateStreamAttribute(string key, string value)
{
	unsigned short Channel;

	string::size_type Ch_Pos = key.find_first_of("Ch", 0);
	string::size_type Ch_EndPos = key.find_first_of("_", 0);

	if(Ch_Pos != 0 
		|| Ch_Pos == string::npos 
		|| Ch_EndPos == string::npos)	//Not a stream attribute
		return false;

	if( !stringToValue(key.substr(2, Ch_EndPos), Channel) )
		return false;    //error converting Channel
	
	if(key.find_first_of("_InputStream", 0) != string::npos)
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

		return true;
	}
	if(key.find_first_of("_SamplePeriod", 0) != string::npos)
	{
		double samplePeriod;
		if( !stringToValue(value, samplePeriod) )
			return false;

		return streamingBuffers[Channel].setSamplePeriod(samplePeriod);
	}
	if(key.find_first_of("_BufferDepth", 0) != string::npos)
	{
		unsigned int bufferDepth;
		if( !stringToValue(value, bufferDepth) )
			return false;

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

//virtual void measureChannel(unsigned short Channel, TDataMixed & data)=0;   Actually how a measurement is made using hardware

void STI_Device::addInputChannel(unsigned short Channel, TData InputType)
{
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
	unsigned i;
	bool valid = true;
	STI_Server_Device::TDeviceChannel tChannel;

	if(Type == Input && OutputType != DataNone)
	{
		valid = false;
	}
	if(Type == Output && InputType != ValueMeas)
	{
		valid = false;
	}

	//check for duplicate channel number
	for(i = 0; i < channels.size(); i++)
	{
		if(channels[i].channel == Channel)
		{
			valid = false;

			cerr << "Error: Duplicate channel number in device " 
				<< getDeviceName() << endl;
		}
	}
	if(valid)
	{
		tChannel.channel    = Channel;
		tChannel.type       = Type;
		tChannel.inputType  = InputType;
		tChannel.outputType = OutputType;

		channels.push_back(tChannel);
	}
	else
	{
		cerr << "Error: Invalid channel specification in device " 
			<< getDeviceName() << endl;
	}

	return valid;
}


vector<STI_Server_Device::TDeviceChannel> * STI_Device::getChannels()
{
	return &channels;
}
