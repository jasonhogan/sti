/*! \file ClientBootstrap_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ClientBootstrap_i
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef CLIENTBOOTSTRAP_I_H
#define CLIENTBOOTSTRAP_I_H

#include "client.h"

class STI_Server;

class ClientBootstrap_i : public POA_STI::Client_Server::ClientBootstrap
{
public:
	ClientBootstrap_i(STI_Server* server);
	~ClientBootstrap_i();

	::CORBA::Boolean connect(STI::Pusher::ServerEventHandler_ptr eventHandler);
	STI::Client_Server::ModeHandler_ptr getModeHandler();
	STI::Client_Server::Parser_ptr getParser();
    STI::Client_Server::ExpSequence_ptr getExpSequence();
    STI::Client_Server::ServerTimingSeqControl_ptr getServerTimingSeqControl();
    STI::Client_Server::RegisteredDevices_ptr getRegisteredDevices();
    STI::Client_Server::ServerCommandLine_ptr getServerCommandLine();

private:
	
	STI_Server* sti_server;
};

#endif
