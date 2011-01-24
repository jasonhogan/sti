/*! \file RegisteredDevices_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class RegisteredDevices_i
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

//Was 'DeviceConfigure_i' 2/6/2010

#ifndef REGISTEREDDEVICES_I_H
#define REGISTEREDDEVICES_I_H

#include "client.h"

class STI_Server;

class RegisteredDevices_i : public POA_STI::Client_Server::RegisteredDevices
{
public:

	RegisteredDevices_i(STI_Server* server);
	~RegisteredDevices_i();

	STI::Types::TLabeledData* getLabledData(const char* deviceID, const char* label);

    STI::Types::TAttributeSeq* getDeviceAttributes(const char* deviceID);
    ::CORBA::Boolean setDeviceAttribute(const char* deviceID, const char* key, const char* value);
    STI::Types::TChannelSeq* getDeviceChannels(const char* deviceID);
	STI::Types::TPartnerSeq* getDevicePartners(const char* deviceID);
    ::CORBA::Boolean deviceStatus(const char* deviceID);
    STI::Types::TDeviceSeq* devices();
	void killDevice(const char* deviceID);
	::CORBA::Long devicePing(const char* deviceID);


private:

	STI_Server* sti_Server;

};

#endif
