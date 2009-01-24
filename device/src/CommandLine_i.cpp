/*! \file CommandLine_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class CommandLine_i
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

#include <device.h>
#include <CommandLine_i.h>
#include <STI_Device.h>

#include <vector>
#include <string>

#include <iostream>
using namespace std;

CommandLine_i::CommandLine_i(STI_Device* device) : sti_device(device)
{
}

CommandLine_i::~CommandLine_i()
{
}

char* CommandLine_i::execute(const char* args)
{
	CORBA::String_var result( sti_device->execute(args).c_str() );
	return result._retn();
}

::CORBA::Boolean CommandLine_i::registerPartnerDevice(STI_Server_Device::CommandLine_ptr partner)
{
	// Partner registration makes use of two maps:
	// STI_Device::requiredPartners:       PartnerName => DeviceID
	// CommandLine_i::registeredPartners:  DeviceID    => PartnerDevice
	//
	// The following searches requiredPartners for the DeviceID that is being registered
	// and gets the associated PartnerName.

	const map<string, string> &requiredPartners = sti_device->getRequiredPartners();
	map<string, string>::const_iterator iter = requiredPartners.begin();

	// Partner registration always overwrites any pre-existing registration.  
	// This gives the server the responsibility to keep all object references current.

	bool found = false;

	try {
		string partnerDeviceID = partner->deviceID();	// try to talk to the partner

		// This is a reverse map search. The item being search for is the
		// second map entry: map<first, second>.  This means map::find()
		// cannot be used here.

		while( !found && iter != requiredPartners.end() )
		{
			if(iter->second.compare(partnerDeviceID) == 0)	//deviceID comparison
			{
				found = true;
				registeredPartners[partnerDeviceID] = PartnerDevice(iter->first, 
					STI_Server_Device::CommandLine::_duplicate(partner));
			}
			iter++;
		}
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " when attempting to contact a partner device." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact a partner device." << endl;
	}

	return found;
}

::CORBA::Boolean CommandLine_i::unregisterPartnerDevice(const char* deviceID)
{
	std::map<std::string, PartnerDevice>::iterator it =	
		registeredPartners.find( string(deviceID) );

	if(it != registeredPartners.end())
		registeredPartners.erase( it );

	return true;
}


STI_Server_Device::TStringSeq* CommandLine_i::requiredPartnerDevices()
{
	using STI_Server_Device::TStringSeq;
	using STI_Server_Device::TStringSeq_var;

	const map<string, string> &requiredPartners = sti_device->getRequiredPartners();
	map<string, string>::const_iterator iter;

	TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length( requiredPartners.size() );

	unsigned i;
	for(iter = requiredPartners.begin(), i = 0; iter != requiredPartners.end(); iter++, i++)
	{
		stringSeq[i] = CORBA::string_dup( iter->second.c_str() );	//deviceID
	}
	return stringSeq._retn();
}

char* CommandLine_i::deviceID()
{
	CORBA::String_var result( sti_device->getTDevice().deviceID );
	return result._retn();
}

