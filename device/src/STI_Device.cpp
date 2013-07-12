/*! \file STI_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class STI_Device
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

//#include <deviceVersion.h>

#include <STI_Device.h>
#include <ORBManager.h>
#include <DeviceConfigure_i.h>
#include <DataTransfer_i.h>
#include <CommandLine_i.h>
#include <DeviceTimingSeqControl_i.h>
#include <DeviceBootstrap_i.h>
#include <DataLogger_i.h>

#include <Attribute.h>
#include <device.h>
#include <EventConflictException.h>
#include <EventParsingException.h>
#include <STI_Exception.h>

#include <ConfigFile.h>

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <map>

#include <MixedData.h>
//#include <LinkedValue.h>

using std::string;
using std::map;
using std::stringstream;


#include <iostream>
using namespace std;

MixedValue STI_Device::emptyValue = MixedValue(); 

STI_Device::STI_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename) :
orbManager(orb_manager), deviceName(DeviceName)

{
	std::string IPAddress;
	unsigned short ModuleNumber;
	bool parseSuccess;

	ConfigFile config(configFilename);
	
	parseSuccess = config.getParameter("IP Address", IPAddress);
	parseSuccess &= config.getParameter("Module", ModuleNumber);

	//attempt to set the log directory from the config file
	if(!config.getParameter("Log Directory", logDir))
		logDir = ".";

	if(parseSuccess)
	{
		init(IPAddress, ModuleNumber);
	}
	else
	{
		cerr << "Error parsing STI Device configuration file '" << configFilename << "'." << endl
			<< "Device cannot initialize." << endl;
		orbManager->ORBshutdown();
		
		init("", 0);	//just make sure nothing is null before shutting down
	}
}

STI_Device::STI_Device(ORBManager* orb_manager, std::string DeviceName, 
					   std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory) : 
orbManager(orb_manager), deviceName(DeviceName), logDir(logDirectory)
{
	init(IPAddress, ModuleNumber);
}

void STI_Device::setLogDirectory(std::string logDirectory)
{
	logDir = logDirectory;
	dataLoggerServant->setLogDirectory(logDir);
}

void STI_Device::startDataLogging()
{
	dataLoggerServant->startLogging();
}

void STI_Device::stopDataLogging()
{
	dataLoggerServant->stopLogging();
}


void STI_Device::init(std::string IPAddress, unsigned short ModuleNumber)
{
	//TDevice
	tDevice = new STI::Types::TDevice;	//_var variable does not need to be deleted
	tDevice->deviceName = getDeviceName().c_str();
	tDevice->address = IPAddress.c_str();
	tDevice->moduleNum = ModuleNumber;

	// Bootstrap servant name -- the STI_Server can look for this name to reregister this devices after a crash
	deviceBootstrapObjectName = "DeviceBootstrap.Object";
	bootstrapIsRegistered = false;

	//servants
	configureServant = new DeviceConfigure_i(this);
	dataTransferServant = new DataTransfer_i(this);
	commandLineServant = new CommandLine_i(this, configureServant);
	deviceControlServant = new DeviceTimingSeqControl_i(this);
	deviceBootstrapServant = new DeviceBootstrap_i(this);
	dataLoggerServant = new DataLogger_i(logDir, this);

	dummyPartner = new PartnerDevice(true);

	alive = true;
	serverConfigureFound = false;
	registedWithServer = false;
	deviceEventHandlerFound = false;

	partnerDevicesInitialized = false;
	attributesInitialized = false;
	channelsInitialized = false;


	registrationAttempts = 0;

	stopWaiting = false;
	numberWaitingForStatus = 0;
	usingDefaultEventParsing = false;

	channelNameFilename = "";
	attributeFilename = "";
	setSaveAttributesToFile(false);	//default false to keep current convention (8/4/2012)

//	addedPartners.clear();
	attributes.clear();
	channels.clear();
//	requiredPartners.clear();
//	mutualPartners.clear();
//	eventPartners.clear();
	measurements.clear();

	timeOfPause = 0;

	mainLoopMutex = new omni_mutex();
	playSingleEventMutex = new omni_mutex();

	executeMutex  = new omni_mutex();
	executingMutex = new omni_mutex();
	executingCondition = new omni_condition(executingMutex);


	playEventsMutex = new omni_mutex();
	playEventsTimer = new omni_condition(playEventsMutex);

	measureMutex = new omni_mutex();
	measureCondition = new omni_condition(measureMutex);

	deviceStatusMutex = new omni_mutex();
	deviceStatusCondition = new omni_condition(deviceStatusMutex);

	deviceLoadingMutex = new omni_mutex();
	deviceLoadingCondition = new omni_condition(deviceLoadingMutex);
	deviceRunningMutex = new omni_mutex();
	deviceRunningCondition = new omni_condition(deviceRunningMutex);
	devicePauseMutex = new omni_mutex();
	devicePauseCondition = new omni_condition(devicePauseMutex);

	requiredPartnerRegistrationMutex = new omni_mutex();
	requirePartnerRegistrationCondition = new omni_condition(requiredPartnerRegistrationMutex);



	loadEventsThread = 0;
	playEventsThread = 0;


	// Must call updateState() only after all condition variables are defined.
	deviceStatus = EventsEmpty;	//initial state
	changeStatus(EventsEmpty);	//calls updateState()

	executing = false;

	// Automatically connect to the STI server and transfer 
	// channels, attributes, partners, etc.
	omni_thread::create(connectToServerWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	//deviceMain loop
	mainThread = omni_thread::create(deviceMainWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

STI_Device::~STI_Device()
{
//	cerr << "Destructor: " << tDevice->deviceID << endl;
	//remove this Device from the Server
	if (orbManager->isRunning() )
	{
		try {
			ServerConfigureRef->removeDevice(tDevice->deviceID);
		}
		catch(...) {
		}
	}

	//if(configureServant != 0)
	//	delete configureServant;
	//if(dataTransferServant != 0)
	//	delete dataTransferServant;
	//if(commandLineServant != 0)
	//	delete commandLineServant;
	//if(deviceControlServant != 0)
	//	delete deviceControlServant;

	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
		delete attributeUpdaters.at(i);

	delete dummyPartner;
	delete mainLoopMutex;
	delete playEventsMutex;
	delete playEventsTimer;

	delete deviceLoadingMutex;
	delete deviceLoadingCondition;
	delete deviceRunningMutex;
	delete deviceRunningCondition;
	delete devicePauseMutex;
	delete devicePauseCondition;

}



void STI_Device::deviceShutdown()
{
	alive = false;	//stops deviceMain loops

//	try {
//		ServerConfigureRef->removeDevice(tDevice->deviceID);
//	}
//	catch(...) {
//	}

	orbManager->ORBshutdown();
}

void STI_Device::deviceMainWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);

	bool run = true;

	thisObject->orbManager->waitForRun();	//this ensure that STI_Device has finished its constructor
	
	cout << thisObject->getDeviceName() 
		<< " (" << thisObject->getTDevice().address << ", " 
		<< "Module " << thisObject->getTDevice().moduleNum << ") is ready." << endl;
	
	while(!thisObject->registedWithServer && thisObject->isAlive()) {omni_thread::yield();}

	while(run && thisObject->isAlive())
	{
		//mutex ensures that execute(argc, argv) is not running
//		thisObject->mainLoopMutex->lock();			
		{
			run = thisObject->deviceMain(			//pure virtual
			thisObject->orbManager->getArgc(), 
			thisObject->orbManager->getArgv() );
		}
	//	thisObject->sti_err << 45;
//		thisObject->mainLoopMutex->unlock();

	};  
}





//*********** Network communication functions ****************//
void STI_Device::connectToServerWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	thisObject->connectToServer();
}

void STI_Device::connectToServer()
{
	orbManager->waitForRun();

	// setup this device's channels
	initializeChannels();

	// Aquire a reference to ServerConfigure from the NameService.
	aquireServerConfigure();
	
	// setup this device's partner devices
	initializePartnerDevices();

	if(serverConfigureFound)
	{
		// When found, register with the server and acquire a unique deviceID.
		registerDevice();
	}

	if(registedWithServer)
	{
		//Get a reference to the device event handler from the server.
		aquireDeviceEventHandler();

		// Register this device's bootstrap servant with the Name Service.
		registerBootstrapServant();

		// setup this device's attributes
		// This will block until all required partners have been registered.
		
		STI::Pusher::TDeviceRefreshEvent refreshEvent;
		refreshEvent.type = STI::Pusher::RefreshDevice;
		refreshEvent.deviceID = CORBA::string_dup( getTDevice().deviceID );

		sendRefreshEvent(refreshEvent);

		initializeAttributes();
		dataLoggerServant->startLogging();
	}
}

void STI_Device::aquireDeviceEventHandler()
{
	deviceEventHandlerFound = false;

	try {
		//Get a reference to the device event handler from the server.
		deviceEventPusher.installDeviceEventHandler( ServerConfigureRef->getDeviceEventHandler() );
	//	deviceEventHandlerRef = ServerConfigureRef->getDeviceEventHandler();
		deviceEventHandlerFound = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " in aquireDeviceEventHandler(). " << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " in aquireDeviceEventHandler(). " << endl;
	}
	catch(...) {
		cerr << "Caught unknown exception in aquireDeviceEventHandler()." << endl;
	}

}

