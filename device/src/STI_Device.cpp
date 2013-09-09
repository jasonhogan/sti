

#include <STI_Device.h>
#include "TimingEngineTypes.h"
#include "TimingEvent.h"
#include "TimingMeasurement.h"
#include "Channel.h"
#include "DeviceEventEngine.h"
#include "TimingEventGroup.h"
#include "PsuedoSynchronousEvent.h"
#include "LocalEventEngineManager.h"
#include "QueuedEventEngineManager.h"
#include "ServerEventEngine.h"
#include "LocalCollector.h"
#include "MasterTrigger.h"
#include "ServerInterface.h"
#include "PartnerDevice.h"
#include "DocumentationOptions.h"
#include "PlayOptions.h"
#include "ParsingResultsHandler.h"
#include "EngineCallbackHandler.h"
#include "PartnerEventHandler.h"
#include "AttributeUpdater.h"
#include "Attribute.h"
#include "ScheduledMeasurement.h"
#include "LocalMeasurementResultsHandler.h"
#include "BlockingMeasurementResultsTarget.h"
#include "NullEngineCallbackTarget.h"

#include "TextPosition.h"
#include "LocalTimingEvent.h"


#include <string>
#include <iostream>
using namespace std;

using STI::Device::STI_Device;
using STI::Device::DeviceID;
using STI::TimingEngine::EngineID;
using STI::TimingEngine::Channel;
using STI::TimingEngine::Channel_ptr;
using STI::TimingEngine::ChannelMap;
using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::EventEngine_ptr;
using STI::TimingEngine::TimingEvent_ptr;
using STI::Utils::MixedValue;
using STI::TimingEngine::TimingEventGroupMap;
using STI::TimingEngine::PsuedoSynchronousEvent;
using STI::TimingEngine::PsuedoSynchronousEvent_ptr;
using STI::TimingEngine::MasterTrigger_ptr;
using STI::TimingEngine::LocalEventEngineManager;
using STI::TimingEngine::LocalEventEngineManager_ptr;
using STI::Device::DeviceCollector_ptr;

STI_Device::STI_Device(const std::string& deviceName, const std::string& deviceConfigFilename) : 
deviceID("", "", 0), localServerEngineID(0, "Local Server")
{
	STI::Utils::ConfigFile configFile(deviceConfigFilename);
	
	deviceID.setName(deviceName);

	if(!initializeUsingConfigFile(configFile, true))
		return; //error
	
	init();
}


STI_Device::STI_Device(const std::string& deviceConfigFilename) : 
deviceID("", "", 0), localServerEngineID(0, "Local Server")
{
	STI::Utils::ConfigFile configFile(deviceConfigFilename);
	
	if(!initializeUsingConfigFile(configFile, false))
		return; //error
	
	init();
}

STI_Device::STI_Device(const STI::Utils::ConfigFile& deviceConfigFile) : 
deviceID("", "", 0), localServerEngineID(0, "Local Server")
{
	if(!initializeUsingConfigFile(deviceConfigFile, false))
		return; //error

	init();
}


STI_Device::STI_Device(const std::string& DeviceName, const std::string& IPAddress, unsigned short ModuleNumber) : 
deviceID(DeviceName, IPAddress, ModuleNumber), localServerEngineID(0, "Local Server")
{
	init();
}

STI_Device::~STI_Device()
{
}


bool STI_Device::initializeUsingConfigFile(const STI::Utils::ConfigFile& deviceConfigFile, bool nameInitialized)
{
	if(!deviceConfigFile.isParsed()) {
		return false;	//error
	}

	bool parseSuccess = true;
	
	std::string name = "";
	std::string address = "";
	unsigned short module = 0;

	//Name can be initialized in the config file or directly through the constructor.
	//If there is a name parameter in the config file, use this.
	if(deviceConfigFile.getParameter("Device Name", name)) {
		deviceID.setName(name);
		parseSuccess &= true;
	}
	else {
		//Used the constructor parameter, if given.
		parseSuccess &= nameInitialized;
	}

	parseSuccess &= deviceConfigFile.getParameter("IP Address", address);
	deviceID.setAddress(address);

	parseSuccess &= deviceConfigFile.getParameter("Module", module);
	deviceID.setModule(module);
	
	return parseSuccess;
}

void STI_Device::makeNewDeviceEventEngine(const EngineID& engineID, EventEngine_ptr& newEngine)
{
	newEngine = EventEngine_ptr(new DeviceEventEngine(*this));
//	newEngine = EventEngine_ptr(new FPGADeviceEventEngine(*this));	//FPGADeviceEventEngine : public DeviceEventEngine
}

