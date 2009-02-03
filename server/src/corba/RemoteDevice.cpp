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


//int RemoteDevice::test = 0;

RemoteDevice::RemoteDevice(STI_Server* STI_server, 
						   STI_Server_Device::TDevice& device) : 
sti_server(STI_server)
{

	active = false;
	timedOut = false;

	eventsReady = false;

	tDevice.deviceName    = CORBA::string_dup(device.deviceName);
	tDevice.address       = CORBA::string_dup(device.address);
	tDevice.moduleNum     = device.moduleNum;
	tDevice.deviceID      = CORBA::string_dup(device.deviceID);
	tDevice.deviceContext = CORBA::string_dup(device.deviceContext);

//	myTest = test++;

	timeOutPeriod = 10; //10 second timeout

	// Make Object Reference names
	string context(tDevice.deviceContext);
	
	configureObjectName     = context + "Configure.Object";
	dataTransferObjectName  = context + "DataTransfer.Object";
	commandLineObjectName   = context + "CommandLine.Object";
	deviceControlObjectName = context + "DeviceControl.Object";

	
	//start the time out countdown
//	timeOutMutex = new omni_mutex();
//	timeOutCondition = new omni_condition(timeOutMutex);

//	timeOutMutex->lock();	//prevents critical functions from getting past waitForActivation()
	
	//timeOutMutex is unlocked by either:
	// 1)activation -- see acquireObjectReferencesWrapper()
	// 2)timeout -- see waitForTimeOut()

//	timeOutThread = omni_thread::create(timeOutWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
}
//
//RemoteDevice::RemoteDevice(const RemoteDevice& copy)
//{
//	active        = copy.active;
//	timedOut      = copy.timedOut;
//	eventsReady   = copy.eventsReady;
//	timeOutPeriod = copy.timeOutPeriod;
//	tDevice       = copy.tDevice;
//
//	configureObjectName     = copy.configureObjectName;
//	dataTransferObjectName  = copy.dataTransferObjectName;
//	commandLineObjectName   = copy.commandLineObjectName;
//	deviceControlObjectName = copy.deviceControlObjectName;
//
//	timeOutMutex     = timeOutMutex;
//	timeOutCondition = timeOutCondition;
//	timeOutThread    = timeOutThread;
//}
//
//RemoteDevice& RemoteDevice::operator=(const RemoteDevice& rhs)
//{
//
//	return *this;
//}

RemoteDevice::~RemoteDevice()
{
//	delete timeOutMutex;
	//_release() references?
//	timeOutCondition->signal();
//	timeOutThread->exit();
}
bool RemoteDevice::servantsActive()
{	
	bool servantsAlive = false;

	try {
		configureRef->deviceName();
		commandLineRef->deviceID();
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
	//the device gets a timeout period for activation before servantsActive() can be accessed

//	if( timedOut || active)
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

		active = true;
//		cout << "RemoteDevice ---> SIGNAL" << endl;
//		timeOutCondition->broadcast();		//wake up timeout

		//can be released before refresh because now the device's servantsAreActive() and other registered devices will get a timeout period
		//timeOutMutex->unlock();

		sti_server->refreshPartnersDevices();
	}

	return active;

	// Activate in a separate thread to avoid hanging 
	// the server due to a bad activation
//	omni_thread::create(acquireObjectReferencesWrapper, (void*)this, 
//		omni_thread::PRIORITY_NORMAL);

	//Wait for activation or timeout before returning; this prevents multiple devices
	//from activating simultaneously.
//	waitForActivation();
}

//void RemoteDevice::waitForActivation()
//{
//	unsigned long wait_s;
//	unsigned long wait_ns;
//
//	// Wait until acquireObjectReferences()  returns (due to successful activation or timeout)
//	timeOutMutex->lock();
//	{
//		if( !timedOut )
//		{
//			omni_thread::get_time(&wait_s, &wait_ns, timeOutPeriod, 0);	//10 seconds
//			
//			cout << "waiting..." << endl;
//			int result = timeOutCondition->timedwait(wait_s, wait_ns);
//
//			cout << "time out finished. Code=" << result << endl;
//			timedOut = true;
//		}
//	}
//	timeOutMutex->unlock();
//}

void RemoteDevice::deactivate()
{
	// _release() references???
	active = false;
	timedOut = false;
}

//void RemoteDevice::waitForTimeOut()
//{
//	timeOutThread->sleep(timeOutPeriod);
//	timedOut = true;
//	timeOutMutex->unlock();
//}



//void RemoteDevice::timeOutWrapper(void* object)
//{
//	RemoteDevice* thisObject = (RemoteDevice*) object;
//	
////	thisObject->waitForTimeOut();
//	thisObject->waitForActivation();
//
//	//thisObject->timeOutMutex->lock();
//	//	thisObject->timeOutThread->sleep(thisObject->timeOutPeriod);
//	//	thisObject->timedOut = true;
//	//thisObject->timeOutMutex->unlock();
//} 

//void RemoteDevice::acquireObjectReferencesWrapper(void* object)
//{
//	RemoteDevice* thisObject = (RemoteDevice*) object;
//	thisObject->acquireObjectReferences();
//}


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

//	bool servants_active = servantsActive();

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
//		cerr << "partnerSeq: " << partnerSeq->length() << endl;
	}
}

const vector<string>& RemoteDevice::getRequiredPartners() const
{
	return requiredPartners;
}

bool RemoteDevice::registerPartner(std::string deviceID, STI_Server_Device::CommandLine_ptr partner)
{
//	waitForActivation();	//partners can't be added until after activation

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
//	waitForActivation();		//avoid removing a device if it's trying to activate

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
//	waitForActivation();	//attributes are not accessible until after activation

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
//	waitForActivation();	//attributes are not accessible until after activation

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
//	waitForActivation();	//dataTransferRef is not available until after activation

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

STI_Server_Device::TMeasurementSeqSeq* RemoteDevice::measurements()
{
//	waitForActivation();	//dataTransferRef is not available until after activation

	STI_Server_Device::TMeasurementSeqSeq* measurements = 0;

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
//	waitForActivation();	//deviceControlRef is not available until after activation

	eventsReady = false;

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
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::transferEvents");
	}
	catch(CORBA::SystemException& ex) {
		cerr << printExceptionMessage(ex, "RemoteDevice::transferEvents");
	}
}

void RemoteDevice::loadEvents()
{	
//	waitForActivation();	//deviceControlRef is not available until after activation

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
//	waitForActivation();	//deviceControlRef is not available until after activation

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
//	waitForActivation();	//deviceControlRef is not available until after activation

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
//	waitForActivation();	//deviceControlRef is not available until after activation

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
//	waitForActivation();	//deviceControlRef is not available until after activation

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

bool RemoteDevice::isTimedOut()
{
	return timedOut;
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

	try {
		if(getDevice().deviceName != 0)
			device_name = getDevice().deviceName;
	}
	catch(...) {}

	error << "Caught exception CORBA::" << ex._name() 
		<< " at location " << location << " when contacting device '"
		<< device_name << "'." << endl;

	return error.str();
}
