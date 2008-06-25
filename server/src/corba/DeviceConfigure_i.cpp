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

#include "DeviceConfigure_i.h"
#include "STI_Server.h"
#include "Attribute.h"

#include <vector>
#include <string>

using std::string;
using std::vector;

typedef std::map<std::string, Attribute> attributeMap;


DeviceConfigure_i::DeviceConfigure_i(STI_Server* server) : sti_Server(server)
{
}


DeviceConfigure_i::~DeviceConfigure_i()
{
}



STI_Client_Server::TAttributeSeq* DeviceConfigure_i::getDeviceAttributes(const char* deviceID)
{
	using STI_Client_Server::TAttributeSeq;

	attributeMap::const_iterator it;
	unsigned i,j;
	const vector<string> * allowedValues = NULL;
	const attributeMap * attribs = sti_Server->registeredDevices[deviceID].getAttributes();

	STI_Client_Server::TAttributeSeq_var attribSeq( new TAttributeSeq );
	attribSeq->length(attribs->size());

	for(it = attribs->begin(), i = 0; it != attribs->end(); it++, i++)
	{
		attribSeq[i].key = CORBA::string_dup(it->first.c_str());
		attribSeq[i].value = CORBA::string_dup(it->second.value().c_str());
	
		allowedValues = it->second.valuelist();	// Attribute::valuelist()

		attribSeq[i].values.length(allowedValues->size());

		for(j = 0; j < allowedValues->size(); j++)
		{
			attribSeq[i].values[j] = 
				CORBA::string_dup( allowedValues->at(j).c_str() );
		}
	}

	return attribSeq._retn();
}


::CORBA::Boolean DeviceConfigure_i::setDeviceAttribute(const char* deviceID, const char* key, const char* value)
{
	bool success = false;

	if(sti_Server->deviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		success = sti_Server->
			registeredDevices[deviceID].setAttribute(key, value);
	}

	return success;
}


STI_Client_Server::TChannelSeq* DeviceConfigure_i::getDeviceChannels(const char* deviceID)
{
	using STI_Client_Server::TChannelSeq;

	unsigned i;

	const vector<STI_Server_Device::TDeviceChannel> * channels = 
		sti_Server->registeredDevices[deviceID].getChannels();

	STI_Client_Server::TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length(channels->size());

	STI_Server_Device::TDevice & tDevice = *sti_Server->registeredDevices[deviceID].device() ;

	for(i = 0; i < channels->size(); i++)
	{
		channelSeq[i].device.deviceType    = CORBA::string_dup(tDevice.deviceType);
		channelSeq[i].device.address       = CORBA::string_dup(tDevice.address);
		channelSeq[i].device.moduleNum     = tDevice.moduleNum;
		channelSeq[i].device.deviceID      = CORBA::string_dup(tDevice.deviceID);
		channelSeq[i].device.deviceContext = CORBA::string_dup(tDevice.deviceContext);

		channelSeq[i].channel    = channels->at(i).channel;
		channelSeq[i].type       = channels->at(i).type;
		channelSeq[i].inputType  = channels->at(i).inputType;
		channelSeq[i].outputType = channels->at(i).outputType;
	}

	return channelSeq._retn();
}

::CORBA::Boolean DeviceConfigure_i::deviceStatus(const char* deviceID)
{

	return sti_Server->deviceStatus(deviceID);
}


STI_Client_Server::TDeviceSeq* DeviceConfigure_i::devices()
{
	using STI_Client_Server::TDeviceSeq;

	int i;
	std::map<std::string, RemoteDevice>::iterator it;
	std::map<std::string, RemoteDevice>& devices = sti_Server->registeredDevices;

	STI_Client_Server::TDeviceSeq_var deviceSeq( new TDeviceSeq );
	deviceSeq->length(devices.size());


	for(it = devices.begin(), i = 0; it != devices.end(); it++, i++)
	{
		deviceSeq[i].deviceType    = CORBA::string_dup(it->second.device()->deviceType);
		deviceSeq[i].address       = CORBA::string_dup(it->second.device()->address);
		deviceSeq[i].moduleNum     = it->second.device()->moduleNum;
		deviceSeq[i].deviceID      = CORBA::string_dup(it->second.device()->deviceID);
		deviceSeq[i].deviceContext = CORBA::string_dup(it->second.device()->deviceContext);
	}

	return deviceSeq._retn();
}