void STI_Device::aquireServerConfigure()
{
	CORBA::Object_var obj;
	registrationAttempts = 0;

	// Try to acquire ServerConfigure Object
	while(!serverConfigureFound && registrationAttempts < 10)
	{
		obj = orbManager->getObjectReference("STI/Device/ServerConfigure.Object");
		
		ServerConfigureRef = STI::Server_Device::ServerConfigure::_narrow(obj);
		
		if( !CORBA::is_nil(ServerConfigureRef) )
		{
			// Object reference was found on the NameService

			try {
				registrationAttempts++;
				serverConfigureFound = ServerConfigureRef->ping();			
			}
			catch(CORBA::TRANSIENT& ex) {
				cerr << "Caught system exception CORBA::" 
					<< ex._name() << " -- unable to contact the "
					<< "STI Server." << endl
					<< "Make sure the server is running and that omniORB is "
					<< "configured correctly." << endl;
			}
			catch(CORBA::SystemException& ex) {
				cerr << "Caught a CORBA::" << ex._name()
					<< " while trying to contact the STI Server." << endl;
			}
			catch(...) {
				cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
			}
		}
		else
		{
			// Reference is nil
			registrationAttempts++;
			cerr << "STI Server not found: Cannot access the ServerConfigure Object." << endl;
		}			
	}
}



void STI_Device::registerDevice()
{
	// Attempt to register this device with the server
	try {
		registedWithServer = ServerConfigureRef->registerDevice(tDevice, deviceBootstrapServant->_this());
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}
}



