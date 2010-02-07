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



::CORBA::Boolean ServerConfigure_i::registerDevice(STI::Types::TDevice& device, STI::Server_Device::DeviceBootstrap_ptr bootstrap)
{
	bool registered = false;;

	registrationMutex->lock();
	{
		registered = sti_Server->registerDevice(device, bootstrap);
	}
	registrationMutex->unlock();

	return registered;
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


::CORBA::Boolean ServerConfigure_i::ping()
{
	return true;
}




