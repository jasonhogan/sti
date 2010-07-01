/*! \file ServerCommandLine_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ServerCommandLine_i
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef SERVERCOMMANDLINE_I_H
#define SERVERCOMMANDLINE_I_H

#include "client.h"

class STI_Server;


class ServerCommandLine_i : public POA_STI::Client_Server::ServerCommandLine
{
public:

	ServerCommandLine_i(STI_Server* server);
	~ServerCommandLine_i();
    
	char* deviceCmdName(const char* deviceID);
    char* executeArgs(const char* deviceID, const char* args);
	
	::CORBA::Boolean writeChannel(const char* deviceID, ::CORBA::UShort channel, const STI::Types::TValMixed& value);
	::CORBA::Boolean readChannel(const char* deviceID, ::CORBA::UShort channel, const STI::Types::TValMixed& value, STI::Types::TDataMixed_out data);
	
	STI::Types::TStringSeq* registeredPartners(const char* deviceID);
	STI::Types::TStringSeq* requiredPartners(const char* deviceID);

private:

	STI_Server* sti_Server;
};

#endif
