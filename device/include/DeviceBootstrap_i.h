/*! \file DeviceBootstrap_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceBootstrap_i
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef DEVICEBOOTSTRAP_H
#define DEVICEBOOTSTRAP_H

#include "device.h"

class STI_Device;

class DeviceBootstrap_i : public POA_STI::Server_Device::DeviceBootstrap
{
public:

	DeviceBootstrap_i(STI_Device* device);
	~DeviceBootstrap_i();
	
	::CORBA::Boolean ping();
	STI::Types::TDevice* getDevice();

	STI::Server_Device::DeviceTimingSeqControl_ptr getDeviceTimingSeqControl();
	STI::Server_Device::DataTransfer_ptr getDataTransfer();
	STI::Server_Device::DeviceConfigure_ptr getDeviceConfigure();
	STI::Server_Device::CommandLine_ptr getCommandLine();

private:

	STI_Device* sti_device;

};

#endif