void STI_Device::reRegisterDevice()
{
	serverConfigureFound = false;
	registedWithServer = false;

	omni_thread::create(connectToServerWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}



void STI_Device::registerBootstrapServant()
{
	if(bootstrapIsRegistered)
		return;

	STI::Server_Device::DeviceBootstrap_var BootstrapRef;
	bool success = false;

	string contextName = string(tDevice->deviceContext);
	
//	orbManager->bindObjectToName(deviceBootstrapServant->_this(), 
//	contextName + deviceBootstrapObjectName);

	orbManager->registerServant(deviceBootstrapServant, 
				contextName + deviceBootstrapObjectName);


	// Try to resolve the servant as a test
	CORBA::Object_var obj = orbManager->getObjectReference(
			contextName + deviceBootstrapObjectName);
	BootstrapRef = STI::Server_Device::DeviceBootstrap::_narrow(obj);

	try 
	{
		success = BootstrapRef->ping();
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " while testing this devices DeviceBootstrap servant." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact this device's DeviceBootstap servant." << endl;
	}
	
	bootstrapIsRegistered = success;

}



void STI_Device::initializeChannels()
{
	if(channelsInitialized)
		return;
	channelsInitialized = true;

	channels.clear();

	defineChannels();	//pure virtual

	loadChannelNames();
}

void STI_Device::saveChannelNames()
{
	ConfigFile channelNameConfig(channelNameFilename);

	for(ChannelMap::iterator channel = channels.begin(); channel != channels.end(); channel++)
	{
		channelNameConfig.setParameter( STI::Utils::valueToString(channel->second.channel), channel->second.channelName);
	}

	std::stringstream header;

	header << "# Channel names for " << getDeviceName() << std::endl
		<< "# Module:  " << getTDevice().moduleNum << std::endl
		<< "# Address: " << getTDevice().address << std::endl << std::endl;

	channelNameConfig.setHeader(header.str());
	channelNameConfig.saveToDisk();

//	std::cout << channelNameConfig.printParameters();
}


void STI_Device::loadChannelNames()
{
	if(channelNameFilename.compare("") == 0)	//no name given; generate default filename
	{
		std::stringstream filenameStream;

		filenameStream << getDeviceName() << "_Module_" << this->getTDevice().moduleNum << "_channels";

		channelNameFilename = STI::Utils::replaceChar(filenameStream.str(), " ", "_");
		channelNameFilename = STI::Utils::replaceChar(channelNameFilename, ".", "_");

		filenameStream.str("");
		filenameStream << channelNameFilename;
		filenameStream << ".ini";
		channelNameFilename = filenameStream.str();
	}

	ConfigFile channelNameConfig(channelNameFilename);
	bool parseSuccess = true;
	
	std::string name;

	for(ChannelMap::iterator channel = channels.begin(); channel != channels.end() && parseSuccess; channel++)
	{
		parseSuccess = channelNameConfig.getParameter(STI::Utils::valueToString(channel->second.channel), name);
		
		if(parseSuccess)
		{
			channel->second.channelName = CORBA::string_dup( name.c_str() );
		}
	}

	//if(!parseSuccess) 
	//{
	//	std::cerr << "Error:  Channel name config file failed to parse. Using default channel names." << std::endl;
	//}

}

void STI_Device::saveAttributes()
{
	ConfigFile attributeConfig(attributeFilename);

	for(AttributeMap::iterator attrib = attributes.begin(); attrib != attributes.end(); attrib++) {
		attributeConfig.setParameter( attrib->first, attrib->second.value());
	}

	std::stringstream header;

	header << "# Attributes for " << getDeviceName() << std::endl
		<< "# Module:  " << getTDevice().moduleNum << std::endl
		<< "# Address: " << getTDevice().address << std::endl << std::endl;

	attributeConfig.setHeader(header.str());
	attributeConfig.saveToDisk();
}

void STI_Device::loadAttributes()
{
	if(attributeFilename.compare("") == 0)	//no name given; generate default filename
	{
		std::stringstream filenameStream;

		filenameStream << getDeviceName() << "_Module_" << this->getTDevice().moduleNum << "_attributes";

		attributeFilename = STI::Utils::replaceChar(filenameStream.str(), " ", "_");
		attributeFilename = STI::Utils::replaceChar(attributeFilename, ".", "_");

		filenameStream.str("");
		filenameStream << attributeFilename;
		filenameStream << ".ini";
		attributeFilename = filenameStream.str();
	}

	ConfigFile attributeConfig(attributeFilename);
	bool parseSuccess = true;
	
	std::string value;

	for(AttributeMap::iterator attrib = attributes.begin(); attrib != attributes.end() && parseSuccess; attrib++) {		
		
		parseSuccess = attributeConfig.getParameter(attrib->first, value);
		
		if(parseSuccess && attrib->second.isAllowed( value)) {
			attrib->second.setValue( value );
		}
	}
}


void STI_Device::initializeAttributes()
{
	if(attributesInitialized)
		return;
	attributesInitialized = true;	//indicates that the initializeAttributes() function has been called.

	bool success = true;
	AttributeMap::iterator it;

	attributes.clear();

	waitForRequiredPartners();

	defineAttributes();	// pure virtual

	for(unsigned i = 0; i < attributeUpdaters.size(); i++)
	{
		attributeUpdaters.at(i)->defineAttributes();
	}

	//override initial values in defineAttributes() with values retrieved from file
	if( saveAttributesToFile ) {
		loadAttributes();
	}

	for(it = attributes.begin(); it != attributes.end(); it++)
	{
		success &= setAttribute(it->first, it->second.value());
	}

	STI::Pusher::TDeviceRefreshEvent refreshEvent;
	refreshEvent.deviceID = CORBA::string_dup(tDevice->deviceID);
	refreshEvent.type = STI::Pusher::RefreshAttributes;

	sendRefreshEvent(refreshEvent);

	if(!success)
	{
		cerr << "Error initializing attributes." << endl;
	}
}

void STI_Device::waitForRequiredPartners()
{
	while( !requiredPartnersRegistered() )
	{
		requiredPartnerRegistrationMutex->lock();
		{
			requirePartnerRegistrationCondition->wait();
		}
		requiredPartnerRegistrationMutex->unlock();
	}
}

void STI_Device::checkForNewPartners()
{
	requiredPartnerRegistrationMutex->lock();
	{
		requirePartnerRegistrationCondition->signal();
	}
	requiredPartnerRegistrationMutex->unlock();
}

bool STI_Device::requiredPartnersRegistered()
{
	bool registered = true;
	
	PartnerDeviceMap::iterator iter = getPartnerDeviceMap().begin();

	for(iter = getPartnerDeviceMap().begin(); iter != getPartnerDeviceMap().end(); iter++)
	{
		if( iter->second->isRequired() )
		{
			registered &= iter->second->isRegistered();
		}
	}

	return registered;
}

void STI_Device::initializePartnerDevices()
{
	if(partnerDevicesInitialized)
		return;
	partnerDevicesInitialized = true;

//	partnerDevices.clear();

	definePartnerDevices();			//pure virtual

	string deviceID;
	PartnerDeviceMap::iterator partner;

	try {
		for(partner = partnerDevices.begin(); partner != partnerDevices.end(); partner++)
		{
			deviceID = ServerConfigureRef->generateDeviceID( partner->second->device() );

			partner->second->setDeviceID(deviceID);
		}
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to initialize partner devices." << endl;
		partnerDevicesInitialized = false;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to initialize partner devices." << endl;
		partnerDevicesInitialized = false;
	}
	catch(...) {
		partnerDevicesInitialized = false;
	}
}



PartnerDevice& STI_Device::partnerDevice(std::string partnerName)
{
	PartnerDeviceMap::iterator partner = partnerDevices.find(partnerName);
	PartnerDeviceMap::iterator partnerByDeviceID = partnerDevices.find( getPartnerDeviceID(partnerName) );

	if(partner != partnerDevices.end())
	{
		return *(partner->second);
	}
	else if(partnerByDeviceID != partnerDevices.end())
	{
		return *(partnerByDeviceID->second);
	}
	else
	{
		return *dummyPartner;
	}




	// requiredPartners:     partnerName => deviceID
	// registeredPartners:   deviceID => PartnerDevice

	// Search to see if the partnerName is a requiredPartner
//	map<string, string>::iterator partner = requiredPartners.find( partnerName );


	//if(partner == requiredPartners.end())	// invalid partnerName
	//{
	//	cerr << "Error: The partner '" << partnerName 
	//		<< "' is not a partner of this device. " << endl
	//		<< "All partners must be added in definePartnerDevices()." << endl;
	//	return *dummyPartner;
	//}
//	else
	
//	PartnerDeviceMap& partnerMap = commandLineServant->getRegisteredPartners();

	// Search to see if partnerName IS a registered deviceID.
	// This happens (for example) if the partner adds itself and/or is not specified in 
	// this device's definePartners() function, or if the partner is 
	// added using addLocalPartnerDevice().

	/*
	PartnerDeviceMap::iterator it = partnerMap.find( partnerName );	
		

	if( partner != requiredPartners.end() )		//partnerName is not a required partner
		it = partnerMap.find( partner->second );	//lookup deviceID using partnerName

	if( it == partnerMap.end() )	// this partner has not been registered
	{
		cerr << "Error: The partner '" << partnerName 
			<< "' is not a registered partner of this device." << endl;

		return *dummyPartner;
	}
	else
		return *(it->second);
	*/
}

bool STI_Device::executeSpecialCommands(vector<string> arguments, std::string& output)
{
	bool success = false;

	if(arguments.size() > 1 && arguments.at(1).compare("sti")==0) {
		if(arguments.size() > 2) {
			if(arguments.at(2).compare("help")==0) {
				output = getDeviceHelp();
				return true;
			}
			else if(arguments.at(2).compare("setAttribute")==0 && arguments.size() == 5) {
				output = "setAttribute(" + arguments.at(3) + ", " + arguments.at(4) + ")\n";
				success = setAttribute(arguments.at(3), arguments.at(4));
				if(success) {
					output += "success\n";
					refreshDeviceAttributes();
				} else {
					output += "failed\n";
				}
				return true;
			}
			else if(arguments.at(2).compare("loadDefaultAttributes")==0 && arguments.size() == 3) {
				output = "Loading default attributes.\n";
				
				bool oldSaveAttributesToFile = saveAttributesToFile;
				saveAttributesToFile = false;
				
				attributesInitialized = false;
				initializeAttributes();
				
				saveAttributesToFile = oldSaveAttributesToFile;
				if(saveAttributesToFile)
					saveAttributes();
				
				return true;
			}
			else {
				//for derived classes that add special commands to sti [...]
				if(executeDelegatedSpecialCommands(arguments, output)) {
					return true;
				}
				else {
					output = "sti:  Error: Unknown command. \n\n" + printSpecialCommandOptions();
					return true;
				}
			}
		} else {
			output = printSpecialCommandOptions();
			return true;
		}
	}

	return false;
}
string STI_Device::printSpecialCommandOptions()
{
	std::stringstream output;

	output << "sti [command] {options...}" << endl
		   << "Valid commands are:" << endl
		   << "    setAttribute [key] [value]   --  Set STI attribute key = value" << endl
		   << "    loadDefaultAttributes        --  Initialize all attributes to default values." << endl
		   << "    help                         --  Print device specific help info." << endl;

	string delegated = printDelegatedSpecialCommandOptions();	//for derived classes that add special commands to sti [...]

	if( delegated.compare("") != 0 ) {
		output << delegated;
	}

	return output.str();
}

string STI_Device::execute(string args)
{
	std::string result = "";
	vector<string> arguments;
	// Using the C++ main(int argc, char** argv) convention: the 
	// first argument is the program's name (in this case the device's name).
	arguments.push_back(deviceName);

	STI::Utils::splitString(args, " ", arguments);

	char** argv = new char*[ arguments.size() ];

//	char** argv;
//	argv = new (char*)[ arguments.size() ];
	unsigned i;

	for(i = 0; i < arguments.size(); i++)
	{
//		cerr << "arg: " << arguments[i] << endl;
		argv[i] = new char[arguments[i].size() + 1];
		strcpy(argv[i], arguments[i].c_str());
	}

	//only one execute command can happen at a time
	//execute is only allowed when events aren't playing on this device
	executeMutex->lock();	//executeMutex is ONLY used here; prevents multiple called to execute from running at the same time
	{
		executingMutex->lock();
		{
			if(executionAllowed)
			{
				executing = true;
				executingMutex->unlock();

				if(!executeSpecialCommands(arguments, result))
				{
					result = execute(arguments.size(), argv);
				}

				executingMutex->lock();
				executing = false;
				executingCondition->signal();
			}
		}
		executingMutex->unlock();
	}
	executeMutex->unlock();


	for(i = 0; i < arguments.size(); i++)
		delete[] argv[i];
	delete[] argv;

	return result;
}

void STI_Device::reportMessage(STI::Pusher::MessageType type, string message)
{
	STI::Pusher::TMessageEvent messageEvent;
	messageEvent.type = type;
	messageEvent.message = CORBA::string_dup( message.c_str() );

	if(deviceEventHandlerFound)
	{
		deviceEventPusher.pushEventToServer(messageEvent);

		//try {
		//	deviceEventHandlerRef->pushMessageEvent( messageEvent );
		//}
		//catch(CORBA::TRANSIENT& ex) {
		//	cerr << "Caught system exception CORBA::" 
		//		<< ex._name() << " -- unable to contact the "
		//		<< "STI Server." << endl
		//		<< "Make sure the server is running and that omniORB is "
		//		<< "configured correctly." << endl;
		//}
		//catch(CORBA::SystemException& ex) {
		//	cerr << "Caught a CORBA::" << ex._name()
		//		<< " while trying to contact the STI Server." << endl;
		//}
	}
}
void STI_Device::sendRefreshEvent(STI::Pusher::TDeviceRefreshEvent event)
{
	if(deviceEventHandlerFound)
	{
		deviceEventPusher.pushEventToServer(event);
		
		//try {
		//	deviceEventHandlerRef->pushDeviceRefreshEvent( event );
		//}
		//catch(CORBA::TRANSIENT& ex) {
		//	cerr << "Caught system exception CORBA::" 
		//		<< ex._name() << " -- unable to contact the "
		//		<< "STI Server." << endl
		//		<< "Make sure the server is running and that omniORB is "
		//		<< "configured correctly." << endl;
		//}
		//catch(CORBA::SystemException& ex) {
		//	cerr << "Caught a CORBA::" << ex._name()
		//		<< " while trying to contact the STI Server." << endl;
		//}
	}
}

void STI_Device::sendDeviceDataEvent(STI::Pusher::TDeviceDataEvent event)
{
	if(deviceEventHandlerFound)
	{
		deviceEventPusher.pushEventToServer(event);

		//try {
		//	deviceEventHandlerRef->pushDeviceDataEvent( event );
		//}
		//catch(CORBA::TRANSIENT& ex) {
		//	cerr << "Caught system exception CORBA::" 
		//		<< ex._name() << " -- unable to contact the "
		//		<< "STI Server." << endl
		//		<< "Make sure the server is running and that omniORB is "
		//		<< "configured correctly." << endl;
		//}
		//catch(CORBA::SystemException& ex) {
		//	cerr << "Caught a CORBA::" << ex._name()
		//		<< " while trying to contact the STI Server." << endl;
		//}
	}
}

void STI_Device::stiError(std::string message)
{
	STI::Pusher::TDeviceRefreshEvent refreshEvent;
	
	refreshEvent.type = STI::Pusher::DeviceErrorStream;
	refreshEvent.deviceID = CORBA::string_dup( tDevice->deviceID );
	refreshEvent.errorMessage = CORBA::string_dup( message.c_str() );

	if(!deviceEventHandlerFound)
		return;

	deviceEventPusher.pushEventToServer( refreshEvent );

	//try {
	//	deviceEventHandlerRef->pushDeviceRefreshEvent( refreshEvent );
	//}
	//catch(CORBA::TRANSIENT& ex) {
	//	cerr << "Caught system exception CORBA::" 
	//		<< ex._name() << " -- unable to contact the "
	//		<< "STI Server." << endl
	//		<< "Make sure the server is running and that omniORB is "
	//		<< "configured correctly." << endl;
	//}
	//catch(CORBA::SystemException& ex) {
	//	cerr << "Caught a CORBA::" << ex._name()
	//		<< " while trying to contact the STI Server." << endl;
	//}
}





STI::Server_Device::DeviceTimingSeqControl_ptr STI_Device::getDeviceTimingSeqControl()
{
	return deviceControlServant->_this();
}

STI::Server_Device::DataTransfer_ptr STI_Device::getDataTransfer()
{
	return dataTransferServant->_this();
}

STI::Server_Device::DeviceConfigure_ptr STI_Device::getDeviceConfigure()
{
	return configureServant->_this();
}

STI::Server_Device::CommandLine_ptr STI_Device::getCommandLine()
{	
	return commandLineServant->_this();
}


//*********** Device attributes functions ****************//
bool STI_Device::setAttribute(string key, string value)
{

	AttributeMap::iterator attrib = attributes.find(key);

	if( attrib == attributes.end() )
		return false;	// Attribute not found

	if( !attrib->second.isAllowed(value) )
		return false;	//attribute not in list of allowed values

	//Pass the 'value' string by reference to updateStreamAttribute.
	//Allows the update functions to modify the newValue string.
	string newValue = value;

	// Derived classes may add attributeUpdaters that implement
	// STI_Device::AttributeUpdater::updateAttributes.
	// This allows for attribute updates without implementing 
	// STI_Device::updateAttributes so that the derived class can act as
	// another abstract base without having to change the name 
	// of the interface function hooks.

	bool success = false;
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
	{
		success |= attributeUpdaters.at(i)->updateAttributes(key, newValue);

		if(success)
			attrib->second.setValue(newValue);
	}

	if( updateAttribute(key, newValue) )	//pure virtual
	{
		success = true;
		attrib->second.setValue(newValue);
	}

	//save current attributes to file
	if( success && saveAttributesToFile ) {
		saveAttributes();
	}

	return success;
}
std::string STI_Device::getAttribute(std::string key) const
{
	return getAttributes().find(key)->second.value();
}

void STI_Device::refreshDeviceAttributes()
{
	unsigned i;
	for(i = 0; i < attributeUpdaters.size(); i++)
		attributeUpdaters.at(i)->refreshAttributes();

	refreshAttributes();		//pure virtual

	STI::Pusher::TDeviceRefreshEvent refreshEvent;
	refreshEvent.deviceID = CORBA::string_dup(tDevice->deviceID);
	refreshEvent.type = STI::Pusher::RefreshAttributes;
	sendRefreshEvent(refreshEvent);
}




bool STI_Device::setDeviceChannelName(short channel, std::string name)
{
	ChannelMap::iterator ch = channels.find(channel);

	if( ch != channels.end() )
	{
		ch->second.channelName = CORBA::string_dup(name.c_str());
		
		saveChannelNames();
		return true;
	}

	return false;
}

//*********** Timing event functions ****************//
bool STI_Device::readChannelDefault(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut, double minimumStartTime_ns)
{
//	DataMeasurement newMeasurement(minimumStartTime_ns, channel, 0);
	RawEvent rawEvent(minimumStartTime_ns, channel, valueIn, 0, true);
//	rawEvent.setMeasurement( &newMeasurement );
	bool success = playSingleEventDefault(rawEvent);
	if(success)
	{
	//	dataOut.setValue(newMeasurement.getMixedData());
		dataOut.setValue(rawEvent.getMeasurement()->getMixedData());
	}
	return success;
}

bool STI_Device::writeChannelDefault(unsigned short channel, const MixedValue& value, double minimumStartTime_ns)
{
	RawEvent rawEvent(minimumStartTime_ns, channel, value, 0, false);
	return playSingleEventDefault(rawEvent);
}	

bool STI_Device::playSingleEventDefault(const RawEvent& event)
{
	playSingleEventMutex->lock();
	{

	if(usingDefaultEventParsing)
	{
		//error: infinite recursion detected.  Only ONE of playSingleEventDefault or parseDeviceEventsDefault can be used in one device
		return false;
	}

	resetEvents();

	if(!changeStatus(EventsEmpty))
		return false;

	//RawEventMap rawEventsIn;
	//rawEventMap[event.time()].push_back( event );

	unsigned errorCount = 0;

	if(!addRawEvent(event, rawEvents, errorCount))
		return false;

	if(!parseEvents(rawEvents))
		return false;

	std::vector<STI::Server_Device::DeviceTimingSeqControl_var> partnerControls;
	
	if(!preparePartnerEvents(partnerControls))
		return false;

	loadEvents();
	
	if(!waitForStatus(EventsLoaded))
		return false;

//	if( !prepareToPlay() )
	if(!changeStatus(PreparingToPlay))
		return false;
	
	playEvents();

	for(unsigned i = 0; i < partnerControls.size(); i++)
	{
		partnerControls.at(i)->play();
	}

	if(!waitForStatus(EventsLoaded))
		if(!changeStatus(EventsLoaded))
			changeStatus(EventsEmpty);
	
	}
	playSingleEventMutex->unlock();
	
	return deviceStatusIs(EventsLoaded);
}



bool STI_Device::read(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return readChannel(channel, valueIn, dataOut);
}
bool STI_Device::read(const RawEvent& measurementEvent)
{
	MixedData data;
	if(readChannel(measurementEvent.channel(), measurementEvent.value(), data))
	{
		measurementEvent.getMeasurement()->setData(data);
		return true;
	}
	else
		return false;
}

bool STI_Device::write(unsigned short channel, const MixedValue& value)
{
	return writeChannel(channel, value);
}
bool STI_Device::write(const RawEvent& event)
{
	return writeChannel(event.channel(), event.value());
}







void STI_Device::resetEvents()
{
	measurements.clear();
	rawEvents.clear();
	synchedEvents.clear();

	PartnerDeviceMap::iterator it;

	for(it = partnerDevices.begin(); it != partnerDevices.end(); it++)
	{
		it->second->resetPartnerEvents();
	}
}

bool STI_Device::preparePartnerEvents(std::vector<STI::Server_Device::DeviceTimingSeqControl_var>& partnerControls)
{
	bool success = true;

	PartnerDeviceMap::iterator partner;
	for(partner = partnerDevices.begin(); success && partner != partnerDevices.end(); partner++)
	{
		success = partner->second->prepareEvents(partnerControls, std::string(getTDevice().deviceID));
	}

	return success;
}
bool STI_Device::transferEvents(const STI::Types::TDeviceEventSeq& events)
{
	unsigned i;
//	RawEventMap::iterator badEvent;
//	STI::Types::TMeasurement measurement;

	bool success = true;
//	bool errors = true;
	evtTransferErr.str("");

	unsigned errorCount = 0;	//limit the number of errors that are reported back during a single parse attempt
	unsigned maxErrors = 10;

	//This is only zero after resetting this device's parsed events.
//	unsigned initialEventNumber = rawEvents.size();
	unsigned initialEventNumber = 0;

	//Move the events from TDeviceEventSeq 'events' (provided by server) to
	//the raw event list 'rawEvents'.  Check for general event errors.
	for(i = 0; i < events.length(); i++)
	{
		success &= addRawEvent(RawEvent(events[i], initialEventNumber + i), rawEvents, errorCount, maxErrors);
		
		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing aborted after " << i 
				<< " of " << events.length() << " events.***" << endl;
			break;
		}
	}

	if( !success )
		return false;

	//All events were added successfully.  
	//Now check for device-specific conflicts and errors while parsing.

	return parseEvents(rawEvents);
}

