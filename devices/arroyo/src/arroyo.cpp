/*! \file STI_Device_Template.cpp
 *  \author Alex Sugarbaker
 *  \brief Source-file for the class arroyo
 *  \section license License
 *
 *  Copyright (C) 2013 Alex Sugarbaker <sugarbak@stanford.edu>
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

// Contructor for a controller of both current and temp (ComboSource)
arroyo::arroyo(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	serConCurrent  = new rs232Controller("COM" + valueToString(comPort), 38400, 8, "None", 1);
	serConTemprtr  = serConCurrent;

	//Check that we can talk to the device
	if (!serConCurrent->initialized)
	{
		initialized = false;
		return;
	}

	//Check that we can talk to the device
	if (!serConTemprtr->initialized)
	{
		initialized = false;
		return;
	}

	//RS232 Settings
	initializeRS232values();

	//Parameter Initialization (based on device state at executable run)
	getAttributesFromDevice();

	return;
}

// Contructor for split current/temp controllers (ComboSource)
arroyo::arroyo(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, unsigned short comPortCurrent, unsigned short comPortTemprtr) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	serConCurrent  = new rs232Controller("COM" + valueToString(comPortCurrent), 38400, 8, "None", 1);
	serConTemprtr  = new rs232Controller("COM" + valueToString(comPortTemprtr), 38400, 8, "None", 1);

	//Check that we can talk to the device
	if (!serConCurrent->initialized)
	{
		initialized = false;
		return;
	}

	//Check that we can talk to the device
	if (!serConTemprtr->initialized)
	{
		initialized = false;
		return;
	}

	//RS232 Settings
	initializeRS232values();

	//Parameter Initialization (based on device state at executable run)
	getAttributesFromDevice();

	return;
}

arroyo::~arroyo()
{
	delete serConCurrent;
	delete serConTemprtr;
}

void arroyo::initializeRS232values()
{
	rs232QuerySleep_ms = 100;
	rs232ReadLength = 35;
	waitAfterTurnLaserOn_s = 4;

	return;
}

bool arroyo::deviceMain(int argc, char **argv)
{
	refreshAttributes();
	
	return false;
}

void arroyo::getAttributesFromDevice()
{
	std::string stringTemp;
	stringToValue(serConTemprtr->queryDevice("TEC:SET:T?", rs232QuerySleep_ms, rs232ReadLength), temperatureSetPoint);
	stringToValue(serConCurrent->queryDevice("LAS:LIM:I?", rs232QuerySleep_ms, rs232ReadLength), currentLimit);
	stringToValue(serConCurrent->queryDevice("LAS:OUT?", rs232QuerySleep_ms, rs232ReadLength), laserOn);
	stringTemp = serConCurrent->queryDevice("LAS:MODE?", rs232QuerySleep_ms, rs232ReadLength);
	boost::trim(stringTemp);
	intensityLock = (stringTemp.compare("MDI") == 0);

	return;
}

void arroyo::defineAttributes() 
{
	addAttribute("Laser On/Off", (laserOn ? "On" : "Off"), "Off, On");
	addAttribute("Laser Mode", (intensityLock ? "Intensity Lock" : "Constant Current"), "Constant Current, Intensity Lock");
	addAttribute("Temperature Set Point (degC)", temperatureSetPoint);
	addAttribute("Current Limit (mA)", currentLimit);
}

void arroyo::refreshAttributes() 
{
	// Updates all attributes based on current device state
	getAttributesFromDevice();
	
	setAttribute("Laser On/Off", (laserOn ? "On" : "Off"));
	setAttribute("Laser Mode", (intensityLock ? "Intensity Lock" : "Constant Current"));
	setAttribute("Temperature Set Point (degC)", temperatureSetPoint);
	setAttribute("Current Limit (mA)", currentLimit);
}

bool arroyo::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	std::string commandString;
	bool commandSuccess;

	bool success = false;

	if(key.compare("Temperature Set Point (degC)") == 0 && successDouble) {
		//Issue command only if setpoint changed
		if(tempDouble != temperatureSetPoint) {
		commandString = "TEC:T " + valueToString(tempDouble);
		commandSuccess = stringToValue(value, temperatureSetPoint);
		std::cerr << commandString << std::endl;
		serConTemprtr->commandDevice(commandString);
		} else {
			commandSuccess = true;
		}
		success = commandSuccess;
	} else if(key.compare("Current Limit (mA)") == 0 && successDouble) {
		//Issue command only if limit changed
		if(tempDouble != currentLimit) {
		commandString = "LAS:LIM:I " + valueToString(tempDouble);
		commandSuccess = stringToValue(value, currentLimit);
		std::cerr << commandString << std::endl;
		serConCurrent->commandDevice(commandString);
		} else {
			commandSuccess = true;
		}
		success = commandSuccess;
	} else if(key.compare("Laser On/Off") == 0) {
		//Issue command only if state changes
		bool changingState;

		if (value.compare("On") == 0) {
			commandString = "LAS:OUT 1";
			changingState = !laserOn;
			laserOn = true;
		} else {
			commandString = "LAS:OUT 0";
			changingState = laserOn;
			laserOn = false;		
		}
		
		if(changingState) {
			std::cerr << commandString << std::endl;
			serConCurrent->commandDevice(commandString);
		}

		if(laserOn && changingState) {
			boost::this_thread::sleep(boost::posix_time::seconds(waitAfterTurnLaserOn_s));
		}

		success = true;
	} else if(key.compare("Laser Mode") == 0) {
		bool changingMode;

		if (value.compare("Intensity Lock") == 0) {
			commandString = "LAS:MODE:IPD";
			changingMode = !intensityLock;
			intensityLock = true;
		} else {
			commandString = "LAS:MODE:ILBW";
			changingMode = intensityLock;
			intensityLock = false;		
		}
		
		// Turn laser off (only if on and mode changing)
		// Change mode (only if changing mode)
		// Return laser to on (only if was on and mode changing)

		if(laserOn && changingMode)
		{
		std::cerr << "LAS:OUT 0" << std::endl;
		serConCurrent->commandDevice("LAS:OUT 0");
		}

		if(changingMode)
		{
		std::cerr << commandString << std::endl;
		serConCurrent->commandDevice(commandString);
		}

		if(laserOn && changingMode)
		{
		std::cerr << "LAS:OUT 1" << std::endl;
		serConCurrent->commandDevice("LAS:OUT 1");
		boost::this_thread::sleep(boost::posix_time::seconds(waitAfterTurnLaserOn_s));
		}

		success = true;
	}

	return success;
}

void arroyo::defineChannels()
{
	addInputChannel(0, DataString, "General Read");   //General Query
	addOutputChannel(1, ValueString, "General Write"); //General Command
	addOutputChannel(2, ValueNumber, "Set Laser Current (mA)");
	addOutputChannel(3, ValueNumber, "Set PD Current (uA)");
	addInputChannel(4, DataDouble, "Read Laser Current (mA)");
	addInputChannel(5, DataDouble, "Read PD Current (uA)");
}

bool arroyo::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::string commandString;
	//By default, commands go to the current controller
	rs232Controller* serConGeneral = serConCurrent;

	if(channel == 0)
	{
		// General Query
		commandString = valueIn.getString();

		//sends to TEC controller if starts with "TEC"
		if(boost::istarts_with(commandString,"TEC")) {
			serConGeneral = serConTemprtr;
		}
		commandString = cleanCommand(commandString);
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
	dataOut = serConGeneral->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);

	//To push attributes, refresh after general query
	if(channel == 0)
	{
		refreshDeviceAttributes();
	}

	return true;

}

bool arroyo::writeChannel(unsigned short channel, const MixedValue& value)
{
	std::string commandString;
	rs232Controller* serConGeneral = serConCurrent;

	if(channel == 2)
	{
		// Set current (mA)
		commandString = "LAS:I " + valueToString(value.getNumber());
	}
	else if(channel == 3)
	{
		// Set photodiode current (uA)
		commandString = "LAS:IPD " + valueToString(value.getNumber());
	}
	else if(channel == 1)
	{
		// General Command
		commandString = value.getString();
		
		//sends to TEC controller if starts with "TEC"
		if(boost::istarts_with(commandString,"TEC")) {
			serConGeneral = serConTemprtr;
		}
		commandString = cleanCommand(commandString);
	}
	else
		return false;

	std::cerr << commandString << std::endl;
	serConGeneral->commandDevice(commandString);

	//To push attributes, refresh after general command
	if(channel == 1)
	{
		refreshDeviceAttributes();
	}

	return true;

}

void arroyo::definePartnerDevices()
{
}

std::string arroyo::execute(int argc, char **argv)
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);
	std::string commandString;

	std::stringstream command;
	rs232Controller* serConGeneral = serConCurrent;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		
		if(i == (args.size() - 1))
			command;	//terminator at end of command
		else
			command << " ";		//spaces between command words
	}

	commandString = command.str();

	//sends to TEC controller if starts with "TEC"
	if(boost::istarts_with(commandString,"TEC")) {
		serConGeneral = serConTemprtr;
	}
	commandString = cleanCommand(commandString);

	std::cerr << commandString << std::endl;
	std::string result = serConGeneral->queryDevice(commandString, rs232QuerySleep_ms, rs232ReadLength);

	//To push attributes, refresh
	refreshDeviceAttributes();

	return result;
}

//Removes the extra "***!" that may have been used to force direction of command
std::string arroyo::cleanCommand(std::string commandString)
{
	if(boost::istarts_with(commandString,"LAS!")) {
		boost::ierase_first(commandString,"LAS!");
	}
	if(boost::istarts_with(commandString,"LASE!")) {
		boost::ierase_first(commandString,"LASE!");
	}
	if(boost::istarts_with(commandString,"LASER!")) {
		boost::ierase_first(commandString,"LASER!");
	}
	if(boost::istarts_with(commandString,"TEC!")) {
		boost::ierase_first(commandString,"TEC!");
	}

	return commandString;
}

void arroyo::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void arroyo::stopEventPlayback()
{
}

std::string arroyo::getDeviceHelp() 
{ 
	std::stringstream help;
	help <<	
		"Device controls both temperature and current for a single Arroyo Instruments powered laser.\n\n" <<
		"Has attributes for Current Limit (mA), Laser Mode, Laser On/Off, and Temperature Set Point (degC).\n" <<
		"Has channels for Set/Read Current (mA) and Set/Read PD Current (uA).\n\n" <<
		"Has additional channels for generic RS232 queries and commands, and RS232 commands are also\n" <<
		"piped through execue.  For example \"LAS:I 500\" or \"TEC:T?\" just as in the Arroyo Computer\n" <<
		"Interfacing Manual.  Since some of these general commands could change the attributes, attributes\n" <<
		"are refreshed after each general command/query/execute.\n\n"
		"The device can also run with split current/temperature controllers, rather than a ComboSource.\n" <<
		"In this case, two COM ports are used, and the device intelligently routes RS232 traffic based on\n" <<
		"the first characters in the command (true for general read/write and execute commands).  Commands\n" <<
		"starting with \"TEC\" are sent to the temperature controller.  All other traffic is sent to the\n" <<
		"laser current controller.\n\n" <<
		"In addition, it is possible to force command routing by adding \"TEC!\" or \"LAS!\" to the\n" <<
		"beginning of the command.  These prepended characters are removed.  For example \"TEC:T?\"\n" <<
		"and \"TEC!TEC:T?\" are both valid ways to query the temperature.  \"LAS!BEEP\" and \"TEC!BEEP\"\n" <<
		"will beep the appropriate Arroyo units, and \"BEEP\" will beep just the laser.\n" <<
		"\n";
	return help.str();

}