/*! \file ServerConfigure_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ServerConfigure_i
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
#include "ServerConfigure_i.h"
#include "RemoteDevice.h"
#include "STI_Server.h"

//#include <sstream>
#include <string>
#include <map>
#include <queue>
using std::map;
//using std::stringstream;
using std::string;
using std::queue;

#include <iostream>
using namespace std;

//omni_mutex* ServerConfigure_i::registrationMutex = new omni_mutex();

ServerConfigure_i::ServerConfigure_i(STI_Server* server) : sti_Server(server)
{
	registrationMutex   = new omni_mutex();
	
	timeOutMutex        = new omni_mutex();
	timeOutCondition = new omni_condition(timeOutMutex);

	activationMutex     = new omni_mutex();

	waitingForActivation = false;

	timeOutPeriod = 10;	//10 seconds

	instanceID = 0;
}

ServerConfigure_i::~ServerConfigure_i()
{
}

void ServerConfigure_i::block()
{
	int thisInstance;

	registrationMutex->lock();
	{
		thisInstance = ++instanceID;

		fifo.push(thisInstance);
	}
	registrationMutex->unlock();
	
	while(fifo.front() != thisInstance) {}
}

void ServerConfigure_i::unblock()
{
	registrationMutex->lock();
	{
		fifo.pop();
	}
	registrationMutex->unlock();
}



void ServerConfigure_i::waitForActivation()
{
	unsigned long wait_s;
	unsigned long wait_ns;

	timeOutMutex->lock();
	{
		//Find the absolute time for waking up (timeOutPeriod seconds from now)
		omni_thread::get_time(&wait_s, &wait_ns, timeOutPeriod, 0);

		cout << "sleeping" << endl;
		timeOutCondition->timedwait(wait_s, wait_ns);
	}
	timeOutMutex->unlock();
}

::CORBA::Boolean ServerConfigure_i::registerDevice(STI_Server_Device::TDevice& device)
{
	bool registered = false;

	registrationMutex->lock();
	{
		if(waitingForActivation) 
			waitForActivation();

		registered = sti_Server->registerDevice(device);
		waitingForActivation = registered;
	}
	registrationMutex->unlock();

	return registered;
}


::CORBA::Boolean 
ServerConfigure_i::setChannels(const char* deviceID, 
							   const STI_Server_Device::TDeviceChannelSeq& channels)
{
	return sti_Server->setChannels(deviceID, channels);
}

::CORBA::Boolean ServerConfigure_i::activateDevice(const char* deviceID)
{
	
	bool active = false;

	activationMutex->lock();
	{
		active = sti_Server->activateDevice(deviceID);
		waitingForActivation = !active;
	//	if( !waitingForActivation ) 
		timeOutCondition->signal();
	}
	activationMutex->unlock();

	cerr << "activated!" << endl;
	return active;
}


::CORBA::Boolean ServerConfigure_i::removeDevice(const char* deviceID)
{
	bool removed = false;

	registrationMutex->lock();
	{
		removed = sti_Server->removeDevice(deviceID);
	}
	registrationMutex->unlock();

	return removed;
}


char* ServerConfigure_i::generateDeviceID(const STI_Server_Device::TDevice& device)
{
	CORBA::String_var deviceID( sti_Server->generateDeviceID(device).c_str() );
	return deviceID._retn();
}


STI_Server_Device::TAttributeSeq* ServerConfigure_i::attributes()
{
	STI_Server_Device::TAttributeSeq* dummy = 0;
	return dummy;
}


char* ServerConfigure_i::serverName()
{
	CORBA::String_var name( sti_Server->getServerName().c_str() );
	return name._retn();
}


/*
::CORBA::Boolean ServerConfigure_i::setAttribute(const char* deviceID, 
const char* key, const char* value)
{
	return sti_Server->setAttribute(deviceID, key, value);
}

char* ServerConfigure_i::getAttribute(const char* deviceID, const char* key)
{
	return CORBA::string_dup(
		sti_Server->getAttributes()->find(key)->second.value().c_str());
}
*/