bool STI_Device::addRawEvent(const RawEvent& rawEvent, RawEventMap& raw_events, unsigned& errorCount, unsigned maxErrors)
{
	bool success = true;
	unsigned j;

	double eventTime = rawEvent.time();

	//add event
	raw_events[eventTime].push_back( rawEvent );
	
	//check for multiple events on the same channel at the same time
	for(j = 0; j < raw_events[eventTime].size() - 1; j++)
	{
		//Has the current event's channel already being set?
		if(rawEvent.channel() == raw_events[eventTime].at(j).channel())
		{
			success = false;
			errorCount++;

			// channel name?  : << "('" << rawEvent. << "')"
			//Error: Multiple events scheduled on channel #24 at time 2.56:
			evtTransferErr << "Error: Multiple events scheduled on channel #" 
				<< rawEvent.channel() << " at time " 
				<< STI::Utils::printTimeFormated(eventTime) << ":" << endl
				<< "       Location: " << endl
				<< "       >>> " << raw_events[eventTime][j].file() << ", line " 
				<< raw_events[eventTime][j].line() << "." << endl
				<< "       >>> " << rawEvent.file() << ", line " 
				<< rawEvent.line() << "." << endl
				<< "       Event trace: " << endl
				<< "       " << raw_events[eventTime][j].print() << endl
				<< "       " << raw_events[eventTime].back().print() << endl;
		}
		if(errorCount > maxErrors)
			break;
	}
	
	//look for the newest event's channel number on this device
	ChannelMap::iterator channel = 
		channels.find(raw_events[eventTime].back().channel());
	
	//check that newest event's channel is defined
	if(channel == channels.end())
	{
		success = false;
		errorCount++;

		//Error: Channel #24 is not defined on this device. Event trace:
		evtTransferErr << "Error: Channel #" 
			<< raw_events[eventTime].back().channel()
			<< " is not defined on this device. "
			<< "       Location:" << endl
			<< "       >>> " << raw_events[eventTime].back().file() << ", line " 
			<< raw_events[eventTime].back().line() << "." << endl
			<< "       Event trace:" << endl
			<< "       " << raw_events[eventTime].back().print() << endl;
	}
	//check that the newest event is of the correct type for its channel
	else if(raw_events[eventTime].back().getSTItype() != channel->second.outputType)
	{
		if(raw_events[eventTime].back().isMeasurementEvent() && 
			raw_events[eventTime].back().getSTItype() == ValueString && channel->second.outputType == ValueNone)
		{
			//In this case, we assume that the measurement's value is actually its description, since a (separate) description was not parsed.
			raw_events[eventTime].back().getMeasurement()->setDescription(
				raw_events[eventTime].back().stringValue() );
			
			raw_events[eventTime].back().setValue( MixedValue() );	//makes this value Empty
		}
		else
		{
			success = false;
			errorCount++;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr 
				<< "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< TValueToStr(channel->second.outputType) << "'. " << endl
				<< "       Location:" << endl
				<< "       >>> " << raw_events[eventTime].back().file() << ", line " 
				<< raw_events[eventTime].back().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << raw_events[eventTime].back().print() << endl;
		}
	}
	if(success && raw_events[eventTime].back().isMeasurementEvent())	//measurement event
	{
		//give ownership of the measurement to the measurements ptr_vector.
		measurements.push_back( raw_events[eventTime].back().getMeasurement() );

		//**** This is done in the RawEvent constructor
		////Install any measurement callbacks
		//measurements.back().installMeasurementCallback(raw_events[eventTime].back().
		//raw_events[eventTime].back().getMeasurement()
	}

	return success;
}

