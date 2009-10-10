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

#if defined(_MSC_VER)
    #define BOOST_NO_SFINAE
#endif
//needed for polymorphic vector of smart pointers -- boost::ptr_vector<DeviceEvent>
#include <boost/ptr_container/ptr_vector.hpp>
//needed for polymorphic map of smart pointers -- boost::ptr_map<PartnerDevice>
#include <boost/ptr_container/ptr_map.hpp>

using STI_Server_Device::TChannelType;
using STI_Server_Device::TData;
using STI_Server_Device::TValue;
//TDeviceChannelType
using STI_Server_Device::Output;
using STI_Server_Device::Input;
using STI_Server_Device::BiDirectional;
//TData
using STI_Server_Device::DataDouble;
using STI_Server_Device::DataLong;
using STI_Server_Device::DataString;
using STI_Server_Device::DataPicture;
using STI_Server_Device::DataNone;
//TValue
using STI_Server_Device::ValueNumber;
using STI_Server_Device::ValueString;
using STI_Server_Device::ValueDDSTriplet;
using STI_Server_Device::ValueMeas;
using STI_Server_Device::ServerConfigure_var;

//TMessageType
using STI_Server_Device::LoadingError;
using STI_Server_Device::PlayingError;


class Attribute;
class Configure_i;
class DataTransfer_i;
class CommandLine_i;
class DeviceControl_i;
class ORBManager;
class STI_Device;
class StreamingBuffer;

typedef std::map<std::string, STI_Server_Device::TDevice> TDeviceMap;
typedef std::map<std::string, Attribute> AttributeMap;
typedef std::map<unsigned short, STI_Server_Device::TDeviceChannel> ChannelMap;
typedef std::map<double, std::vector<RawEvent> > RawEventMap;
//typedef std::map<unsigned short, std::vector<ParsedMeasurement> > ParsedMeasurementMap;
typedef boost::ptr_vector<ParsedMeasurement> ParsedMeasurementVector;
typedef std::map<unsigned short, StreamingBuffer> StreamingBufferMap;
//typedef std::vector<STI_Server_Device::TMeasurement> measurementVec;
typedef boost::ptr_map<std::string, PartnerDevice> PartnerDeviceMap;

//typedef bool (*ReadChannel)(unsigned short, STI_Server_Device::TMeasurement &);
//typedef bool (*WriteChannel)(unsigned short, STI_Server_Device::TDeviceEvent &);


class STI_Device
{
protected:
	class SynchronousEvent;
	typedef boost::ptr_vector<SynchronousEvent> SynchronousEventVector;
	friend class NetworkMessenger;

public:

	STI_Device(ORBManager* orb_manager,  std::string    DeviceName, 
			std::string IPAddress,    unsigned short ModuleNumber);
	virtual ~STI_Device();

Clock setAttribClock;
Clock updateAttributeClock;

private:

	// Device main()
	virtual bool deviceMain(int argc, char* argv[]) = 0;	//called in a loop while it returns true

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
	virtual std::string execute(int argc, char* argv[]) = 0;

	// Device-specific event parsing
	virtual void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) = 0;

	// Event Playback control
	virtual void stopEventPlayback() = 0;	//for devices that require non-generic stop commands
	virtual void pauseEventPlayback() = 0;	//for devices that require non-generic pause commands
	virtual void resumeEventPlayback() = 0; //for devices that require non-generic resume commands
	virtual void loadDeviceEvents();
	virtual void playDeviceEvents();
	virtual void waitForEvent(unsigned eventNumber);


	//**************** Device setup helper functions ****************//

