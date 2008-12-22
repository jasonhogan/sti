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
#include <Attribute.h>

#include <string>
#include <vector>
#include <map>

class ORBManager;
class STI_Server;
class Attribute;


typedef std::map<std::string, Attribute> attributeMap;

class RemoteDevice
{
public:

	RemoteDevice() {};
	RemoteDevice(STI_Server* STI_server,
		STI_Server_Device::TDevice& device);

	~RemoteDevice();

	bool isActive();
	void activate();
	void deactivate();

	STI_Server_Device::TDevice & device();

	bool addChannel(const STI_Server_Device::TDeviceChannel & tChannel);

	void printChannels();

	//Forwarding functions
	bool setAttribute(std::string key, std::string value);
	attributeMap const * getAttributes();

	const std::vector<STI_Server_Device::TDeviceChannel> & getChannels() const;

	STI_Server_Device::TMeasurementSeq*	getStreamingData(
		                                             unsigned short channel,
                                                     double         initial_t, 
                                                     double         final_t, 
                                                     double         delta_t);
	std::string DataTransferErrMsg() const;
	STI_Server_Device::TMeasurementSeqSeq* measurements();

	const std::vector<std::string> & getRequiredPartners() const;

//	std::map<std::string, bool> partners;		//DeviceID => isRegistered

	bool registerPartner(std::string DeviceID, STI_Server_Device::CommandLine_ptr partner);
	bool unregisterPartner(std::string DeviceID);
	STI_Server_Device::CommandLine_var CommandLineRef;

	bool eventsParsed;
	void transferEvents(std::vector<STI_Server_Device::TDeviceEvent_var> &events);

private:

	// missingPartners  RemotePartnerDevice
	void setupCommandLine();
	std::vector<std::string> requiredPartners;

	//RemotePartnerDevice has bool isRegistered and DeviceID

	bool isUnique(const STI_Server_Device::TDeviceChannel & tChannel);

	static void acquireObjectReferencesWrapper(void* object);
	void acquireObjectReferences();

	ORBManager* orbManager;
	STI_Server* sti_server;

	attributeMap attributes;

	std::vector<STI_Server_Device::TDeviceChannel> channels;
	bool active;

	STI_Server_Device::TDevice tDevice;

	STI_Server_Device::Configure_var ConfigureRef;
	STI_Server_Device::DataTransfer_var DataTransferRef;
	STI_Server_Device::DeviceControl_var DeviceControlRef;

	std::string configureObjectName;
	std::string dataTransferObjectName;
	std::string commandLineObjectName;
	std::string deviceControlObjectName;
};

#endif
