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
#include "STI_Server.h"
#include <ORBManager.h>

#include <string>
using std::string;

#include <iostream>
using namespace std;

RemoteDevice::RemoteDevice(STI_Server* STI_server, 
						   STI_Server_Device::TDevice& device) : 
sti_server(STI_server)
{
	active = false;

	doneTransfering = false;
	eventsReady = false;

	tDevice.deviceName    = CORBA::string_dup(device.deviceName);
	tDevice.address       = CORBA::string_dup(device.address);
	tDevice.moduleNum     = device.moduleNum;
	tDevice.deviceID      = CORBA::string_dup(device.deviceID);
	tDevice.deviceContext = CORBA::string_dup(device.deviceContext);

	// Make Object Reference names
	string context(tDevice.deviceContext);
	
	configureObjectName     = context + "Configure.Object";
	dataTransferObjectName  = context + "DataTransfer.Object";
	commandLineObjectName   = context + "CommandLine.Object";
	deviceControlObjectName = context + "DeviceControl.Object";
}

RemoteDevice::~RemoteDevice()
{
	//_release() references?
}

bool RemoteDevice::servantsActive()
{	
	bool servantsAlive = false;

	try {
		configureRef->deviceName();
		commandLineRef->device();
		dataTransferRef->errMsg();
		deviceControlRef->controlMsg();
		servantsAlive = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::servantsActive()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::servantsActive()");
	}

	return servantsAlive;
}

bool RemoteDevice::isActive()
{
	active = servantsActive();	//refresh status

	return active;
}

bool RemoteDevice::activate()
{
	active = false;

	acquireObjectReferences();
	
	if( isActive() )
	{
		setupCommandLine();

		sti_server->refreshPartnersDevices();
	}

	return active;
}

void RemoteDevice::deactivate()
{
	// _release() references???
	active = false;
}


void RemoteDevice::acquireObjectReferences()
{
	CORBA::Object_var obj;

	bool configureFound     = false;
	bool dataTransferFound  = false;
	bool commandLineFound   = false;
	bool deviceControlFound = false;
	bool allServantsFound   = false;

	int orbTimeout = 10;	// try 10 times

	while( !allServantsFound && (--orbTimeout > 0) )
	{
		if( !configureFound )
		{
			obj = sti_server->getORBManager()->getObjectReference(configureObjectName);
			configureRef = STI_Server_Device::Configure::_narrow(obj);
			if( !CORBA::is_nil(configureRef) )
				configureFound = true;
		}
		if( !dataTransferFound )
		{
			obj = sti_server->getORBManager()->getObjectReference(dataTransferObjectName);
			dataTransferRef = STI_Server_Device::DataTransfer::_narrow(obj);
			if( !CORBA::is_nil(dataTransferRef) )
				dataTransferFound = true;
		}
		if( !commandLineFound )
		{
			obj = sti_server->getORBManager()->getObjectReference(commandLineObjectName);
			commandLineRef = STI_Server_Device::CommandLine::_narrow(obj);
			if( !CORBA::is_nil(commandLineRef) )
				commandLineFound = true;
		}
		if( !deviceControlFound )
		{
			obj = sti_server->getORBManager()->getObjectReference(deviceControlObjectName);
			deviceControlRef = STI_Server_Device::DeviceControl::_narrow(obj);
			if( !CORBA::is_nil(deviceControlRef) )
				deviceControlFound = true;
		}
		
		allServantsFound = configureFound && dataTransferFound && 
						   commandLineFound && deviceControlFound;
	}
}

void RemoteDevice::setupCommandLine()
{
	requiredPartners.clear();
	
	bool success = false;
	STI_Server_Device::TStringSeq_var partnerSeq;

	try {
		partnerSeq = commandLineRef->requiredPartnerDevices();
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::setupCommandLine()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::setupCommandLine()");
	}

	if(success)
	{
		for(unsigned i = 0; i < partnerSeq->length(); i++)
		{
			requiredPartners.push_back( string(partnerSeq[i]) );
			cerr << requiredPartners.back() << endl;
		}
	}
}

const vector<string>& RemoteDevice::getRequiredPartners() const
{
	return requiredPartners;
}

vector<string>& RemoteDevice::getRegisteredPartners()
{
	registeredPartners.clear();

	bool success = false;
	STI_Server_Device::TStringSeq_var partnerSeq;

	try {
		partnerSeq = commandLineRef->registeredPartnerDevices();
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getRegisteredPartners()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getRegisteredPartners()");
	}

	if(success)
	{
		for(unsigned i = 0; i < partnerSeq->length(); i++)
		{
			registeredPartners.push_back( string(partnerSeq[i]) );
		}
	}

	return registeredPartners;
}

bool RemoteDevice::registerPartner(std::string deviceID, STI_Server_Device::CommandLine_ptr partner)
{
	bool success = false;

	try {
		success = commandLineRef->registerPartnerDevice(partner);
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::registerPartner");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::registerPartner");
	}

	return success;
}

bool RemoteDevice::unregisterPartner(std::string deviceID)
{
	bool success = false;

	try {
		success = commandLineRef->unregisterPartnerDevice(deviceID.c_str());
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::unregisterPartner");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::unregisterPartner");
	}

	return success;
}


