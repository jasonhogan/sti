/*! \file DeviceConfigure_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceConfigure_i
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

// For Clients to get channel and attribute information about available 
// Devices and directly control them

#ifndef DEVICECONFIGURE_I_H
#define DEVICECONFIGURE_I_H

#include "client.h"

class STI_Server;

class DeviceConfigure_i : public POA_STI_Client_Server::DeviceConfigure
{
public:

	DeviceConfigure_i(STI_Server* server);
	virtual ~DeviceConfigure_i();

    virtual STI_Client_Server::TAttributeSeq* getDeviceAttributes(const char* deviceID);
    virtual ::CORBA::Boolean setDeviceAttribute(const char* deviceID, const char* key, const char* value);
    virtual STI_Client_Server::TChannelSeq* getDeviceChannels(const char* deviceID);
    virtual ::CORBA::Boolean deviceStatus(const char* deviceID);
    virtual STI_Client_Server::TDeviceSeq* devices();


private:

	STI_Server* sti_Server;

};

#endif
