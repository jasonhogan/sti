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
#include <Clock.h>
#include <types.h>
#include <CompositeEvent.h>
#include <DataMeasurement.h>
#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
#include <vector>
#include <map>

class STI_Server;

typedef std::map<std::string, Attribute> AttributeMap;
typedef boost::ptr_vector<DataMeasurement> DataMeasurementVector;

class RemoteDevice
{
public:

	RemoteDevice() {};
	RemoteDevice(STI_Server* STI_server, STI::Types::TDevice& device);
	~RemoteDevice();

	const AttributeMap& getAttributes() const;
	const std::vector<std::string>& getRegisteredPartners() const;


	bool isActive();
	bool activate();
	void deactivate();

	bool addChannel(const STI::Types::TDeviceChannel& tChannel);

	void printChannels();

	//Forwarding functions
	bool setAttribute(std::string key, std::string value);

	bool registerPartner(std::string deviceID, STI::Server_Device::CommandLine_ptr partner);
	bool unregisterPartner(std::string deviceID);

	void addPartnerDependency(std::string deviceID);
	void removePartnerDependency(std::string deviceID);
	void waitForDependencies();
	void checkDependencies();

	void resetMeasurements();
	bool hasMeasurementsRemaining();
	void getNewMeasurementsFromServer();


	std::string printDeviceIndentiy() const;
	const STI::Types::TDevice& getDevice() const;
	const AttributeMap& getAttributes();
	const std::vector<STI::Types::TDeviceChannel>& getChannels() const;
	STI::Types::TChannel getChannel(unsigned short channel) const;

	const std::vector<std::string>& getRequiredPartners() const;
	std::vector<std::string>& getRegisteredPartners();
	std::vector<std::string>& getEventPartners();
	STI::Types::TDeviceEventSeq* getPartnerEvents(std::string deviceID);

	std::string getDataTransferErrMsg() const;
	std::string getTransferErrLog() const;
	STI::Server_Device::CommandLine_var getCommandLineRef() const;

	STI::Types::TMeasurementSeq*	getStreamingData(
		                                             unsigned short channel,
                                                     double         initial_t, 
                                                     double         final_t, 
                                                     double         delta_t);
	const DataMeasurementVector& getMeasurements() const;

	void loadEvents();
	bool prepareToPlay();
	void playEvents();
	void reset();
	void stop();
	void pause();
	void transferEvents(std::vector<CompositeEvent>& events);
	void killDevice();
	long pingDevice();

	bool finishedEventsTransferAttempt();
	bool eventsTransferSuccessful();
	bool eventsParsed();
	bool eventsLoaded();
	bool eventsPlayed();

	std::string execute(string args);

private:
	//Dummy private copy constructor and assignment to prevent this class 
	//from ever being copied.
	RemoteDevice(const RemoteDevice& copy);
	RemoteDevice& operator=(const RemoteDevice& rhs);

	bool isUnique(const STI::Types::TDeviceChannel& tChannel);
	bool servantsActive();

	void setupRequiredPartners();
	void setupEventPartners();
	void acquireObjectReferences();

	void stopWaitingForDependencies();

	std::string printExceptionMessage(CORBA::SystemException& ex, std::string location) const;

	AttributeMap attributes;
	std::vector<STI::Types::TDeviceChannel> channels;
	std::vector<std::string> requiredPartners;
	std::vector<std::string> registeredPartners;
	std::vector<std::string> eventPartners;
	std::vector<std::string> partnerDependencies;

	STI::Server_Device::CommandLine_var   commandLineRef;
	STI::Server_Device::Configure_var     configureRef;
	STI::Server_Device::DataTransfer_var  dataTransferRef;
	STI::Server_Device::DeviceControl_var deviceControlRef;

	bool active;
	bool eventsReady;
	bool doneTransfering;

	DataMeasurementVector measurements;
	unsigned numberOfMeasurements;

	
	STI::Types::TDevice tDevice;

	std::string configureObjectName;
	std::string dataTransferObjectName;
	std::string commandLineObjectName;
	std::string deviceControlObjectName;

	omni_mutex* eventDependencyMutex;
	omni_condition*	eventDependencyCondition;

	STI_Server* sti_server;
};

#endif
