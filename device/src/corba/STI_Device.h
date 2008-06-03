/*! \file STI_Device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_Device
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

#ifndef STI_DEVICE_H
#define STI_DEVICE_H

#include <string>
#include <sstream>
#include <map>

#include "device.h"
#include "Attribute.h"

class Attribute;
class Configure_i;
class DataTransfer_i;
class ORBManager;

typedef std::map<std::string, Attribute> attributeMap;

using STI_Server_Device::TDeviceChannelType;
using STI_Server_Device::TData;
using STI_Server_Device::TValue;


class STI_Device
{
public:

	STI_Device(ORBManager* orb_manager, std::string DeviceName, 
		std::string DeviceType, std::string Address, 
		unsigned short ModuleNumber);
	virtual ~STI_Device();

	virtual std::string deviceType() = 0;
	virtual void defineAttributes() = 0;
//	virtual void defineChannels() = 0;

	attributeMap const * getAttributes();
	bool setAttribute(std::string key, std::string value);

//	stopLookingForServer()
//	lookForServer()

	void initServer();
	static void initServerWrapper(void* object);

	void acquireServerReference();
	static void acquireServerReferenceWrapper(void* object);	
	
	std::string dataTransferErrorMsg();
	std::string getServerName();
	std::string getDeviceName();

	ORBManager* orbManager;


	//should be protected; currently public for debugging
	Configure_i* configureServant;

	void addAttribute(
		std::string key, 
		std::string initialValue, 
		std::string allowedValues = "");

	void addChannel(
		unsigned short		channel, 
		TDeviceChannelType	type, 
		TData				inputType, 
		TValue				outputType);


protected:

	// servants
	DataTransfer_i* dataTransferServant;

	std::stringstream dataTransferError;
	attributeMap attributes;

	STI_Server_Device::TDeviceChannelSeq* getChannels() const;

	std::vector<STI_Server_Device::TDeviceChannel> channels;

private:

	void getDeviceID();

	STI_Server_Device::TDevice tDevice;
	STI_Server_Device::TDeviceID* tDeviceID;


	STI_Server_Device::ServerConfigure_var ServerConfigureRef;
	bool serverConfigureFound;
	bool registedWithServer;

	std::string serverName;
	std::string deviceName;
	
	attributeMap* attributes_ptr;
};

#endif
