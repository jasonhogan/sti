/*! \file Configure_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Configure_i
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
#include "Configure_i.h"
#include "STI_Device.h"

#include <string>
#include <map>

using std::string;
using std::map;

#include <iostream>
using namespace std;

Configure_i::Configure_i(STI_Device* device) : sti_Device(device)
{
}

Configure_i::~Configure_i()
{
}

void Configure_i::reRegisterWithServer()
{
	sti_Device->reRegisterDevice();
}

void Configure_i::kill()
{
	sti_Device->deviceShutdown();
}

::CORBA::Boolean Configure_i::setAttribute(const char *key, const char *value)
{
	if( sti_Device->setAttribute(key, value) )
	{
		sti_Device->refreshDeviceAttributes();
		return true;
	}
	return false;
}

char* Configure_i::getAttribute(const char *key)
{
	CORBA::String_var value( 
		sti_Device->getAttributes().find(key)->second.value().c_str() );
	return value._retn();
}

STI::Types::TAttributeSeq* Configure_i::attributes()
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


char* Configure_i::deviceName()
{
	CORBA::String_var type( sti_Device->getDeviceName().c_str() );
	return type._retn();
}
