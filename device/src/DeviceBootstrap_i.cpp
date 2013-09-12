/*! \file DeviceBootstrap_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceBootstrap_i
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

#include "DeviceBootstrap_i.h"
#include <STI_Device.h>

DeviceBootstrap_i::DeviceBootstrap_i(STI_Device* device) : sti_device(device)
{
}

DeviceBootstrap_i::~DeviceBootstrap_i()
{
}


::CORBA::Boolean DeviceBootstrap_i::ping()
{
	return true;
}

STI::Types::TDevice* DeviceBootstrap_i::getDevice()
{
	STI::Types::TDevice_var tDevice( new STI::Types::TDevice() );
	const STI::Types::TDevice& dev = sti_device->getTDevice();

	tDevice->address = CORBA::string_dup(dev.address);
	tDevice->deviceContext = CORBA::string_dup(dev.deviceContext);
	tDevice->deviceID = CORBA::string_dup(dev.deviceID);
	tDevice->deviceName = CORBA::string_dup(dev.deviceName);
	tDevice->moduleNum =dev.moduleNum;

	return tDevice._retn();
}


STI::Server_Device::DeviceTimingSeqControl_ptr DeviceBootstrap_i::getDeviceTimingSeqControl()
{
	return sti_device->getDeviceTimingSeqControl();
}

STI::Server_Device::DataTransfer_ptr DeviceBootstrap_i::getDataTransfer()
{
	return sti_device->getDataTransfer();
}

STI::Server_Device::DeviceConfigure_ptr DeviceBootstrap_i::getDeviceConfigure()
{
	return sti_device->getDeviceConfigure();
}

STI::Server_Device::CommandLine_ptr DeviceBootstrap_i::getCommandLine()
{
	return sti_device->getCommandLine();
}

