/*! \file andoAQ6140Device.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class andoAQ6140Device
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



#include "andoAQ6140Device.h"
#include <cmath>

andoAQ6140Device::andoAQ6140Device(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber, 
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; //normally 19
	secondaryAddress = 0;
	outputOn = false; // default to power off
	frequency = 0; // in THz
	wavelength = 0; // in nm
	power = 0; // in dBm
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet

	enableLock = false;
	temperatureGain = 0.0;
	temperatureSetPoint = 0.53;
	temperatureVoltage = temperatureSetPoint / 2;
	wavelengthSetPoint = 1529.3; //nanometers
	daSlowChannel = 0;
}

andoAQ6140Device::~andoAQ6140Device()
{
}


void andoAQ6140Device::defineAttributes() 
{
	//addAttribute("GPIB ID", gpibID); //response to the IDN? query
	//addAttribute("Frequency (THz)", frequency);
	addAttribute("Wavelength (nm)", wavelength);
	addAttribute("Power (dBm)", power);
	addAttribute("1530 Laser Lock", "Off", "Off, On");
	addAttribute("Wavelength Set Point (nm)", wavelengthSetPoint);
	addAttribute("Feedback Gain", temperatureGain);
	addAttribute("Temperature Set Point (V)", temperatureSetPoint);
	//addAttribute("Output", "Off", "Off, On");
	//addAttribute("Output Power (dBm)", outputPower);
}

void andoAQ6140Device::refreshAttributes() 
{
	//setAttribute("GPIB ID", gpibID); //response to the IDN? query
	//setAttribute("Frequency (THz)", frequency);
	setAttribute("Wavelength (nm)", wavelength);
	setAttribute("Power (dBm)", power);
	setAttribute("1530 Laser Lock", (enableLock ? "On" : "Off"));
	setAttribute("Feedback Gain", temperatureGain);
	setAttribute("Wavelength Set Point (nm)", wavelengthSetPoint);
	setAttribute("Temperature Set Point (V)", temperatureSetPoint);
	//setAttribute("Output", (outputOn ? "On" : "Off"));
	//setAttribute("Output Power (dBm)", outputPower);
}

bool andoAQ6140Device::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	//bool commandSuccess;
	bool success = false;
	string result;

/*
	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("*idn?");
		//result = queryDevice("*ESR?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
		//std::cerr << "Event Status Register: " << result << std::endl;
	}
	else if(key.compare("Frequency (THz)") == 0)
	{
		
		//commandSuccess = commandDevice("*RST");
		//bool test = commandDevice(":CALC2:WLIM:CENT:FREQ 208.87 THz");
		//
		//result = queryDevice(":MEAS:SCAL:POW:WAV?");
		//result = queryDevice(":SYST:VERS?");
		//result = queryDevice(":STAT:OPER:COND?");
		//result = queryDevice(":STAT:OPER:ENAB?");
		//result = queryDevice(":STAT:OPER:PTR?");
		//bool test = commandDevice(":STAT:OPER:PTR 32");
		//test = commandDevice(":STAT:OPER:ENAB 32");
		//result = queryDevice(":STAT:OPER:ENAB?");
		//result = queryDevice(":STAT:OPER:PTR?");
		//result = queryDevice(":STAT:OPER:COND?");

		//bool test = commandDevice("*STAT:PRES");

		result = queryDevice(":MEAS:ARR:POW:FREQ?");
		result.erase(0,2);


		bool conversionSuccess = stringToValue(result, frequency);
		//frequency = frequency / pow(10,12);

		std::cerr << "The frequency of the marker position is:" << result << "Hz" << std::endl;
		success = true;//conversionSuccess;
	}
	*/
	if(key.compare("Power (dBm)") == 0)
	{
		result = queryDevice(":MEAS:ARR:POW?");
		result.erase(0,2);
		bool conversionSuccess = stringToValue(result, power);
		std::cerr << "The power at the peak is: " << power << "dBm" << std::endl;
		success = conversionSuccess;
	}
	else if(key.compare("Wavelength (nm)") == 0)
	{
		//commandSuccess = commandDevice("*RST");
		//result = queryDevice(":INIT:CONT?");
		//bool test = commandDevice(":CALC2:WLIM:CENT:FREQ 208.87 THz");
		//
		//result = queryDevice(":MEAS:SCAL:POW:WAV?");
		//result = queryDevice(":SYST:VERS?");
		//result = queryDevice(":STAT:OPER:COND?");
		//result = queryDevice(":STAT:OPER:ENAB?");
		//result = queryDevice(":STAT:OPER:PTR?");
		//bool test = commandDevice(":STAT:OPER:PTR 32");
		//test = commandDevice(":STAT:OPER:ENAB 32");
		//result = queryDevice(":STAT:OPER:ENAB?");
		//result = queryDevice(":STAT:OPER:PTR?");
		//result = queryDevice(":STAT:OPER:COND?");

		//bool test = commandDevice("*STAT:PRES");
		result = queryDevice(":MEAS:ARR:POW:WAV?");
		result.erase(0,2);
		bool conversionSuccess = stringToValue(result, wavelength);
		wavelength = wavelength * 1000000000; // multiply by 10^9
		std::cerr << "The wavelength of the marker position is: " << wavelength << " m" << std::endl;
		success = conversionSuccess;
	}
	else if(key.compare("1530 Laser Lock") == 0)
	{
		//switch the enableLock bool
		if(value.compare("Off") == 0)
			enableLock = false;
		else
			enableLock = true;
		success = true;
	}
	else if(key.compare("Wavelength Set Point (nm)") == 0)
	{
		if(successDouble)
			wavelengthSetPoint = tempDouble;
		success = successDouble;
	}
	else if(key.compare("Feedback Gain") == 0)
	{
		if(successDouble)
			temperatureGain = tempDouble;
		success = successDouble;
	}
	else if(key.compare("Temperature Set Point (V)") == 0)
	{
		if(successDouble)
			temperatureSetPoint = tempDouble;
		success = successDouble;
	}
	


	return success;
}

