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
#include "pusher.h"
#include <Attribute.h>
#include <NetworkFileSource.h>
#include <PartnerDevice.h>
#include <RawEvent.h>
#include <DataMeasurement.h>
#include <Clock.h>
#include <EventConflictException.h>
#include <EventParsingException.h>
#include <utils.h>

#include "DynamicValue.h"

#include <DeviceEventPusher.h>

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

using STI::Types::TChannelType;
using STI::Types::TData;
using STI::Types::TValue;
//TDeviceChannelType
using STI::Types::Output;
using STI::Types::Input;
using STI::Types::BiDirectional;
//TData
using STI::Types::DataDouble;
using STI::Types::DataLong;
using STI::Types::DataString;
using STI::Types::DataPicture;
using STI::Types::DataNone;
using STI::Types::DataVector;
using STI::Types::DataBoolean;
using STI::Types::DataFile;
using STI::Types::DataOctet;
//TValue
using STI::Types::ValueNumber;
using STI::Types::ValueString;
using STI::Types::ValueVector;
using STI::Types::ValueNone;

//TMessageType
using STI::Pusher::LoadingError;
using STI::Pusher::PlayingError;

using STI::Server_Device::ServerConfigure_var;

//DeviceStatus
using STI::Types::DeviceStatus;
using STI::Types::EventsEmpty;
using STI::Types::EventsLoading;
using STI::Types::EventsLoaded;
using STI::Types::PreparingToPlay;
using STI::Types::Playing;
using STI::Types::Paused;


class Attribute;
class DeviceConfigure_i;
class DataTransfer_i;
class CommandLine_i;
class DeviceTimingSeqControl_i;
class DeviceBootstrap_i;
class ORBManager;
class STI_Device;
class DataLogger_i;


//class LinkedValue;
//typedef boost::shared_ptr<LinkedValue> LinkedValue_ptr;



//typedef std::map<std::string, STI::Types::TDevice> TDeviceMap;
typedef std::map<std::string, Attribute> AttributeMap;
typedef std::map<unsigned short, STI::Types::TDeviceChannel> ChannelMap;
typedef std::map<double, std::vector<RawEvent> > RawEventMap;
//typedef std::map<unsigned short, std::vector<DataMeasurement> > DataMeasurementMap;
typedef boost::ptr_vector<DataMeasurement> DataMeasurementVector;
//typedef std::vector<STI::Types::TMeasurement> measurementVec;
typedef boost::ptr_map<std::string, PartnerDevice> PartnerDeviceMap;
//typedef std::map<std::string, std::vector<STI::Types::TDeviceEvent> > PartnerDeviceEventMap;

typedef std::map<std::string, MixedData> MixedDataMap;


//typedef bool (*ReadChannel)(unsigned short, STI::Types::TMeasurement &);
//typedef bool (*WriteChannel)(unsigned short, STI::Types::TDeviceEvent &);


class STI_Device
{
public:
	class SynchronousEvent;
	typedef boost::ptr_vector<SynchronousEvent> SynchronousEventVector;

public:

	STI_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);
	STI_Device(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".");
	virtual ~STI_Device();

private:

	// Device main()
	virtual bool deviceMain(int argc, char* argv[]) = 0;	//called in a loop while it returns true
//	virtual void initializeDevice();	//called when the device is registered with server and has all required partners

	// Device Attributes
	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

	// Device Channels
	virtual void defineChannels() = 0;
	virtual bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) = 0;
	virtual bool writeChannel(unsigned short channel, const MixedValue& value) = 0;

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
	virtual void measureData();
//	virtual void waitForEvent(unsigned eventNumber);

	virtual std::string getDeviceHelp() { return ""; }

	//**************** Device setup helper functions ****************//
public:
	static MixedValue emptyValue;
	void addLoggedMeasurement(unsigned short channel,   unsigned int measureInterval=60, unsigned int saveInterval=60, double deviationThreshold=2.0, MixedValue &valueIn = emptyValue);
	void addLoggedMeasurement(std::string attributeKey, unsigned int measureInterval=60, unsigned int saveInterval=60, double deviationThreshold=2.0);
	void startDataLogging();
	void stopDataLogging();
	
	void reportMessage(STI::Pusher::MessageType type, std::string message);
	void sendRefreshEvent(STI::Pusher::TDeviceRefreshEvent event);
	void sendDeviceDataEvent(STI::Pusher::TDeviceDataEvent event);

	void setChannelNameFilename(std::string path) { channelNameFilename = path; }