protected:

	template<class T>
	void addAttribute(std::string key, T initialValue, std::string allowedValues = "")
		{ attributes[key] = Attribute( valueToString(initialValue), allowedValues); }

	void addInputChannel (unsigned short Channel, TData InputType);
    void addOutputChannel(unsigned short Channel, TValue OutputType);
    void enableStreaming (unsigned short Channel, 
                          std::string    SamplePeriod = "1", //double in seconds
                          std::string    BufferDepth = "10");
	
	void addPartnerDevice(std::string partnerName, std::string IP, short module, std::string deviceName);
	void addMutualPartnerDevice(std::string partnerName, std::string IP, short module, std::string deviceName);
	
	PartnerDevice& partnerDevice(std::string partnerName);	//usage: partnerDevice("lock").execute("--e1");

	void setupPartnerEvents();

	void reportMessage(STI_Server_Device::TMessageType type, std::string message);

	void parseDeviceEventsDefault(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut);

	void stiError(std::string message) { reportMessage(STI_Server_Device::DeviceError, message); };

	//class NetworkMessenger
	//{
	//public:
	//	NetworkMessenger(STI_Server_Device::TMessageType messageType, STI_Device* device) 
	//		: messageType_(messageType), device_(device) {};
	//	~NetworkMessenger() {};

	//	template<class T>
	//	NetworkMessenger& operator<< (T message)
	//	{
	//		stringstream tempStream;
	//		tempStream << message;
	//		device_->reportMessage(getMessageType(), tempStream.str());
	//		return (*this);
	//	}
	//	STI_Server_Device::TMessageType getMessageType() const { return messageType_; }
	//private:
	//	STI_Server_Device::TMessageType messageType_;
	//	STI_Device* device_;
	//};

	//NetworkMessenger sti_err;

public:	

	void addLocalPartnerDevice(std::string partnerName, const STI_Device& partnerDevice);

	template<class T> 
	bool setAttribute(std::string key, T value)
		{ return setAttribute(key, valueToString(value)); }
	bool setAttribute(std::string key, std::string value);
	void refreshDeviceAttributes();

	STI_Server_Device::CommandLine_var STI_Device::generateCommandLineReference();

	std::string execute(std::string args);

	void convertArgs(int argc, char** argvInput, std::vector<std::string>& argvOutput) const;
	void splitString(std::string inString, std::string delimiter, std::vector<std::string>& outVector) const;

	//**************** Access functions ****************//

	std::string getServerName() const;
	std::string getDeviceName() const;
	const STI_Server_Device::TDevice& getTDevice() const;

	CommandLine_i* getCommandLineServant() const;
	const AttributeMap& getAttributes() const;
	const ChannelMap& getChannels() const;
	const std::map<std::string, std::string>& getRequiredPartners() const;
	const std::vector<std::string>& getMutualPartners() const;
	const PartnerDeviceMap& getRegisteredPartners() const;
	SynchronousEventVector& getSynchronousEvents();
	ParsedMeasurementVector& getMeasurements();
	unsigned getMeasuredEventNumber() const;

	std::string dataTransferErrorMsg() const;
	std::string eventTransferErr() const;

	//*************** External event control **********//

	void loadEvents();
	void playEvents();
	void stop();
	void pause();
	void resume();	//could be private
	bool transferEvents(const STI_Server_Device::TDeviceEventSeq& events);
	
	bool eventsLoaded();
	bool eventsPlayed();
	bool running();
	
	bool makeMeasurement(ParsedMeasurement& Measurement);
	bool playSingleEvent(const RawEvent& Event);

	void reRegisterDevice();
	void deviceShutdown();

