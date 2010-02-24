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
#include <DataMeasurement.h>

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

/*
PartnerDeviceMap& CommandLine_i::getRegisteredPartners()
{
	return registeredPartners;
}
*/

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

::CORBA::Boolean CommandLine_i::writeChannel(::CORBA::UShort channel, const STI::Types::TValMixed& value)
{
	return sti_device->write(channel, MixedValue(value));
}

::CORBA::Boolean CommandLine_i::readChannel(::CORBA::UShort channel, const STI::Types::TValMixed& value, STI::Types::TDataMixed_out data)
{
//	DataMeasurement measurement(100000, channel, 1);

	MixedData mixedData;

	bool success = sti_device->read(channel, MixedValue(value), mixedData);

	data = new STI::Types::TDataMixed();

	if(success)
	{
		(*data) = mixedData.getTDataMixed();
	}

	return success;
}


::CORBA::Boolean CommandLine_i::registerPartnerDevice(STI::Server_Device::CommandLine_ptr partnerCmdLine)
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
//	const map<string, string> &requiredPartners = sti_device->getRequiredPartners();

	bool alive = false;
	bool registered = false;
	string partnerDeviceID;

	// check the 'partner' is actually alive
	try {
		partnerDeviceID = string(partnerCmdLine->device()->deviceID);	// try to talk to the partner
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


	PartnerDevice* partner;
	std::string partnerName;
	
	if( alive )
	{

		partnerName = sti_device->getPartnerName(partnerDeviceID);

		PartnerDevice* partner = &(sti_device->partnerDevice( partnerName ));
		
		if( !partner->exists() )
		{
			//check if it is stored by deviceID
			partner = &(sti_device->partnerDevice( 
				sti_device->getPartnerDeviceID(partnerDeviceID) ));
		}

		if( partner->exists() )
		{
			partner->registerPartnerDevice( CommandLine::_duplicate(partnerCmdLine) );
			registered = true;
		}
		else
		{
			//this partner is not required (it was not added using 'addPartnerDevice' inside
			//'definePartnerDevices'.  Add it using the deviceID as the partner name.
			(
				(sti_device->getPartnerDeviceMap())[partnerDeviceID] = 
				PartnerDevice(partnerDeviceID, partnerDeviceID, false, false)
				
			).registerPartnerDevice( CommandLine::_duplicate(partnerCmdLine) );
			
			registered = true;
		}
/*
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
*/
	}

	// Check if the newly registered partner is a mutual partner.  If so, pass a reference of this device
	// to the partner.
	if( registered )
	{
		partner = &(sti_device->partnerDevice(
			sti_device->getPartnerName(partnerDeviceID) ));

		if(partner->exists() && partner->isRegistered() && partner->isMutual() )
		{
			registered = partner->registerMutualPartner( sti_device->generateCommandLineReference() );
		}


/*
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

		*/
	}

	//let the device know that there might be newly registered partners
	sti_device->checkForNewPartners();

	return registered;
}

::CORBA::Boolean CommandLine_i::unregisterPartnerDevice(const char* deviceID)
{

	PartnerDevice& partner = sti_device->partnerDevice( sti_device->getPartnerName(deviceID) );

	if( partner.exists() )
	{
		partner.unregisterPartner();
		return true;
	}
	return false;
/*
	PartnerDeviceMap::iterator it =	
		registeredPartners.find( string(deviceID) );

	if(it != registeredPartners.end())
		registeredPartners.erase( it );

	return true;
*/
}


STI::Types::TStringSeq* CommandLine_i::eventPartnerDevices()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	PartnerDeviceMap& partners = sti_device->getPartnerDeviceMap();
	PartnerDeviceMap::iterator iter;

	TStringSeq_var stringSeq( new TStringSeq );

	//count first
	unsigned count = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( (iter->second)->getPartnerEventsSetting() )
			count++;
	}

	stringSeq->length( count );

	unsigned i = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( iter->second->getPartnerEventsSetting() )
		{
			stringSeq[i] = CORBA::string_dup( iter->second->getDeviceID().c_str() );	//deviceID
			i++;
		}
	}
	return stringSeq._retn();
}

STI::Types::TStringSeq* CommandLine_i::requiredPartnerDevices()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	PartnerDeviceMap& partners = sti_device->getPartnerDeviceMap();
	PartnerDeviceMap::iterator iter;

	TStringSeq_var stringSeq( new TStringSeq );

	//count first
	unsigned count = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( iter->second->isRequired() )
			count++;
	}

	stringSeq->length( count );

	unsigned i = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( iter->second->isRequired() )
		{
			stringSeq[i] = CORBA::string_dup( iter->second->getDeviceID().c_str() );	//deviceID
			i++;
		}
	}
	return stringSeq._retn();
}


STI::Types::TStringSeq* CommandLine_i::registeredPartnerDevices()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	PartnerDeviceMap& partners = sti_device->getPartnerDeviceMap();
	PartnerDeviceMap::iterator iter;

	TStringSeq_var stringSeq( new TStringSeq );
	
	//count first
	unsigned count = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( iter->second->isRegistered() )
			count++;
	}

	stringSeq->length( count );
		
	unsigned i = 0;
	for(iter = partners.begin(); iter != partners.end(); iter++)
	{
		if( iter->second->isRegistered() )
		{
			stringSeq[i] = CORBA::string_dup( iter->second->getDeviceID().c_str() );	//deviceID
			i++;
		}
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


STI::Types::TPartnerDeviceEventSeq* CommandLine_i::getPartnerEvents(const char* deviceID)
{
	using STI::Types::TPartnerDeviceEventSeq;

	std::vector<STI::Types::TPartnerDeviceEvent>& deviceEvents = 
		sti_device->getPartnerEvents(deviceID);

	STI::Types::TPartnerDeviceEventSeq_var eventSeq( new TPartnerDeviceEventSeq );
	eventSeq->length(deviceEvents.size());

	for(unsigned i = 0; i < deviceEvents.size(); i++)
	{
		eventSeq[i] = deviceEvents.at(i);
	}

	return eventSeq._retn();
}

