/*! \file ServerConfigure_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ServerConfigure_i
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

#ifndef SERVERCONFIGURE_I_H
#define SERVERCONFIGURE_I_H

#include "device.h"
#include <string>
#include <queue>

class STI_Server;

class ServerConfigure_i : public POA_STI_Server_Device::ServerConfigure
{
public:

	ServerConfigure_i(STI_Server* server);
	virtual ~ServerConfigure_i();

    virtual ::CORBA::Boolean registerDevice(
		const char* deviceName, 
		STI_Server_Device::TDevice& device);
//    virtual ::CORBA::Boolean setAttribute(const char* deviceID, const char* key, const char* value);
//    virtual char* getAttribute(const char* deviceID, const char* key);
    virtual ::CORBA::Boolean setChannels(const char* deviceID, 
		const STI_Server_Device::TDeviceChannelSeq& channels);
    virtual ::CORBA::Boolean activateDevice(const char* deviceID);
    virtual ::CORBA::Boolean removeDevice(const char* deviceID);
    virtual STI_Server_Device::TAttributeSeq* attributes();
    virtual char* serverName();

	void block();
	void unblock();

private:

	int instanceID;
	std::queue<int> fifo;

	STI_Server* sti_Server;
};

#endif
