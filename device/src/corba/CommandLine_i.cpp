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

#include "device.h"
#include "CommandLine_i.h"
#include <vector>
#include <string>

CommandLine_i::CommandLine_i(STI_Device* device) : sti_device(device)
{
}

CommandLine_i::~CommandLine_i()
{
}

char* CommandLine_i::executeArgs(const char* args)
{
	CORBA::String_var result( sti_device->executeArgs(args).c_str() );
	return result._retn();
//	return CORBA::string_dup( sti_device->executeArgs(args).c_str() );
}

::CORBA::Boolean CommandLine_i::registerPartnerDevice(STI_Server_Device::CommandLine_ptr partner)
{
	if(registeredPartners.count(partner->deviceName()) == 0)  // No instances of this partner are registered
	{
		registeredPartners[partner->deviceName()] = partner;
		return true;
	}
	return false;	// partner device with same name is already registered
}

STI_Server_Device::TStringSeq* CommandLine_i::partnerDevices()
{
	using STI_Server_Device::TStringSeq;
	using STI_Server_Device::TStringSeq_var;

	const vector<string> & partnerDevices = * sti_device->getPartnerDevices();
	TStringSeq_var stringSeq( new TStringSeq(partnerDevices.size()) );

	unsigned i;
	for(i = 0; i < partnerDevices.size(); i++)
	{
		stringSeq[i] = CORBA::string_dup( partnerDevices[i].c_str() );
	}
	return stringSeq._retn();
}

char* CommandLine_i::deviceName()
{
	CORBA::String_var name( sti_device->commandLineDeviceName().c_str() );
	return name._retn();
//	return CORBA::string_dup( sti_device->commandLineDeviceName().c_str() );
}
