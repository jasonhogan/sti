/*! \file PicomotorsDevice.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class PicomotorsDevice
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

#include "PicomotorsDevice.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>


PicomotorsDevice::PicomotorsDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 19200);

	rs232QuerySleep_ms = 200;
	
	readMotorParameters();
}


void PicomotorsDevice::defineAttributes()
{
	for(unsigned i = 0; i < 3; i++)
		addAttribute("Acceleration "+ STI::Utils::valueToString(i), motorAcceleration[i]);
	
	for(unsigned i = 0; i < 3; i++)
		addAttribute("Velocity "+ STI::Utils::valueToString(i), motorVelocity[i]);
}

void PicomotorsDevice::setMotorAcceleration(unsigned i, unsigned acceleration)
{
	if(i > 2 || acceleration > 32000)
		return;
	
	serialController->queryDevice("ACC a1 " 
		+ STI::Utils::valueToString(i) 
		+ "=" + STI::Utils::valueToString(acceleration), rs232QuerySleep_ms );
}
void PicomotorsDevice::setMotorVelocity(unsigned i, unsigned velocity)
{
	if(i > 2 || velocity > 2000)
		return;
	
	serialController->queryDevice("VEL a1 " 
		+ STI::Utils::valueToString(i) 
		+ "=" + STI::Utils::valueToString(velocity), rs232QuerySleep_ms );
}

void PicomotorsDevice::refreshMotorVelocity(unsigned i)
{
	if(i > 2)
		return;
	
	std::string response;
	int velocity;
	bool success = false;

	response = serialController->queryDevice("VEL a1 " + STI::Utils::valueToString(i), rs232QuerySleep_ms );
	success = STI::Utils::stringToValue(response.substr(response.find_last_of("=") + 1), velocity);
	motorVelocity[i] = (success ? velocity : 2000);
//	std::cout << motorVelocity[i] << std::endl;
}

void PicomotorsDevice::refreshMotorAcceleration(unsigned i)
{
	if(i > 2)
		return;
	std::string response;
	int acceleration;
	bool success = false;
	
	response = serialController->queryDevice("ACC a1 " + STI::Utils::valueToString(i), rs232QuerySleep_ms );
	success = STI::Utils::stringToValue(response.substr(response.find_last_of("=") + 1), acceleration);
	motorAcceleration[i] = (success ? acceleration : 32000);
//	std::cout << motorAcceleration[i] << std::endl;
}

void PicomotorsDevice::readMotorParameters()
{
	for(unsigned i = 0; i < 3; i++)
	{
		refreshMotorVelocity(i);
		refreshMotorAcceleration(i);
	}
}

void PicomotorsDevice::refreshAttributes()
{
	readMotorParameters();

	for(unsigned i = 0; i < 3; i++)
	{
		setAttribute("Acceleration "+ STI::Utils::valueToString(i), motorAcceleration[i]);
		setAttribute("Velocity "+ STI::Utils::valueToString(i), motorVelocity[i]);
	}
}

bool PicomotorsDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	unsigned acceleration;
	unsigned velocity;

	for(unsigned i = 0; i < 3; i++)
	{
		if(key.compare("Acceleration "+ STI::Utils::valueToString(i)) == 0)
		{
			success = STI::Utils::stringToValue(value, acceleration);
			if(success && acceleration <= 32000)
			{
				setMotorAcceleration(i, acceleration);
			}
		}
		if(key.compare("Velocity "+ STI::Utils::valueToString(i)) == 0)
		{
			success = STI::Utils::stringToValue(value, velocity);
			if(success && velocity <= 2000)
			{
				setMotorVelocity(i, velocity);
			}
		}
	}

	return success;
}

void PicomotorsDevice::defineChannels()
{
	addOutputChannel(0, ValueNumber, "Motor 0");
	addOutputChannel(1, ValueNumber, "Motor 1");
	addOutputChannel(2, ValueNumber, "Motor 2");
}

bool PicomotorsDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	bool success = false;
	std::stringstream command;
	std::string response;

	if(true)
	{
		//Change driver a1 to motor defined by channel number
		command << "CHL a1=" << STI::Utils::valueToString(channel);
		response = serialController->queryDevice( command.str(), rs232QuerySleep_ms );
//		std::cout << command.str() << std::endl << response << std::endl;
		stiError(command.str());
		command.str("");



		int pos = static_cast<int>(value.getNumber());
		
		//Set position of motor. This is a relative position change only!
		command << "ABS a1 " << STI::Utils::valueToString(pos) << " g";
		response = serialController->queryDevice( command.str(), rs232QuerySleep_ms );
//		std::cout << command << std::endl << response << std::endl;

		stiError(command.str());
	
		success = true;
	}


	return success;
}


std::string PicomotorsDevice::execute(int argc, char* argv[])
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i) << " ";
	}

//	std::cout << command.str() << std::endl;

	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);

//	refreshAttributes();

	return result;
}
