/*! \file ClientBootstrap_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ClientBootstrap_i
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

#include "ClientBootstrap_i.h"
#include <STI_Server.h>

ClientBootstrap_i::ClientBootstrap_i(STI_Server* server) : sti_server(server)
{
}

ClientBootstrap_i::~ClientBootstrap_i()
{
}

::CORBA::Boolean ClientBootstrap_i::connect(STI::Pusher::ServerEventHandler_ptr eventHandler)
{
	return sti_server->addNewClient(eventHandler);
}

STI::Client_Server::ModeHandler_ptr ClientBootstrap_i::getModeHandler()
{
	return sti_server->getModeHandler();
}

STI::Client_Server::Parser_ptr ClientBootstrap_i::getParser()
{
	return sti_server->getParser();
}

STI::Client_Server::ExpSequence_ptr ClientBootstrap_i::getExpSequence()
{
	return sti_server->getExpSequence();
}

STI::Client_Server::ServerTimingSeqControl_ptr ClientBootstrap_i::getServerTimingSeqControl()
{
	return sti_server->getServerTimingSeqControl();
}

STI::Client_Server::RegisteredDevices_ptr ClientBootstrap_i::getRegisteredDevices()
{
	return sti_server->getRegisteredDevices();
}

STI::Client_Server::ServerCommandLine_ptr ClientBootstrap_i::getServerCommandLine()
{
	return sti_server->getServerCommandLine();
}

