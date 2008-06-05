/*! \file RemoteDevice.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class RemoteDevice
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

#ifndef REMOTEDEVICE_H
#define REMOTEDEVICE_H

#include "device.h"

#include <string>
#include <vector>

class ORBManager;

class RemoteDevice
{
public:

	RemoteDevice(ORBManager* orb_manager, std::string name, 
		const STI_Server_Device::TDevice & device, 
		const STI_Server_Device::TDeviceID & device_id);

	RemoteDevice() {};
	~RemoteDevice();

	bool isMounted();
	void mount();
	void unmount();

	std::string deviceName();

	// Question - What is the 1st const doing here???
	STI_Server_Device::TDevice const * device() const;
	STI_Server_Device::TDeviceID * deviceID();

	bool addChannel(const STI_Server_Device::TDeviceChannel & tChannel);

private:

	bool isUnique(const STI_Server_Device::TDeviceChannel & tChannel);

	static void acquireObjectReferencesWrapper(void* object);
	void RemoteDevice::acquireObjectReferences();

	ORBManager* orbManager;

	std::vector<STI_Server_Device::TDeviceChannel> channels;
	bool mounted;

	std::string name_l;
	STI_Server_Device::TDevice tDevice;
	STI_Server_Device::TDeviceID tDeviceID;

	STI_Server_Device::Configure_var ConfigureRef;
	STI_Server_Device::DataTransfer_var timeCriticalDataRef;
	STI_Server_Device::DataTransfer_var streamingDataRef;

	std::string configureObjectName;
	std::string timeCriticalObjectName;
	std::string streamingObjectName;

};

#endif