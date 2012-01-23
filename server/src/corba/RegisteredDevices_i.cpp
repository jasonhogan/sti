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

STI::Types::TLabeledData* RegisteredDevices_i::getLabledData(const char* deviceID, const char* label)
{
	return sti_Server->getLabledData(deviceID, label);
	//STI::Types::TLabeledData_var labeledData(
	//	sti_Server->getRegisteredDevices().find(deviceID)->second->getLabedData(label));

	//return labeledData._retn();
}
void RegisteredDevices_i::stopDevice(const char* deviceID)
{
	sti_Server->stopDevice(deviceID);
//	sti_Server->getRegisteredDevices().find(deviceID)->second->stop();
}

void RegisteredDevices_i::killDevice(const char* deviceID)
{
	sti_Server->killDevice(deviceID);
	//if(sti_Server->getDeviceStatus(deviceID))
	//{
	//	// deviceID found and Device is alive
	//	sti_Server->
	//		getRegisteredDevices().find(deviceID)->second->killDevice();
	//}
	//else
	//{
	//	refreshDevices();
	//}
}


STI::Types::TAttributeSeq* RegisteredDevices_i::getDeviceAttributes(const char* deviceID)
{
	return sti_Server->getDeviceAttributes(deviceID);

	//using STI::Types::TAttributeSeq;

	//attributeMap::const_iterator it;
	//unsigned i,j;
	//const vector<string>* allowedValues = NULL;
	//const AttributeMap& attribs = sti_Server->getRegisteredDevices()[deviceID].getAttributes();

	//STI::Types::TAttributeSeq_var attribSeq( new TAttributeSeq );
	//attribSeq->length(attribs.size());

	//for(it = attribs.begin(), i = 0; it != attribs.end(); it++, i++)
	//{
	//	attribSeq[i].key = CORBA::string_dup(it->first.c_str());
	//	attribSeq[i].value = CORBA::string_dup(it->second.value().c_str());
	//
	//	allowedValues = it->second.valuelist();	// Attribute::valuelist()

	//	attribSeq[i].values.length(allowedValues->size());

	//	//get allowed values for this attribute
	//	for(j = 0; j < allowedValues->size(); j++)
	//	{
	//		attribSeq[i].values[j] = 
	//			CORBA::string_dup( allowedValues->at(j).c_str() );
	//	}
	//}

	//return attribSeq._retn();
}


::CORBA::Boolean RegisteredDevices_i::setDeviceAttribute(const char* deviceID, const char* key, const char* value)
{
	return sti_Server->setDeviceAttribute(deviceID, key, value);

	//bool success = false;

	//if(sti_Server->getDeviceStatus(deviceID))
	//{
	//	// deviceID found and Device is alive
	//	success = sti_Server->
	//		getRegisteredDevices().find(deviceID)->second->setAttribute(key, value);
	//}

	//return success;
}

::CORBA::Boolean RegisteredDevices_i::setDeviceChannelName(const char* deviceID, ::CORBA::Short channel, const char* name)
{
	return sti_Server->setDeviceChannelName(deviceID, channel, name);
}


STI::Types::TChannelSeq* RegisteredDevices_i::getDeviceChannels(const char* deviceID)
{
	return sti_Server->getDeviceChannels(deviceID);
	
	/*using STI::Types::TChannelSeq;

	unsigned i;

	const vector<STI::Types::TDeviceChannel> & channels = 
		sti_Server->getRegisteredDevices().find(deviceID)->second->getChannels();

	STI::Types::TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length(channels.size());

	const STI::Types::TDevice& tDevice = sti_Server->getRegisteredDevices().find(deviceID)->second->getDevice();

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

	return channelSeq._retn();*/
}

STI::Types::TPartnerSeq* RegisteredDevices_i::getDevicePartners(const char* deviceID)
{
	return sti_Server->getDevicePartners(deviceID);

	//using STI::Types::TPartnerSeq;

	//unsigned i;

	//const vector<STI::Types::TPartner>& partners = 
	//	sti_Server->getRegisteredDevices().find(deviceID)->second->getPartners();

	//STI::Types::TPartnerSeq_var partnerSeq( new TPartnerSeq );
	//partnerSeq->length(partners.size());

	//for(i = 0; i < partners.size(); i++)
	//{
	//	partnerSeq[i].partnerDeviceID = CORBA::string_dup( partners.at(i).partnerDeviceID );
	//	
	//	partnerSeq[i].deviceName = CORBA::string_dup( partners.at(i).deviceName );
	//	partnerSeq[i].ipAddress  = CORBA::string_dup( partners.at(i).ipAddress );
	//	partnerSeq[i].moduleNum  = partners.at(i).moduleNum;
	//	
	//	partnerSeq[i].isRequired    = partners.at(i).isRequired;
	//	partnerSeq[i].isEventTarget = partners.at(i).isEventTarget;
	//	partnerSeq[i].isMutual      = partners.at(i).isMutual;
	//	partnerSeq[i].isRegistered  = partners.at(i).isRegistered;
	//}

	//return partnerSeq._retn();
}

::CORBA::Boolean RegisteredDevices_i::deviceStatus(const char* deviceID)
{
	return sti_Server->getDeviceStatus(deviceID);
}


STI::Types::TDeviceSeq* RegisteredDevices_i::devices()
{
	return sti_Server->getDevices();

	//using STI::Types::TDeviceSeq;

	//int i;
	//RemoteDeviceMap::const_iterator it;
	//const RemoteDeviceMap& devices = sti_Server->getRegisteredDevices();

	//STI::Types::TDeviceSeq_var deviceSeq( new TDeviceSeq );
	//deviceSeq->length(devices.size());


	//for(it = devices.begin(), i = 0; it != devices.end(); it++, i++)
	//{
	//	deviceSeq[i].deviceName    = CORBA::string_dup( (it->second)->getDevice().deviceName );
	//	deviceSeq[i].address       = CORBA::string_dup( (it->second)->getDevice().address );
	//	deviceSeq[i].moduleNum     = (it->second)->getDevice().moduleNum;
	//	deviceSeq[i].deviceID      = CORBA::string_dup( (it->second)->getDevice().deviceID );
	//	deviceSeq[i].deviceContext = CORBA::string_dup( (it->second)->getDevice().deviceContext );
	//}

	//return deviceSeq._retn();
}


void RegisteredDevices_i::refreshDevices()
{
	sti_Server->refreshDevices();

	STI::Pusher::TDeviceRefreshEvent refreshEvent;
	refreshEvent.type = STI::Pusher::RefreshDeviceList;
	
	sti_Server->sendEvent(refreshEvent);
}

void RegisteredDevices_i::stopRefreshing()
{
	sti_Server->stopServer();
}

::CORBA::Long RegisteredDevices_i::devicePing(const char* deviceID)
{
	return sti_Server->devicePing(deviceID);

	/*RemoteDeviceMap::const_iterator it = sti_Server->getRegisteredDevices().
		find( string(deviceID) );

	if(it != sti_Server->getRegisteredDevices().end())
		return it->second->pingDevice();
	else
		return -2;*/
}

