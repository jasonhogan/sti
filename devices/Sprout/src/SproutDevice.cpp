/*! \file SproutDevice.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class SproutDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
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

#include "SproutDevice.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


SproutDevice::SproutDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort));

	rs232QuerySleep_ms = 200;
}


void SproutDevice::defineAttributes()
{
	addAttribute("Mode", getMode(), "Off, On, Idle, Interlock, Warmup, Config, AutoDetect");
}

void SproutDevice::refreshAttributes()
{
	std::cout << "Mode: " << getMode() << std::endl;
	setAttribute("Mode", getMode());
}

bool SproutDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;

	if(key.compare("Mode") == 0)
	{
		success = changeMode(value);
	}

	return success;
}

std::string SproutDevice::execute(int argc, char* argv[])
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
	}

	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);

	refreshAttributes();

	return result;
}

std::string SproutDevice::getMode()
{
	std::string result = serialController->queryDevice("OPMODE?", rs232QuerySleep_ms);
	string::size_type equalPos = result.find_first_of("=");
	std::string mode = result.substr(equalPos + 1);

	//Convert to expected capitalization of attribute name
	std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
	std::string::iterator stopIter = mode.begin();
	stopIter++;
	std::transform(mode.begin(), stopIter, mode.begin(), ::toupper);
	
	//if(newMode.at(newMode.size() - 1) == '\b')
	mode = mode.substr(0, mode.size() - 1);

	return mode;
}

bool SproutDevice::changeMode(std::string value)
{
	bool success = false;
	std::string result;

	if( value.compare("AutoDetect") == 0 )
	{
		//result = serialController->queryDevice("OPMODE?", rs232QuerySleep_ms);
		//string::size_type equalPos = result.find_first_of("=");
		//std::string newMode = result.substr(equalPos + 1);

		////Convert to expected capitalization of attribute name
		//std::transform(newMode.begin(), newMode.end(), newMode.begin(), ::tolower);
		//std::string::iterator stopIter = newMode.begin();
		//stopIter++;
		//std::transform(newMode.begin(), stopIter, newMode.begin(), ::toupper);
		//
		////if(newMode.at(newMode.size() - 1) == '\b')
		//newMode = newMode.substr(0, newMode.size() - 1);



		//for(unsigned i = 0; i < newMode.size(); i++)
		//{
		//	std::cout << "{" << (char)newMode.at(i) << "}" << std::endl;
		//}
		//std::cout << "Equal? " << ((newMode.compare("Off")==0) ? "Yes" : "No") << std::endl;

		success = true;
		refreshAttributes();
//		success = setAttribute("Mode", getMode());
	}
	else
	{
		result = serialController->queryDevice("OPMODE=" + value, rs232QuerySleep_ms);
		string::size_type zeroPos = result.find_first_of("0");	//sprout returns 0 on success
		string::size_type twoPos = result.find_first_of("2");	//and 2 on failure
//		cout << "mode change response: " << result << std::endl;
		success = (zeroPos != std::string::npos && twoPos == std::string::npos);
	}

	return success;
}
//
//void makeFirstLetterCaps(std::string& word)
//{
//	std::transform(word.begin(), word.end(), word.begin(), ::tolower);
//	std::transform(word.begin(), word.begin()++, word.begin(), ::toupper);
//
//	//for(unsigned i = 0; i < word.size(); i++)
//	//{
//	//	result.at(i) = tolower(result.at(i))
//	//}
//}