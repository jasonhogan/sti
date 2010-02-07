/*! \file CommandLine_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class CommandLine_i
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

#ifndef COMMANDLINE_I_H
#define COMMANDLINE_I_H

#include <device.h>
#include <PartnerDevice.h>
#include <STI_Device.h>

#include <map>
#include <vector>
#include <string>


class DeviceConfigure_i;


class CommandLine_i : public POA_STI::Server_Device::CommandLine
{
public:
	CommandLine_i(STI_Device* device, DeviceConfigure_i* configureServant);
	~CommandLine_i();

	char* execute(const char* args);
    ::CORBA::Boolean registerPartnerDevice(STI::Server_Device::CommandLine_ptr partnerCmdLine);
    ::CORBA::Boolean unregisterPartnerDevice(const char* deviceID);
    STI::Types::TStringSeq* eventPartnerDevices();
	STI::Types::TStringSeq* requiredPartnerDevices();
	STI::Types::TStringSeq* registeredPartnerDevices();
	STI::Types::TDevice* device();
	::CORBA::Boolean setAttribute(const char *key, const char *value);
	char* getAttribute(const char *key);
//	::CORBA::Boolean transferPartnerEvents(const STI::Types::TDeviceEventSeq& events);
	STI::Types::TPartnerDeviceEventSeq* getPartnerEvents(const char* deviceID);
	::CORBA::Boolean ping();

//	PartnerDeviceMap& getRegisteredPartners();

private:

	//PartnerDeviceMap registeredPartners;	// DeviceID    => PartnerDevice

	DeviceConfigure_i* _configureServant;
	STI_Device* sti_device;

};

#endif