bool RemoteDevice::addChannel(const STI_Server_Device::TDeviceChannel& tChannel)
{
	if(isUnique(tChannel))
	{
		channels.push_back(tChannel);
		return true;
	}
	else
	{
		cerr << "Error: Duplicate channel in device '" 
			<< getDevice().deviceName << "'." << endl;
		return false;
	}
}


bool RemoteDevice::setAttribute(std::string key, std::string value)
{
	bool success = false;

	try {
		success = configureRef->setAttribute(key.c_str(), value.c_str());
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::setAttribute");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::setAttribute");
	}

	return success;
}


bool RemoteDevice::isUnique(const STI_Server_Device::TDeviceChannel& tChannel)
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


STI_Server_Device::CommandLine_var RemoteDevice::getCommandLineRef() const
{
	return commandLineRef;
}

const STI_Server_Device::TDevice& RemoteDevice::getDevice() const
{
	return tDevice;
}

std::string RemoteDevice::getDataTransferErrMsg() const
{
	string error = "";

	try {
		error = string( dataTransferRef->errMsg() );
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getDataTransferErrMsg()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getDataTransferErrMsg()");
	}

	return error;
}


void RemoteDevice::printChannels()
{
	for(unsigned i=0; i < channels.size(); i++)
	{
		cerr << "Channel " << i << ": " << channels[i].channel << endl;
	}
}


const AttributeMap& RemoteDevice::getAttributes()
{
	attributes.clear();

	unsigned i,j;
	bool success = false;
	string allowedValues;

	STI_Server_Device::TAttributeSeq_var attribSeq;

	try {
		attribSeq = configureRef->attributes();
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getAttributes()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getAttributes()");
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

	return attributes;
}


const vector<STI_Server_Device::TDeviceChannel>& RemoteDevice::getChannels() const
{
	return channels;
}


STI_Server_Device::TMeasurementSeq*	RemoteDevice::getStreamingData(
		                                             unsigned short channel,
                                                     double         initial_t, 
                                                     double         final_t, 
                                                     double         delta_t)
{
	STI_Server_Device::TMeasurementSeq* measurements = 0;

	try {
		measurements = dataTransferRef->getStreamingData(channel, initial_t, final_t, delta_t);
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getStreamingData");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getStreamingData");
	}

	return measurements;
}

STI_Server_Device::TMeasurementSeq* RemoteDevice::measurements()
{
	STI_Server_Device::TMeasurementSeq* measurements = 0;

	try {
		measurements = dataTransferRef->measurements();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::measurements");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::measurements");
	}

	return measurements;
}


void RemoteDevice::transferEvents(std::vector<STI_Server_Device::TDeviceEvent_var>& events)
{
	eventsReady = false;
	doneTransfering = false;

	using STI_Server_Device::TDeviceEventSeq;
	using STI_Server_Device::TDeviceEventSeq_var;

	TDeviceEventSeq_var eventSeq( new TDeviceEventSeq );
	eventSeq->length( events.size() );

	for(unsigned i=0; i < eventSeq->length(); i++)
	{
		eventSeq[i] = events[i];	//deep copy?
	}

	try {
		eventsReady = deviceControlRef->transferEvents(eventSeq, false);
		doneTransfering = true;
		getTransferErrLog();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::transferEvents");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::transferEvents");
	}
	doneTransfering = true;
}

void RemoteDevice::loadEvents()
{	
	try {
		deviceControlRef->load();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::loadEvents()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::loadEvents()");
	}
}
void RemoteDevice::playEvents()
{	
	try {
		deviceControlRef->play();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::playEvents()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::playEvents()");
	}
}

void RemoteDevice::stop()
{
	try {
		deviceControlRef->stop();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::stop()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::stop()");
	}
}


bool RemoteDevice::eventsParsed()
{	
	bool parsed = false;

	try {
		parsed = deviceControlRef->eventsParsed();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::eventsParsed()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::eventsParsed()");
	}

	return parsed;
}

bool RemoteDevice::eventsLoaded()
{	
	bool loaded = false;

	try {
		loaded = deviceControlRef->eventsLoaded();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::eventsLoaded()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::eventsLoaded()");
	}

	return loaded;
}

bool RemoteDevice::finishedEventsTransferAttempt()
{
	return doneTransfering;
}

bool RemoteDevice::eventsTransferSuccessful()
{
	return eventsReady;
}

std::string RemoteDevice::printDeviceIndentiy() const
{
	std::stringstream id;
	id << tDevice.deviceName << " <" << tDevice.address 
		<< ", Module_" << tDevice.moduleNum << ">";

	return id.str();
}


std::string RemoteDevice::getTransferErrLog() const
{	
	string error = "";

	try {
		error = string( deviceControlRef->transferErr() );
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getTransferErrLog()");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::getTransferErrLog()");
	}

	return error;
}

std::string RemoteDevice::printExceptionMessage(
	CORBA::SystemException& ex, std::string location) const
{
	std::stringstream error;

	string device_name = "UNKNOWN";

	//try {
	//	if(getDevice().deviceName != 0)
	//		device_name = getDevice().deviceName;
	//}
	//catch(...) {}

	error << "Caught exception CORBA::" << ex._name() 
		<< " at location " << location << " when contacting device '"
		<< device_name << "'." << endl;

	return error.str();
}


