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

ServerConfigure_i::ServerConfigure_i(STI_Server* server) : sti_Server(server)
{
	registrationMutex = new omni_mutex();
	activationMutex   = new omni_mutex();
	
	timeOutMutex      = new omni_mutex();
	timeOutCondition  = new omni_condition(timeOutMutex);

	waitingForActivation = false;
	timeOutPeriod = 10;		//10 seconds
}

ServerConfigure_i::~ServerConfigure_i()
{
}

STI::Pusher::DeviceEventHandler_ptr ServerConfigure_i::getDeviceEventHandler()
{
	return sti_Server->getDeviceEventHandler();
}


void ServerConfigure_i::pauseServer(const char* deviceID)
{
	sti_Server->pauseServer(true);
	sti_Server->pauseAllDevicesExcept(deviceID);

}


void ServerConfigure_i::unpauseServer(const char* deviceID)
{
//	if( sti_Server->isPausedByDevice() )
//		sti_Server->playEvents();

	sti_Server->unpauseServer(true);
//	sti_Server->playAllDevices();

}


void ServerConfigure_i::waitForActivation()
{
	unsigned long wait_s;
	unsigned long wait_ns;

	timeOutMutex->lock();
	{
		//Find the absolute time for waking up (timeOutPeriod seconds from now)
		omni_thread::get_time(&wait_s, &wait_ns, timeOutPeriod, 0);

		timeOutCondition->timedwait(wait_s, wait_ns);
	}
	timeOutMutex->unlock();
}

::CORBA::Boolean ServerConfigure_i::registerDevice(STI::Types::TDevice& device)
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
							   const STI::Types::TDeviceChannelSeq& channels)
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
		timeOutCondition->signal();
	}
	activationMutex->unlock();

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


char* ServerConfigure_i::generateDeviceID(const STI::Types::TDevice& device)
{
	CORBA::String_var deviceID( sti_Server->generateDeviceID(device).c_str() );
	return deviceID._retn();
}


STI::Types::TAttributeSeq* ServerConfigure_i::attributes()
{
	STI::Types::TAttributeSeq* dummy = 0;
	return dummy;
}


char* ServerConfigure_i::serverName()
{
	CORBA::String_var name( sti_Server->getServerName().c_str() );
	return name._retn();
}



//::CORBA::Boolean ServerConfigure_i::setAttribute(const char* deviceID, 
//const char* key, const char* value)
//{
//	return sti_Server->setAttribute(deviceID, key, value);
//}
//
//char* ServerConfigure_i::getAttribute(const char* deviceID, const char* key)
//{
//	CORBA::String_var value( sti_Server->getAttributes()->find(key)->second.value().c_str() );
//	return value._retn();
//}

