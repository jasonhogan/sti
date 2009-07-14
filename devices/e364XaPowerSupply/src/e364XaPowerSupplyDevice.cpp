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
	outputOn = false; //default to no supplied power
	voltageMode = true; //set the supply to be in constant voltage mode
	voltage = 0; //default to 0 Volts
	current = 0; //default to 0 Amps 


}

e364XaPowerSupplyDevice::~e364XaPowerSupplyDevice()
{
}


void e364XaPowerSupplyDevice::defineAttributes() 
{
	addAttribute("Output", "Off", "Off, On");
	addAttribute("Voltage", voltage);
	addAttribute("Current", current);
	addAttribute("Mode", "Voltage", "Voltage, Current");
}

void e364XaPowerSupplyDevice::refreshAttributes() 
{
	setAttribute("Output", (outputOn ? "On" : "Off") );
	setAttribute("Voltage", voltage );
	setAttribute("Current", current );
	setAttribute("Mode", (voltageMode ? "Voltage" : "Current") );
}

bool e364XaPowerSupplyDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool commandSuccess;
	bool success = false;

	if(key.compare("Output") == 0)
	{
		if(value.compare("On") == 0)
		{
			//
		}
		else
		{
			//
		}
	}
	
	return success;
}

void e364XaPowerSupplyDevice::defineChannels()
{
	addOutputChannel(0, ValueNumber);
}

bool e364XaPowerSupplyDevice::writeChannel(const RawEvent& Event)
{	
	return setAttribute("Voltage", Event.numberValue() );
}

bool e364XaPowerSupplyDevice::readChannel(ParsedMeasurement& Measurement)
{
	return false;
}

void e364XaPowerSupplyDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void e364XaPowerSupplyDevice::definePartnerDevices()
{
}

void e364XaPowerSupplyDevice::stopEventPlayback()
{
}

std::string e364XaPowerSupplyDevice::execute(int argc, char **argv)
{
	return "";	
}
bool e364XaPowerSupplyDevice::deviceMain(int argc, char **argv)
{
	return false;
}
bool e364XaPowerSupplyDevice::setOutputVoltage(double outputVoltage)
{
	bool success = false;

	//UDStat = cbVOut (BoardNum, channel, outputGain, outputVoltage, Options);
	//if(UDStat == NOERRORS)
	//	success = true;

	return success;
}