void andoAQ6140Device::defineChannels()
{
}


void andoAQ6140Device::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void andoAQ6140Device::definePartnerDevices()
{
	addPartnerDevice("gpibController", "li-gpib.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("slow", "ep-timing1.stanford.edu", 4, "Slow Analog Out"); //local name (shorthand), IP address, module #, device name as defined in main function
}


std::string andoAQ6140Device::execute(int argc, char **argv)
{
	return "";
}
bool andoAQ6140Device::deviceMain(int argc, char **argv)
{
	std::string result;
	std::string newTemperatureString;

	double deltaWavelength;

	bool conversionSuccess;

	while(1)
	{
		if(enableLock)
		{
			result = queryDevice(":MEAS:ARR:POW:WAV?");
			result.erase(0,2);
			conversionSuccess = stringToValue(result, wavelength);
			wavelength = wavelength * 1000000000; // multiply by 10^9
			std::cerr << "The wavelength of the marker position is:" << wavelength << "m" << std::endl;

			deltaWavelength = wavelength - wavelengthSetPoint;
			if(deltaWavelength > 0.005 || deltaWavelength < 0.005)
				temperatureVoltage = temperatureVoltage + temperatureGain * deltaWavelength;
		}
		else
		{
			temperatureVoltage = temperatureSetPoint / 2; //note the factor of 2 difference due to the mysteries of the temperature controller
			Sleep(500);
		}

		if(temperatureVoltage < 0.6 && temperatureVoltage > 0.2)
			{
				newTemperatureString = valueToString(daSlowChannel) + " " + valueToString(temperatureVoltage);
				std::cerr << "command sent to DA Slow: " << newTemperatureString << std::endl;
				partnerDevice("slow").execute(newTemperatureString.c_str()); //usage: partnerDevice("lock").execute("--e1");
				temperatureSetPoint = temperatureVoltage * 2; // again the usual factor of 2 difference
			}
		else
		{
			std::cerr << "Temperature Voltage of " << temperatureVoltage << " V is out of range." << std::endl;
			if(enableLock)
			{
				std::cerr << "Laser is out of lock!" << std::endl;
				enableLock = false;
			}
		}

	}

	return false;
}
std::string andoAQ6140Device::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
bool andoAQ6140Device::commandDevice(std::string command)
{
	std::string commandString;
	std::string result;
	commandString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " 0";

	result = partnerDevice("gpibController").execute(commandString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	if(result.compare("1")==0)
		return true;
	else
		return false;
}