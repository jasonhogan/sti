/*! \file DeviceConfigure_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceConfigure_i
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

#include "RegisteredDevices_i.h"
#include "STI_Server.h"
#include <Attribute.h>

#include <vector>
#include <string>

using std::string;
using std::vector;

typedef std::map<std::string, Attribute> attributeMap;


RegisteredDevices_i::RegisteredDevices_i(STI_Server* server) : sti_Server(server)
{
}


RegisteredDevices_i::~RegisteredDevices_i()
{
}

void RegisteredDevices_i::killDevice(const char* deviceID)
{
	if(sti_Server->getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		sti_Server->
			registeredDevices[deviceID].killDevice();
	}
}


STI::Types::TAttributeSeq* RegisteredDevices_i::getDeviceAttributes(const char* deviceID)
{
	using STI::Types::TAttributeSeq;

	attributeMap::const_iterator it;
	unsigned i,j;
	const vector<string>* allowedValues = NULL;
	const AttributeMap& attribs = sti_Server->registeredDevices[deviceID].getAttributes();

	STI::Types::TAttributeSeq_var attribSeq( new TAttributeSeq );
	attribSeq->length(attribs.size());

	for(it = attribs.begin(), i = 0; it != attribs.end(); it++, i++)
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

	return attribSeq._retn();
}


::CORBA::Boolean RegisteredDevices_i::setDeviceAttribute(const char* deviceID, const char* key, const char* value)
{
	bool success = false;

	if(sti_Server->getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		success = sti_Server->
			registeredDevices[deviceID].setAttribute(key, value);
	}

	return success;
}


STI::Types::TChannelSeq* RegisteredDevices_i::getDeviceChannels(const char* deviceID)
{
	using STI::Types::TChannelSeq;

	unsigned i;

	const vector<STI::Types::TDeviceChannel> & channels = 
		sti_Server->registeredDevices[deviceID].getChannels();

	STI::Types::TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length(channels.size());

	const STI::Types::TDevice& tDevice = sti_Server->registeredDevices[deviceID].getDevice();

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
	}

	return channelSeq._retn();
}

::CORBA::Boolean RegisteredDevices_i::deviceStatus(const char* deviceID)
{
	return sti_Server->getDeviceStatus(deviceID);
}


STI::Types::TDeviceSeq* RegisteredDevices_i::devices()
{
	sti_Server->refreshDevices();

	using STI::Types::TDeviceSeq;

	int i;
	RemoteDeviceMap::iterator it;
	RemoteDeviceMap& devices = sti_Server->registeredDevices;

	STI::Types::TDeviceSeq_var deviceSeq( new TDeviceSeq );
	deviceSeq->length(devices.size());


	for(it = devices.begin(), i = 0; it != devices.end(); it++, i++)
	{
		deviceSeq[i].deviceName    = CORBA::string_dup( (it->second)->getDevice().deviceName );
		deviceSeq[i].address       = CORBA::string_dup( (it->second)->getDevice().address );
		deviceSeq[i].moduleNum     = (it->second)->getDevice().moduleNum;
		deviceSeq[i].deviceID      = CORBA::string_dup( (it->second)->getDevice().deviceID );
		deviceSeq[i].deviceContext = CORBA::string_dup( (it->second)->getDevice().deviceContext );
	}

	return deviceSeq._retn();
}

::CORBA::Long RegisteredDevices_i::devicePing(const char* deviceID)
{
	RemoteDeviceMap::iterator it = sti_Server->registeredDevices.
		find( string(deviceID) );

	if(it != sti_Server->registeredDevices.end())
		return it->second->pingDevice();
	else
		return -2;
}