bool STI_Device::parseEvents(RawEventMap& rawEvents)
{
	unsigned i;
	RawEventMap::iterator badEvent;
	STI::Types::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	unsigned errorCount = 0;	//limit the number of errors that are reported back during a single parse attempt
	unsigned maxErrors = 10;


	do {
		errors = false;	//Each time through the loop any offending events 
						//are removed before trying again. This way all events
						//can generate errors messages before returning.
		try {
			parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
		}
		catch(EventConflictException& eventConflict)
		{
			errorCount++;
			success = false;
			errors = true;
			//Error: Event conflict. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event conflict. "
				<< eventConflict.printMessage() << endl
				<< "       Location:" << endl
				<< "       >>> " << eventConflict.getEvent1().file() << ", line " 
				<< eventConflict.getEvent1().line() << "." << endl;

			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       >>> " << eventConflict.getEvent2().file() << ", line " 
				<< eventConflict.getEvent2().line() << "." << endl;
			}

			evtTransferErr
				<< "       Event trace:" << endl
				<< "       " << eventConflict.getEvent1().print() << endl;
			
			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       " << eventConflict.getEvent2().print() << endl;
			}
			
			//Add to list of conflicting events; this will be sent to the client
			conflictingEvents.insert(eventConflict.getEvent1().eventNum());
			conflictingEvents.insert(eventConflict.getEvent2().eventNum());

			//find the latest event associated with this exception
			badEvent = rawEvents.find( eventConflict.lastTime() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(EventParsingException& eventParsing)
		{
			errorCount++;
			success = false;
			errors = true;
			//Error: Event parsing error. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event parsing error. "
				<< eventParsing.printMessage() << endl
				<< "       Location:" << endl 
				<< "       >>> " << eventParsing.getEvent().file() << ", line " 
				<< eventParsing.getEvent().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << eventParsing.getEvent().print() << endl;
			
			//Add to list of unparseable events; this will be sent to the client
			unparseableEvents.insert(eventParsing.getEvent().eventNum());

			//find the event associated with this exception
			badEvent = rawEvents.find( eventParsing.getEvent().time() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );	
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(STI_Exception& exception)
		{
			errorCount++;
			success = false;
			errors = true;

			evtTransferErr 
				<< "Error: " << exception.printMessage() << endl;
				
			return false;		//break the error loop immediately
		}
		catch(...)	//generic conflict or error
		{
			errorCount++;
			success = false;
			//Error: Event error or conflict detected. Debug info not available.
			evtTransferErr 
				<< "Error: Event error or conflict detected. " << endl
				<< "       Debug info not available." << endl;

			errors = false;		//break the error loop immediately
		}
		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing device events aborted after " << errorCount 
				<< " errors." << endl;

			return false;		//break the error loop immediately

		}
		if(rawEvents.size() == 0)
			break;

	} while(errors);

	if( !success )
		return false;

	errorCount = 0;

	//Check that all measurements are associated with a SynchronousEvent
	for(i = 0; i < measurements.size(); i++)
	{
		if( !measurements.at(i).isScheduled() )
		{
			errorCount++;
			success = false;
			evtTransferErr << "Error: The following measurement is not associated with a SynchronousEvent." << endl;
			
			//find the original RawEvent
			for(unsigned k = 0; k < rawEvents[ measurements.at(i).time() ].size(); k++)
			{
				if(rawEvents[ measurements.at(i).time() ].at(k).eventNum() == measurements.at(i).eventNum())
				{
					//found the raw event
					evtTransferErr
					<<         "       Location:" << endl
					<<         "       >>> " <<  rawEvents[ measurements.at(i).time() ].at(k).file() << ", line " 
					<< rawEvents[ measurements.at(i).time() ].at(k).line() << "." << endl;
				}
			}
			evtTransferErr
				<<             "       Measurement trace:" << endl
				<<             "       " << measurements.at(i).print() << endl;
			evtTransferErr
				<<             "       Measurement description:" << endl
				<<             "       " << measurements.at(i).getDescription() << endl;
		}
		
		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing aborted after " << errorCount 
				<< " measurement schedule errors." << endl;

			break;		//break the error loop immediately
		}
	}

	//check that no measurement is scheduled for more than one SynchronousEvent
	unsigned numberScheduled = 0;
	for(i = 0; i < synchedEvents.size(); i++)
	{
		numberScheduled += synchedEvents.at(i).getNumberOfMeasurements();
	}
	
	if(numberScheduled != measurements.size() )
	{
		success = false;
		evtTransferErr << "Error: Measurement scheduling mismatch. Each DataMeasurement must be added" << endl
					   << "       to exactly one SynchronousEvent during parseDeviceEvents(...)." << endl
					   << "       Total Number of DataMeasurements: <<" << measurements.size() << ">>" << endl
					   << "       Number added to SynchronousEvents:  <<" << numberScheduled  << ">>" << endl;
	}

	//check that all event times are positive
	//shift all event times forward if they are negative? tell server the offset? include ping offset?

	//sort in time order
	synchedEvents.sort();

	int synchedEventsLength = static_cast<int>( synchedEvents.size() );
	
	//check that synchedEvents has only one entry for each time
	for(int k = 0; k < synchedEventsLength - 1; k++)
	{
		if( synchedEvents.at(k).getTime() == synchedEvents.at(k + 1).getTime() )
		{
			evtTransferErr 
			<< "Error: Multiple parsed events are scheduled " << endl
			<< "       to occur at the same time on device '" << getDeviceName() << "'." << endl
			<< "       Events that occur on multiple channels at the same time must be grouped" << endl
			<< "       into a single SynchonousEvent object during STI_Device::parseDeviceEvents(...)." << endl
			<< "       Only one SynchonousEvent is allowed at any time." << endl;

			success = false;
			break;
		}
	}

	//Assign event numbers
	for(i = 0; i < synchedEvents.size(); i++)
	{
		synchedEvents.at(i).setEventNumber( i );
	}

//	cout << "Event Transfer Error:" << endl << evtTransferErr.str() << endl;

	return success;
}


void STI_Device::loadEvents()
{
	if(loadEventsThread != 0)
	{
//		loadEventsThread->exit();
//		loadEventsThread = 0;
	}

	//Loading takes place in its own thread to allow for "double buffering".
	//Loading can continue to go on in the background while the first events run.
	//This also allows the calling function on the server to return quickly so
	//that the next device's event's can be loaded.
	
	//change load status to loading
	if( !changeStatus(EventsLoading) )
		return;

	omni_thread::create(loadDeviceEventsWrapper, (void*)this, 
		omni_thread::PRIORITY_HIGH);

}

void STI_Device::loadDeviceEventsWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);

	thisObject->loadDeviceEvents();
}


void STI_Device::loadDeviceEvents()
{
	for(unsigned i = 0; i < synchedEvents.size(); i++)
	{
		synchedEvents.at(i).setup();
		synchedEvents.at(i).waitBeforeLoad();
		synchedEvents.at(i).load();
	}
	changeStatus(EventsLoaded);

	//uInt32 waitTime = 0;
	//for(unsigned i = 0; i < synchedEvents.size(); i++)
	//{
	//	do {
	//		waitTime = synchedEvents.at(i).loadEvent();
	//		
	//		cout << "loadEvent() " << getTDevice().deviceID << ": " << synchedEvents.at(i).getTime() << endl;

	//		//sleep for waitTime
	//		if(waitTime > 0)
	//		{
	//			changeStatus(EventsLoaded);		//change load status
	//			loadEventsThread->sleep(static_cast<unsigned long>(waitTime));	//waitTime in seconds
	//		}

	//	} while(waitTime > 0);
	//}
}


bool STI_Device::prepareToPlay()
{
	bool success = false;

	int timeout = 5;	//5 attempts before timeout
	unsigned long secs, nsecs;

	if(changeStatus(PreparingToPlay))
	{
		while(!success && timeout > 0)
		{
			timeout--;
			omni_thread::yield();

			executingMutex->lock();
			{
				if(executing)
				{
					omni_thread::get_time(&secs, &nsecs, 1, 0);	//attempt every 1 seconds
					executingCondition->timedwait(secs, nsecs);
				}
				else
				{
					success = true;
					executionAllowed = false;
				}
			}
			executingMutex->unlock();
		}
	}

	if(success)
	{
		for(unsigned i = 0; i < synchedEvents.size(); i++)
		{
			synchedEvents.at(i).reset();
		}
	}

	return success;
}


