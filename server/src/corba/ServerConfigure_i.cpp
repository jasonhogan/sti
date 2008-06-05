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
#include "STI_Server.h"
#include "RemoteDevice.h"

#include <sstream>
#include <string>
#include <map>
#include <queue>
using std::map;
using std::stringstream;
using std::string;
using std::queue;

#include <iostream>
using namespace std;

typedef map<string, RemoteDevice> RemoteDeviceMap;


//static int instanceID = 0;
//static std::queue<int> fifo;


ServerConfigure_i::ServerConfigure_i(STI_Server* server) : sti_Server(server)
{
	instanceID = 0;
	
	attributeSeq = NULL;
}

ServerConfigure_i::~ServerConfigure_i()
{
}

void ServerConfigure_i::block()
{
	int thisInstance = ++instanceID;

	fifo.push(thisInstance);
	
	cerr << "Pushed the fifo : " << thisInstance << " front = " << fifo.front() << endl;

	while(fifo.front() != thisInstance) {}

	cerr << "End of block(" << thisInstance << ")" << endl;

//	blocked = true;
}

void ServerConfigure_i::unblock()
{
	cerr << "Front of FIFO is: " << fifo.front() << endl;
	fifo.pop();
//	cerr << "...and now front of FIFO is: " << fifo.front() << endl;
//	blocked = false;
}


STI_Server_Device::TDeviceID* 
ServerConfigure_i::registerDevice(const char* deviceName, 
								  const STI_Server_Device::TDevice& device)
{

	int thisInstance =0;
	
//	int thisInstance = ++instanceID;
	cerr << "cin " << thisInstance << endl;

	int test = 0;
//	cin >> test;

	cerr << "Block " << thisInstance << endl;
	block();
	cerr << "Unblocked " << thisInstance << endl;


	STI_Server_Device::TDeviceID* deviceID  = 
		sti_Server->registerDevice(deviceName, device);
	
	unblock();

	return new STI_Server_Device::TDeviceID(*deviceID);
}


::CORBA::Boolean 
ServerConfigure_i::setChannels(const char* deviceID, 
							   const STI_Server_Device::TDeviceChannelSeq& channels)
{
	bool success = true;
	int i;

	block();

	for(i = 0; i < channels.length(); i++)
	{
		success &= sti_Server->registeredDevices[deviceID].addChannel(channels[i]);
	}

	unblock();

	return success;
}

::CORBA::Boolean ServerConfigure_i::mountDevice(const char* deviceID)
{
	
	bool Mounted = false;

	int thisInstance =0;
//	int thisInstance = ++instanceID;
	cerr << "cin " << thisInstance << endl;
	int test = 0;
//	cin >> test;
	
	cerr << "mountDevice()" << endl;

	cerr << "Block " << thisInstance << endl;
	block();
	cerr << "after block(" << thisInstance << ")" << endl;
	
	sti_Server->mountDevice(deviceID);
	
	cerr << "unblock()" << endl;
	unblock();

	return Mounted;
}

::CORBA::Boolean ServerConfigure_i::unmountDevice(const char* deviceID)
{
	RemoteDeviceMap::iterator iter;
	
	for(iter = sti_Server->registeredDevices.begin(); iter != sti_Server->registeredDevices.end(); iter++)
	{
		cerr << "Device: " << iter->first << ", " << iter->second.device()->moduleNum << endl;
	}
		

	return true;
}

STI_Server_Device::TAttributeSeq* ServerConfigure_i::attributes()
{
	STI_Server_Device::TAttributeSeq* dummy = 0;
	return dummy;
}

char* ServerConfigure_i::serverName()
{
	return CORBA::string_dup(sti_Server->serverName().c_str());
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