protected:

	template<class T>
	void addAttribute(std::string key, T initialValue, std::string allowedValues = "")
		{ attributes[key] = Attribute( valueToString(initialValue), allowedValues); }

	void addInputChannel (unsigned short Channel, TData InputType);
	void addInputChannel (unsigned short Channel, TData InputType, TValue OutputType, std::string defaultName = "");
	void addInputChannel (unsigned short Channel, TData InputType, std::string defaultName);

//	void addInputChannel (unsigned short Channel, TData InputType, TValue OutputType=ValueNone, std::string defaultName = "");

	void addOutputChannel(unsigned short Channel, TValue OutputType, std::string defaultName = "");

	void installGraphicalParser(std::string parserJarPath);
    
	
	bool addPartnerDevice(std::string partnerName, std::string IP, short module, std::string deviceName);
	bool addMutualPartnerDevice(std::string partnerName, std::string IP, short module, std::string deviceName);


	void parseDeviceEventsDefault(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut);

	void stiError(std::string message);

	//class NetworkMessenger
	//{
	//public:
	//	NetworkMessenger(STI::Types::TMessageType messageType, STI_Device* device) 
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
	//	STI::Types::TMessageType getMessageType() const { return messageType_; }
	//private:
	//	STI::Types::TMessageType messageType_;
	//	STI_Device* device_;
	//};

	//NetworkMessenger sti_err;
public:
	STI::Server_Device::DeviceTimingSeqControl_ptr getDeviceTimingSeqControl();
	STI::Server_Device::DataTransfer_ptr getDataTransfer();
	STI::Server_Device::DeviceConfigure_ptr getDeviceConfigure();
	STI::Server_Device::CommandLine_ptr getCommandLine();

