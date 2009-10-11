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

STI::Types::TStringSeq* ServerCommandLine_i::registeredPartners(const char* deviceID)
{
	sti_Server->refreshPartnersDevices();
	const vector<string>& registeredPartnerList = sti_Server->getRegisteredPartners( deviceID );

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
	const vector<string>& requiredPartnerList = sti_Server->getRequiredPartners( deviceID );

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