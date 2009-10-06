/*! \file PartnerDevice.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class PartnerDevice
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

#ifndef PARTNERDEVICE_H
#define PARTNERDEVICE_H

#include <device.h>
#include <string>

class CommandLine_i;


class PartnerDevice
{
public:

	PartnerDevice();
	PartnerDevice(std::string PartnerName, STI_Server_Device::CommandLine_ptr commandLine);
	PartnerDevice(std::string PartnerName, CommandLine_i* LocalCommandLine);
	~PartnerDevice();

	std::string name() const;
	STI_Server_Device::TDevice device() const;
	std::string execute(std::string args);

	bool setAttribute(std::string key, std::string value);
	std::string getAttribute(std::string key);

//	addEvent(time, channel, value, RawEvent&)

	void setCommandLine(STI_Server_Device::CommandLine_ptr commandLine);
	bool isRegistered() const;
	bool isAlive();
	bool isLocal();

private:

	bool registered;
	bool local;

	std::string partnerName;
	STI_Server_Device::TDevice partnerDevice;
	STI_Server_Device::CommandLine_var commandLine_l;
	CommandLine_i* localCommandLine;


};

#endif

