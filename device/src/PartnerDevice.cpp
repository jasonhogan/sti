/*! \file PartnerDevice.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class PartnerDevice
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
#include "device.h"
#include <CommandLine_i.h>
#include <PartnerDevice.h>
#include <RawEvent.h>

#include <iostream>
#include <string>
#include <sstream>

using std::cerr;
using std::endl;
using std::string;


PartnerDevice::PartnerDevice(bool dummy)
{
	_dummy = true;
		
	registered = false;
	local = false;
	partnerEventsEnabled = false;
	partnerEventsEnabledLocked = true;
	
	_required = false;
	_mutual = false;
}

PartnerDevice::PartnerDevice(std::string PartnerName, std::string deviceID, bool required, bool mutual) :
partnerName(PartnerName)
{
	registered = false;
	local = false;
	partnerEventsEnabled = false;
	partnerEventsEnabledLocked = false;

	_required = required;
	_mutual = mutual;
	_deviceID = deviceID;
	_dummy = false;

}

PartnerDevice::PartnerDevice(std::string PartnerName, std::string IP, short module, std::string deviceName, bool required, bool mutual) :
partnerName(PartnerName)
{
	registered = false;
	local = false;
	partnerEventsEnabled = false;
	partnerEventsEnabledLocked = false;

	_required = required;
	_mutual = mutual;
	_dummy = false;

	partnerDevice.address    = CORBA::string_dup( IP.c_str() );
	partnerDevice.moduleNum  = module;
	partnerDevice.deviceName = CORBA::string_dup( deviceName.c_str() );

}


PartnerDevice::PartnerDevice(std::string PartnerName, CommandLine_i* LocalCommandLine, bool required, bool mutual) :
partnerName(PartnerName)
{
	registerLocalPartnerDevice(LocalCommandLine);
	
	partnerEventsEnabled = false;
	partnerEventsEnabledLocked = false;
	
	partnerDevice.deviceName    = CORBA::string_dup(localCommandLine->device()->deviceName);
	partnerDevice.address       = CORBA::string_dup(localCommandLine->device()->address);
	partnerDevice.moduleNum     = localCommandLine->device()->moduleNum;
	partnerDevice.deviceID      = CORBA::string_dup(localCommandLine->device()->deviceID);
	partnerDevice.deviceContext = CORBA::string_dup(localCommandLine->device()->deviceContext);
}

PartnerDevice::~PartnerDevice()
{
}


void PartnerDevice::setDeviceID(std::string deviceID)
{
	_deviceID = deviceID;
}




void PartnerDevice::registerPartnerDevice(STI::Server_Device::CommandLine_ptr commandLine)
{
	local = false;
	setCommandLine(commandLine);

	partnerDevice.deviceName    = CORBA::string_dup(commandLine->device()->deviceName);
	partnerDevice.address       = CORBA::string_dup(commandLine->device()->address);
	partnerDevice.moduleNum     = commandLine->device()->moduleNum;
	partnerDevice.deviceID      = CORBA::string_dup(commandLine->device()->deviceID);
	partnerDevice.deviceContext = CORBA::string_dup(commandLine->device()->deviceContext);
}


void PartnerDevice::registerLocalPartnerDevice(CommandLine_i* LocalCommandLine)
{
	registered = true;
	local = true;
	localCommandLine = LocalCommandLine;

	partnerDevice.deviceName    = CORBA::string_dup(localCommandLine->device()->deviceName);
	partnerDevice.address       = CORBA::string_dup(localCommandLine->device()->address);
	partnerDevice.moduleNum     = localCommandLine->device()->moduleNum;
	partnerDevice.deviceID      = CORBA::string_dup(localCommandLine->device()->deviceID);
	partnerDevice.deviceContext = CORBA::string_dup(localCommandLine->device()->deviceContext);
}


string PartnerDevice::name() const
{
	return partnerName;
}

string PartnerDevice::getDeviceID() const
{
	return _deviceID;
}


STI::Types::TDevice PartnerDevice::device() const
{
	return partnerDevice;
}
bool PartnerDevice::isRegistered() const
{
	return registered;
}
bool PartnerDevice::isLocal()
{
	return local;
}

bool PartnerDevice::isRequired() const
{
	return _required;
}
bool PartnerDevice::isMutual() const
{
	return _mutual;
}

void PartnerDevice::enablePartnerEvents()
{
	if(partnerEventsEnabled)
	{
		cerr << "Warning: PartnerDevice::enablePartnerEvents() should only be called once." << endl;
	}

	if(!partnerEventsEnabledLocked)
	{
		partnerEventsEnabled = true;
	}
	else
	{
		cerr << "Error: PartnerDevice::enablePartnerEvents() must be called inside " 
			<< "STI_Device::definePartnerDevices()." << endl;
	}
}

void PartnerDevice::disablePartnerEvents()
{

	if(!partnerEventsEnabledLocked)
	{
		partnerEventsEnabled = false;
		partnerEventsEnabledLocked = true;
	}
	else
	{
		cerr << "Error: PartnerDevice::disablePartnerEvents() must be called inside " 
			<< "STI_Device::definePartnerDevices()." << endl;
	}
}

bool PartnerDevice::prepareEvents(std::vector<STI::Server_Device::DeviceControl_var>& partnerControls)
{
	using STI::Types::TDeviceEventSeq;
	STI::Server_Device::DeviceControlSeq_var partnerControlSeq = new STI::Server_Device::DeviceControlSeq();
	
	bool success = prepareEvents(partnerControlSeq);

	if(success)
	{
		for(unsigned i = 0; i < partnerControlSeq->length(); i++)
		{
			partnerControls.push_back( 
				STI::Server_Device::DeviceControl::_duplicate(partnerControlSeq[i]) );
		}
	}
	return success;
}

bool PartnerDevice::prepareEvents(STI::Server_Device::DeviceControlSeq& partnerControlSeq)
{
	std::vector<STI::Types::TDeviceEvent>& deviceEvents = getEvents();

	if(deviceEvents.size() == 0)
		return true;

	STI::Types::TDeviceEventSeq_var eventSeq( new STI::Types::TDeviceEventSeq );
	eventSeq->length(deviceEvents.size());

	for(unsigned i = 0; i < deviceEvents.size(); i++)
	{
		eventSeq[i] = deviceEvents.at(i);
	}

	if(!isRegistered())
		return false;

	bool success = true;

	if(isLocal())
		success = localCommandLine->preparePartnerEvents(eventSeq, partnerControlSeq);
	else
		success = commandLine_l->preparePartnerEvents(eventSeq, partnerControlSeq);

	return success;

}


void PartnerDevice::event(double time, unsigned short channel, const MixedValue& value, const RawEvent& referenceEvent, std::string description) 
throw(std::exception)
{
	event(time, channel, value.getTValMixed(), referenceEvent, description, false);
}

void PartnerDevice::meas(double time, unsigned short channel, const STI::Types::TValMixed& value, const RawEvent& referenceEvent, std::string description) throw(std::exception)
{
	event(time, channel, value, referenceEvent, description, true);
}

void PartnerDevice::meas(double time, unsigned short channel, const MixedValue& value, const RawEvent& referenceEvent, std::string description) throw(std::exception)
{
	event(time, channel, value.getTValMixed(), referenceEvent, description, true);
}


void PartnerDevice::event(double time, unsigned short channel, const STI::Types::TValMixed& value, const RawEvent& referenceEvent, std::string description, bool isMeasurement) 
throw(std::exception)
{
	if( partnerEventsEnabled )
	{
		STI::Types::TDeviceEvent partnerEvent;

		partnerEvent.time     = time;
		partnerEvent.channel  = channel;
		partnerEvent.value    = value;
		partnerEvent.eventNum = referenceEvent.eventNum();
		partnerEvent.isMeasurementEvent = isMeasurement;
		partnerEvent.description = CORBA::string_dup(description.c_str());

		partnerEvents.push_back(partnerEvent);
	}
	else
	{
		throw EventParsingException(referenceEvent,
			"An event was requested on partner '" + name() + "' but partner events\n"
			+"are not enabled on this partner.  \n"
			+ "Partner events must first be enabled inside ::definePartnerDevices() using the expression\n" 
			+ "    partnerDevice("  + name() + ").enablePartnerEvents();\n" );
	}
}

bool PartnerDevice::read(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	if(!registered)		//this partner has not been registered by the server
		return false;

	bool success = false;
	STI::Types::TDataMixed_var tData;

	if( isLocal() )
	{
		success = localCommandLine->readChannel(channel, valueIn.getTValMixed(), tData.out());
	}
	else
	{
		try {
			success = commandLine_l->readChannel(channel, valueIn.getTValMixed(), tData.out());
		}
		catch(CORBA::TRANSIENT& ex) {
			cerr << "Caught system exception CORBA::" << ex._name() 
				<< " when trying to read from a partner channel: " 
				<< endl << "--> partner(\"" << name() << "\").read(" 
				<< valueIn.print() << ", <Data Not Available>)" << endl;
		}
		catch(CORBA::SystemException& ex) {
			cerr << "Caught system exception CORBA::" << ex._name() 
				<< " when trying to read from a partner channel: " 
				<< endl << "--> partner(\"" << name() << "\").read(" 
				<< valueIn.print() << ", <Data Not Available>)" << endl;
		}
	}

	if( success )
	{
		dataOut.setValue( tData.in() );
	}

	return success;
}

bool PartnerDevice::write(unsigned short channel, const MixedValue& value)
{
	if(!registered)		//this partner has not been registered by the server
		return false;

	if( isLocal() )
	{
		return localCommandLine->writeChannel(channel, value.getTValMixed());
	}

	bool result = false;

	try {
		result = commandLine_l->writeChannel(channel, value.getTValMixed());
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to write to a partner channel: " 
			<< endl << "--> partner(\"" << name() << "\").write(" 
			<< value.print() << ")" << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to write to a partner channel: " 
			<< endl << "--> partner(\"" << name() << "\").write(" 
			<< value.print() << ")" << endl;
	}
	return result;
}



void PartnerDevice::resetPartnerEvents()
{
	partnerEvents.clear();
}

std::vector<STI::Types::TDeviceEvent>& PartnerDevice::getEvents()
{
	return partnerEvents;
}

bool PartnerDevice::getPartnerEventsSetting()
{
	return partnerEventsEnabled;
}

bool PartnerDevice::registerMutualPartner(STI::Server_Device::CommandLine_ptr partner)
{
	if( !isLocal() )
	{
		return commandLine_l->registerPartnerDevice(partner);
	}

	return false;
}

bool PartnerDevice::isAlive()
{
	if( isRegistered() )
	{
		if( isLocal() )
		{
			// The assumption is that if this device is alive then any local
			// partner must be alive too since they are running in the same program.
			return true;
		}

		try {
			commandLine_l->device()->deviceID;	//try to contact partner
			return true;
		}
		catch(CORBA::TRANSIENT& ex) {
			cerr << "Caught system exception CORBA::" << ex._name() 
				<< " when trying to contact partner device " << name() 
				<< "." << endl;
		}
		catch(CORBA::SystemException& ex) {
			cerr << "Caught a CORBA::" << ex._name()
			<< " when trying to contact partner device " << name() 
			<< "." << endl;
		}
	}
	return false;
}

void PartnerDevice::setCommandLine(STI::Server_Device::CommandLine_ptr commandLine)
{
	commandLine_l = commandLine;
	registered = true;
}

string PartnerDevice::execute(string args)
{
	string result = "";

	if(!registered)		//this partner has not been registered by the server
		return result;

	if( isLocal() )
	{
		result = localCommandLine->execute( args.c_str() );
		return result;
	}

	try {
		result = commandLine_l->execute(args.c_str());
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").execute(" 
			<< args << ")" << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").execute(" 
			<< args << ")" << endl;
	}

	return result;
}

bool PartnerDevice::setAttribute(std::string key, std::string value)
{
	if( !registered )		//this partner has not been registered by the server
		return false;

	if( isLocal() )
	{
		return localCommandLine->setAttribute( key.c_str(), value.c_str() );
	}

	try {
		return commandLine_l->setAttribute( key.c_str(), value.c_str() );
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").setAttribute(" 
			<< key << ", " << value << ")" << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").setAttribute(" 
			<< key << ", " << value << ")" << endl;
	}

	return false;
}

std::string PartnerDevice::getAttribute(std::string key)
{
	string value = "";

	if(!registered)		//this partner has not been registered by the server
		return value;

	if( isLocal() )
	{
		value = localCommandLine->getAttribute( key.c_str() );
		return value;
	}

	try {
		value = commandLine_l->getAttribute( key.c_str() );
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").getAttribute(" 
			<< key << ")" << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " when trying to execute a partner device command: " 
			<< endl << "--> partner(\"" << name() << "\").getAttribute(" 
			<< key << ")" << endl;
	}

	return value;
}


