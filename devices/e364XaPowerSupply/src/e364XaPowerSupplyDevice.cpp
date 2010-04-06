/*! \file e364XaPowerSupplyDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class e364XaPowerSupplyDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  This uses code from Measurement Computing to control the USB 1408FS DAQ.
 *  The copywrite status of that code is unknown. 
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



#include "e364XaPowerSupplyDevice.h"

e364XaPowerSupplyDevice::e364XaPowerSupplyDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	rs232Bridge = new agilentRS232Bridge(ModuleNumber);
	rs232Bridge->commandDevice("System:Remote"); //commands the power supplies to be in remote mode
	
	outputOn = false; //default to no supplied power
	voltageMode = true; //set the supply to be in constant voltage mode
	voltage = 0; //default to 0 Volts
	current = 0; //default to 0 Amps 
	gpibID = "System has not been queried";


}
void e364XaPowerSupplyDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
	addAttribute("Output", "On", "Off, On");
	addAttribute("Voltage", voltage);
	addAttribute("Current", current);
	//addAttribute("Mode", "Voltage", "Voltage, Current");
}

void e364XaPowerSupplyDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //will send the IDN? query
	setAttribute("Output", (outputOn ? "On" : "Off") );
	setAttribute("Voltage", voltage );
	setAttribute("Current", current );
	//setAttribute("Mode", (voltageMode ? "Voltage" : "Current") );
}

bool e364XaPowerSupplyDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;

	if(key.compare("Output") == 0)
	{
		if(value.compare("On") == 0)
		{
			if( rs232Bridge->commandDevice("OUTP ON") )
			{
				success = true;
				outputOn = true;
			}
		}
		else
		{
			if( rs232Bridge->commandDevice("OUTP OFF") )
				{
				success = true;
				outputOn = false;
			}
		}
	}
	else if(key.compare("GPIB ID") == 0)
	{
		if( rs232Bridge->queryDevice("*idn?", result) )
		{
			gpibID = result; 
			std::cerr << "Device Name: " << gpibID << std::endl;
			success = true;
		}
		
	}
	else if(key.compare("Voltage") == 0)
	{
		std::string voltageCommand = "VOLT " + value;
		
		if( rs232Bridge->commandDevice(voltageCommand) )
		{
			bool successVoltage = stringToValue(value, voltage);
			if( rs232Bridge->queryDevice("MEAS:VOLT?", result) )
			{
				std::cerr << "Measured Voltage: " << result << std::endl;
				//std:cerr << "Converted Voltage: " << stringToValue(result, voltage) << std::endl;
				success = true;
			}
		}
	}
	else if(key.compare("Current") == 0)
	{
		std::string currentCommand = "CURR " + value;
		if( rs232Bridge->commandDevice(currentCommand) )
		{
			bool successCurrent = stringToValue(value, current);
			if( rs232Bridge->queryDevice("MEAS:CURR?", result) )
			{
				std::cerr << "Measured Current: " << result << std::endl;
				//std:cerr << "Converted Voltage: " << stringToValue(result, voltage) << std::endl;
				success = true;
			}
		}
	}
	
	return success;
}

void e364XaPowerSupplyDevice::defineChannels()
{
	addOutputChannel(0, ValueNumber);
}

bool e364XaPowerSupplyDevice::writeChannel(unsigned short channel, const MixedValue& value)
{	
	return setAttribute("Voltage", value.getNumber() );
}