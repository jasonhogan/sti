/*! \file RemoteDevice.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class RemoteDevice
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


#include "RemoteDevice.h"
#include <ORBManager.h>
#include "STI_Server.h"

#include <string>
using std::string;


#include <iostream>
using namespace std;


RemoteDevice::RemoteDevice(STI_Server* STI_server, 
						   STI_Server_Device::TDevice&	device) : 
sti_server(STI_server)
{
	orbManager = sti_server->orbManager;

	active = false;

	tDevice.deviceName = CORBA::string_dup(device.deviceName);
	tDevice.address = CORBA::string_dup(device.address);
	tDevice.moduleNum = device.moduleNum;
	tDevice.deviceID = CORBA::string_dup(device.deviceID);
	tDevice.deviceContext = CORBA::string_dup(device.deviceContext);

	// Make Object Reference names
	string context = CORBA::string_dup(device.deviceContext);
	
	configureObjectName = context + "Configure.Object";
	dataTransferObjectName = context + "DataTransfer.Object";
	commandLineObjectName = context + "CommandLine.Object";
}

RemoteDevice::~RemoteDevice()
{
}

bool RemoteDevice::isActive()
{
	bool servantsAlive = false;

	try{
		// Just look for one servant
		ConfigureRef->deviceName();
		servantsAlive = true;
	}
	catch(CORBA::TRANSIENT& ex) {		
		servantsAlive = false;
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << endl << "Unable to contact the "
			<< "Device '" << device().deviceName << "'." << endl
			<< "Make sure the device is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		servantsAlive = false;
		cerr << "RemoteDevice::isActive: Caught a CORBA::" << ex._name()
			<< " while trying to contact Device '" 
			<< device().deviceName << "'." << endl;
	}

	return servantsAlive;
}

void RemoteDevice::activate()
{
	// Activate in a separate thread to avoid hanging 
	// the server due to a bad activation
	omni_thread::create(acquireObjectReferencesWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

void RemoteDevice::deactivate()
{
	// _release() references???
	active = false;
}

void RemoteDevice::acquireObjectReferencesWrapper(void* object)
{
	RemoteDevice* thisObject = (RemoteDevice*) object;
	thisObject->acquireObjectReferences();
}


void RemoteDevice::acquireObjectReferences()
{
	CORBA::Object_var obj;

	bool configureFound = false;
	bool dataTransferFound = false;
	bool commandLineFound = false;
	bool deviceControlFound = false;

	int timeout = 10;	// try 10 times

	while( (!configureFound || !dataTransferFound || !commandLineFound)
		&& (--timeout > 0) )
	{
		if( !configureFound )
		{
			obj = orbManager->getObjectReference(configureObjectName);
			ConfigureRef = STI_Server_Device::Configure::_narrow(obj);
			if( !CORBA::is_nil(ConfigureRef) )
				configureFound = true;
		}
		if( !dataTransferFound )
		{
			obj = orbManager->getObjectReference(dataTransferObjectName);
			DataTransferRef = STI_Server_Device::DataTransfer::_narrow(obj);
			if( !CORBA::is_nil(DataTransferRef) )
				dataTransferFound = true;
		}
		if( !commandLineFound )
		{
			obj = orbManager->getObjectReference(commandLineObjectName);
			CommandLineRef = STI_Server_Device::CommandLine::_narrow(obj);
			if( !CORBA::is_nil(CommandLineRef) )
				commandLineFound = true;
		}
		if( !deviceControlFound )
		{
			obj = orbManager->getObjectReference(deviceControlObjectName);
			DeviceControlRef = STI_Server_Device::DeviceControl::_narrow(obj);
			if( !CORBA::is_nil(DeviceControlRef) )
				deviceControlFound = true;
		}
	}
	active = isActive();

	if(commandLineFound)
	{
		setupCommandLine();
		sti_server->refreshPartnersDevices();
	}
}

void RemoteDevice::setupCommandLine()
{
	requiredPartners.clear();
	
	bool success = false;
	STI_Server_Device::TStringSeq_var partnerSeq;

	try {
		partnerSeq = CommandLineRef->requiredPartnerDevices();
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << endl << "Unable to contact the "
			<< "Device '" << device().deviceName << "'." << endl
			<< "Make sure the device is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "RemoteDevice::setupCommandLine: Caught a CORBA::" << ex._name()
			<< endl << " while trying to contact Device '" 
			<< device().deviceName << "'." << endl;
	}

	if(success)
	{
		for(unsigned i = 0; i < partnerSeq->length(); i++)
		{
			requiredPartners.push_back( string(partnerSeq[i]) );
			cerr << requiredPartners.back() << endl;
		}
		cerr << "partnerSeq: " << partnerSeq->length() << endl;
	}
}

const vector<string> & RemoteDevice::getRequiredPartners() const
{
	return requiredPartners;
}

bool RemoteDevice::registerPartner(std::string DeviceID, STI_Server_Device::CommandLine_ptr partner)
{
	return CommandLineRef->registerPartnerDevice(partner);
}

bool RemoteDevice::unregisterPartner(std::string DeviceID)
{
	return CommandLineRef->unregisterPartnerDevice(DeviceID.c_str());
}


bool RemoteDevice::addChannel(const STI_Server_Device::TDeviceChannel & tChannel)
{
	if(isUnique(tChannel))
	{
		channels.push_back(tChannel);
		return true;
	}
	else
	{
		cerr << "Error: Duplicate channel in device '" 
			<< device().deviceName << "'." << endl;
		return false;
	}
}



bool RemoteDevice::setAttribute(std::string key, std::string value)
{
	bool success = false;

	try {
		success = ConfigureRef->setAttribute(key.c_str(), value.c_str());
	}
	catch(CORBA::TRANSIENT& ex) {		
		success = false;
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << endl << "Unable to contact the "
			<< "Device '" << device().deviceName << "'." << endl
			<< "Make sure the device is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		success = false;
		cerr << "RemoteDevice::setAttribute: Caught a CORBA::" << ex._name()
			<< " while trying to contact Device '" 
			<< device().deviceName << "'." << endl;
	}

	return success;
}


bool RemoteDevice::isUnique(const STI_Server_Device::TDeviceChannel & tChannel)
{
	bool unique = true;
	unsigned i;
	for(i = 0; i < channels.size(); i++)
	{
		if(channels[i].channel == tChannel.channel)	//same channel not allowed
		{
			unique = false;
		}
	}
	return unique;
}


STI_Server_Device::TDevice & RemoteDevice::device()
{
	return tDevice;
}

std::string RemoteDevice::DataTransferErrMsg() const
{
	return string(DataTransferRef->errMsg());
}


void RemoteDevice::printChannels()
{
	for(unsigned i=0; i < channels.size(); i++)
	{
		cerr << "Channel " << i << ": " << channels[i].channel << endl;
	}
}



attributeMap const * RemoteDevice::getAttributes()
{
	attributes.clear();

	unsigned i,j;
	bool success = false;
	string allowedValues;

	STI_Server_Device::TAttributeSeq_var attribSeq;

	try {
		attribSeq = ConfigureRef->attributes();
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << endl << "Unable to contact the "
			<< "Device '" << device().deviceName << "'." << endl
			<< "Make sure the device is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "RemoteDevice::getAttributes: Caught a CORBA::" << ex._name()
			<< " while trying to contact Device '" 
			<< device().deviceName << "'." << endl;
	}

	if(success)
	{
		for(i = 0; i < attribSeq->length(); i++)
		{
			for(j = 0; j < attribSeq[i].values.length(); j++)
			{
				if(j > 0)
				{
					allowedValues += ",";
				}

				allowedValues += attribSeq[i].values[j];
			}
			attributes[CORBA::string_dup(attribSeq[i].key)] = 
				Attribute(CORBA::string_dup(attribSeq[i].value), allowedValues);

			allowedValues = "";		//reset
		}
	}

	return &attributes;
}


const vector<STI_Server_Device::TDeviceChannel> & RemoteDevice::getChannels() const
{
	return channels;
}


STI_Server_Device::TMeasurementSeq*	RemoteDevice::getStreamingData(
		                                             unsigned short channel,
                                                     double         initial_t, 
                                                     double         final_t, 
                                                     double         delta_t)
{
	return DataTransferRef->getStreamingData(channel, initial_t, final_t, delta_t);
}

STI_Server_Device::TMeasurementSeqSeq* RemoteDevice::measurements()
{
	return DataTransferRef->measurements();
}


void RemoteDevice::transferEvents(std::vector<STI_Server_Device::TDeviceEvent_var> &events)
{
	eventsParsed = false;

	using STI_Server_Device::TDeviceEventSeq;
	using STI_Server_Device::TDeviceEventSeq_var;

	TDeviceEventSeq_var eventSeq( new TDeviceEventSeq );
	eventSeq->length( events.size() );

	for(unsigned i=0; i < eventSeq->length(); i++)
	{
		eventSeq[i] = events[i];	//deep copy?
	}

	eventsParsed = DeviceControlRef->transferEvents(eventSeq, false);
}