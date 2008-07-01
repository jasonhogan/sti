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

#include "device.h"
#include "Attribute.h"
#include "StreamingBuffer.h"

#include <string>
#include <sstream>
#include <map>

using STI_Server_Device::TChannelType;
using STI_Server_Device::TData;
using STI_Server_Device::TValue;
//TDeviceChannelType
using STI_Server_Device::Output;
using STI_Server_Device::Input;
using STI_Server_Device::BiDirectional;
//TData
using STI_Server_Device::DataNumber;
using STI_Server_Device::DataString;
using STI_Server_Device::DataPicture;
using STI_Server_Device::DataNone;
//TValue
using STI_Server_Device::ValueNumber;
using STI_Server_Device::ValueString;
using STI_Server_Device::ValueDDSTriplet;
using STI_Server_Device::ValueMeas;

class Attribute;
class Configure_i;
class DataTransfer_i;
class ORBManager;
class STI_Device;

typedef std::map<std::string, Attribute> attributeMap;
//typedef bool (*ReadChannel)(unsigned short, STI_Server_Device::TMeasurement &);
//typedef bool (*WriteChannel)(unsigned short, STI_Server_Device::TDeviceEvent &);


class STI_Device
{
public:

	STI_Device(
		ORBManager *   orb_manager, 
		std::string    DeviceName, 
		std::string    DeviceType, 
		std::string    Address, 
		unsigned short ModuleNumber);

	virtual ~STI_Device();

	// Device setup
	virtual std::string deviceType() = 0;
	virtual void defineAttributes() = 0;
	virtual void defineChannels() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;
	virtual bool deviceMain() = 0;	//called in a loop while it returns true

	virtual bool readChannel(STI_Server_Device::TMeasurement & Measurement) = 0;
	virtual bool writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event) = 0;

	// Device setup helper functions
	void addAttribute(
		std::string key, 
		std::string initialValue, 
		std::string allowedValues = "");

	bool addChannel(
		unsigned short		channel, 
		TChannelType		type, 
		TData				inputType, 
		TValue				outputType);

    void addInputChannel(
        unsigned short Channel, 
        TData          InputType);

    void addOutputChannel(
        unsigned short Channel, 
        TValue         OutputType);

    void enableStreaming(
		unsigned short Channel, 
		std::string    SamplePeriod = "1", //double in seconds
		std::string    BufferDepth = "10");

	// Access functions
	attributeMap const * getAttributes();
	bool setAttribute(std::string key, std::string value);

	std::vector<STI_Server_Device::TDeviceChannel> * getChannels();

	std::string getServerName();
	std::string getDeviceName();

	std::string dataTransferErrorMsg();

	ORBManager* orbManager;

//	stopLookingForServer()
//	lookForServer()

protected:

	// servants
	Configure_i* configureServant;
	DataTransfer_i* timeCriticalDataServant;
	DataTransfer_i* streamingDataServant;

	std::stringstream dataTransferError;
	attributeMap attributes;

	std::vector<STI_Server_Device::TDeviceChannel> channels;

//	std::map<unsigned short, ReadChannel> readChannels;
//	std::map<unsigned short, WriteChannel> writeChannels;

	STI_Server_Device::ServerConfigure_var ServerConfigureRef;
	STI_Server_Device::TDevice_var tDevice;

	template<typename T> bool stringToValue(std::string inString, T& outValue)
	{
        //Returns true if the conversion is successful
        stringstream tempStream;
        
        tempStream << inString;
        tempStream >> outValue;

        return !tempStream.fail();
	};


private:
	
	std::map<unsigned short, StreamingBuffer*> streamingBuffers;

	bool updateStreamAttribute(std::string key, std::string value);
	void initializeAttributes();

	static void deviceMainWrapper(void* object);

	void initServer();
	static void initServerWrapper(void* object);

	void acquireServerReference();
	static void acquireServerReferenceWrapper(void* object);	

	void setChannels();

	bool serverConfigureFound;
	bool registedWithServer;
	unsigned short registrationAttempts;

	std::string serverName;
	std::string deviceName;

	std::string configureObjectName;
	std::string timeCriticalObjectName;
	std::string streamingObjectName;
};

#endif
