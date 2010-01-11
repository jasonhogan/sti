/*! \file ModeHandler_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ModeHandler_i
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


#include "ModeHandler_i.h"
#include <iostream>
#include <string>
#include <vector>

ModeHandler_i::ModeHandler_i() :
requestPending(false),
requestYielded(false),
modeInterrupt(NULL)
{
	requestMutex = new omni_mutex();
}


ModeHandler_i::~ModeHandler_i()
{
}


::CORBA::Boolean ModeHandler_i::requestControl(const char* myName, const char* ipAddress, STI::Client_Server::ModeInterrupt_ptr interrupt)
{
	requestYielded = false;

	requestMutex->lock();
	{
		if(requestPending)
			return false;	//only one request at a time
		else
			requestPending = true;
	}
	requestMutex->unlock();

	int timeout = 20;
	unsigned long wait_secs = 1;
	if(modeInterrupt != NULL)
	{
		if( !modeInterrupt->_is_equivalent(interrupt) )
		{
			try {
				modeInterrupt->requestControl(myName, ipAddress);
			} catch(...) {
				// can't reach the old client
				requestYielded = true;
			}
		}
		else
		{
			//the new client already has control
			return true;
		}
	}
	else
	{
		//there is no old client
		requestYielded = true;
	}

	while(requestPending && !requestYielded && timeout > 0)
	{
		omni_thread::sleep(wait_secs);
		timeout--;
	}

	if(requestYielded)
	{
		setName(myName);
		setIP(ipAddress);
		modeInterrupt = STI::Client_Server::ModeInterrupt::_duplicate(interrupt);
	}

	requestMutex->lock();
	{
		requestPending = false;
	}
	requestMutex->unlock();

	return requestYielded;
}

::CORBA::Boolean ModeHandler_i::takeControl(const char* myName, const char* ipAddress, STI::Client_Server::ModeInterrupt_ptr interrupt)
{
	if(modeInterrupt != NULL)
	{
		try {
			modeInterrupt->controlTakenBy(myName, ipAddress);
		} catch(...) {}
	}
	setName(myName);
	setIP(ipAddress);
	modeInterrupt = STI::Client_Server::ModeInterrupt::_duplicate(interrupt);

	return true;
}

void ModeHandler_i::cancelRequest()
{
	requestMutex->lock();
	{
		requestPending = false;
	}
	requestMutex->unlock();
}

void ModeHandler_i::answerRequest(::CORBA::Boolean yield)
{
	requestYielded = yield;
	
	requestMutex->lock();
	{
		requestPending = false;
	}
	requestMutex->unlock();
}

char* ModeHandler_i::controllerName()
{
	CORBA::String_var name( controllerName_l.c_str() );
	return name._retn();
}

char* ModeHandler_i::controllerIP()
{
	CORBA::String_var ip( controllerIP_l.c_str() );
	return ip._retn();
}

void ModeHandler_i::setName(std::string name)
{
	controllerName_l = name;
}

void ModeHandler_i::setIP(std::string ip)
{
	controllerIP_l = ip;
}
