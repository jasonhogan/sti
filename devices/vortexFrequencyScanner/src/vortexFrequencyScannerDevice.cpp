/*! \file vortexFrequencyScannerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class vortexFrequencyScannerDevice
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



#include "vortexFrequencyScannerDevice.h"

vortexFrequencyScannerDevice::vortexFrequencyScannerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	//initialize values
	enable = false;
	dFrequency = 0;
	lockSetPointVoltage = 0;

	beatFrequency = 0; // this is only a measured quantity
	vortexPiezoVoltage = 0; // this is only a measured quantity
	isRedDeturning = true; // this determines what the vortex piezo range should be
}

vortexFrequencyScannerDevice::~vortexFrequencyScannerDevice()
{

}


void vortexFrequencyScannerDevice::defineAttributes() 
{
	addAttribute("Enable", "Off", "On, Off"); //response to the IDN? query
	addAttribute("Delta Frequency", dFrequency); //response to the IDN? query
	addAttribute("Detuning", "Red", "Red, Blue"); //response to the IDN? query
}

void vortexFrequencyScannerDevice::refreshAttributes() 
{
	setAttribute("Enable", (enable ? "On" : "Off")); //response to the IDN? query
	setAttribute("Delta Frequency", dFrequency); //response to the IDN? query
	setAttribute("Detuning", (isRedDeturning ? "Red" : "Blue")); //response to the IDN? query
}

bool vortexFrequencyScannerDevice::updateAttribute(string key, string value)
{

	bool success = false; 

	if(key.compare("Enable") == 0)
	{
		if(value.compare("On") == 0)
			enable = true;
		else
			enable = false;

		success = true;
	}
	else if(key.compare("Delta Frequency") == 0)
	{
		//
	}
	else if(key.compare("Detuning") == 0)
	{
		//
	}
	
	return success;
}
void vortexFrequencyScannerDevice::definePartnerDevices()
{
	addPartnerDevice("spectrumAnalyzer", "eplittletable.stanford.edu", 5, "agilentL1500aSpectrumAnalyzer"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("vortex", "eplittletable.stanford.edu", 2, "Scanning Vortex");
	addPartnerDevice("slow", "ep-timing1.stanford.edu", 4, "Slow Analog Out"); //local name (shorthand), IP address, module #, device name as defined in main function
	/*
	newTemperatureString = valueToString(daSlowChannel) + " " + valueToString(temperatureVoltage);
	std::cerr << "command sent to DA Slow: " << newTemperatureString << std::endl;
	partnerDevice("slow").execute(newTemperatureString.c_str()); //usage: partnerDevice("lock").execute("--e1");
	
	//partner device commands
	bool setAttribute(std::string key, std::string value);
	std::string getAttribute(std::string key);
	
	*/
	addPartnerDevice("marconi", "eplittletable.stanford.edu", 13, "marconi");
	
}

bool vortexFrequencyScannerDevice::deviceMain(int argc, char **argv)
{
	
	clock_t startTime;
	clock_t endTime;

	std::string attribute = "Output";
	std::string on = attribute + " On";
	std::string off = attribute + " Off";

	while(1)
	{
		// this goes forever
		if(enable)
		{
			//

			// step 1. switch repump on & off
			partnerDevice("repump").execute(off);
			partnerDevice("repump").execute(on);
			// step 2. scan for 1 second
			startTime = clock();
			endTime = startTime + 1 * CLOCKS_PER_SEC; 
			while( difftime (endTime, clock()) > 0 )
			{
			}
		}
	}

	return true;
}
