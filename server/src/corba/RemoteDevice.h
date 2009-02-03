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
#include <types.h>

#include <string>
#include <vector>
#include <map>

class STI_Server;
class Attribute;


typedef std::map<std::string, Attribute> AttributeMap;

class RemoteDevice
{
private:
	//Dummy private copy constructor and assignment to prevent this class 
	//from ever being copied.
	RemoteDevice(const RemoteDevice& copy);
	RemoteDevice& operator=(const RemoteDevice& rhs);

public:

	static int test;
	int myTest;

	RemoteDevice() {};
	RemoteDevice(STI_Server* STI_server, STI_Server_Device::TDevice& device);
	~RemoteDevice();

	bool isActive();
	void activate();
	void deactivate();


	bool addChannel(const STI_Server_Device::TDeviceChannel& tChannel);

	void printChannels();

	//Forwarding functions
	bool setAttribute(std::string key, std::string value);

	bool registerPartner(std::string deviceID, STI_Server_Device::CommandLine_ptr partner);
	bool unregisterPartner(std::string deviceID);

	const STI_Server_Device::TDevice& getDevice() const;
	const AttributeMap& getAttributes();
	const std::vector<STI_Server_Device::TDeviceChannel>& getChannels() const;
	const std::vector<std::string>& getRequiredPartners() const;
	std::string getDataTransferErrMsg() const;
	std::string getTransferErrLog() const;
	STI_Server_Device::CommandLine_var getCommandLineRef() const;

	STI_Server_Device::TMeasurementSeq*	getStreamingData(
		                                             unsigned short channel,
                                                     double         initial_t, 
                                                     double         final_t, 
                                                     double         delta_t);
	STI_Server_Device::TMeasurementSeqSeq* measurements();

	void loadEvents();
	void playEvents();
	void stop();
	void transferEvents(std::vector<STI_Server_Device::TDeviceEvent_var>& events);

	bool eventsParsed();
	bool eventsLoaded();
	bool isTimedOut();

private:

	// missingPartners  RemotePartnerDevice
	//RemotePartnerDevice has bool isRegistered and DeviceID

	bool isUnique(const STI_Server_Device::TDeviceChannel& tChannel);
	void setupCommandLine();
	static void acquireObjectReferencesWrapper(void* object);
	void acquireObjectReferences();
	void waitForTimeOut();
	static void timeOutWrapper(void* object);
	bool servantsActive();
	void waitForActivation();

	std::string printExceptionMessage(CORBA::SystemException& ex, std::string location) const;

	AttributeMap attributes;
	std::vector<STI_Server_Device::TDeviceChannel> channels;
	std::vector<std::string> requiredPartners;

	STI_Server_Device::CommandLine_var   commandLineRef;
	STI_Server_Device::Configure_var     configureRef;
	STI_Server_Device::DataTransfer_var  dataTransferRef;
	STI_Server_Device::DeviceControl_var deviceControlRef;

	bool active;
	bool eventsReady;
	bool timedOut;
	
	STI_Server_Device::TDevice tDevice;
	uInt32 timeOutPeriod;

	std::string configureObjectName;
	std::string dataTransferObjectName;
	std::string commandLineObjectName;
	std::string deviceControlObjectName;

	omni_thread* timeOutThread;
	omni_mutex* timeOutMutex;
	omni_condition* timeOutCondition;

	STI_Server* sti_server;
};

#endif
