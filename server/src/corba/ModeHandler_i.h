/*! \file ModeHandler_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ModeHandler_i
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

#ifndef MODEHANDLER_I_H
#define MODEHANDLER_I_H

#include "client.h"
#include <string>

class ModeHandler_i : public POA_STI::Client_Server::ModeHandler
{
public:

	ModeHandler_i();
	~ModeHandler_i();

	::CORBA::Boolean requestControl(
		const char* myName, 
		const char* ipAddress, 
		STI::Client_Server::ModeInterrupt_ptr interrupt);
	::CORBA::Boolean takeControl(
		const char* myName, 
		const char* ipAddress, 
		STI::Client_Server::ModeInterrupt_ptr interrupt);
    void cancelRequest();
    void answerRequest(::CORBA::Boolean yield);
    char* controllerName();
    char* controllerIP();

	void setName(std::string name);
	void setIP(std::string ip);

private:

	std::string controllerName_l;
	std::string controllerIP_l;
	bool requestPending;
	bool requestYielded;

	STI::Client_Server::ModeInterrupt_var modeInterrupt;

	omni_mutex *requestMutex;
};

#endif
