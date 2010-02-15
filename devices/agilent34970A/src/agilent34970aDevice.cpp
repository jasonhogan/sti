/*! \file agilent34970aDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class agilent34970aDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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



#include "agilent34970aDevice.h"

agilent34970aDevice::agilent34970aDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; // normally 16
	secondaryAddress = 0;
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	dmmEnabled = true;
	allowChanges = false;
	activeChannel = 301; //ranges from 301-322.
	upperChannel = 320; //321 & 322 are thermocouple channels
	lowerChannel = 301;
	numberOfChannels = upperChannel - lowerChannel + 1;

	for( unsigned int i = 0; i < numberOfChannels; i++ )
		MuxChannels.push_back(muxChannel());
}

agilent34970aDevice::~agilent34970aDevice()
{
}


void agilent34970aDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("DMM Enabled", "True", "True, False");
	addAttribute("Allow Calibration Changes", "False", "True, False");
	addAttribute("Active Channel", activeChannel);
	addAttribute("Active Channel Name", MuxChannels.at(activeChannel-lowerChannel).channelName);
	addAttribute("Active Channel Expected Value", MuxChannels.at(activeChannel-lowerChannel).expectedValue);
	addAttribute("Active Channel Measured Value", MuxChannels.at(activeChannel-lowerChannel).mostRecentMeasuredValue);
}

void agilent34970aDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //response to the IDN? query
	setAttribute("DMM Enabled", (dmmEnabled ? "True" : "False"));
	setAttribute("Allow Calibration Changes", (allowChanges ? "True" : "False"));
	setAttribute("Active Channel", activeChannel);
	setAttribute("Active Channel Name", MuxChannels.at(activeChannel-lowerChannel).channelName);
	setAttribute("Active Channel Expected Value", MuxChannels.at(activeChannel-lowerChannel).expectedValue);
	setAttribute("Active Channel Measured Value", MuxChannels.at(activeChannel-lowerChannel).mostRecentMeasuredValue);
}

bool agilent34970aDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	std::string tempString;
	double tempValue = 0;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;
	std::string result;
	std::string commandString;

	if(key.compare("GPIB ID") == 0)
	{
		tempString = queryDevice("*idn?");
		if(tempString.compare("") == 0)
			success = false;
		else
		{
			success = true;
			gpibID = tempString;
		}
		std::cerr << std::endl << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("DMM Enabled") == 0)
	{
		if(value.compare("True") == 0)
			commandSuccess = commandDevice("INSTRUMENT:DMM ON");
		else
			commandSuccess = commandDevice("INSTRUMENT:DMM OFF");
		
		if(commandSuccess)
		{
			success = stringToValue(queryDevice("INSTRUMENT:DMM?"), tempValue);
			if(tempValue == 1)
				dmmEnabled = true;
			else if(tempValue == 0)
				dmmEnabled = false;
			else
			{
				success = false;
				dmmEnabled = false;
			}
		}

		std::cerr << std::endl << "DMM Status: " << tempValue << std::endl;
	}
	else if(key.compare("Allow Calibration Changes") == 0)
	{
		if(value.compare("True") == 0)
		{
			success = true;
			allowChanges = true;
		}
		else if(value.compare("False") == 0)
		{
			success = true;
			allowChanges = false;
		}
	}
	else if(key.compare("Active Channel") == 0)
	{
		if(successDouble)
		{
			uInt32 tempChannel = (uInt32)(tempDouble);
			if((tempChannel <= upperChannel) && (tempChannel >= lowerChannel))
				activeChannel = tempChannel;	
			else
				std::cerr << std::endl << "Please choose a channel between 301 & 322." << std::endl;
		}
		
		success = true;
	}
	else if(key.compare("Active Channel Name") == 0)
	{
		if(allowChanges)
			MuxChannels.at(activeChannel-lowerChannel).channelName = value;

		success = true;
	}
	else if(key.compare("Active Channel Expected Value") == 0)
	{
		if(allowChanges)
			MuxChannels.at(activeChannel-lowerChannel).expectedValue = tempDouble;
		success = true;
	}
	else if(key.compare("Active Channel Measured Value") == 0)
	{
		commandString = "MEAS:VOLT:DC? (@" + valueToString(activeChannel) + ")";
		result = queryDevice(commandString);

		successDouble = stringToValue(result, tempDouble);

		std::cerr << std::endl << "Channel " << activeChannel << ": " << result << std::endl;

		if(successDouble)
		{
			MuxChannels.at(activeChannel-lowerChannel).mostRecentMeasuredValue = tempDouble;
			success = true;
		}

	}


	return success;
}

void agilent34970aDevice::defineChannels()
{
	for( unsigned int i = 0; i < numberOfChannels; i++ )
		addInputChannel(i, DataDouble);
	
}


bool agilent34970aDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	double tempMeasurement;

	uInt32 measureChannel = static_cast<uInt32>(channel) + 301;

	std::string commandString = "MEAS:VOLT:DC? (@" + valueToString(measureChannel) + ")";

	bool measureSuccess = stringToValue(queryDevice(commandString),tempMeasurement);
	if(measureSuccess)
	{
		dataOut.setValue(tempMeasurement);
	}
	return measureSuccess;
}

void agilent34970aDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}
void agilent34970aDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "li-gpib.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void agilent34970aDevice::stopEventPlayback()
{
}

std::string agilent34970aDevice::execute(int argc, char **argv)
{
	unsigned short channel; 
	std::string result;
	
	//command comes as "channelToRead"
	if(argc == 2)
	{
		bool channelSuccess = stringToValue(argv[1], channel);
	//	DataMeasurement DataMeasurement(1, channel, 0);
		MixedData data;
		read(channel, 0, data);
		result = valueToString(data.getNumber());
	}
	else
		result = ""; //command needs to contain only 1 piece of information

	return result;
}
bool agilent34970aDevice::deviceMain(int argc, char **argv)
{
	return false;
}
std::string agilent34970aDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool agilent34970aDevice::commandDevice(std::string command)
{
	std::string commandString;
	std::string result;
	commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 0";

	result = partnerDevice("gpibController").execute(commandString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	if(result.compare("1")==0)
		return true;
	else
		return false;
}

agilent34970aDevice::muxChannel::muxChannel()
{
	//set the default values for a muxChannel
	channelName = "I haven't been named yet";
	expectedValue = 5; //in Volts
	mostRecentMeasuredValue = 0; //in Volts
}