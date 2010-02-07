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
#include "STI_Device.h"

#include <string>
#include <map>

using std::string;
using std::map;

#include <iostream>
using namespace std;

DeviceConfigure_i::DeviceConfigure_i(STI_Device* device) : sti_Device(device)
{
}

DeviceConfigure_i::~DeviceConfigure_i()
{
}

STI::Types::TDeviceChannelSeq* DeviceConfigure_i::channels()
{
	using STI::Types::TDeviceChannel;
	using STI::Types::TDeviceChannelSeq;
	using STI::Types::TDeviceChannelSeq_var;

	unsigned i;
	ChannelMap::const_iterator it;

	const ChannelMap& channels = sti_Device->getChannels();

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

	return channelSeq._retn();
}

::CORBA::Boolean DeviceConfigure_i::setChannel(::CORBA::Short channel, const STI::Types::TValMixed& value)
{
	return false;
}


::CORBA::Boolean DeviceConfigure_i::ping()
{
	return true;
}

void DeviceConfigure_i::reRegisterWithServer()
{
	sti_Device->reRegisterDevice();
}

void DeviceConfigure_i::kill()
{
	sti_Device->deviceShutdown();
}

::CORBA::Boolean DeviceConfigure_i::setAttribute(const char *key, const char *value)
{
	if( sti_Device->setAttribute(key, value) )
	{
		sti_Device->refreshDeviceAttributes();
		return true;
	}
	return false;
}

char* DeviceConfigure_i::getAttribute(const char *key)
{
	CORBA::String_var value( 
		sti_Device->getAttributes().find(key)->second.value().c_str() );
	return value._retn();
}

STI::Types::TAttributeSeq* DeviceConfigure_i::attributes()
{
	using STI::Types::TAttributeSeq;

	AttributeMap::const_iterator it;
	unsigned i,j;
	const vector<string> *allowedValues = NULL;
	const AttributeMap &attribs = sti_Device->getAttributes();

	STI::Types::TAttributeSeq_var attribSeq( new TAttributeSeq );
	attribSeq->length(attribs.size());

	for(it = attribs.begin(), i = 0; it != attribs.end(); it++, i++)
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

