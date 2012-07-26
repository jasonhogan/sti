/*! \file STI_Device_Template.cpp
 *  \author Alex Sugarbaker
 *  \brief Source-file for the class arroyo
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



#include "arroyo.h"


arroyo::arroyo(ORBManager*    orb_manager, 
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
	rs232QuerySleep_ms = 100;
	rs232ReadLength = 35;

	//Parameter Initialization
	stringToValue(serialController->queryDevice("TEC:SET:T?"), temperatureSetPoint);
	stringToValue(serialController->queryDevice("LAS:OUT?"), laserOn);
	intensityLock = !(serialController->queryDevice("LAS:MODE?") == "ILBW\n"); //need to fix with string.compare?

	return;
}

arroyo::~arroyo()
{
	delete serialController;
}


bool arroyo::deviceMain(int argc, char **argv)
{
	refreshAttributes();
	
	return false;
}

void arroyo::defineAttributes() 
{
	addAttribute("Laser On/Off", (laserOn ? "On" : "Off"), "Off, On");
	addAttribute("Laser Mode", (intensityLock ? "Intensity Lock" : "Constant Current"), "Constant Current, Intensity Lock");
	addAttribute("Temperature Set Point", temperatureSetPoint);
}

void arroyo::refreshAttributes() 
{
	setAttribute("Laser On/Off", (laserOn ? "On" : "Off"));
	setAttribute("Laser Mode", (intensityLock ? "Intensity Lock" : "Constant Current"));
	setAttribute("Temperature Set Point", temperatureSetPoint);
}

bool arroyo::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	std::string commandString;
	bool commandSuccess;

	bool success = false;

	if(key.compare("Temperature Set Point") == 0 && successDouble) {
		commandString = "TEC:T " + valueToString(tempDouble);
		commandSuccess = stringToValue(value, temperatureSetPoint);
		std::cerr << commandString << std::endl;
		serialController->commandDevice(commandString);
		
		success = commandSuccess;
	} else if(key.compare("Laser On/Off") == 0) {
		if (value.compare("On") == 0) {
			commandString = "LAS:OUT 1";
			laserOn = true;
		} else {
			commandString = "LAS:OUT 0";
			laserOn = false;		
		}
		
		std::cerr << commandString << std::endl;
		serialController->commandDevice(commandString);
		
		success = true;
	} else if(key.compare("Laser Mode") == 0) {
		if (value.compare("Intensity Lock") == 0) {
			commandString = "LAS:MODE:IPD";
			intensityLock = true;
		} else {
			commandString = "LAS:MODE:ILBW";
			intensityLock = false;		
		}
		
		std::cerr << commandString << std::endl;
		serialController->commandDevice(commandString);
		
		std::cerr << "ERR?" << std::endl;
		std::cerr << serialController->queryDevice("ERR?");

		success = true;
	}

	return success;
}

void arroyo::defineChannels()
{
	addInputChannel(0, DataString);   //General Query
	addOutputChannel(1, ValueString); //General Command
	addOutputChannel(2, ValueString); //Laser power: "ON" or "OFF"
	addOutputChannel(3, ValueString); //Laser mode: "ConstCurr" or "ConstInt"
	addInputChannel(4, DataString);   //Read Laser Current
	addInputChannel(5, DataString);   //Read Photodiode Current
}

bool arroyo::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::string commandString;

	if(channel == 0)
	{
		// General Query
		commandString = valueIn.getString();
	}
	else if(channel == 4)
	{
		// Read Laser Current
		commandString = "LAS:I?";
	}
	else if(channel == 5)
	{
		// Read Photodiode Current
		commandString = "LAS:IPD?";
	}
	else
		return false;

	std::cerr << commandString << std::endl;
	dataOut = serialController->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);
//	std::cerr << dataOut << std::endl;

	return true;

}

bool arroyo::writeChannel(unsigned short channel, const MixedValue& value)
{
	std::string commandString;

	if(channel == 2)
	{
		// Turn laser power ON/OFF
		if(value.getString().compare("On")==0 || 
			value.getString().compare("ON")==0 || 
			value.getString().compare("on")==0)
			commandString = "LAS:OUT 1";
		else if(value.getString().compare("Off")==0 || 
			value.getString().compare("OFF")==0 || 
			value.getString().compare("off")==0)
			commandString = "LAS:OUT 0";
		else
			return false;
	}
	else if(channel == 3)
	{
		// Set Laser Mode "ConstCurr" or "ConstInt"
		if(value.getString().compare("ConstCurr")==0)
			commandString = "LAS:MODE:ILBW";
		else if(value.getString().compare("ConstInt")==0)
			commandString = "LAS:MODE:IPD";
		else
			return false;
	}
	else if(channel == 1)
	{
		// General Command
		commandString = value.getString();
	}
	else
		return false;

	std::cerr << commandString << std::endl;
	serialController->queryDevice(commandString);

	return true;

}

void arroyo::definePartnerDevices()
{
}

std::string arroyo::execute(int argc, char **argv)
{
// Problems: executes STI "isApplication" on startup, doesn't scroll in client?
/*
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		if (i < (args.size() - 1))
			command << " ";
	}

	std::cerr << command.str() << std::endl;
	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);

//	refreshAttributes();

	return result;
*/

	return "";
}

void arroyo::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void arroyo::stopEventPlayback()
{
}