void STI_Device::init()
{
	//Initialize partners

	using STI::TimingEngine::PartnerEventHandler;
	using STI::TimingEngine::PartnerEventHandler_ptr;

	DefaultPartnerPolicy_ptr policy(new DefaultPartnerPolicy(this));
	partnerCollector = DeviceCollector_ptr(new STI::Device::LocalDeviceCollector(policy) );
	requiredEventPartners = STI::Device::DeviceIDSet_ptr(new STI::Device::DeviceIDSet());
	partnerIDs = STI::Device::DeviceIDSet_ptr(new STI::Device::DeviceIDSet());

	partnerEventHandler = PartnerEventHandler_ptr( new PartnerEventHandler() );

	//Initialize engines
	using STI::TimingEngine::QueuedEventEngineManager_ptr;
	using STI::TimingEngine::QueuedEventEngineManager;
	using STI::TimingEngine::ServerEventEngine;

	eventEngineManager = LocalEventEngineManager_ptr(new LocalEventEngineManager());
	queuedEventEngineManager = QueuedEventEngineManager_ptr(new QueuedEventEngineManager(eventEngineManager, 2));


	//add main engine for testing
	EngineID mainEngine(1, "Main");
//	EventEngine_ptr engine = EventEngine_ptr(new DeviceEventEngine(*this));
	EventEngine_ptr engine;
	makeNewDeviceEventEngine(mainEngine, engine);
	eventEngineManager->addEventEngine(mainEngine, engine);

	//Engine 2 (local server for single line timing files)
//	EventEngine_ptr localEngine = EventEngine_ptr(new DeviceEventEngine(*this));
	EventEngine_ptr localEngine;
	makeNewDeviceEventEngine(localServerEngineID, localEngine);
	engine = EventEngine_ptr(
		new ServerEventEngine(localServerEngineID, localEngine, partnerCollector, getDeviceID()) );
	eventEngineManager->addEventEngine(localServerEngineID, engine);

	usingDefaultEventParsing = false;

	channelsInitialized = false;
	attributesInitialized = false;
	partnerDevicesInitialized = false;


//	dummyPartner = new PartnerDevice(true);
}

void STI_Device::connect(const STI::Server::ServerInterface& serverRef)
{
	server = &serverRef;

	initializeDevice();
	
//	server.getDeviceEventHandler(
}


void STI_Device::initializeDevice()
{
	initializeChannels();

	initializePartnerDevices();

	waitForRequiredPartners();

//	initializeAttributes();

}


void STI_Device::initializeChannels()
{
	if(channelsInitialized)
		return;
	channelsInitialized = true;

	channels.clear();

	defineChannels();	//pure virtual

//	loadChannelNames();
}



void STI_Device::initializePartnerDevices()
{
	if(partnerDevicesInitialized)
		return;
	partnerDevicesInitialized = true;

	partnerDevices.clear();
	partnerIDs->clear();
	requiredEventPartners->clear();

	definePartnerDevices();			//pure virtual

	//Check for requiredEventPartners
	PartnerDevice_ptr partner;
	PartnerAliasSet aliases;
	partnerDevices.getKeys(aliases);

	for(PartnerAliasSet::iterator it = aliases.begin(); it != aliases.end(); it++) {
		if(partnerDevices.get(*it, partner) && partner != 0) {
			if(partner->getPartnerEventsSetting()) {
				requiredEventPartners->insert(partner->getDeviceID());
			}
		}
	}

}

void STI_Device::waitForRequiredPartners()
{
	while( !requiredPartnersRegistered() )
	{
		boost::unique_lock< boost::shared_mutex > waitLock(initializationMutex);
		initializationCondition.wait(waitLock);
	}
}

void STI_Device::checkForNewPartners()
{
	initializationCondition.notify_one();
}

bool STI_Device::requiredPartnersRegistered()
{
	bool allRegistered = true;
	STI::Device::DeviceIDSet::iterator id;
	
	//partnerIDs is the list of required partners, listed by DeviceID
	for(id = partnerIDs->begin(); id != partnerIDs->end() && allRegistered; id++) {
		//allRegistered &= partnerDevices.contains(*id);
		allRegistered &= partnerCollector->contains(*id);
	}

	return allRegistered;
}

bool STI_Device::getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager) { 
	manager = queuedEventEngineManager;
	return (manager != 0);
}

void STI_Device::setPartnerEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& eventTarget)
{
	partnerEventHandler->setEventTarget(eventTarget);
}

