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
#include <Configure_i.h>

#include <vector>
#include <string>

#include <iostream>
using namespace std;

CommandLine_i::CommandLine_i(STI_Device* device, Configure_i* configureServant) :
_configureServant(configureServant), 
sti_device(device)
{
}

CommandLine_i::~CommandLine_i()
{
}

PartnerDeviceMap& CommandLine_i::getRegisteredPartners()
{
	return registeredPartners;
}


char* CommandLine_i::execute(const char* args)
{
	CORBA::String_var result( sti_device->execute(args).c_str() );
	return result._retn();
}

::CORBA::Boolean CommandLine_i::setAttribute(const char *key, const char *value)
{
	return _configureServant->setAttribute(key, value);
}

char* CommandLine_i::getAttribute(const char *key)
{
	CORBA::String_var value( _configureServant->getAttribute(key) );
	return value._retn();
}

::CORBA::Boolean CommandLine_i::registerPartnerDevice(STI::Server_Device::CommandLine_ptr partner)
{
	// Partner registration makes use of two maps:
	// STI_Device::requiredPartners:       PartnerName => DeviceID
	// CommandLine_i::registeredPartners:  DeviceID    => PartnerDevice
	//
	// The following searches requiredPartners for the DeviceID that is being registered
	// and gets the associated PartnerName.

	// Partner registration always overwrites any pre-existing registration.  
	// This gives the server the responsibility to keep all object references current.

	using STI::Server_Device::CommandLine;
	const map<string, string> &requiredPartners = sti_device->getRequiredPartners();

	bool alive = false;
	bool foundInRequired = false;
	bool registered = false;
	string partnerDeviceID;

	// check the 'partner' is actually alive
	try {
		partnerDeviceID = string(partner->device()->deviceID);	// try to talk to the partner
		alive = true;

		//remove previously registered partner
		unregisterPartnerDevice( partnerDeviceID.c_str() );

	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " when attempting to contact a partner device." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact a partner device." << endl;
	}

	// This is a reverse map search. The item being search for is the
	// second map entry: map<first, second>.  This means map::find()
	// cannot be used here.

	
	if( alive )
	{
		map<string, string>::const_iterator iter = requiredPartners.begin();

		while( !foundInRequired && iter != requiredPartners.end() )
		{
			if(iter->second.compare(partnerDeviceID) == 0)	//deviceID comparison
			{
				foundInRequired = true;

				registeredPartners.insert(partnerDeviceID, 
					new PartnerDevice(iter->first, CommandLine::_duplicate(partner)) );
				registered = true;
			}
			iter++;
		}

		//this partner is not required (it was not added using 'addPartnerDevice' inside
		//'definePartnerDevices'.  Add it using the deviceID as the partner name.
		if( !foundInRequired )
		{
			registeredPartners.insert(partnerDeviceID, 
				new PartnerDevice(partnerDeviceID, CommandLine::_duplicate(partner)) );
			registered = true;
		}
	}

	// Check if the newly registered partner is a mutual partner.  If so, pass a reference of this device
	// to the partner.
	if( registered )
	{
		unsigned i;
		const std::vector<std::string>& mutualPartners = sti_device->getMutualPartners();
		
		PartnerDeviceMap::iterator newPartner = registeredPartners.find(partnerDeviceID);

		if(newPartner != registeredPartners.end())
		{
			for(i = 0; i < mutualPartners.size(); i++)
			{
				if(mutualPartners.at(i).compare(newPartner->second->name()) == 0)
				{
					registered = newPartner->second->
						registerMutualPartner( sti_device->generateCommandLineReference() );
				}
			}
		}
	}

	return registered;
}

::CORBA::Boolean CommandLine_i::unregisterPartnerDevice(const char* deviceID)
{
	PartnerDeviceMap::iterator it =	
		registeredPartners.find( string(deviceID) );

	if(it != registeredPartners.end())
		registeredPartners.erase( it );

	return true;
}


STI::Types::TStringSeq* CommandLine_i::requiredPartnerDevices()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

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


STI::Types::TStringSeq* CommandLine_i::registeredPartnerDevices()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length( registeredPartners.size() );

	unsigned i;
	PartnerDeviceMap::iterator partner;
	for(partner = registeredPartners.begin(), i = 0; partner != registeredPartners.end(); partner++, i++)
	{
		stringSeq[i] = CORBA::string_dup( partner->first.c_str() );	//deviceID
	}
	return stringSeq._retn();
}


STI::Types::TDevice* CommandLine_i::device()
{
	STI::Types::TDevice_var tDevice( new STI::Types::TDevice );

	tDevice->deviceName    = CORBA::string_dup(sti_device->getTDevice().deviceName);
	tDevice->address       = CORBA::string_dup(sti_device->getTDevice().address);
	tDevice->moduleNum     = sti_device->getTDevice().moduleNum;
	tDevice->deviceID      = CORBA::string_dup(sti_device->getTDevice().deviceID);
	tDevice->deviceContext = CORBA::string_dup(sti_device->getTDevice().deviceContext);

	return tDevice._retn();
}

::CORBA::Boolean CommandLine_i::transferPartnerEvents(const STI::Types::TDeviceEventSeq& events)
{
	return false;
}

