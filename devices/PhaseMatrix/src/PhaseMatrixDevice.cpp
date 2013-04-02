/*! \file PhaseMatrixDevice.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class PhaseMatrixDevice
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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

#include "PhaseMatrixDevice.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <regex>


std::string PhaseMatrixDevice::trash = "";

using STI::Utils::valueToString;

PhaseMatrixDevice::PhaseMatrixDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 115200, 8, "None", 1);

	rs232QuerySleep_ms = 200;

	listStartTimeHoldoff = 200000000;	//200 ms

	currentFreqHz = 0;
	currentPower = 0;

	triggerMode = ListTrig;
}


void PhaseMatrixDevice::defineAttributes()
{
	addAttribute("RF Output", "On" , "On, Off");
	addAttribute("Reference Source", "External" , "Internal, External");
	addAttribute("Blanking Mode", "On" , "On, Off");
	addAttribute("Triggering Mode", "List Trig" , "List Trig, List Point Trig");
	addAttribute("RS232 sleep time", 200);
}

void PhaseMatrixDevice::refreshAttributes()
{
	std::string result;

	//Check RF Output status
	result = serialController->queryDevice("OUTP:STAT?\n", rs232QuerySleep_ms);

	if (result.find("ON") != std::string::npos)
		setAttribute("RF Output", "On");
	else
		setAttribute("RF Output", "Off");

	//Check Reference Source
	result = serialController->queryDevice("ROSC:SOUR?\n", rs232QuerySleep_ms);

	if (result.find("EXT") != std::string::npos)
		setAttribute("Reference Source", "External");
	else
		setAttribute("Reference Source", "Internal");

	//Check Blanking Mode
	result = serialController->queryDevice("OUTP:BLAN?\n", rs232QuerySleep_ms);
	
	if (result.find("ON") != std::string::npos)
		setAttribute("Blanking Mode", "On");
	else
		setAttribute("Blanking Mode", "Off");
}

bool PhaseMatrixDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	std::string result;
	
	if( key.compare("RS232 sleep time") == 0 ) {
		int newRS232;
		if( STI::Utils::stringToValue(value, newRS232) && newRS232 > 0 ) {
			rs232QuerySleep_ms = newRS232;
			success = true;
		}
	}

	if( key.compare("RF Output") == 0 ) {
		if( value.compare("On") == 0 ) {
			result = serialController->queryDevice("OUTP:STAT ON\n", rs232QuerySleep_ms);
			success = true;
		}
		else if( value.compare("Off") == 0 ) {
			result = serialController->queryDevice("OUTP:STAT OFF\n", rs232QuerySleep_ms);
			success = true;
		}
	}

	if( key.compare("Reference Source") == 0 ) {
		if( value.compare("Internal") == 0 ) {
			result = serialController->queryDevice("ROSC:SOUR INT\n", rs232QuerySleep_ms);
			success = true;
		}
		else if( value.compare("External") == 0 ) {
			result = serialController->queryDevice("ROSC:SOUR EXT\n", rs232QuerySleep_ms);
			success = true;
		}
	}

	if( key.compare("Blanking Mode") == 0 ) {
		if( value.compare("On") == 0 ) {
			result = serialController->queryDevice("OUTP:BLAN ON\n", rs232QuerySleep_ms);
			success = true;
		}
		else if( value.compare("Off") == 0 ) {
			result = serialController->queryDevice("OUTP:BLAN OFF\n", rs232QuerySleep_ms);
			success = true;
		}
	}

	if( key.compare("Triggering Mode") == 0 ) {
		if( value.compare("List Trig") == 0 ) {
			triggerMode = ListTrig;
			success = true;
		}
		else if( value.compare("List Point Trig") == 0 ) {
			triggerMode = ListPointTrig;
			success = true;
		}
	}

	return success;
}

std::string PhaseMatrixDevice::execute(int argc, char* argv[])
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		
		if(i == (args.size() - 1))
			command << "\n";	//terminator at end of command
		else
			command << " ";		//spaces between command words
	}

	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);

	return result;
}


void PhaseMatrixDevice::defineChannels()
{
	addOutputChannel(0, ValueString, "Set Frequency");
	addOutputChannel(1, ValueString, "Set Power");
	
	addInputChannel(2, DataDouble, "Read Frequency");
	addInputChannel(3, DataDouble, "Read Power");

	addOutputChannel(4, ValueVector, "List Command Entry");

}

bool PhaseMatrixDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	bool success = false;

	if(channel == 2) {	//Read Frequency
		std::stringstream command;
		double frequency;

		success = measureFrequency(frequency);

		if(success) {
			dataOut.setValue(frequency / 1000);		//Phase Matrix returns milli Hz; return value in Hz
			currentFreqHz = frequency / 1000;
		}
	}

	if(channel == 3) {	//Read Power
		double power;

		success = measurePower(power);

		if(success) {
			dataOut.setValue(power);
			currentPower = power;
		}
	}

	return success;
}

bool PhaseMatrixDevice::measureFrequency(double& frequency)
{
	std::stringstream command;

	command << "FREQ?\n";
	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
	return STI::Utils::stringToValue(result, frequency);
}

bool PhaseMatrixDevice::measurePower(double& power)
{
	std::stringstream command;

	command << "POW?\n";
	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
	return STI::Utils::stringToValue(result, power);
}

bool PhaseMatrixDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	bool success = false;
	std::stringstream command;

	if(channel == 0) {	//Set Frequency
		std::string freq = value.getString();

		if( checkFrequencyFormat(freq) ) {
			command << "FREQ " << freq << "\n";

			std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
			success = true;
		}
	}

	if(channel == 1) {	//Set Power
		std::string pow = value.getString();

		if( checkPowerFormat(pow) ) {
			command << "POW " << pow << "\n";

			serialController->queryDevice(command.str(), rs232QuerySleep_ms);
			success = true;
		}
	}

	return success;
}



void PhaseMatrixDevice::parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut)
{
	//Channels 0, 1, 2, 3 are "soft timing" -- they are triggered by the computer on OS time scales.
	//Events on these channels can be added separately (if there are no conflicts) using parseDeviceEventsDefault.
	RawEventMap softEvents;

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	double listStartTime = 0;
	unsigned long listPointNum = 0;

//	double eventTime = 0;
//	double previousTime = 0;
	double lastDwell = 0;

	const unsigned short listChannel = 4;

	//Sets currentPower and currentFreqHz to the current output power and frequency; 
	//these are by default in the list point entry if unspecified.
	measureFrequency(currentFreqHz);
	measurePower(currentPower);

	clearList();

	//The Phase Matrix seems to queue external triggers.
	//Run a couple of LIST:START 1 to clear the trigger buffer.
	startList();
//	startList();

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The Phase Matrix cannot currently have multiple events at the same time.");
		}

		if(events->second.at(0).channel() == listChannel) {
			//New list point edge; add the previous point
			listPointNum++;
			if(listPointNum > 1) {
				lastDwell = events->second.at(0).time() - lastListEvent->second.at(0).time();

				//add the previous point using the calculated dwell time
				addListPoint(listPointNum - 1, lastDwell, lastListEvent->second.at(0));
			}
			else {
				//first list point
				listStartTime = events->first;

				if(listStartTime < listStartTimeHoldoff) {
					throw EventParsingException(events->second.at(0),
					"There must be " + valueToString(listStartTimeHoldoff / 1000000) + " ms at the beginning of the timing file before the first list point.");
				}
			}
			lastListEvent = events;
		}
		else {
			//use soft timing for this event
			softEvents[events->first].push_back( RawEvent(events->second.at(0) ) );
		}
	}
	
	if(listPointNum > 0) {
		//Add final list point with arbitrary dwell time of 10 us
		addListPoint(listPointNum, 10000, lastListEvent->second.at(0));

		//Setup List Mode

		//Command format:  LIST:SETUP followed by the following:
		//1) Dwell time in us, ms, s
		//(from 5us to 4,294 s (~1hr)) ,
		//default - us
		//2) # of times to run list (1 to
		//32767), 0 - infinite
		//3) Trigger:
		//0 – Software Trig
		//1 – List Trig
		//2 – List Point Trig
		//4) Direction:
		//0 – Lo to Hi
		//1 – Hi to Lo
		//2 – Up & Down


		//Using a dwell time of 0, which means use the dwell times defined by the individual list point commands.
		//Run the list 1 time.
		//Use the trigger mode define by the attribute (enum value is 0, 1, or 2)
		//Run Lo to Hi
		serialController->queryDevice("LIST:SETUP 0,1," + valueToString(triggerMode) + ",0\n", 
			rs232QuerySleep_ms);

//		startList();

		eventsOut.push_back(
			new PhaseMatrixListEvent(listStartTime - listStartTimeHoldoff, this) );
	}

	//add soft events
	parseDeviceEventsDefault(softEvents, eventsOut);
}
void PhaseMatrixDevice::clearList()
{
	serialController->queryDevice("LIST:STOP\n", rs232QuerySleep_ms);
	serialController->queryDevice("LIST:ERAS\n", rs232QuerySleep_ms);
}

void PhaseMatrixDevice::PhaseMatrixListEvent::playEvent()
{
	device_->startList();
}

void PhaseMatrixDevice::startList()
{
	//Starts the list playback. Waits for external trigger before actually playing.
	serialController->queryDevice("LIST:START 1\n", rs232QuerySleep_ms);
}

void PhaseMatrixDevice::addListPoint(unsigned long point, double dwell, const RawEvent& listEvent)
{
	std::stringstream command;

	const double minimumDwell = 5000.0;	//5 us
	const double maximumDwell = (4294.0 * 1000000000);	//4294 s

	//default value
	double frequencyMHz = currentFreqHz / 1000000;	//should never need this (should be invalid syntax)
	double power = currentPower;		//used when power is not specified

	//Check syntax and parameter ranges
	if(point > 32767) {
		throw EventParsingException(listEvent,
		"Phase Matrix list memory overflow. Only 32,767 points are allowed in the list.");
	}
	if(dwell < minimumDwell) {
		throw EventParsingException(listEvent,
			"Invalid dwell time " + valueToString(dwell / 1000) 
			+ " us for list point #" + valueToString(point) 
			+ ". Minimum dwell time is 5 us.");
	}
	if(dwell > maximumDwell) {
		throw EventParsingException(listEvent,
			"Invalid dwell time " + valueToString(dwell / 1000000000) 
			+ " s for list point #" + valueToString(point) 
			+ ". Maximum dwell time is 4294 s.");
	}

	//Rounding dwell time to the nearest 5 us.
	unsigned long dwell_us = 5 * static_cast<unsigned long>(dwell / 5000);

	//Determine the format type; check syntax
	bool isVector = (listEvent.value().getType() == MixedValue::Vector && listEvent.value().getVector().size() == 2);
	if(isVector) {
		//check vector entry format
		isVector &= (listEvent.value().getVector().at(0).getType() == MixedValue::Double 
			|| listEvent.value().getVector().at(0).getType() == MixedValue::Int) 
			&& (listEvent.value().getVector().at(1).getType() == MixedValue::Double 
			|| listEvent.value().getVector().at(1).getType() == MixedValue::Int);
	}
	bool isFrequency = (listEvent.value().getType() == MixedValue::Double 
		|| listEvent.value().getType() == MixedValue::Int);

	if( (!isVector && !isFrequency) || (isVector && isFrequency)) {
		//invalid format
		throw EventParsingException(listEvent,
			"Invalid list value format. Value must be either a frequency number or a vector of the form (frequency, power).");
	}

	//Extract point parameters
	if(isVector) {
		frequencyMHz = listEvent.value().getVector().at(0).getNumber();
		power = listEvent.value().getVector().at(1).getNumber();
		currentPower = power;	//Uses the last specified power value for the rest of the list.
	}
	if(isFrequency) {
		frequencyMHz = listEvent.value().getVector().at(0).getNumber();
	}

	//check parameter ranges
	if(frequencyMHz > 10000 || frequencyMHz < 500) {
		throw EventParsingException(listEvent,
			"Invalid frequency: " + valueToString(frequencyMHz) 
			+ " MHz. Allowed frequency range is 500 MHz - 10 GHz.");
	}

	command << "LIST:PVEC " 
		<< valueToString(point) << "," 
		<< valueToString(frequencyMHz) << "MHz" << "," 
		<< valueToString(power) << "dBm" << ","
		<< valueToString(dwell_us) << "us" << ","
		<< "OFF,ON"
		<< "\n";
	
	serialController->queryDevice(command.str(), rs232QuerySleep_ms);
}

bool PhaseMatrixDevice::checkFrequencyFormat(std::string frequency, std::string& formatErrorMessage)
{
	std::vector<std::tr1::regex> units;
	units.push_back(std::tr1::regex("GHz", std::tr1::regex_constants::icase));
	units.push_back(std::tr1::regex("MHz", std::tr1::regex_constants::icase));
	units.push_back(std::tr1::regex("KHz", std::tr1::regex_constants::icase));
	units.push_back(std::tr1::regex("mlHz", std::tr1::regex_constants::icase));


	for(unsigned i = 0; i < units.size(); i++) {
		if (std::tr1::regex_match(frequency, units.at(i))) {
			break;
		}
	}

	
		//	unsigned found = frequency.find("GHz");
	
//	if (found!=std::string::npos)
  //  std::cout << "first 'needle' found at: " << found << '\n';
	return true;
}

bool PhaseMatrixDevice::checkPowerFormat(std::string power, std::string& formatErrorMessage)
{
	return true;
}


std::string PhaseMatrixDevice::getDeviceHelp() 
{ 
	std::stringstream help;
	help <<	
		"Channels: \n\n" <<
		"Channel 0:  Set Frequency. Requires a string consisting of the new frequency\n" <<
		"value and the units, with no space. Allowed units are GHz, MHz, KHz, mlHz (default).\n" <<
		"For example:  '3.52GHz'\n" <<
		"\n" <<
		"Channel 1:  Set Power in dBm. Requires a string of the form +/- XX.X [DBM]\n" <<
		"\n" <<
		"Channel 2:  Read the current frequency in units of Hz\n" <<
		"\n" <<
		"Channel 3:  Read the current power in units of dBm\n" <<
		"\n" <<
		"Channel 4:  Add a point to the triggered list.\n" <<
		"The value must be one of the following forms:\n" << 
		"(1) Frequency in units of MHz. In this case the output power will remain at the current power.\n" << 
		"(2) A vector of the form (Frequency, Power) in units of (MHz, dBm). For example,\n" << 
		"the vector (6834.15, 15.3) will set the frequency to 6834.15 MHz and power to 15.3 dBm.\n" << 
		"\n" <<
		"Note: When adding points to the list, the dwell time at each point is automatically\n" << 
		"computed using the spacing between list events defined in the timing file.\n" << 
		"\n";

	return help.str();

}