//Partner::event(...)
//{
//	if(partnerEventTarget) {
//		partnerEventTarget->addEvent(getDevice().getID(), evt);
//	}
//}

const DeviceID& STI_Device::getDeviceID() const
{
	return deviceID;
}

const ChannelMap& STI_Device::getChannels() const
{
	return channels;
}

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	addChannel(channel, STI::TimingEngine::Input, inputType, outputType, defaultName);
}

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType)
{
	addInputChannel(channel, inputType, STI::Utils::Empty, "");
}

void STI_Device::addInputChannel(unsigned short channel, STI::Utils::MixedValueType inputType, std::string defaultName)
{
	addInputChannel(channel, inputType, STI::Utils::Empty, defaultName);
}

void STI_Device::addOutputChannel(unsigned short channel, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	addChannel(channel, STI::TimingEngine::Output, STI::Utils::Empty, outputType, defaultName);
}

bool STI_Device::addChannel(unsigned short channel, STI::TimingEngine::ChannelType type, 
							STI::Utils::MixedValueType inputType, STI::Utils::MixedValueType outputType, std::string defaultName)
{
	bool valid = true;
//	STI::TimingEngine::Channel tChannel;

	//if(Type == Input && OutputType != ValueNone)
	//{
	//	valid = false;
	//}
	if(type == STI::TimingEngine::Output && inputType != STI::Utils::Empty)
	{
		valid = false;
	}

	//check for duplicate channel number
	ChannelMap::iterator duplicate = channels.find(channel);

	if(duplicate != channels.end())
	{
		valid = false;

		std::cerr << "Error: Duplicate channel number in device " 
			<< getDeviceID().getName() << endl;
	}

	if(valid)
	{
		Channel_ptr tChannel = Channel_ptr(
			new Channel(getDeviceID(), channel, type, defaultName, inputType, outputType));
		channels[channel] = tChannel;
	}
	else
	{
		cerr << "Error: Invalid channel specification in device " 
			<< getDeviceID().getName() << endl;
	}

	return valid;
}

bool STI_Device::addPartnerDevice(std::string partnerAlias, std::string IP, short module, std::string deviceName)
{
	using STI::Device::PartnerDevice;
	using STI::Device::PartnerDevice_ptr;

	bool success = false;

	STI::Device::DeviceID partnerID(deviceName, IP, module);

	if(!partnerDevices.contains(partnerAlias)) {	//this is an original partner alias
		PartnerDevice_ptr partner(new PartnerDevice(partnerAlias, partnerID, partnerEventHandler));
		
		success = partnerDevices.add(partnerAlias, partner);
	
		if(success) {
			partnerIDs->insert(partnerID);
		}
	}
	else {
		success = false;
		cerr << "Error adding partner '" << partnerAlias 
			<< "'. This partner alias is already in use." << endl;
	}

	return success;
}

STI::Device::PartnerDevice& STI_Device::partnerDevice(std::string partnerAlias)
{
	using STI::Device::PartnerDevice;
	using STI::Device::PartnerDevice_ptr;

	PartnerDevice_ptr partner;

	if(!partnerDevices.get(partnerAlias, partner)) {
		//Partner alias not found; return the null partner.
		partner = PartnerDevice::getNullPartner();
	}

	return (*partner);
}

bool STI_Device::includePartner(STI::Device::DeviceID deviceID)
{
	return (partnerIDs->count(deviceID) > 0) && (deviceID != getDeviceID());
}

void STI_Device::parseDeviceEventsDefault(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
		STI::TimingEngine::SynchronousEventVector& eventsOut)
{
	//This event parser works well for non time critical devices (i.e. non-FPGA devices) and devices that
	//are fundamentally serial. Events scheduled for the same time will instead be played sequentially
	//using calls to STI_Device::writeChannel(...).

	usingDefaultEventParsing = true;

	TimingEventGroupMap::const_iterator iter;
	unsigned i;

	PsuedoSynchronousEvent_ptr evt;

	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		evt = PsuedoSynchronousEvent_ptr( 
			new PsuedoSynchronousEvent(iter->first, iter->second, this) );

		eventsOut.push_back( evt );

		// register all measurement events
		for(i = 0; i < iter->second->size(); i++)
		{
			if( iter->second->at(i)->isMeasurementEvent() )	// measurement event
				evt->addMeasurement( iter->second->at(i) );
		}
	}
}