void STI_Device::playEvents()
{

	if( deviceStatusIs(Paused) )
	{
		resume();
		return;
	}
	if( deviceStatusIs(Playing) )
	{
		return; //cannot play if already playing
	}

	if( !changeStatus(Playing) )
		return;

	//Playing takes place in its own thread because playEvents() must return promptly
	//to allow the server to call playEvents() on other devices.  This allows playing
	//to occur in parallel on all devices.
	playEventsThread = omni_thread::create(playDeviceEventsWrapper, (void*)this, 
		omni_thread::PRIORITY_HIGH);

	omni_thread::create(measureDataWrapper, (void*)this, 
		omni_thread::PRIORITY_HIGH);

}

void STI_Device::playDeviceEventsWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	thisObject->playDeviceEvents();
}
void STI_Device::measureDataWrapper(void* object)
{
	STI_Device* thisObject = static_cast<STI_Device*>(object);
	thisObject->measureData();
}
void STI_Device::playDeviceEvents()
{
	eventsArePlayed = false;

	time.reset();

	for(unsigned i = 0; i < synchedEvents.size(); i++)
	{
		if(!stopPlayback && !pausePlayback)
			synchedEvents.at(i).waitBeforePlay();

		if(pausePlayback)
		{
cout << "STI_Device Paused" << endl;
			devicePauseMutex->lock();
			{
				devicePauseCondition->wait();
			}
			devicePauseMutex->unlock();
cout << "STI_Device UNpaused" << endl;		
			synchedEvents.at(i).waitBeforePlay();	//this event is interrupted by the pause; resume by waiting for it again
		}

		if(stopPlayback)
			break;
		
		synchedEvents.at(i).play();

	}

	measureMutex->lock();
	{
		if(!eventsAreMeasured)
		{
			measureCondition->wait();
		}
	}
	measureMutex->unlock();

//	eventsArePlayed = true;

	if( !changeStatus(EventsLoaded) )
	{
		stop();
		changeStatus(EventsEmpty);
	}
	
//	std::cout << "Poll Counter = " << pollCounter << std::endl;

}



void  STI_Device::measureData()
{
	measuredEventNumber = 0;
	eventsAreMeasured = false;

	for(unsigned i = 0; i < synchedEvents.size() && !stopPlayback; i++)
	{
		if(!stopPlayback)
			synchedEvents.at(i).waitBeforeCollectData();
		
		if(stopPlayback)
			break;

		synchedEvents.at(i).collectData();
		measuredEventNumber = i;
	}

	measureMutex->lock();
	{
		eventsAreMeasured = true;
		measureCondition->signal();
	}
	measureMutex->unlock();

}

//void STI_Device::waitForEvent(unsigned eventNumber)
//{
//
//	Int64 wait = static_cast<Int64>( 
//			synchedEvents.at(eventNumber).getTime() - time.getCurrentTime() );
//
//	if(wait > 0 && !stopPlayback && !pausePlayback)
//	{
//
//		playEventsMutex->lock();
//		{
//			//calculate absolute time to wake up
//			omni_thread::get_time(&wait_s, &wait_ns, 
//				Clock::get_s(wait), Clock::get_ns(wait));
//			
//			playEventsTimer->timedwait(wait_s, wait_ns);	//put thread to sleep
//		}
//		playEventsMutex->unlock();
//	}
//}
Int64 STI_Device::getCurrentTime()
{
	return time.getCurrentTime();
}


unsigned STI_Device::getMeasuredEventNumber() const
{
	return measuredEventNumber;
}

void STI_Device::parseDeviceEventsDefault(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
{
	//This event parser works well for non time critical devices (i.e. non-FPGA devices) and devices that
	//are fundamentally serial. Events scheduled for the same time will instead be played sequentially
	//using calls to STI_Device::writeChannel(...).

	usingDefaultEventParsing = true;

	RawEventMap::const_iterator iter;
	unsigned i;

	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		eventsOut.push_back( 
			new STI_Device::PsuedoSynchronousEvent(iter->first, iter->second, this) );

		// register all measurement events
		for(i = 0; i < iter->second.size(); i++)
		{
			if( iter->second.at(i).isMeasurementEvent() )	// measurement event
				eventsOut.back().addMeasurement( iter->second.at(i) );
		}
	}
}




void STI_Device::pauseServer()
{
	bool success = false;
	try {
		ServerConfigureRef->pauseServer( getTDevice().deviceID );
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}

}

void STI_Device::unpauseServer()
{
	bool success = false;
	try {
		ServerConfigureRef->unpauseServer( getTDevice().deviceID );
		success = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}
}



//*********** SynchronousEvent subclass functions ****************//
void STI_Device::SynchronousEvent::addMeasurement(const RawEvent& measurementEvent)
{
	DataMeasurement* measurement = measurementEvent.getMeasurement();

	if( measurement != 0 )
	{
		eventMeasurements.push_back( measurement );
		measurement->setScheduleStatus(true);
	}
}



void STI_Device::SynchronousEvent::setup()
{
	setupDone = false;
	loaded = false;

	setupEvent();	//pure virtual

	statusMutex->lock();
	{
		setupDone = true;
		loadCondition->signal();
	}
	statusMutex->unlock();

}

void STI_Device::SynchronousEvent::load()
{
	if(loaded)
		return;

	loadEvent();	//pure virtual

	statusMutex->lock();
	{
		loaded = true;
		playCondition->signal();
	}
	statusMutex->unlock();
}

void STI_Device::SynchronousEvent::play()
{
	if(played)
		return;

	playEvent();	//pure virtual

	statusMutex->lock();
	{
		played = true;
		collectionCondition->signal();
	}
	statusMutex->unlock();
}

void STI_Device::SynchronousEvent::collectData()
{
	collectMeasurementData();	//pure virtual

//	performMeasurementCallbacks();
}

void STI_Device::SynchronousEvent::waitBeforeLoad()
{
	statusMutex->lock();
	{
		if(!setupDone)
			loadCondition->wait();
	}
	statusMutex->unlock();
}

void STI_Device::SynchronousEvent::waitBeforePlay()
{
	statusMutex->lock();
	{
		if(!loaded)
			playCondition->wait();
	}
	statusMutex->unlock();

	unsigned long wait_s;
	unsigned long wait_ns;

	statusMutex->lock();
	{
		Int64 wait = static_cast<Int64>(getTime()) - device_->getCurrentTime() ;
		
		if(wait > 0 && !played)
		{
			//calculate absolute time to wake up
			omni_thread::get_time(&wait_s, &wait_ns, 
				Clock::get_s(wait), Clock::get_ns(wait));

			playCondition->timedwait(wait_s, wait_ns);
		}
	}
	statusMutex->unlock();
}

void STI_Device::SynchronousEvent::waitBeforeCollectData()
{
	if(getNumberOfMeasurements() == 0)
		return;

	statusMutex->lock();
	{
		if(!played)
			collectionCondition->wait();
	}
	statusMutex->unlock();

//	cout << "waitBeforeCollectData() " << getEventNumber() << endl;
}

void STI_Device::SynchronousEvent::stop()
{
	statusMutex->lock();
	{
		setupDone = true;
		loaded = true;
		loadCondition->broadcast();
	}
	statusMutex->unlock();

	statusMutex->lock();
	{
		played = true;
		playCondition->broadcast();
	}
	statusMutex->unlock();

	statusMutex->lock();
	{
		collectionCondition->broadcast();
	}
	statusMutex->unlock();

}
void STI_Device::SynchronousEvent::reset()
{
	played = false;
	for(unsigned i = 0; i < eventMeasurements.size(); i++)
	{
		eventMeasurements.at(i)->clearData();
	}
}




void STI_Device::PsuedoSynchronousEvent::playEvent()
{
	//MixedValue* value = 0;
	//for(unsigned i = 0; i < events_.size(); i++) {
	//	getValue(events_.at(i), value);
	//	if(value != 0) {
	//		device_->write( events_.at(i).channel(), *value );
	//	}
	//}
	for(unsigned i = 0; i < events_.size(); i++) {
		device_->write( events_.at(i) );
	}
}

//void STI_Device::PsuedoSynchronousEvent::getValue(const RawEvent& evt, const MixedValue* value)
//{
//	//Look for any LinkedValue replacement values.
//	ChannelValueMap::iterator it = updatedValues.find(evt.channel());
//	if(it != updatedValues.end()) {
//		value = &(it->second);
//		return;
//	}
//
//	//Default value is the original event's value.
//	value = &(evt.value());
//}

void STI_Device::PsuedoSynchronousEvent::collectMeasurementData()
{
	for(unsigned i = 0; i < events_.size(); i++)
	{
		if( events_.at(i).isMeasurementEvent() && !events_.at(i).getMeasurement()->isMeasured() )
		{
			device_->read(events_.at(i));
		}
	}

	//for(unsigned i = 0; i < eventMeasurements.size(); i++)
	//{
	//	if( !eventMeasurements.at(i)->isMeasured() )
	//	{
	//		device_->readChannel( *( eventMeasurements.at(i) ) );
	//	}
	//}
}


