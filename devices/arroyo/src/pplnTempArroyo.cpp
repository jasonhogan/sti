/*! \file STI_Device_Template.cpp
 *  \author Alex Sugarbaker
 *  \brief Source-file for the class pplnTempArroyo
 *  \section license License
 *
 *  Copyright (C) 2012 Alex Sugarbaker <sugarbak@stanford.edu>
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *	This structure derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>, David Johnson <david.m.johnson@stanford.edu>
 *  and Susannah Dickerson <sdickers@stanford.edu>
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



#include "pplnTempArroyo.h"


pplnTempArroyo::pplnTempArroyo(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	serialController  = new rs232Controller("COM" + valueToString(comPort), 38400, 8, "None", 1);

	//Check that we can talk to the device
	if (!serialController->initialized)
	{
		initialized = false;
		return;
	}

	//RS232 Settings
	rs232QuerySleep_ms = 200;
	rs232ReadLength = 35;

	//Attribute Initialization
	iLim = serialController->queryDevice("TEC:LIM:ITE?", rs232QuerySleep_ms, rs232ReadLength);
	tLimHi = serialController->queryDevice("TEC:LIM:THI?", rs232QuerySleep_ms, rs232ReadLength);
	tLimLo = serialController->queryDevice("TEC:LIM:TLO?", rs232QuerySleep_ms, rs232ReadLength);
	pidGains = serialController->queryDevice("TEC:PID?", rs232QuerySleep_ms, rs232ReadLength);
	sensorCoef = serialController->queryDevice("TEC:CONST?", rs232QuerySleep_ms, rs232ReadLength);

	return;
}

pplnTempArroyo::~pplnTempArroyo()
{
	delete serialController;
}


bool pplnTempArroyo::deviceMain(int argc, char **argv)
{
	refreshAttributes();
	
	return false;
}

// Increase temp by 0.01degC times stepSize
// Assumes Arroyo's internal TEC:STEP is 1
void pplnTempArroyo::incTemp(unsigned short stepSize)
{
	std::string commandString;
	commandString = "TEC:INC " + valueToString(stepSize);

	std::cerr << commandString << std::endl;
	serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);
}

// Decrease temp by 0.01degC times stepSize
// Assumes Arroyo's internal TEC:STEP is 1
void pplnTempArroyo::decTemp(unsigned short stepSize)
{
	std::string commandString;
	commandString = "TEC:DEC " + valueToString(stepSize);

	std::cerr << commandString << std::endl;
	serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);
}

void pplnTempArroyo::defineAttributes() 
{
	addAttribute("Current Limit", iLim);
	addAttribute("Temperature High Limit", tLimHi);
	addAttribute("Temperature Low Limit", tLimLo);
	addAttribute("PID Gains", pidGains);
	addAttribute("Sensor Coefficients", sensorCoef);
}

void pplnTempArroyo::refreshAttributes() 
{
	iLim = serialController->queryDevice("TEC:LIM:ITE?", rs232QuerySleep_ms, rs232ReadLength);
	tLimHi = serialController->queryDevice("TEC:LIM:THI?", rs232QuerySleep_ms, rs232ReadLength);
	tLimLo = serialController->queryDevice("TEC:LIM:TLO?", rs232QuerySleep_ms, rs232ReadLength);
	pidGains = serialController->queryDevice("TEC:PID?", rs232QuerySleep_ms, rs232ReadLength);
	sensorCoef = serialController->queryDevice("TEC:CONST?", rs232QuerySleep_ms, rs232ReadLength);

	setAttribute("Current Limit", iLim);
	setAttribute("Temperature High Limit", tLimHi);
	setAttribute("Temperature Low Limit", tLimLo);
	setAttribute("PID Gains", pidGains);
	setAttribute("Sensor Coefficients", sensorCoef);
}

bool pplnTempArroyo::updateAttribute(string key, string value)
{
	return true;
}

void pplnTempArroyo::defineChannels()
{
	addInputChannel(0, DataString, "General Query");
	addOutputChannel(1, ValueString, "General Command");
	addInputChannel(2, DataDouble, "Read Temperature");
	addOutputChannel(3, ValueNumber, "Write Temperature");
}

bool pplnTempArroyo::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::string commandString;
	bool success = false;
	std::string result;
	double numValue;

	if(channel == 0)
	{
		// General Query
		commandString = valueIn.getString();
		std::cerr << commandString << std::endl;
		dataOut = serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);
	}
	else if(channel == 2)
	{
		// Read Temperature
		commandString = "TEC:T?";
		std::cerr << commandString << std::endl;
		result = serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);
		success = STI::Utils::stringToValue(result, numValue);

		if(success) {
			dataOut.setValue(numValue);		//in percent
		}
	}
	else
		return false;

	return true;

}

bool pplnTempArroyo::writeChannel(unsigned short channel, const MixedValue& value)
{
	std::string commandString;

	if(channel == 1)
	{
		// General Command
		commandString = value.getString();
	}
	else if(channel == 3)
	{
		// Write Temperature
		commandString = "TEC:T " + valueToString(value.getNumber());
	}
	else
		return false;

	std::cerr << commandString << std::endl;
	serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);

	return true;

}

void pplnTempArroyo::definePartnerDevices()
{
}

std::string pplnTempArroyo::execute(int argc, char* argv[])
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		
		if(i == (args.size() - 1))
			command;	//terminator at end of command
		else
			command << " ";		//spaces between command words
	}

	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms, rs232ReadLength);

	return result;
}

void pplnTempArroyo::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void pplnTempArroyo::stopEventPlayback()
{
}