protected:

	enum DeviceStatus { EventsEmpty, EventsLoading, EventsLoaded, Running, Paused };
	
	bool changeStatus(DeviceStatus newStatus);
	DeviceStatus getDeviceStatus() const;

	bool stopPlayback;
	bool pausePlayback;
	bool eventsAreLoaded;
	bool eventsArePlayed;

	omni_mutex* deviceLoadingMutex;
	omni_condition* deviceLoadingCondition;
	omni_mutex* deviceRunningMutex;
	omni_condition* deviceRunningCondition;
	omni_mutex* devicePauseMutex;
	omni_condition* devicePauseCondition;


	template<typename T> bool stringToValue(std::string inString, T& outValue, ios::fmtflags numBase=ios::dec) const
	{
        //Returns true if the conversion is successful
        stringstream tempStream;
        tempStream.setf( numBase, ios::basefield );

        tempStream << inString;
        tempStream >> outValue;

        return !tempStream.fail();
	};

	template<typename T> std::string valueToString(T inValue, std::string Default="", ios::fmtflags numBase=ios::dec) const
	{
		std::string outString;
        stringstream tempStream;
        tempStream.setf( numBase, ios::basefield );

        tempStream << inValue;
		outString = tempStream.str();

        if( !tempStream.fail() )
			return outString;
		else
			return Default;
	};

	// Derived classes may add attributeUpdaters that implement
	// STI_Device::AttributeUpdater::updateAttributes(...).
	// This allows for attribute updates without implementing 
	// STI_Device::updateAttributes(...) so that the derived class can act as
	// another abstract base class without having to change the name of the 
	// interface function hooks.
	class AttributeUpdater
	{ 
	public: 
		AttributeUpdater(STI_Device* thisDevice) : device_(thisDevice) {};
		virtual void defineAttributes() = 0;
		virtual bool updateAttributes(std::string key, std::string value) = 0; 
		virtual void refreshAttributes() = 0;

	protected:
		template<class T>
		void addAttribute(std::string key, T initialValue, std::string allowedValues = "")
		{
			device_->addAttribute(key, initialValue, allowedValues);
		}
		
		template<class T> 
		bool setAttribute(std::string key, T value)
		{
			return device_->setAttribute(key, value);
		}

	private:
		STI_Device* device_;
	};
	
	void addAttributeUpdater(AttributeUpdater* updater);

private:
	
	// Containers
	TDeviceMap                         addedPartners;
	vector<AttributeUpdater*>          attributeUpdaters;
	AttributeMap                       attributes;
	ChannelMap                         channels;
	std::set<unsigned>                 conflictingEvents;
	ParsedMeasurementVector            measurements;
	RawEventMap                        rawEvents;	//as delivered by the python parser
	std::map<std::string, std::string> requiredPartners;
	std::vector<std::string>           mutualPartners;
	StreamingBufferMap                 streamingBuffers;
	SynchronousEventVector             synchedEvents;	//generated by device specific parseDeviceEvents() (pure virtual)
	std::set<unsigned>                 unparseableEvents;

	// servants
	Configure_i*     configureServant;
	DataTransfer_i*  dataTransferServant;
	CommandLine_i*   commandLineServant;
	DeviceControl_i* deviceControlServant;

	ServerConfigure_var ServerConfigureRef;
	
	void addPartnerDevice(std::string partnerName, std::string deviceID);
	bool addChannel(unsigned short channel, TChannelType type, 
                    TData inputType, TValue outputType);

	void activateDevice();
	void connectToServer();
	void registerDevice();
	void init(std::string IPAddress, unsigned short ModuleNumber);
	void initializeChannels();
	void initializeAttributes();
	void initializePartnerDevices();
	void registerServants();
	void updateState();


	bool isStreamAttribute(std::string key) const;
	bool updateStreamAttribute(std::string key, std::string& value);

	static void connectToServerWrapper(void* object);	
	static void deviceMainWrapper(void* object);
	static void loadDeviceEventsWrapper(void* object);
	static void playDeviceEventsWrapper(void* object);

	//private copy constructor and assignment prevents copying/assigning
	STI_Device(const STI_Device& copy);
	STI_Device& operator=(const STI_Device& rhs);
	
	std::stringstream evtTransferErr;
	std::stringstream dataTransferError;

//	public:
	ORBManager* orbManager;
private:
	bool registedWithServer;
	bool serverConfigureFound;
	std::string serverName;
	std::string deviceName;
	std::string configureObjectName;
	std::string dataTransferObjectName;
	std::string commandLineObjectName;
	std::string deviceControlObjectName;
	unsigned short registrationAttempts;
	unsigned measuredEventNumber;

	Clock time;		//for event playback

	Int64 timeOfPause;

	STI_Server_Device::TDevice_var tDevice;

	omni_mutex* mainLoopMutex;

	omni_thread* mainThread;
	omni_thread* loadEventsThread;
	omni_thread* playEventsThread;

	unsigned long wait_s;
	unsigned long wait_ns;

	omni_mutex* playEventsMutex;
	omni_condition* playEventsTimer;

	PartnerDevice* dummyPartner;

	DeviceStatus deviceStatus;

	//****************** Device-specific event classes *******************//