//bool STI_Device::read(const TimingEvent_ptr& measurementEvent)
//{
//	MixedValue_ptr data;
//	measurementEvent->getMeasurement(
//	if(readChannel(measurementEvent->channelNum(), measurementEvent->value(), data))
//	{
//		measurementEvent.getMeasurement()->setData(data);
//		return true;
//	}
//	else
//		return false;
//}

bool STI_Device::read(unsigned short channel, 
	const MixedValue& commandIn, MixedValue& measurementOut)
{
	return readChannel(channel, commandIn, measurementOut);
}

bool STI_Device::write(unsigned short channel, const MixedValue& value)
{
	return writeChannel(channel, value);
}



bool STI_Device::waitForTrigger(const MasterTrigger_ptr& masterTrigger)
{
	return masterTrigger->waitForAll(STI::TimingEngine::WaitingForTrigger);		//waits for all, including this device
}

//bool STI_Device::addEventEngine(const STI::TimingEngine::EngineID& engineID)
//{
//	EventEngine_ptr engine = EventEngine_ptr(new DeviceEventEngine(*this));
//	if(eventEngineManager != 0 && engine != 0) {
//		return eventEngineManager->addEventEngine(engineID, engine);
//	}
//	return false;
//}


bool STI_Device::readChannelDefault(unsigned short channel, const STI::Utils::MixedValue& commandIn, 
						STI::Utils::MixedValue& measurementOut, double minimumStartTime_ns)
{
	using STI::TimingEngine::Channel;
	using STI::TimingEngine::LocalTimingEvent;
	using STI::TimingEngine::TimingEvent_ptr;
	using STI::TimingEngine::TimingMeasurement_ptr;

	bool success = false;

	ChannelMap::iterator chan = channels.find(channel);

	if(chan != channels.end()) {
		STI::TimingEngine::TextPosition pos("", 0);
		
		TimingEvent_ptr evt = TimingEvent_ptr( 
					new LocalTimingEvent(minimumStartTime_ns, *(chan->second), commandIn, 0, pos, "readChannel", true) );
		
		STI::TimingEngine::TimingMeasurement_ptr measurement;
		success = playSingleEventDefault(evt, measurement);
		
		if(success) {
			measurementOut.setValue( measurement->measuredValue() );
			success = false;
		} else {
			success = false;
		}
	}

	return success;
}

bool STI_Device::writeChannelDefault(unsigned short channel, const STI::Utils::MixedValue& commandIn, 
						 double minimumStartTime_ns)
{
	using STI::TimingEngine::Channel;
	using STI::TimingEngine::LocalTimingEvent;
	using STI::TimingEngine::TimingEvent_ptr;

	ChannelMap::iterator chan = channels.find(channel);

	if(chan != channels.end()) {
		STI::TimingEngine::TextPosition pos("", 0);
		
		TimingEvent_ptr evt = TimingEvent_ptr( 
					new LocalTimingEvent(minimumStartTime_ns, *(chan->second), commandIn, 0, pos, false) );
			
		STI::TimingEngine::TimingMeasurement_ptr measurement;
	
		return playSingleEventDefault(evt, measurement);
	}

	return false;
}

