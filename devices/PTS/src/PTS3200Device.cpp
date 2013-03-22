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


PhaseMatrixDevice::PhaseMatrixDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 115200, 8, "None", 1);

	rs232QuerySleep_ms = 200;
}


void PhaseMatrixDevice::defineAttributes()
{
	addAttribute("RF Output", "On" , "On, Off");
	addAttribute("Reference Source", "External" , "Internal, External");
	addAttribute("Blanking Mode", "On" , "On, Off");
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

//	cout << "updateAttribute(" << key << ", " << value << ") ? " << success << endl;

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

	addOutputChannel(4, ValueVector, "List Entry");

}

bool PhaseMatrixDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	bool success = false;
	std::stringstream command;

	if(channel == 2) {	//Read Frequency
		double frequency;

		command << "FREQ?\n";
		std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
		success = STI::Utils::stringToValue(result, frequency);

		if(success) {
			dataOut.setValue(frequency / 1000);		//Phase Matrix returns milli Hz; return value in Hz
		}
	}

	if(channel == 3) {	//Read Power
		double power;

		command << "POW?\n";
		std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
		success = STI::Utils::stringToValue(result, power);

		if(success) {
			dataOut.setValue(power);
		}
	}

	return success;
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
		"Channel 4:  Add a point to the list. (not implemented yet)\n" <<
		"\n";

	return help.str();

}