protected:

	class SynchronousEvent
	{
	public:
		SynchronousEvent() {}
		SynchronousEvent(const SynchronousEvent &copy) { device_ = copy.device_; time_ = copy.time_; }
		SynchronousEvent(double time, STI_Device* device) : device_(device) { setTime(time); }
		virtual ~SynchronousEvent() {}

		bool operator< (const SynchronousEvent &rhs) const { return (time_ < rhs.time_); }
		bool operator> (const SynchronousEvent &rhs) const { return (time_ > rhs.time_); }
		bool operator==(const SynchronousEvent &rhs) const { return (time_ == rhs.time_); }
		bool operator!=(const SynchronousEvent &rhs) const { return !((*this) == rhs); }
		
		virtual void setupEvent() = 0;
		virtual void loadEvent() = 0;
		virtual void playEvent() = 0;		//Plays the event NOW
		virtual void collectMeasurementData() = 0;

		uInt64 getTime() { return time_; }
		unsigned getEventNumber() { return eventNumber_; }
		unsigned getNumberOfMeasurements() { return eventMeasurements.size(); }
		
		template<typename T> void setTime(T time) { time_ = static_cast<uInt64>(time); }
	//	template<typename T> void setData(T data) { time_ = static_cast<uInt64>(time); }
		void setEventNumber(unsigned eventNumber) { eventNumber_ = eventNumber; }
		void addMeasurement(const RawEvent& measurementEvent);

	protected:
		STI_Device* device_;
		std::vector<ParsedMeasurement*> eventMeasurements;
	private:
		uInt64 time_;
		unsigned eventNumber_;
	};

	template<int N>
	class BitLineEvent : public SynchronousEvent
	{
	public:
		BitLineEvent() : SynchronousEvent() { bits.reset(); }
		BitLineEvent(const BitLineEvent &copy) 
			: SynchronousEvent(copy) { }
		BitLineEvent(double time, STI_Device* device) 
			: SynchronousEvent(time, device) { bits.reset(); }
		BitLineEvent(double time, uInt32 value, STI_Device* device) 
			: SynchronousEvent(time, device) { setBits(value); }
		virtual ~BitLineEvent() {};

		//assign 'value' to bits LSB to MSB
		template <typename T>
		BitLineEvent<N>* setBits(T value, unsigned LSB=0, unsigned MSB=(N-1)) 
		{
			unsigned numBits = sizeof(T) * CHAR_BIT;
			unsigned i,j;
			for(i = LSB, j = 0; i <= MSB && j < numBits && i < N; i++, j++)
				bits.set(i, ((value >> j) & 0x1) == 0x1 );

			return this;
		};
		//get the value of bits 'first' to 'last'
		uInt32 getBits(unsigned first=0, unsigned last=(N-1)) const
		{
			unsigned i,j;
			uInt32 value = 0;
			for(i = first, j = 0; i <= last && j < 32 && i < N; i++, j++)
				value += ( (bits.test(i) ? 0x1 : 0x0) << j);
			return value;
		}
		uInt32 getValue() const { return getBits(); }

		virtual void setupEvent() = 0;
		virtual void loadEvent() = 0;
		virtual void playEvent() = 0;
		virtual void collectMeasurementData() = 0;

	protected:
		std::bitset<N> bits;
	};

	class PsuedoSynchronousEvent : public SynchronousEvent
	{
	public:
		PsuedoSynchronousEvent(double time, const std::vector<RawEvent>& events, STI_Device* device) 
			: SynchronousEvent(time, device), events_(events) {}
		PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy)
			: SynchronousEvent(copy), events_(copy.events_) {}
		
		void setupEvent() { }
		void loadEvent() { }
		void playEvent();
		void collectMeasurementData();

	protected:
		const std::vector<RawEvent>& events_;
	};
};



#endif
