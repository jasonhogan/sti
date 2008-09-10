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
#include <device/src/corba/Attribute.h>
#include "StreamingBuffer.h"

#include <vector>
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
class StreamingBuffer;

typedef std::map<std::string, Attribute> attributeMap;
typedef std::vector<STI_Server_Device::TMeasurement> measurementVec;

//typedef bool (*ReadChannel)(unsigned short, STI_Server_Device::TMeasurement &);
//typedef bool (*WriteChannel)(unsigned short, STI_Server_Device::TDeviceEvent &);


class STI_Device
{
public:

	STI_Device(
		ORBManager *   orb_manager, 
		std::string    DeviceName, 
		std::string    Address, 
		unsigned short ModuleNumber);

	virtual ~STI_Device();

	// Device setup
	virtual bool deviceMain() = 0;	//called in a loop while it returns true

	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

	virtual void defineChannels() = 0;
	virtual bool readChannel(STI_Server_Device::TMeasurement & Measurement) = 0;
	virtual bool writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event) = 0;
	
	// Command line interface setup
	virtual std::string executeArgs(std::string args) = 0;
	virtual std::string commandLineDeviceName() = 0;
	virtual void definePartnerDevices() = 0;

	// Device setup helper functions
	void addPartnerDevice(std::string deviceName);

	template<typename T>
	void addAttribute(
		std::string key, 
		T initialValue, 
		std::string allowedValues = "")
	{
		attributes[key] = Attribute( valueToString(initialValue), allowedValues);
	}

	template<class T> bool setAttribute(std::string key, T value)
	{
		return setAttribute(key, valueToString(value));
	}

	bool setAttribute(std::string key, std::string value);

/*	template<> bool setAttribute<std::string>(std::string key, std::string value)
	{
		return set_attribute(key, value);
	}
*/
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
	const std::vector<STI_Server_Device::TDeviceChannel> * getChannels() const;
	const std::vector<measurementVec> * getMeasurements() const;
	const std::vector<std::string> * getPartnerDevices() const;

//	std::map<std::string, STI_Server_Device::CommandLine_var> partnerDevices;

//	partnerDevice("lock").executeArgs("--e1");

	std::string getServerName() const;
	std::string getDeviceName() const;

	std::string dataTransferErrorMsg();

	ORBManager* orbManager;

//	stopLookingForServer()
//	lookForServer()

protected:

	// servants
	Configure_i* configureServant;
	DataTransfer_i* dataTransferServant;

	std::stringstream dataTransferError;

	attributeMap attributes;
	std::vector<STI_Server_Device::TDeviceChannel> channels;
	std::vector<std::string> partnerDeviceList;


//	std::map<unsigned short, ReadChannel> readChannels;
//	std::map<unsigned short, WriteChannel> writeChannels;

	STI_Server_Device::ServerConfigure_var ServerConfigureRef;
	STI_Server_Device::TDevice_var tDevice;


	void splitString(std::string inString, std::string delimiter, std::vector<std::string> & outVector);


	template<typename T> bool stringToValue(std::string inString, T& outValue)
	{
        //Returns true if the conversion is successful
        stringstream tempStream;
        
        tempStream << inString;
        tempStream >> outValue;

        return !tempStream.fail();
	};

	template<typename T> std::string valueToString(T inValue, std::string Default="")
	{
		std::string outString;
        stringstream tempStream;
        
        tempStream << inValue;
        tempStream >> outString;

        if( !tempStream.fail() )
			return outString;
		else
			return Default;
	};


private:

	std::map<unsigned short, StreamingBuffer*> streamingBuffers;

	std::vector<measurementVec> measurements;

//	STI_Server_Device::TMeasurementSeqSeq_var measurements;
	
	bool isStreamAttribute(std::string key);
	bool updateStreamAttribute(std::string key, std::string & value);
	void initializeAttributes();
	
	bool addChannel(
		unsigned short		channel, 
		TChannelType		type, 
		TData				inputType, 
		TValue				outputType);

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
	std::string dataTransferObjectName;

};

#endif
