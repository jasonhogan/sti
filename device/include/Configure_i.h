/*! \file Configure_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Configure_i
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

#ifndef CONFIGURE_I_H
#define CONFIGURE_I_H

#include "device.h"

#include <string>
#include <map>

class STI_Device;

class Configure_i : public POA_STI::Server_Device::Configure
{
public:

	Configure_i(STI_Device* device);
	~Configure_i();

	::CORBA::Boolean setAttribute(const char *key, const char *value);
	char* getAttribute(const char *key);
	STI::Types::TAttributeSeq* attributes();
	char* deviceName();
	void reRegisterWithServer();
	void kill();


private:

	STI_Device* sti_Device;
};

#endif
