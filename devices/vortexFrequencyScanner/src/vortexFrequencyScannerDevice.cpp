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
	enableLock = false;
	digitalChannel = 22;
	daSlowChannel = 38;
	frequency = 0;
	frequencyRange = 500;
	offsetFrequency = 27; // in MHz - depending on FSR of microwave interferometer

	lockSetPointVoltage = 0;
	setPointVoltsPerMHz = 0.1; //using crazy voltage divider

	beatFrequency = 0; // this is only a measured quantity
	vortexPiezoVoltage = 0; // this is only a measured quantity
	isRedDetuning = true; // this determines what the vortex piezo range should be
	piezoClicksPerMHz = 1/50;
	piezoVoltsPerClick = 0.1;
	midPiezoVoltage = 51.8;
}

void vortexFrequencyScannerDevice::defineAttributes() 
{
	addAttribute("Enable", "Off", "On, Off"); //response to the IDN? query
	addAttribute("Frequency (MHz)", frequency); //response to the IDN? query
	addAttribute("Detuning", "Red", "Red, Blue"); //response to the IDN? query
}

void vortexFrequencyScannerDevice::refreshAttributes() 
{
	setAttribute("Enable", (enable ? "On" : "Off")); //response to the IDN? query
	setAttribute("Frequency (MHz)", frequency); //response to the IDN? query
	setAttribute("Detuning", (isRedDetuning ? "Red" : "Blue")); //response to the IDN? query
}

bool vortexFrequencyScannerDevice::updateAttribute(string key, string value)
{

	bool success = false;
	bool successDouble;

	if(key.compare("Enable") == 0)
	{
		if(value.compare("On") == 0)
			enable = true;
		else
			enable = false;
		success = true;
	}
	else if(key.compare("Frequency (MHz)") == 0)
	{
		successDouble = stringToValue(value, frequency);
		vortexPiezoVoltage = (-2 * isRedDetuning + 1) * frequency * piezoClicksPerMHz * piezoVoltsPerClick  + midPiezoVoltage;
		beatFrequency = (-2 * isRedDetuning + 1) * offsetFrequency + frequency;
		if (beatFrequency < offsetFrequency)
		{
			lockSetPointVoltage = (-2 * isRedDetuning + 1) * setPointVoltsPerMHz * beatFrequency;
			beatFrequency = 0;
		}
		else
			lockSetPointVoltage = 0;
			
		
		// disable the lock
		enableLock = false;
		//digital board execute command takes "channel, bool"
		std::string digitalBoardCommand = digitalChannel + " " + enableLock;
		std::cerr << "disengaging lock " << partnerDevice("Digital Board").execute(digitalBoardCommand) << std::endl;
		partnerDevice("vortex").setAttribute( "Piezo Voltage (V)", valueToString(vortexPiezoVoltage) );
		partnerDevice("marconi").setAttribute( "Frequency (MHz)", valueToString(beatFrequency) );
		
		std::string newSetPointString = valueToString(daSlowChannel) + " " + valueToString(lockSetPointVoltage);
		partnerDevice("slow").execute(newSetPointString.c_str()); //usage: partnerDevice("lock").execute("--e1");

		enableLock = false;
		//digital board execute command takes "channel, bool"
		digitalBoardCommand = digitalChannel + " " + enableLock;
		std::cerr << "engaging lock " << partnerDevice("Digital Board").execute(digitalBoardCommand) << std::endl;
	
		success = true;
	}
	else if(key.compare("Detuning") == 0)
	{
		if(value.compare("Red") == 0)
			isRedDetuning = true;
		else
			isRedDetuning = false;

		success = true;
	}
	
	return success;
}
void vortexFrequencyScannerDevice::definePartnerDevices()
{
	addPartnerDevice("spectrumAnalyzer", "eplittletable.stanford.edu", 5, "agilentL1500aSpectrumAnalyzer"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("vortex", "eplittletable.stanford.edu", 2, "Scanning Vortex");
	addPartnerDevice("slow", "ep-timing1.stanford.edu", 4, "Slow Analog Out"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("marconi", "eplittletable.stanford.edu", 13, "marconi2022dFunctionGenerator");
	addPartnerDevice("Digital Board", "ep-timing1.stanford.edu", 2, "Digital Out");
	
}

bool vortexFrequencyScannerDevice::deviceMain(int argc, char **argv)
{
	return false;
}