public:
	bool read(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool write(unsigned short channel, const MixedValue& value);
	
	bool read(const RawEvent& measurementEvent);
	bool write(const RawEvent& event);

protected:
	bool readChannelDefault(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut, double minimumStartTime_ns=10000);
	bool writeChannelDefault(unsigned short channel, const MixedValue& value, double minimumStartTime_ns=10000);
	virtual bool playSingleEventDefault(const RawEvent& event);

	bool preparePartnerEvents(std::vector<STI::Server_Device::DeviceTimingSeqControl_var>& partnerControls);

public:	

	void setSaveAttributesToFile(bool save) { saveAttributesToFile = save; }

	//Labeled Data
	MixedData getLabeledData(std::string label);
	bool hasLabeledData(std::string label);
	void setLabeledData(std::string label, MixedData& data);

	//Logging
	void setLogDirectory(std::string logDirectory);

	//Partners
	void addLocalPartnerDevice(std::string partnerName, const STI_Device& partnerDevice);
	PartnerDevice& partnerDevice(std::string partnerName);	//usage: partnerDevice("lock").execute("--e1");

	//Attributes
	template<class T> 
	bool setAttribute(std::string key, T value)
		{ return setAttribute(key, valueToString(value)); }
	bool setAttribute(std::string key, std::string value);
	std::string getAttribute(std::string key) const;
	void refreshDeviceAttributes();

	std::string execute(std::string args);
	bool executeSpecialCommands(std::vector<std::string> arguments, std::string& output);
	virtual bool executeDelegatedSpecialCommands(std::vector<std::string> arguments, std::string& output) { return false; };
	std::string printSpecialCommandOptions();
	virtual std::string printDelegatedSpecialCommandOptions() { return ""; }

	bool setDeviceChannelName(short channel, std::string name);

	template<typename T> static bool stringToValue(std::string inString, T& outValue, ios::fmtflags numBase=ios::dec, std::streamsize precision=9)
	{
		return STI::Utils::stringToValue(inString, outValue, numBase, precision);
	}
	template<typename T> static std::string valueToString(T inValue, std::string Default="", ios::fmtflags numBase=ios::dec, std::streamsize precision=9)
	{
		return STI::Utils::valueToString(inValue, Default, numBase, precision);
	}

	//**************** Access functions ****************//

	std::string getServerName() const;
	std::string getDeviceName() const;
	const STI::Types::TDevice& getTDevice() const;

	std::string getIP() const;
	unsigned short getModule() const;

	bool isAlive() const { return alive; }

	CommandLine_i* getCommandLineServant() const;
	const AttributeMap& getAttributes() const;
	const ChannelMap& getChannels() const;
//	const std::map<std::string, std::string>& getRequiredPartners() const;
//	const std::vector<std::string>& STI_Device::getEventPartners() const;
//	const std::vector<std::string>& getMutualPartners() const;
//	const PartnerDeviceMap& getRegisteredPartners() const;
	SynchronousEventVector& getSynchronousEvents();
	DataMeasurementVector& getMeasurements();
	unsigned getMeasuredEventNumber() const;
	std::vector<STI::Types::TDeviceEvent>& getPartnerEvents(std::string deviceID);

	std::string getPartnerDeviceID(std::string partnerName);
	std::string getPartnerName(std::string deviceID);

	PartnerDeviceMap& getPartnerDeviceMap();
	void checkForNewPartners();

	std::string dataTransferErrorMsg() const;
	std::string eventTransferErr() const;

	//*************** External event control **********//


	bool prepareToPlay();
	void resetEvents();
	void loadEvents();
	void playEvents();
	void stop();
	void pause();
	void resume();	//could be private
	bool transferEvents(const STI::Types::TDeviceEventSeq& events);
	bool parseEvents(RawEventMap& rawEvents);
	bool addRawEvent(const RawEvent& rawEvent, RawEventMap& raw_events, unsigned& errorCount, unsigned maxErrors=10);


	bool eventsLoaded();
	bool eventsPlayed();
	bool running();
	Int64 getCurrentTime();



	void reRegisterDevice();
	void deviceShutdown();

	void pauseServer();
	void unpauseServer();


public:
//	enum DeviceStatus { EventsEmpty, EventsLoading, EventsLoaded, PreparingToPlay, Playing, Paused };
	bool waitForStatus(DeviceStatus status);	//waits until the DeviceStatus matches status or until it is explicitly stopped.  Returns true if the DeviceStatus matches status at return time.

protected:

	
	bool changeStatus(DeviceStatus newStatus);
	bool deviceStatusIs(DeviceStatus status);	//tests if the device is in DeviceStatus 'status'.  This is thread safe. 

	bool executing;
	bool executionAllowed;
	bool stopPlayback;
	bool pausePlayback;
	bool eventsAreLoaded;
	bool eventsArePlayed;
	bool eventsAreMeasured;

	bool alive;

	omni_mutex* deviceStatusMutex;
	omni_condition* deviceStatusCondition;
	
	omni_mutex* measureMutex;
	omni_condition* measureCondition;

	omni_mutex* executeMutex;
	omni_mutex* executingMutex;
	omni_condition* executingCondition;

	omni_mutex* deviceLoadingMutex;
	omni_condition* deviceLoadingCondition;
	omni_mutex* deviceRunningMutex;
	omni_condition* deviceRunningCondition;
	omni_mutex* devicePauseMutex;
	omni_condition* devicePauseCondition;

	omni_mutex* requiredPartnerRegistrationMutex;
	omni_condition* requirePartnerRegistrationCondition;



	std::string TValueToStr(STI::Types::TValue tValue);

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
	vector<AttributeUpdater*>          attributeUpdaters;
	AttributeMap                       attributes;
	ChannelMap                         channels;
	std::set<unsigned>                 conflictingEvents;
	DataMeasurementVector            measurements;
	RawEventMap                        rawEvents;	//as delivered by the python parser
	SynchronousEventVector             synchedEvents;	//generated by device specific parseDeviceEvents() (pure virtual)
	std::set<unsigned>                 unparseableEvents;
	PartnerDeviceMap                   partnerDevices;
	MixedDataMap                       labeledData;

	// servants
	DeviceConfigure_i* configureServant;
	DataTransfer_i*    dataTransferServant;
	CommandLine_i*   commandLineServant;
	DeviceTimingSeqControl_i* deviceControlServant;
	DeviceBootstrap_i* deviceBootstrapServant;
	DataLogger_i*    dataLoggerServant;

	NetworkFileSource* networkJarFile;

	bool bootstrapIsRegistered;

	ServerConfigure_var ServerConfigureRef;
//	STI::Pusher::DeviceEventHandler_var deviceEventHandlerRef;
	DeviceEventPusher deviceEventPusher;

	bool addPartnerDevice(std::string partnerName, string IP, short module, std::string deviceName, bool mutual);

//	void addPartnerDevice(std::string partnerName, std::string deviceID, bool mutual);
	bool addChannel(unsigned short channel, TChannelType type, 
                    TData inputType, TValue outputType, std::string defaultName);

	void aquireServerConfigure();
	void aquireDeviceEventHandler();
	void connectToServer();
	void init(std::string IPAddress, unsigned short ModuleNumber);
	void initializeAttributes();
	void initializeChannels();
	void initializePartnerDevices();
	void registerDevice();
	void registerBootstrapServant();
	void updateState();

	void loadChannelNames();
	void saveChannelNames();

	void loadAttributes();
	void saveAttributes();



	void waitForRequiredPartners();
	bool requiredPartnersRegistered();

	bool isStreamAttribute(std::string key) const;
	bool updateStreamAttribute(std::string key, std::string& value);

	static void connectToServerWrapper(void* object);	
	static void deviceMainWrapper(void* object);
	static void loadDeviceEventsWrapper(void* object);
	static void playDeviceEventsWrapper(void* object);
	static void measureDataWrapper(void* object);

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
	bool deviceEventHandlerFound;

	bool partnerDevicesInitialized;
	bool attributesInitialized;
	bool channelsInitialized;

	std::string serverName;
	std::string deviceName;
	std::string deviceBootstrapObjectName;

	std::string channelNameFilename;
	std::string attributeFilename;
	bool saveAttributesToFile;

	unsigned short registrationAttempts;
	unsigned measuredEventNumber;
	
	bool stopWaiting;
	int numberWaitingForStatus;
	bool usingDefaultEventParsing;

	Clock time;		//for event playback


	Int64 timeOfPause;

	STI::Types::TDevice_var tDevice;

	omni_mutex* mainLoopMutex;
	omni_mutex* playSingleEventMutex;

	omni_thread* mainThread;
	omni_thread* loadEventsThread;
	omni_thread* playEventsThread;


	omni_mutex* playEventsMutex;
	omni_condition* playEventsTimer;

	PartnerDevice* dummyPartner;

	std::string logDir;

	DeviceStatus deviceStatus;

	//****************** Device-specific event classes *******************//

public:

	class SynchronousEvent
	{
	public:
//		SynchronousEvent() {}
		SynchronousEvent(const SynchronousEvent &copy)  { device_ = copy.device_; time_ = copy.time_; }
		SynchronousEvent(double time, STI_Device* device) : device_(device) 
		{
			setTime(time);
			statusMutex = new omni_mutex();
			loadCondition = new omni_condition(statusMutex);
			playCondition = new omni_condition(statusMutex);
			collectionCondition = new omni_condition(statusMutex);
			setupDone = false;
			loaded = false;
			played = false;
		}
		virtual ~SynchronousEvent() {}

		bool operator< (const SynchronousEvent &rhs) const { return (time_ < rhs.time_); }
		bool operator> (const SynchronousEvent &rhs) const { return (time_ > rhs.time_); }
		bool operator==(const SynchronousEvent &rhs) const { return (time_ == rhs.time_); }
		bool operator!=(const SynchronousEvent &rhs) const { return !((*this) == rhs); }

		void setup();
		void load();
		void play();
		void collectData();

		virtual void stop();
		virtual void reset();

		virtual void waitBeforeLoad();
		virtual void waitBeforePlay();
		virtual void waitBeforeCollectData();

		uInt64 getTime() { return time_; }
		unsigned getEventNumber() { return eventNumber_; }
		unsigned getNumberOfMeasurements() { return eventMeasurements.size(); }
		
		template<typename T> void setTime(T time) { time_ = static_cast<uInt64>(time); }
	//	template<typename T> void setData(T data) { time_ = static_cast<uInt64>(time); }

		void setEventNumber(unsigned eventNumber) { eventNumber_ = eventNumber; }
		void addMeasurement(const RawEvent& measurementEvent);

	private:
		virtual void setupEvent() = 0;
		virtual void loadEvent() = 0;
		virtual void playEvent() = 0;
		virtual void collectMeasurementData() = 0;

	protected:
		STI_Device* device_;
		std::vector<DataMeasurement*> eventMeasurements;

		void performMeasurementCallbacks()
		{
			for(unsigned i = 0; i < eventMeasurements.size(); i++) {
				eventMeasurements.at(i)->sendMeasurementCallback();
			}
		}

		omni_mutex* statusMutex;
		omni_condition* loadCondition;
		omni_condition* playCondition;
		omni_condition* collectionCondition;

		bool played;
		bool loaded;
		bool setupDone;

	private:
		uInt64 time_;
		unsigned eventNumber_;
	};




	class DynamicSynchronousEvent : public SynchronousEvent, public DynamicValueListener
	{
	public:

		DynamicSynchronousEvent(double time, STI_Device* device)    //provide as well, so no linked value is an option?
			: SynchronousEvent(time, device) {}
//		DynamicSynchronousEvent(double time, const RawEvent& sourceEvent, STI_Device* device);
		DynamicSynchronousEvent(double time, const std::vector<RawEvent>& sourceEvents, STI_Device* device);
		~DynamicSynchronousEvent();

		virtual void refresh(const DynamicValueEvent& evt);

//		virtual void setLinkedValue(unsigned short channel, const MixedValue& newVal);

	protected:

		void addSourceEvents(const std::vector<RawEvent>& sourceEvents);

//		typedef std::map<unsigned short, MixedValue> ChannelValueMap;
//		ChannelValueMap updatedValues;
//		std::vector<RawEvent> dummy;
		const std::vector<RawEvent>* sourceEvents_l;

	private:
		
//		virtual void updateValue(unsigned short channel, const MixedValue& newVal) = 0;
		virtual void updateValue(const std::vector<RawEvent>& sourceEvents) = 0;

		std::vector<DynamicValue_ptr> dynamicValues;
	};

	//template<int N>
	//class BitLineEvent : public SynchronousEvent
	template<int N, class E=SynchronousEvent>
	class BitLineEvent : public E
	{
	public:
		BitLineEvent() : E() { bits.reset(); }
		BitLineEvent(const BitLineEvent &copy) 
			: E(copy) { }
		BitLineEvent(double time, STI_Device* device) 
			: E(time, device) { bits.reset(); }
		BitLineEvent(double time, uInt32 value, STI_Device* device) 
			: E(time, device) { setBits(value); }
		virtual ~BitLineEvent() {};

		//assign 'value' to bits LSB to MSB
		template <typename T>
		BitLineEvent<N, E>* setBits(T value, unsigned LSB=0, unsigned MSB=(N-1)) 
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

	private:
		virtual void setupEvent() = 0;
		virtual void loadEvent() = 0;
		virtual void playEvent() = 0;
		virtual void collectMeasurementData() = 0;

	protected:
		std::bitset<N> bits;
	};

	class PsuedoSynchronousEvent : public DynamicSynchronousEvent
	{
	public:
		PsuedoSynchronousEvent(double time, const std::vector<RawEvent>& events, STI_Device* device) 
			: DynamicSynchronousEvent(time, events, device), events_(events) {}
		PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy)
			: DynamicSynchronousEvent(copy), events_(copy.events_) {}

	protected:
		virtual void updateValue(const std::vector<RawEvent>& sourceEvents) {}

	private:
		void setupEvent() { }
		void loadEvent() { }
		void playEvent();
		void collectMeasurementData();

//		void getValue(const RawEvent& evt, const MixedValue* value);

	protected:
		const std::vector<RawEvent>& events_;
	};
	
};



#endif