//*********** State machine functions ****************//
bool STI_Device::waitForStatus(DeviceStatus status)
{
	if(stopWaiting)
		return (deviceStatus == status);

	bool wrongStatus = true;

	deviceStatusMutex->lock();
	{
		numberWaitingForStatus++;
	}
	deviceStatusMutex->unlock();

	while(wrongStatus && !stopWaiting)
	{
		deviceStatusMutex->lock();
		{
			wrongStatus = (deviceStatus != status);
			
			if(wrongStatus)
			{
				deviceStatusCondition->wait();
				wrongStatus = (deviceStatus != status);	//status changed; check new status
			}
		}
		deviceStatusMutex->unlock();
	}

	deviceStatusMutex->lock();
	{
		numberWaitingForStatus--;
	}
	deviceStatusMutex->unlock();

	return (deviceStatus == status);
}


bool STI_Device::changeStatus(DeviceStatus newStatus)
{
	bool allowedTransition = false;	

	deviceStatusMutex->lock();
	{

	switch(deviceStatus) 
	{
	case EventsEmpty:
		allowedTransition = 
			(newStatus == EventsLoading);
		break;
	case EventsLoading:
		allowedTransition = 
			(newStatus == EventsEmpty) ||
			(newStatus == EventsLoaded);
		break;
	case EventsLoaded:
		allowedTransition =
			(newStatus == EventsEmpty) ||
			(newStatus == EventsLoading) ||
			(newStatus == PreparingToPlay);
		break;
	case PreparingToPlay:
		allowedTransition =
			(newStatus == EventsLoaded) ||
			(newStatus == Playing) || 
			(newStatus == EventsEmpty);
		break;
	case Playing:
		allowedTransition = 
			(newStatus == Paused) ||
			(newStatus == EventsLoaded) || 
			(newStatus == EventsEmpty);
		break;
	case Paused:
		allowedTransition = 
			(newStatus == Playing) ||
			(newStatus == EventsLoaded) || 
			(newStatus == EventsEmpty);
		break;
	default:
		break;
	}

	allowedTransition |= (newStatus == deviceStatus); //same state is allowed

	if(allowedTransition)
	{
		deviceStatus = newStatus;
		updateState();
		deviceStatusCondition->broadcast();	//wake up all waitForStatus() holds
	}
	
	}
	deviceStatusMutex->unlock();
	
	return allowedTransition;
}

void STI_Device::updateState()
{
	switch(deviceStatus) 
	{
	case EventsEmpty:
//cout << "STI_Device State = EventsEmpty" << endl;
		stopPlayback = true;
		eventsAreLoaded = false;
		eventsArePlayed = true;
		eventsAreMeasured = true;
		pausePlayback = false;
		executionAllowed = true;
	
		deviceLoadingMutex->lock();
		{
			deviceLoadingCondition->broadcast();
		}
		deviceLoadingMutex->unlock();

		playEventsMutex->lock();
		{
			playEventsTimer->broadcast();	//wakes up the play thread if sleeping
		}
		playEventsMutex->unlock();
		
		measureMutex->lock();
		{
			measureCondition->broadcast();
		}
		measureMutex->unlock();

		deviceRunningMutex->lock();
		{
			deviceRunningCondition->broadcast();
		}
		deviceRunningMutex->unlock();

		devicePauseMutex->lock();
		{
			devicePauseCondition->broadcast();
		}
		devicePauseMutex->unlock();

		break;
	case EventsLoading:
		stopPlayback = true;
		eventsAreLoaded = false;
		eventsArePlayed = true;
		eventsAreMeasured = true;
		pausePlayback = false;
		break;
	case EventsLoaded:
//cout << "STI_Device State = EventsLoaded" << endl;
		stopPlayback = true;
		eventsAreLoaded = true;
		eventsAreMeasured = true;
		pausePlayback = false;
		eventsArePlayed = true;
		executionAllowed = true;
		
		deviceLoadingMutex->lock();
		{
			deviceLoadingCondition->broadcast();
		}
		deviceLoadingMutex->unlock();

		playEventsMutex->lock();
		{
			playEventsTimer->broadcast();	//wakes up the play thread if sleeping
		}
		playEventsMutex->unlock();
		
		measureMutex->lock();
		{
			measureCondition->broadcast();
		}
		measureMutex->unlock();

		deviceRunningMutex->lock();
		{
			deviceRunningCondition->broadcast();
		}
		deviceRunningMutex->unlock();

		devicePauseMutex->lock();
		{
			devicePauseCondition->broadcast();
		}
		devicePauseMutex->unlock();
		

		break;
	case PreparingToPlay:
		stopPlayback = true;
		eventsAreLoaded = true;
		eventsArePlayed = true;
		eventsAreMeasured = true;
		pausePlayback = false;
		executionAllowed = false;
		
		
		break;
	case Playing:
//cout << "STI_Device State = Playing" << endl;
		stopPlayback = false;
		eventsAreLoaded = true;
		eventsArePlayed = false;
		eventsAreMeasured = false;
		pausePlayback = false;
		executionAllowed = false;
		
		time.unpause();	//does nothing if not currently paused

		devicePauseMutex->lock();
		{
			devicePauseCondition->broadcast();
		}
		devicePauseMutex->unlock();
		break;
	case Paused:
//cout << "STI_Device State = Paused" << endl;
		stopPlayback = false;
		eventsAreLoaded = true;
		eventsArePlayed = false;
		pausePlayback = true;
		executionAllowed = false;
		
		time.pause();

		playEventsMutex->lock();
		{
			playEventsTimer->broadcast();
		}
		playEventsMutex->unlock();
	}
}




bool STI_Device::eventsLoaded()
{
	return eventsAreLoaded;
}

bool STI_Device::eventsPlayed()
{
	return eventsArePlayed;
}

bool STI_Device::running()
{
	return deviceStatusIs(Playing);
}

void STI_Device::stop()
{
	switch(deviceStatus) 
	{
	case EventsLoading:
		changeStatus(EventsEmpty);
		break;
	case Paused:
	case PreparingToPlay:
	case Playing:
		changeStatus(EventsLoaded);

		for(unsigned i = 0; i < synchedEvents.size(); i++)
		{
			synchedEvents.at(i).stop();
		}

		stopEventPlayback();	//pure virtual
		break;
	default:
		changeStatus(EventsEmpty);
		break;
	}

	//wake up all threads that are held in waitForStatus()
	while(numberWaitingForStatus > 0)
	{
		deviceStatusMutex->lock();
		{
			stopWaiting = true;
			deviceStatusCondition->broadcast();
		}
	deviceStatusMutex->unlock();
	}
	stopWaiting = false;
}


void STI_Device::pause()
{
	//if(pausePlayback)	//if already paused then resume
	//{
	//	resume();
	//}
	//else

	//pause
	if( changeStatus(Paused) )
	{
		pauseEventPlayback();	//pure virtual
	}
}

void STI_Device::resume()
{
cout << "STI_Device::resume()" << endl;
	if( changeStatus(Playing) )
	{
		resumeEventPlayback();	//pure virtual
	}
	else
	{
		if( !changeStatus(EventsLoaded) )
		{
			changeStatus(EventsEmpty);
		}
	}
}
bool STI_Device::deviceStatusIs(DeviceStatus status)
{
	bool equal = false;
	
	//This mutex lock makes sure that the device is not currenly changing it's status
	//during the inquiry.
	deviceStatusMutex->lock();
	{
		equal = (deviceStatus == status);
	}
	deviceStatusMutex->unlock();

	return equal;
}

//*********** Device setup helper functions ****************//
void STI_Device::addInputChannel(unsigned short Channel, TData InputType, TValue OutputType, std::string defaultName)
{
	addChannel(Channel, Input, InputType, OutputType, defaultName);
}

void STI_Device::addInputChannel(unsigned short Channel, TData InputType)
{
	addInputChannel(Channel, InputType, ValueNone, "");
}


void STI_Device::addInputChannel(unsigned short Channel, TData InputType, std::string defaultName)
{
	addInputChannel(Channel, InputType, ValueNone, defaultName);
}


void STI_Device::addOutputChannel(unsigned short Channel, TValue OutputType, std::string defaultName)
{
	addChannel(Channel, Output, DataNone, OutputType, defaultName);
}


bool STI_Device::addChannel(unsigned short Channel, TChannelType Type, 
							TData InputType, TValue OutputType, std::string defaultName)
{
	bool valid = true;
	STI::Types::TDeviceChannel tChannel;

	//if(Type == Input && OutputType != ValueNone)
	//{
	//	valid = false;
	//}
	if(Type == Output && InputType != DataNone)
	{
		valid = false;
	}

	//check for duplicate channel number
	ChannelMap::iterator duplicate = channels.find(Channel);

	if(duplicate != channels.end())
	{
		valid = false;

		cerr << "Error: Duplicate channel number in device " 
			<< getDeviceName() << endl;
	}

	if(valid)
	{
		tChannel.channel    = Channel;
		tChannel.type       = Type;
		tChannel.inputType  = InputType;
		tChannel.outputType = OutputType;
		tChannel.channelName = CORBA::string_dup(defaultName.c_str());

		channels[Channel] = tChannel;
	}
	else
	{
		cerr << "Error: Invalid channel specification in device " 
			<< getDeviceName() << endl;
	}

	return valid;
}



