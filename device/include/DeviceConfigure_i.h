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

//Was Configure_i -- 2/5/2010

#ifndef CONFIGURE_I_H
#define CONFIGURE_I_H

#include "device.h"

#include <string>
#include <map>

class STI_Device;

class DeviceConfigure_i : public POA_STI::Server_Device::DeviceConfigure
{
public:

	DeviceConfigure_i(STI_Device* device);
	~DeviceConfigure_i();
	
	STI::Types::TAttributeSeq* attributes();
	STI::Types::TDeviceChannelSeq* channels();
	STI::Types::TPartnerSeq* partners();


	::CORBA::Boolean setAttribute(const char *key, const char *value);
	char* getAttribute(const char *key);

	::CORBA::Boolean setChannel(::CORBA::Short channel, const STI::Types::TValMixed& value);

	void reRegisterWithServer();
	void kill();
	::CORBA::Boolean ping();


private:

	STI_Device* sti_Device;
};

#endif