bool STI_Device::playSingleEventDefault(const STI::TimingEngine::TimingEvent_ptr& event, STI::TimingEngine::TimingMeasurement_ptr& measurement)
{
	using STI::TimingEngine::ParsingResultsTarget_ptr;
	using STI::TimingEngine::NullParsingResultsTarget;
	using STI::TimingEngine::EngineCallbackTarget_ptr;
	using STI::TimingEngine::NullEngineCallbackTarget;
	using STI::TimingEngine::TimingEventVector;
	using STI::TimingEngine::TimingEventVector_ptr;
	using STI::TimingEngine::MeasurementResultsTarget_ptr;
//	using STI::TimingEngine::NullMeasurementResultsTarget;

	using STI::TimingEngine::ParsingResultsHandler_ptr;
	using STI::TimingEngine::ParsingResultsHandler;
	using STI::TimingEngine::EngineCallbackHandler_ptr;
	using STI::TimingEngine::EngineCallbackHandler;
	using STI::TimingEngine::DocumentationOptions_ptr;
	using STI::TimingEngine::PlayOptions_ptr;
	using STI::TimingEngine::MeasurementResultsHandler_ptr;
	using STI::TimingEngine::LocalMeasurementResultsHandler;
	using STI::TimingEngine::BlockingMeasurementResultsTarget;
	using STI::TimingEngine::BlockingMeasurementResultsTarget_ptr;
	using STI::TimingEngine::TimingMeasurementVector_ptr;

	STI::TimingEngine::EngineInstance instance(localServerEngineID);
	instance.parseTimestamp.timestamp = 0;
	instance.playTimestamp.timestamp = 1;

	TimingEventVector_ptr events = TimingEventVector_ptr(new TimingEventVector());
	events->push_back(event);

	//Measurement results
	TimingMeasurementVector_ptr data;

	//Null callbacks targets (ignore callbacks)
	ParsingResultsTarget_ptr parsingTarget(new NullParsingResultsTarget());
	EngineCallbackTarget_ptr engineCallbackTarget(new NullEngineCallbackTarget());

	//Callback handlers
	ParsingResultsHandler_ptr parsingHandler(new ParsingResultsHandler(deviceID, instance, parsingTarget));
	EngineCallbackHandler_ptr engineCallback(new EngineCallbackHandler(deviceID, instance, engineCallbackTarget));

	//Measurement handling
	BlockingMeasurementResultsTarget_ptr engineResultsTarget(new BlockingMeasurementResultsTarget(instance));
	MeasurementResultsHandler_ptr engineResultsHandler(new LocalMeasurementResultsHandler(engineResultsTarget, 0));

	DocumentationOptions_ptr docOptions( 
		new STI::TimingEngine::LocalDocumentationOptions(true, "") );
	PlayOptions_ptr playOptions( 
		new STI::TimingEngine::PlayOptions(0, 1000, 0) );

	eventEngineManager->clear(instance.id, engineCallback);
	eventEngineManager->parse(instance, events, parsingHandler);
	eventEngineManager->load(instance, engineCallback);
	
	bool success = eventEngineManager->inState(instance.id, STI::TimingEngine::Loaded);
	
	eventEngineManager->play(instance, playOptions, docOptions, engineResultsHandler, engineCallback);

	eventEngineManager->publishData(instance, engineResultsHandler, docOptions);

	//Get results of any measurements.
	//This call blocks until data has been returned.
	//Can be aborted from STI_Device::stop() using engineResultsTarget->stopWaiting().
	success = engineResultsTarget->getData(data, 100, true);	//100 ms timeout before checking for abort; keeps trying after timing out (unless aborted).

	if(success && data != 0 && data->size() > 0) {
		measurement = data->at(0);
	}

	return success;

}



//*********** Device attributes functions ****************//
void STI_Device::addAttribute(const std::string& key, const std::string& initialValue, const std::string& allowedValues)
{
	Attribute_ptr attribute(new Attribute( initialValue, allowedValues) );
	attributes.add(key, attribute);
}


bool STI_Device::setAttribute(const std::string& key, const std::string& value)
{
	Attribute_ptr attribute;

	if(!attributes.get(key, attribute) || attribute == 0)
		return false;	// Attribute not found

	if( !attribute->isAllowed(value) )
		return false;	//attribute not in list of allowed values
	
	// Derived classes may add attributeUpdaters that implement
	// AttributeUpdater::updateAttributes.
	// This allows for attribute updates without implementing 
	// STI_Device::updateAttributes so that the derived class can act as
	// another abstract base without having to change the name 
	// of the interface function hooks.

	bool success = false;
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
	{
		success |= attributeUpdaters.at(i)->updateAttributes(key, value);
	}

	success |= updateAttribute(key, value);		//pure virtual

	if(success) {
		attribute->setValue(value);
	}

	////save current attributes to file
	//if( success && saveAttributesToFile ) {
	//	saveAttributes();
	//}

	return success;
}

//std::string STI_Device::getAttribute(std::string key) const
bool STI_Device::getAttribute(const std::string& key, std::string& value) const
{
//	STI::Device::AttributeMap::iterator it = attributes.find(key);

	Attribute_ptr attribute;
	if(attributes.get(key, attribute) && attribute != 0) {
		value = attribute->value();
		return true;
	}
	return false;

	//if(it != attributes.end()) {
	//	value = it->second.value();
	//	return true;
	//}
	//return false;
}

void STI_Device::refreshDeviceAttributes()
{
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
		attributeUpdaters.at(i)->refreshAttributes();

	refreshAttributes();		//pure virtual

	//STI::Pusher::TDeviceRefreshEvent refreshEvent;
	//refreshEvent.deviceID = CORBA::string_dup(tDevice->deviceID);
	//refreshEvent.type = STI::Pusher::RefreshAttributes;
	//sendRefreshEvent(refreshEvent);
}


void STI_Device::addAttributeUpdater(const STI::Device::AttributeUpdater_ptr& updater)
{
	attributeUpdaters.push_back(updater);
}

