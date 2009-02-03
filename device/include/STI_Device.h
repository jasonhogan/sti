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
#include <Attribute.h>
#include <StreamingBuffer.h>
#include <PartnerDevice.h>
#include <RawEvent.h>
#include <ParsedMeasurement.h>
#include <Clock.h>
#include <EventConflictException.h>
#include <EventParsingException.h>

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <bitset>
#include <exception>

//needed for polymorphic vector of smart pointers -- boost::ptr_vector<DeviceEvent>
#if defined(_MSC_VER)
    #define BOOST_NO_SFINAE
#endif
#include <boost/ptr_container/ptr_vector.hpp>

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
using STI_Server_Device::ServerConfigure_var;

class Attribute;
class Configure_i;
class DataTransfer_i;
class CommandLine_i;
class DeviceControl_i;
class ORBManager;
class STI_Device;
class StreamingBuffer;

typedef std::map<std::string, Attribute> AttributeMap;
typedef std::map<unsigned short, STI_Server_Device::TDeviceChannel> ChannelMap;
typedef std::map<double, std::vector<RawEvent> > RawEventMap;
typedef std::map<unsigned short, std::vector<ParsedMeasurement> > ParsedMeasurementMap;
typedef std::map<unsigned short, StreamingBuffer> StreamingBufferMap;
//typedef std::vector<STI_Server_Device::TMeasurement> measurementVec;

//typedef bool (*ReadChannel)(unsigned short, STI_Server_Device::TMeasurement &);
//typedef bool (*WriteChannel)(unsigned short, STI_Server_Device::TDeviceEvent &);


class STI_Device
{
protected:
	class SynchronousEvent;
	typedef boost::ptr_vector<SynchronousEvent> SynchronousEventVector;

public:

	STI_Device(ORBManager* orb_manager,  std::string    DeviceName, 
			std::string IPAddress,    unsigned short ModuleNumber);
	virtual ~STI_Device();
	
	// Device main()
	virtual bool deviceMain(int argc, char** argv) = 0;	//called in a loop while it returns true

	// Device Attributes
	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

	// Device Channels
	virtual void defineChannels() = 0;
	virtual bool readChannel(ParsedMeasurement& Measurement) = 0;
	virtual bool writeChannel(const RawEvent& Event) = 0;

	// Device Command line interface setup
	virtual void definePartnerDevices() = 0;
	virtual std::string execute(int argc, char** argv) = 0;

	// Device-specific event parsing
	virtual void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) = 0;

	// Event Playback control
	virtual void stopEventPlayback() = 0;	//for devices that require non-generic stop commands

	//**************** Device setup helper functions ****************//

	template<class T>
	void addAttribute(std::string key, T initialValue, std::string allowedValues = "")
		{ attributes[key] = Attribute( valueToString(initialValue), allowedValues); }
	
	template<class T> 
	bool setAttribute(std::string key, T value)
		{ return setAttribute(key, valueToString(value)); }
	bool setAttribute(std::string key, std::string value);

    void addInputChannel (unsigned short Channel, TData InputType);
    void addOutputChannel(unsigned short Channel, TValue OutputType);
    void enableStreaming (unsigned short Channel, 
                          std::string    SamplePeriod = "1", //double in seconds
                          std::string    BufferDepth = "10");
	
	void addPartnerDevice(std::string partnerName, std::string IP, short module, std::string deviceName);

	void parseDeviceEventsDefault(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut);

	void convertArgs(int argc, char** argvInput, std::vector<std::string>& argvOutput) const;

//	Partner device usage: partnerDevice("lock").execute("--e1");
	PartnerDevice& partnerDevice(std::string partnerName);
	std::string execute(std::string args);

	//**************** Access functions ****************//

	std::string getServerName() const;
	std::string getDeviceName() const;
	const STI_Server_Device::TDevice& getTDevice() const;

	const AttributeMap& getAttributes() const;
	const ChannelMap& getChannels() const;
	const ParsedMeasurementMap& getMeasurements() const;
	const std::map<std::string, std::string>& getRequiredPartners() const;

	std::string dataTransferErrorMsg() const;
	std::string eventTransferErr() const;

	//*************** External event control **********//
	
	void loadEvents();
	void playEvents();
	void stop();
	bool transferEvents(const STI_Server_Device::TDeviceEventSeq& events);


protected:

	void splitString(std::string inString, std::string delimiter, std::vector<std::string>& outVector) const;

	template<typename T> bool stringToValue(std::string inString, T& outValue) const
	{
        //Returns true if the conversion is successful
        stringstream tempStream;
        
        tempStream << inString;
        tempStream >> outValue;

        return !tempStream.fail();
	};

	template<typename T> std::string valueToString(T inValue, std::string Default="") const
	{
		std::string outString;
        stringstream tempStream;
        
        tempStream << inValue;
		outString = tempStream.str();

        if( !tempStream.fail() )
			return outString;
		else
			return Default;
	};

