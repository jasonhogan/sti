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

#include "device.h"
#include "STI_Device.h"
#include <map>
#include <string>


class CommandLine_i : public POA_STI_Server_Device::CommandLine
{
public:
	CommandLine_i(STI_Device* device);
	virtual ~CommandLine_i();

	virtual char* executeArgs(const char* args);
    virtual ::CORBA::Boolean registerPartnerDevice(STI_Server_Device::CommandLine_ptr partner);
    virtual STI_Server_Device::TStringSeq* partnerDevices();
    virtual char* deviceCmdName();

	std::map<std::string, STI_Server_Device::CommandLine_var> registeredPartners;

private:

	STI_Device* sti_device;

};

#endif

