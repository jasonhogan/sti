/*! \file pdMonitorDevice.cpp
 *  \author David M.S. Johnson; modified from digitelSPC by Susannah Dickerson
 *  \brief Source-file for the class pdMonitorDevice
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



#include "pdMonitorDevice.h"
#include <iostream>
#include <fstream>
#include <time.h>

pdMonitorDevice::pdMonitorDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory, 
							std::string calibrationDirectoryIn) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	calibrationDirectory = calibrationDirectoryIn;

	pd1.tag = "PD 1 power (mW or V)";
	pd1.calibrationFilename = "pd1.txt";
	pd1.usbCommand = "5 1";

	pd2.tag = "PD 2 power (mW or V)";
	pd2.calibrationFilename = "pd2.txt";
	pd2.usbCommand = "7 1";

	enableCalibration = false;
	enableDataLogging = false;

}
pdMonitorDevice::~pdMonitorDevice()
{
//	savedata(timeVector, pressureVector);
}
void pdMonitorDevice::defineAttributes() 
{
	addAttribute("Calibration?", "Off", "On, Off, Refresh");
	addAttribute(pd1.tag, pd1.value);
	addAttribute(pd2.tag, pd2.value);
	addAttribute("Data Logging?", "On", "On, Off");

	addLoggedMeasurement(pd1.tag, 6, 60);
	addLoggedMeasurement(pd2.tag, 6, 60);
}

void pdMonitorDevice::refreshAttributes() 
{
	setAttribute("Calibration?", (enableCalibration ? "On" : "Off"));
	setAttribute(pd1.tag, pd1.value);
	setAttribute(pd2.tag, pd1.value);
	setAttribute("Data Logging?", (enableDataLogging ? "On" : "Off")); //response to the IDN? query
}

bool pdMonitorDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;
	bool measureSuccess = false;
	size_t length;
	char buffer[30];


	if(key.compare("Calibration?") == 0)
	{
		success = true;
		// fill calibration vector if the calibration is not already set
		// or if the user has requested the vector to be refreshed
		if ((value.compare("On") == 0 && !enableCalibration) || value.compare("Refresh") == 0)
		{
			enableCalibration = getCalibration(pd1);
			enableCalibration &= getCalibration(pd2);
			success = enableCalibration;
		} else if (value.compare("Off") == 0 && enableCalibration) {
			enableCalibration = false;
			pd1.slope = 0;
			pd1.offset = 0;
			pd2.slope = 0;
			pd2.offset = 0;
			success = true;
		}
	}
	else if(key.compare(pd1.tag) == 0 && successDouble)
	{
		success = stringToValue(partnerDevice("usb_daq").execute(pd1.usbCommand),tempDouble);
		if (success)
			pd1.value = (enableCalibration) ? (pd1.slope * tempDouble + pd1.offset) : tempDouble;
	}
	else if(key.compare(pd2.tag) == 0 && successDouble)
	{
		success = stringToValue(partnerDevice("usb_daq").execute(pd2.usbCommand),tempDouble);
		if (success)
			pd2.value = (enableCalibration) ? (pd2.slope * tempDouble + pd2.offset) : tempDouble;
	}
	else if(key.compare("Data Logging?") == 0)
	{
		if(value.compare("On") == 0)
		{
			enableDataLogging = true;
			startDataLogging();
		}
		else
		{
			enableDataLogging = false;
			stopDataLogging();
		}

		success = true;
	}


	return success;
}

void pdMonitorDevice::defineChannels()
{
//	addInputChannel(0, ,ValueNumber);
}

void pdMonitorDevice::definePartnerDevices()
{
	addPartnerDevice("usb_daq", "eplittletable.stanford.edu", 31, "usb1408fs"); //local name (shorthand), IP address, module #, device name as defined in main function
}

std::string pdMonitorDevice::execute(int argc, char **argv)
{
	return "";	
}
bool pdMonitorDevice::deviceMain(int argc, char **argv)
{

	return false;
}

bool pdMonitorDevice::getCalibration(attributePDMonitor &pd)
{
	std::string fullFilename;

	double val;

	std::ifstream file;

	// load x-axis	
	fullFilename = calibrationDirectory + pd.calibrationFilename;
	file.open(&fullFilename[0]);

	file >> val;
	if (!(file.good()))
		return false;
	pd.slope = val;

	file >> val;
	if (!(file.good()))
		return false;
	pd.offset = val;

	file.clear();
	file.close();
	return true;
}