private:
	
	enum DeviceStatus { EventsEmpty, EventsLoading, EventsLoaded, Running };

	// Containers
	AttributeMap                       attributes;
	ChannelMap                         channels;
	std::set<unsigned>                 conflictingEvents;
	ParsedMeasurementMap               measurements;
	RawEventMap                        rawEvents;	//as delivered by the python parser
	std::map<std::string, std::string> requiredPartners;
	StreamingBufferMap                 streamingBuffers;
	SynchronousEventVector             synchedEvents;	//generated by device specific parseDeviceEvents() (pure virtual)
	std::set<unsigned>                 unparseableEvents;
	
	// servants
	Configure_i*        configureServant;
	DataTransfer_i*     dataTransferServant;
	CommandLine_i*      commandLineServant;
	DeviceControl_i*    deviceControlServant;

	ServerConfigure_var ServerConfigureRef;
	
	void addPartnerDevice(std::string partnerName, std::string deviceID);
	bool addChannel(unsigned short channel, TChannelType type, 
                    TData inputType, TValue outputType);

	void init(std::string IPAddress, unsigned short ModuleNumber);
	void activateDevice();
	void registerDevice();
	void initializeChannels();
	void initializeAttributes();
	void loadDeviceEvents();
	void playDeviceEvents();
	void registerServants();
	void updateState();
	void waitForRegistration();
	
	bool changeStatus(DeviceStatus newStatus);
	bool isStreamAttribute(std::string key) const;
	bool updateStreamAttribute(std::string key, std::string& value);

	static void activateDeviceWrapper(void* object);
	static void registerDeviceWrapper(void* object);	
	static void deviceMainWrapper(void* object);
	static void loadDeviceEventsWrapper(void* object);
	static void playDeviceEventsWrapper(void* object);

	std::stringstream evtTransferErr;
	std::stringstream dataTransferError;

	ORBManager* orbManager;

	bool stopPlayback;
	bool registedWithServer;
	bool serverConfigureFound;
	std::string serverName;
	std::string deviceName;
	std::string configureObjectName;
	std::string dataTransferObjectName;
	std::string commandLineObjectName;
	std::string deviceControlObjectName;
	unsigned numMeasurementEvents;
	unsigned short registrationAttempts;

	Clock time;		//for event playback

	STI_Server_Device::TDevice_var tDevice;

	omni_mutex* mainLoopMutex;

	omni_thread* mainThread;
	omni_thread* loadEventsThread;
	omni_thread* playEventsThread;

	omni_mutex* playEventsMutex;
	omni_condition* playEventsTimer;

	omni_mutex* registrationMutex;
	omni_condition* registrationCondition;

	PartnerDevice* dummyPartner;

	DeviceStatus deviceStatus;

	//****************** Device-specific event classes *******************//

protected:

	class SynchronousEvent
	{
	public:
		SynchronousEvent() {}
		SynchronousEvent(const SynchronousEvent &copy) { time_ = copy.time_; }
		SynchronousEvent(double time) { setTime(time); }
		virtual ~SynchronousEvent() {}

		bool operator< (const SynchronousEvent &rhs) const { return (time_ < rhs.time_); }
		bool operator> (const SynchronousEvent &rhs) const { return (time_ > rhs.time_); }
		bool operator==(const SynchronousEvent &rhs) const { return (time_ == rhs.time_); }
		bool operator!=(const SynchronousEvent &rhs) const { return !((*this) == rhs); }
		
		//On load error (e.g., overflow) returns time until next recommended load; else returns zero.
		virtual uInt32 loadEvent() = 0;
		//Plays the event NOW
		virtual void playEvent() = 0;

		uInt64 getTime() { return time_; }

		template<typename T> 
		void setTime(T time) { time_ = static_cast<uInt64>(time); }

	private:
		uInt64 time_;
	};

	template<int N>
	class BitLineEvent : public SynchronousEvent, public std::bitset<N>
	{
	public:
		BitLineEvent() : SynchronousEvent() {}
		BitLineEvent(const BitLineEvent &copy) 
			: SynchronousEvent(copy) { }
		BitLineEvent(double time) 
			: SynchronousEvent(time) { setBits(0); }
		BitLineEvent(double time, uInt32 value) 
			: SynchronousEvent(time) { setBits(value); }
		virtual ~BitLineEvent() {};

		//assign 'value' to bits LSB to MSB
		void setBits(uInt32 value, unsigned LSB=0, unsigned MSB=(N-1)) 
		{
			unsigned i,j;
			std::bitset<N>::reset();
			for(i = LSB, j = 0; i <= MSB && j < 32 && i < N; i++, j++)
				std::bitset<N>::set(i, ((value >> j) & 0x1) == 0x1 );
		};
		//get the value of bits 'first' to 'last'
		uInt32 getBits(unsigned first=0, unsigned last=(N-1)) const
		{
			unsigned i,j;
			uInt32 value = 0;
			for(i = first, j = 0; i <= last && j < 32 && i < N; i++, j++)
				value += ( (std::bitset<N>::at(i) ? 0x1 : 0x0) << j);
			return value;
		}
		uInt32 getValue() const { return getBits(); }

		virtual uInt32 loadEvent() = 0;
		virtual void playEvent() = 0;
	};

	class PsuedoSynchronousEvent : public SynchronousEvent
	{
	public:
		PsuedoSynchronousEvent(double time, const std::vector<RawEvent>& events, STI_Device* device) 
			: SynchronousEvent(time), events_(events), device_(device) {}
		PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy)
			: SynchronousEvent(copy), events_(copy.events_) {}

		uInt32 loadEvent() {return 0;}
		void playEvent();

	private:
		const std::vector<RawEvent>& events_;
		STI_Device* device_;
	};
};



#endif
