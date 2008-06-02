/*! \file DeviceControl_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceControl_i
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
#include "DeviceControl_i.h"


DeviceControl_i::DeviceControl_i()
{
}

DeviceControl_i::~DeviceControl_i()
{
}

STI_Server_Device::TStatus DeviceControl_i::status()
{
	STI_Server_Device::TStatus dummy;
	dummy.curTime = 0;
	return dummy;

}

void DeviceControl_i::reset()
{
}

void DeviceControl_i::start()
{
}

void DeviceControl_i::trigger()
{
}

void DeviceControl_i::stop()
{
}

char* DeviceControl_i::errMsg()
{
	char* dummy = 0;
	return dummy;
}