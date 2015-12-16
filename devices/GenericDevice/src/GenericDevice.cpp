/*! \file GenericDevice.cpp
 *  \author Brannon Klopfer
 *  \author David M.S. Johnson
 *  \author Susannah Dickerson
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>,
 *  Brannon Klopfer <bbklopfer@stanford.edu>
 *
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

#include "GenericDevice.hpp"
#include <iostream>

// I'm a printf debugging kind of dude. Redefine for quieter device.
//#define PRINTF_DEBUG
#ifdef PRINTF_DEBUG
#define DEBUGHERE cerr << __FUNCTION__ << " (" << __LINE__ << ")" << endl
#define IMPLEMENT cerr << "Implement (if needed): " <<  __FUNCTION__ << "() in " << __FILE__ << ":" << __LINE__ << endl
#define FIXME(feature) cerr << "FIXME: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define ADD_FEATURE(feature) cerr << "TODO: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define DEBUG(msg) cerr << __FUNCTION__ << "(): " << msg << endl
#else
#define DEBUGHERE // Do nothing if PRINTF_DEBUG isn't defined
#define IMPLEMENT
#define FIXME(feature)
#define ADD_FEATURE(feature)
#define DEBUG(msg)
#endif
using namespace std;

GenericDevice::GenericDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber,
                             string initScriptFn,
                             GenericDeviceConfig* xmlConfig,
                             string logDirectory,
                             string GCipAddress,
							 std::string comPort,
                             unsigned short GCmoduleNumber) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	DEBUG("GenericDevice instantiated");
	// TODO: Set name from config file.
	deviceConfig = xmlConfig;
	deviceType = deviceConfig->getDeviceType();
	serialController = NULL;
	viController = NULL;
	remoteChannels = NULL;
	initialized = false;

	if (deviceType == TYPE_GPIB) {
		// TODO: This is just copied from hp3458a.
		// Should modify to be more general, taking  values from config.
		gpibControllerIPAddress = GCipAddress;
		gpibControllerModule = GCmoduleNumber;
		primaryAddress = ModuleNumber;
		secondaryAddress = 0; // TODO: Is this the board index in MATLAB-speak? If so, set from config.
		string result;
		subprogramName = "";
		subprogramSet = false;
		hasTriggerPartner = false;
		trackReadTimeDelay = true;
		readTimeDelay = readTimeDefault = 0.000900; //default of 40 us per point, as acquired by many trials
		gpibCommDelay = 0.000025; // approximate time it takes to communicate with HP, given a single point to read back

		// TODO: Are we even allowed to do this? Also, no hardcoding!
		addPartnerDevice("gpibController", gpibControllerIPAddress, gpibControllerModule, "gpib");
	} else if (deviceType == TYPE_RS232) {
		// TODO: Should we allow for a single device to control multiple physical devices? For example, a rack of NHQ supplies is sort of a single unit...
		serialController  = new rs232Controller(deviceConfig->serialSettings->portStr,
		                                        deviceConfig->serialSettings->baudRate,
		                                        deviceConfig->serialSettings->dataBits,
		                                        deviceConfig->serialSettings->parity,
		                                        deviceConfig->serialSettings->stopBits);

		// TODO: Hacky. If keeping, read from file.
		rs232QuerySleep_ms = deviceConfig->serialSettings->querySleep;
	} else if (deviceType == TYPE_VISA) {
		viController = new visa32Controller(comPort);
	}

	initialized = true;

	DEBUG("End of GenericDevice constructor.");
}

GenericDevice::~GenericDevice()
{
	if (remoteChannels)
		delete remoteChannels;
	delete serialController;
	serialController = NULL;
	delete viController;
	viController = NULL;
}

bool GenericDevice::deviceMain(int argc, char** argv)
{
	vector<ConfigPartnerDevice> *pds = deviceConfig->partnerDevices;
	if (pds->size()) {
		DEBUG("Waiting for partners...");
		for (short i = 0; i < pds->size(); i++) { 
			while (!partnerDevice(pds->at(i).name).isRegistered()) {
				Sleep(100);
			}
		}
		DEBUG("Partners ready. Naming channels.");

		// Oh gosh, what an algorithmic abomination. Rethink this, please!
		map<short, RemoteChannel>::iterator it;
		for (it = remoteChannels->begin(); it != remoteChannels->end() ; it++) {
			short localChan = it->first;
			RemoteChannel rc = it->second;
			map<unsigned short, STI::Types::TDeviceChannel> chanMap;
			partnerDevice(rc.device).getChannels(chanMap);
			for (short i = 0; i < pds->size(); i++) {
				if (pds->at(i).name == rc.device) {
					DEBUG("Setting device channel name. Local: "
						<< localChan << ", Remote: "
						<< rc.device << ":"
						<< rc.channel << endl);
					stringstream ss;
					ss << chanMap.at(rc.channel).channelName;
					setDeviceChannelName(localChan, ss.str());
				}
			}
		}
	}

	return false;
}

// TODO: Refactor, decomp
string GenericDevice::queryDevice(std::string query)
{
	DEBUG("Query: '" << query << "'");
	std::string result;
	if (deviceType == TYPE_GPIB) {
		std::string queryString;
		char readType = deviceConfig->gpibSettings->readType + '0'; // Add '0' to convert int to char
		DEBUG("READ TYPE: " << readType);
		queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " " + readType;
		result = partnerDevice("gpibController").execute(queryString.c_str());
	} else if (deviceType == TYPE_RS232) {
		if (deviceConfig->serialSettings->charDelay) {
			result = serialController->queryDeviceSingleChar(query,
			                                                 rs232QuerySleep_ms,
			                                                 deviceConfig->serialSettings->charDelay,
			                                                 "\x0D\x0A", // TODO: from config
			                                                 deviceConfig->serialSettings->echo);
		} else {
			result = serialController->queryDevice(query, rs232QuerySleep_ms, 24);
		}
	} else if (deviceType == TYPE_VISA) { 
		result = viController->queryDevice(query, 100);
	} else cerr << "Warning: unknown device!" << endl;

	boost::trim(result);

	return result;
 }

bool GenericDevice::commandDevice(std::string command)
{
	DEBUG("Command: '" << command << "'");
	if (deviceType == TYPE_GPIB) {
		std::string commandString;
		std::string result;
		commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 0";

		result = partnerDevice("gpibController").execute(commandString.c_str());

		return !result.compare("1");
	} else if (deviceType == TYPE_RS232) {
		// TODO: rs232Controller should return success
		if (deviceConfig->serialSettings->charDelay)
			serialController->queryDeviceSingleChar(command,
			                                        rs232QuerySleep_ms,
			                                        deviceConfig->serialSettings->charDelay,
			                                        "\x0D\x0A", // TODO: from config
			                                        deviceConfig->serialSettings->echo);
		else
			serialController->commandDevice(command);

		return true; // Stab in the dark!
	} else if (deviceType == TYPE_VISA){
		viController->commandDevice(command);

		return true;
	}

	// TODO: Should this output to the error stream (per the tab in the client)?
	cerr << "Warning: unknown device!" << endl;
	return false;
}

string GenericDevice::getDeviceHelp()
{
	stringstream stringStream;

	stringStream << "Notes: Write this documentation.\n";

	return stringStream.str();
}

void GenericDevice::defineAttributes()
{
	GenericAttribute attribute;

	FIXME("Where should init script be executed?");
	ADD_FEATURE("Additional per-application init in .ini file");
	
	// TODO: Decomp?
	vector<Command> cmds = deviceConfig->initRoutine;
	for (XMLSize_t i = 0; i < cmds.size(); i++) {
		Command cmd = cmds.at(i);
		string cmdstr = cmd.cmdstr;
		commandDevice(cmdstr);
	}

	// Grab attributes from XML.
	// This whole setup should be cleaned up, with a better division
	// of labor between what's the XML parser's responsibility
	// and what's the device's responsibility.
	map<std::string, DevAttribute>* devAttrs = deviceConfig->devAttributes;
	map<string, DevAttribute>::iterator dit;

	for (dit = devAttrs->begin(); dit != devAttrs->end() ; dit++) {
		attribute.name = dit->second.name;

		// TODO: Down the road, we want get command and set command for attributes.
		attribute.getCommand = dit->second.getCommand;
		attribute.setCommand = dit->second.setCommand;
		// Add values:
		DEBUG("Allowed values for " << attribute.name << ": " << dit->second.values.size());
		for (int j = 0; j < dit->second.values.size(); j++) {
			attribute.choices[dit->second.values.at(j)]
				= dit->second.values.at(j);

			// FIXME: Currently just uses first value as default...
			if (!j) attribute.currentLabel = dit->second.values.at(j);
		}

		// Add attributes to mapping
		genAttributes[dit->second.name] = attribute;

		// Recycle attribute
		attribute.choices.clear();
		attribute.currentLabel.clear();
	}

	// Borrowed from hp3458a
	map<string, GenericAttribute>::iterator it;
	string choices;
	for(it = genAttributes.begin(); it != genAttributes.end(); it++)
	{
		map <string,string>::iterator mapIt;
		choices = "";
		for(mapIt = it->second.choices.begin(); mapIt != it->second.choices.end(); mapIt++)
		{
			if(mapIt != it->second.choices.begin()) choices += ", ";
			choices += mapIt->second;
		}

		string currentValue = queryDevice(it->second.getCommand.cmdstr);
		//deviceConfig->getAttributeDisplayStringFromValue(it->second.name, currentValue, &(it->second.currentLabel));
		deviceConfig->getAttributeLabel(it->second.name, currentValue, &(it->second.currentLabel));
		DEBUG("Adding attribute " << it->second.name << ", label " << it->second.currentLabel << ", choices '" << choices << "'");
		addAttribute(it->second.name, it->second.currentLabel, choices);
	}
}

void GenericDevice::refreshAttributes()
{
	string newValue;
	map<string, GenericAttribute>::iterator it;
	for(it = genAttributes.begin(); it != genAttributes.end(); it++) {
		newValue = queryDevice(it->second.getCommand.cmdstr);
		boost::trim(newValue); //remove trailing whitespace
	
		deviceConfig->getAttributeLabel(it->second.name,
		                                newValue,
		                                &(it->second.currentLabel));

		setAttribute(it->second.name, it->second.currentLabel);
	}
}

// Update (set) the given attribute
bool GenericDevice::updateAttribute(string key, string value)
{
	// Pretty hacky to have a seperate GenericAttribute and GenericDeviceConfig attribute...
	GenericAttribute attr;

	// Workaround for initializing arbitrary-input attributes
	if (value.empty()) return true;

	DEBUG("Setting attribute " << key << " to " << value);

	try {
		attr = genAttributes.at(key);

		// Quick hack (cf. conversation between Jason and Brannon)
		// If we're requesting a value which is already set, don't do anything.
		// I think there could be some problems with this, though.
		string correctLabel;
		deviceConfig->getAttributeLabel(attr.name, value, &correctLabel);
		if (attr.currentLabel == correctLabel) {
			DEBUG("Correct label, returning.");
			return true;
		}

	} catch (exception &e) {
		FIXME("Implement this!");
		
		return false;
	}
	// TODO: Implement get routine, set routine

	string setValue;
	deviceConfig->getAttributeValueFromDisplayString(attr.name, value, &setValue);
	DEBUG("setValue is " << setValue);
	MixedValue valueIn = MixedValue(setValue);
	string setcmd = GenericDeviceConfig::commandSub(attr.setCommand.cmdstr, valueIn);
	if (attr.setCommand.output) {
		queryDevice(setcmd);
	} else {
		commandDevice(setcmd);
	}
	// refreshAttribute() gets called, and deals with setting labels, etc.
	return true;
}


void GenericDevice::definePartnerDevices()
{
	vector<ConfigPartnerDevice> *pds = deviceConfig->partnerDevices;
	for(int i = 0; i < pds->size(); i++) {
		addPartnerDevice(pds->at(i).name,
		                 pds->at(i).addr,
						 pds->at(i).module,
						 pds->at(i).name);
	}
}


void GenericDevice::defineChannels()
{
	//definePartnerDevices();
	//cout << "Defined partner devices early!" << endl;
	// Direct channels
	unsigned short chan = 0;
	for (; chan < deviceConfig->numChannels(); chan++) {
		// FIXME: add write-only channels, which are actually output channels, e.g.,
		// addOutputChannel(i, ValueVector, deviceConfig->channels->at(i).name);
		string name = deviceConfig->channels->at(chan).name;
		// Hacky...overwrite the channel name with one from the ini, of form "Channel <channel name from XML> = new channel name"
		deviceConfig->getIniConfig()->getParameter("Channel " + name, name);
		addInputChannel(chan, DataVector, ValueVector, name);
	}

	if (chan) chan++;

	// Partner channels
	remoteChannels = new map<short, RemoteChannel>();
	vector<ConfigPartnerDevice> *pds = deviceConfig->partnerDevices;
	for(int i = 0; i < pds->size(); i++) {
		for (int j = 0; j < pds->at(i).channels.size(); j++) {
			RemoteChannel rc;
			rc.device = pds->at(i).name;
			rc.channel = pds->at(i).channels.at(j);
			remoteChannels->insert(std::pair<short, RemoteChannel> (chan, rc));
			addInputChannel(chan++, DataVector, ValueVector, "REMOTE CHANNEL");
		}
	}

	
}

// valueIn is passed from, e.g., GUI "Value" field.
// NB: Currently this returns a vector even for a single datum.
bool GenericDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	// Meta device channels
	if (remoteChannels && remoteChannels->size() && remoteChannels->count(channel)) {
		RemoteChannel rc = remoteChannels->at(channel);

		return partnerDevice(rc.device).read(rc.channel, valueIn, dataOut);
	}

	// Determine number of arguments. 0 for empty, n for vector, 1 for everything else
	int nargs = 1;
	if (valueIn.getType() == MixedValue::Vector)
		nargs = valueIn.getVector().size(); // If a vector, get vector size
	else if (valueIn.getType() == MixedValue::Empty)
		nargs = 0; // If empty

	if (channel > deviceConfig->numChannels()) return false;

	vector<Command>* cmds = deviceConfig->getCommands(channel, nargs);

	// TODO: Print to the error stream tab?
	if (!cmds) {
		cerr << "No routine with " << nargs << " arguments available!" << endl;

		// Client seems to hang otherwise Should return false...?
		return true;
	}

	for (XMLSize_t i = 0; i < cmds->size(); i++) {
		Command cmd = cmds->at(i);
		string cmdstr = GenericDeviceConfig::commandSub(cmd.cmdstr, valueIn);

		if (cmd.output && dataOut != NULL) {
			dataOut.addValue(queryDevice(cmdstr));
		} else {
			commandDevice(cmdstr);
		}
	}

	DEBUG("dataOut size: " << dataOut.getVector().size());

	return true;
}

bool GenericDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	return readChannel(channel, value, (MixedData)NULL);
}

void GenericDevice::sendTriggerEvents(double eventTime, const RawEvent& evt)
{
	FIXME("Borrowed; no idea if correct/needed for generic case!");
	//The HP triggers on the falling edge, and needs at least 250 ns for the trigger
	partnerDevice("Trigger").event(eventTime, 
					triggerChannel, triggerLow, evt);
	partnerDevice("Trigger").event(eventTime + 10000, 
					triggerChannel, triggerHigh, evt);
}

string GenericDevice::execute(int argc, char** argv)
{
	string tempArg;
	string msg;
	bool readData = false;

	if(argc < 1)
		return "Not enough arguments";

	for(int i = 1; i < argc; i++)
	{
		if(i > 1)
			msg += " ";
		tempArg = argv[i];
		msg += tempArg;
	}

	// TODO: be more intelligent about returning value.
	// This asumes an RS232 device will return something!
	if (!msg.compare(msg.size()-1,1,"?") || deviceType == TYPE_RS232)
		return queryDevice(msg);
	else if(commandDevice(msg))
		return "OK";

	return "Error";
}

bool GenericDevice::prepareDeviceToPlay()
{
	IMPLEMENT;

	return true;
}

bool GenericDevice::readFromDevice(string command, int readSize, MixedData & readings)
{
	bool success;

	if (deviceType == TYPE_GPIB) {
		// the final 3 tells the partner to read 'til termination.
		MixedValue inVector;
		inVector.addValue(primaryAddress);
		inVector.addValue(secondaryAddress);
		inVector.addValue(command);
		inVector.addValue(readSize);

		success = partnerDevice("gpibController").read(0, inVector, readings);
	} else if (deviceType == TYPE_RS232) {
		IMPLEMENT;
	}

	return success;
}

double GenericDevice::GenericReadEvent::getDoubleFromChars(vector<MixedData> &charVector, int pos, int numChars)
{
	FIXME("Borrowed; no idea if correct/needed for generic case!");
	char tempChar;
	double tempDouble, sign;
	vector <double> measurementVector;
	string::iterator it;
	int i, j, jStart;
	//Go from back to front, 'cause measurements are read from latest to earliest

	if (charVector.size() < (unsigned) (pos + numChars))
		return 0;

	tempDouble = 0;
	sign = 1;
	for (i = 0; i < numChars; i++)
	{ DEBUGHERE;
		if (charVector.at(pos + i).getType() != MixedData::Octet)
			return 0;

		tempChar = charVector.at(pos + i).getOctet();

		//get sign
		if (i == 0)
		{ DEBUGHERE;
			tempDouble += -1*((tempChar >> 7) & 1)*pow(2.0,numChars*8-1);
			jStart = 6;
		}
		else
			jStart = 7;

		// get most significant bits
		for (j = jStart; j >= 0; j--)
		{ DEBUGHERE;
			tempDouble += ((tempChar >> j) & 1)*pow(2.0, j + 8*(numChars - (i + 1)));	
		}
	}

	return tempDouble;
}
string GenericDevice::GenericAttribute::inverseFind(string value)
{
	FIXME("Borrowed; no idea if correct/needed for generic case!");
	map<string, string>::iterator iter;

	//Find string associated with flag
	for (iter = choices.begin(); iter != choices.end(); iter++ ){ DEBUGHERE;
		if (value.compare(iter->second) == 0){ DEBUGHERE;
			return iter->first;
		}
	}

	cerr << "Error in " << name << " selection: " << value << " not allowed." << endl;

	return "";
}

double GenericDevice::roundTo100ns(double n)
{
	// Or is this required?
	IMPLEMENT;

	double nBig, nRound;
	//rounds to nearest 100 ns, given a number in seconds (aper is min)
	nBig = n*10000000;

	nRound = nBig < 0.0 ? ceil(nBig - 0.5) : floor(nBig + 0.5);

	return nRound/10000000;
}
void GenericDevice::rememberReadTime(double newReadTime, int numPoints)
{
	// This required?
	FIXME("Borrowed; no idea if correct/needed for generic case!");
	//weight towards large data sets
	readTimeDelay = (readTimeDelay + numPoints*newReadTime/1000)/(1 + numPoints/1000);
	cerr << "New read time is " << readTimeDelay << " s" << endl;
}

/* To implement (right?): */
void GenericDevice::defineGpibAttributes()
{
	IMPLEMENT;
}

void GenericDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void GenericDevice::parseInputVector(const vector <MixedValue> &valueIn, vector <double> &parsedValues, const RawEvent& evt) throw(exception)
{
	IMPLEMENT;
}

void GenericDevice::GenericInitEvent::playEvent()
{
	IMPLEMENT;
}

void GenericDevice::GenericReadEvent::playEvent()
{
	IMPLEMENT;
}

void GenericDevice::GenericReadEvent::collectMeasurementData()
{
	IMPLEMENT;
}