/*! \file ServerCommandLine_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ServerCommandLine_i
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

#include "ServerCommandLine_i.h"

#include "STI_Server.h"

#include <MixedData.h> 

ServerCommandLine_i::ServerCommandLine_i(STI_Server* server) :
sti_Server(server)
{
}

ServerCommandLine_i::~ServerCommandLine_i()
{
}
    
char* ServerCommandLine_i::deviceCmdName(const char* deviceID)
{
	char* dummy = 0;
	return dummy;
}

char* ServerCommandLine_i::executeArgs(const char* deviceID, const char* args)
{
	CORBA::String_var returnValue( sti_Server->executeArgs(deviceID, args).c_str() );
	return returnValue._retn();
}

::CORBA::Boolean ServerCommandLine_i::writeChannel(const char* deviceID, ::CORBA::UShort channel, const STI::Types::TValMixed& value)
{
	return sti_Server->writeChannelDevice(deviceID, channel, MixedValue(value));
}



::CORBA::Boolean ServerCommandLine_i::readChannel(const char* deviceID, ::CORBA::UShort channel, const STI::Types::TValMixed& value, STI::Types::TDataMixed_out data)
{
	MixedData mixedData;

	bool success = sti_Server->readChannelDevice(deviceID, channel, MixedValue(value), mixedData);

	data = new STI::Types::TDataMixed();

	if(success)
	{
		(*data) = mixedData.getTDataMixed();
	}

	return success;
}

STI::Types::TStringSeq* ServerCommandLine_i::registeredPartners(const char* deviceID)
{
	sti_Server->refreshPartnersDevices();
	
	vector<string> registeredPartnerList;
	sti_Server->getRegisteredPartners(deviceID, registeredPartnerList);

	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length( registeredPartnerList.size() );

	unsigned i;
	for(i = 0; i < registeredPartnerList.size(); i++)
	{
		stringSeq[i] = CORBA::string_dup( registeredPartnerList[i].c_str() );
	}
	return stringSeq._retn();
}

STI::Types::TStringSeq* ServerCommandLine_i::requiredPartners(const char* deviceID)
{
	sti_Server->refreshPartnersDevices();
	vector<string> requiredPartnerList;
	sti_Server->getRequiredPartners(deviceID, requiredPartnerList);

	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length( requiredPartnerList.size() );

	unsigned i;
	for(i = 0; i < requiredPartnerList.size(); i++)
	{
		stringSeq[i] = CORBA::string_dup( requiredPartnerList[i].c_str() );
	}
	return stringSeq._retn();
}