void STI_Device::installGraphicalParser(std::string parserJarPath)
{
	if( STI::Utils::fileExists(parserJarPath) ) {
		networkJarFile = new NetworkFileSource(parserJarPath);
	
		STI::Types::TFile file;

		file.description = CORBA::string_dup("");
		file.fileName = CORBA::string_dup(parserJarPath.c_str());
		file.fileServerAddress = CORBA::string_dup("");
		file.fileServerDirectory = CORBA::string_dup("");
		file.networkFile = networkJarFile->getNetworkFileReference();

		//Put the JAR file into the labeled data as a File
		MixedData graphicalParser;
	//	graphicalParser.addValue(GUIjavaclasspath);
		graphicalParser.addValue(file);
		setLabeledData("GraphicalParser", graphicalParser);
	}

}

void STI_Device::addAttributeUpdater(AttributeUpdater* updater)
{
	attributeUpdaters.push_back(updater);
}



bool STI_Device::addMutualPartnerDevice(string partnerName, string IP, short module, string deviceName)
{
	return addPartnerDevice(partnerName, IP, module, deviceName, true);
}

bool STI_Device::addPartnerDevice(string partnerName, string IP, short module, string deviceName)
{
	return addPartnerDevice(partnerName, IP, module, deviceName, false);
}

bool STI_Device::addPartnerDevice(string partnerName, string IP, short module, string deviceName, bool mutual)
{
	bool success = true;

	PartnerDeviceMap::iterator it = partnerDevices.find(partnerName);

	if( it == partnerDevices.end() )  //this is an original partnerName
	{
		partnerDevices.insert(partnerName, new PartnerDevice(partnerName, IP, module, deviceName, true, mutual) );
	}
	else if(it->second->getPartnerDeviceName().compare(deviceName) == 0 && 
		it->second->getIPAddress().compare(IP) == 0 &&
		it->second->getModuleNum() == module && 
		it->second->isRequired() && 
		it->second->isMutual() == mutual)
	{
		//same partner
	}
	else
	{
		success = false;
		cerr << "Error adding partner '" << partnerName 
			<< "'. This partner name is already in use." << endl;
	}

	return success;
}


void STI_Device::addLoggedMeasurement(unsigned short channel,   unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold, MixedValue& valueIn)
{
	ChannelMap::iterator existingChannel = channels.find(channel);

	if(existingChannel != channels.end())
	{
		dataLoggerServant->addLoggedMeasurement(channel, measureInterval, saveInterval, deviationThreshold, valueIn);
	}
	else
	{
		cerr << "Error: Unable to log measurement data for channel #" << channel << " on device " 
			<< getDeviceName() << ": Channel does not exist." << endl;
	}
}

void STI_Device::addLoggedMeasurement(std::string attributeKey, unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold)
{	
	AttributeMap::iterator existingAttribute = attributes.find(attributeKey);

	if(existingAttribute != attributes.end())
	{
		dataLoggerServant->addLoggedMeasurement(attributeKey, measureInterval, saveInterval, deviationThreshold);
	}
	else
	{
		cerr << "Error: Unable to log measurement data of attribute '" << attributeKey << "' on device " 
			<< getDeviceName() << ": Attribute does not exist." << endl;
	}
}

void STI_Device::addLocalPartnerDevice(std::string partnerName, const STI_Device& partnerDevice)
{
	//ment to be called from outside this STI_Device (i.e., in main.cpp )
	
	PartnerDeviceMap::iterator it = partnerDevices.find(partnerName);

	if( it == partnerDevices.end() )  //this is not an original partnerName
	{
		partnerDevices[partnerName] = PartnerDevice(partnerName, partnerDevice.getCommandLineServant(), true, false);
	}
	else
	{
		cerr << "Error adding partner '" << partnerName 
			<< "'. This partner name is already in use." << endl;
	}
}




std::string STI_Device::TValueToStr(STI::Types::TValue tValue)
{
	std::string result = "";
	switch(tValue)
	{
	case STI::Types::ValueNumber:
		result = "Number";
		break;
	case STI::Types::ValueString:
		result = "String";
		break;
	case STI::Types::ValueVector:
		result = "Vector";
		break;
	default:
		//this should never happen
		break;
	}
	return result;
}

//*********** Getter functions ****************//
const AttributeMap& STI_Device::getAttributes() const
{
	return attributes;
}

const ChannelMap& STI_Device::getChannels() const
{
	return channels;
}




/*
const std::map<std::string, std::string>& STI_Device::getRequiredPartners() const
{
	return requiredPartners;
}
const std::vector<std::string>& STI_Device::getMutualPartners() const
{
	return mutualPartners;
}
const std::vector<std::string>& STI_Device::getEventPartners() const
{
	return eventPartners;
}

const PartnerDeviceMap& STI_Device::getRegisteredPartners() const
{
	return commandLineServant->getRegisteredPartners();
}
*/
const STI::Types::TDevice& STI_Device::getTDevice() const
{
	return tDevice;
}
std::string STI_Device::getIP() const
{
	return std::string(tDevice->address);
}

unsigned short STI_Device::getModule() const
{
	return tDevice->moduleNum;
}

std::string STI_Device::getDeviceName() const
{
	return deviceName;
}


std::string STI_Device::getServerName() const
{
	if(serverConfigureFound)
		return serverName;
	else
		return "NOT FOUND";
}
DataMeasurementVector& STI_Device::getMeasurements()
{
	return measurements;
}


CommandLine_i* STI_Device::getCommandLineServant() const
{
	return commandLineServant;
}


STI_Device::SynchronousEventVector& STI_Device::getSynchronousEvents()
{
	return synchedEvents;
}
std::string STI_Device::dataTransferErrorMsg() const
{
	return dataTransferError.str();
}


std::string STI_Device::eventTransferErr() const
{
	return evtTransferErr.str();
}

std::vector<STI::Types::TDeviceEvent>& STI_Device::getPartnerEvents(std::string deviceID)
{
	return partnerDevice( getPartnerName(deviceID) ).getEvents();
}



PartnerDeviceMap& STI_Device::getPartnerDeviceMap()
{
	return partnerDevices;
}


std::string STI_Device::getPartnerDeviceID(std::string partnerName)
{
	PartnerDeviceMap::iterator it = partnerDevices.find(partnerName);

	if(it == partnerDevices.end())
	{
		for(it = partnerDevices.begin(); it != partnerDevices.end(); it++)
		{
			if(it->second->name().compare(partnerName) == 0)
				break;
		}
	}
	
	if(it != partnerDevices.end())
	{
		return it->second->getDeviceID();
	}
	else
	{
		return "";
	}
}

std::string STI_Device::getPartnerName(std::string deviceID)
{
	PartnerDeviceMap::iterator it = partnerDevices.find(deviceID);

	if(it == partnerDevices.end())
	{
		for(it = partnerDevices.begin(); it != partnerDevices.end(); it++)
		{
			if(it->second->getDeviceID().compare(deviceID) == 0)
				break;
		}
	}
	
	if(it != partnerDevices.end())
	{
		return it->second->name();
	}
	else
	{
		return "";
	}

}


bool STI_Device::hasLabeledData(std::string label)
{
	MixedDataMap::iterator it = labeledData.find(label);

	if(it != labeledData.end())
		return true;
	else
		return false;
}

MixedData STI_Device::getLabeledData(std::string label)
{
	MixedDataMap::iterator it = labeledData.find(label);

	if(it != labeledData.end()) {
		return it->second;
	}
	else {
		MixedData data;
		return data;
	}
}

void STI_Device::setLabeledData(std::string label, MixedData& data)
{
	labeledData[label] = data;
}



//STI_Device::DynamicSynchronousEvent::DynamicSynchronousEvent(double time, const RawEvent& sourceEvent, STI_Device* device)
//: STI_Device::SynchronousEvent(time, device)
//{
//	DynamicValue_ptr dynamicValue;
//
//	if(sourceEvent.getLinkedValue(dynamicValue)) {
//		dynamicValue->addLink(this);
//		dynamicValues.push_back(dynamicValue);
//		//device->addLinkedEvent(this);
//	}
//}

STI_Device::DynamicSynchronousEvent::DynamicSynchronousEvent(double time, const std::vector<RawEvent>& sourceEvents, STI_Device* device)
: STI_Device::SynchronousEvent(time, device)
{
	addSourceEvents(sourceEvents);
}

void STI_Device::DynamicSynchronousEvent::addSourceEvents(const std::vector<RawEvent>& sourceEvents)
{
	sourceEvents_l = &sourceEvents;

	DynamicValue_ptr dynamicValue;

	for(unsigned i = 0; i < sourceEvents_l->size(); i++) {
		
		if(sourceEvents_l->at(i).getDynamicValue(dynamicValue)) {
			dynamicValue->addLink(this);
			dynamicValues.push_back(dynamicValue);
		}
	}
}

STI_Device::DynamicSynchronousEvent::~DynamicSynchronousEvent()
{
	for(unsigned i = 0; i < dynamicValues.size(); i++) {
		if(dynamicValues.at(i) != 0) {
			dynamicValues.at(i)->unLink(this);
		}
	}
}

void STI_Device::DynamicSynchronousEvent::refresh(const DynamicValueEvent& evt)
{
	if(sourceEvents_l != 0) {
		updateValue(*sourceEvents_l);
	}

	setup();
	load();
}

