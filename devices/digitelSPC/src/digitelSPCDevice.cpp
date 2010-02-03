/*! \file digitelSPCDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class digitelSPCDevice
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



#include "digitelSPCDevice.h"
#include <iostream>
#include <fstream>
#include <time.h>

digitelSPCDevice::digitelSPCDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	std::string comportString = "COM" + valueToString(comPort);
	myRS232Controller = new rs232Controller(comportString);
	pressure = 1;
	voltage = 0;
	current = 0;
	enableDataLogging = false;
	pumpName = DeviceName;
	
}
digitelSPCDevice::~digitelSPCDevice()
{
	savedata(timeVector, pressureVector);
};
void digitelSPCDevice::defineAttributes() 
{
	addAttribute("Pressure", pressure); // 10L/s ion pump pressure
	addAttribute("Voltage", voltage); //
	addAttribute("Current", current); //
	addAttribute("Enable Data Logging", "Off", "On, Off");
}

void digitelSPCDevice::refreshAttributes() 
{
	setAttribute("Pressure", pressure); // 10L/s ion pump pressure
	setAttribute("Voltage", voltage); //
	setAttribute("Current", current); //
	setAttribute("Enable Data Logging", (enableDataLogging ? "On" : "Off")); //response to the IDN? query
}

bool digitelSPCDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;
	bool measureSuccess = false;
	size_t length;
	char buffer[30];

	if(key.compare("Pressure") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0B 33");
		if(success)
		{
			size_t length = result.size();
			size_t found = result.find(" Torr");
			result.erase(found, length);
			result.erase(0, 8);
			std::cerr << pumpName << " pressure is: " << result << " Torr" << std::endl;
			measureSuccess = stringToValue( result, pressure);
		}
	}
	else if(key.compare("Current") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0A 32");
		if(success)
		{
			length=result.copy(buffer,6,9);
			buffer[length]='\0';
			std::cerr << pumpName << " current is: " << string(buffer) << " Amps" << std::endl;
			measureSuccess = stringToValue( string(buffer), current);
		}
	}
	else if(key.compare("Voltage") == 0)
	{
		success = true;
		result = myRS232Controller->queryDevice("~ 01 0C 34");
		if(success)
		{	
			length=result.copy(buffer,4,9);
			buffer[length]='\0';
			std::cerr << pumpName << " Voltage is: " << string(buffer) << " Volts" << std::endl;
			measureSuccess = stringToValue( string(buffer), voltage);
		}
	}
	else if(key.compare("Enable Data Logging") == 0)
	{
		if(value.compare("On") == 0)
			enableDataLogging = true;
		else
			enableDataLogging = false;

		success = true;
	}


	return success;
}

void digitelSPCDevice::defineChannels()
{
	
	//addOutputChannel(0, ValueNumber);
}
bool digitelSPCDevice::readChannel(ParsedMeasurement& Measurement)
{
	
	return false;
}
std::string digitelSPCDevice::execute(int argc, char **argv)
{
	return "";	
}
bool digitelSPCDevice::deviceMain(int argc, char **argv)
{
	__int64 local_time;
	while(1)
	{
		if(enableDataLogging)
		{
			refreshAttributes();
			//push pressure & time out to a text file
			_time64( &local_time );
			timeVector.push_back(local_time);
			pressureVector.push_back(pressure);
		}
		Sleep(60000); //wait for 1 minute
		if(timeVector.size() > 1440)
		{
			savedata(timeVector, pressureVector);
			timeVector.clear();
			pressureVector.clear();
		}
	}


	return false;
}
std::string digitelSPCDevice::generateDate()
{
	//this generates the date string for the file name
	struct tm newtime;
	__int64 local_time;
	char time_buf[26];
	errno_t err;

	_time64( &local_time );

	// Obtain coordinated universal time: 
	err = _localtime64_s( &newtime, &local_time );
	if (err)
	{
		printf("Invalid Argument to _gmtime64_s.");
	}
   
	// Convert to an ASCII representation 
	err = asctime_s(time_buf, 26, &newtime);
	if (err)
	{
		printf("Invalid Argument to asctime_s.");
	}

	std::string date_string = time_buf;

	size_t found;

	found=date_string.find_first_of(":");
		
	while (found!=std::string::npos)
	{
		date_string[found]='_';
		found=date_string.find_first_of(":",found+1);
	}

	found=date_string.find_first_of("\n");
		
	while (found!=std::string::npos)
	{
		date_string.erase(found, 1);
		found=date_string.find_first_of("\n",found+1);
	}
	return date_string;
}

void digitelSPCDevice::savedata(std::vector <__int64> &timeVector, std::vector <double> &pressureVector)
{
	//generic data saving
	std::ofstream myfile;
	std::string filename_raw_data;

	filename_raw_data = "pumpPressure " + pumpName + " " + generateDate() + ".csv"; 
	std::string path = "\\\\atomsrv1\\EP\\Data\\pumpPressure\\";

	std::string fullPath = path + filename_raw_data;

	myfile.open(const_cast<char*>( fullPath.c_str()) );
	
	/*
	myfile << "Pull Amplifier: 10-4200 MHz" << std::endl;
	myfile << "Push Amplifier: 700-4200 MHz" << std::endl;
	myfile << "Push NLTL: 7113-110" << std::endl;
	myfile << "Pull NLTL: 7112-110" << std::endl;
	myfile << "Push Function generator frequency: " << pushFrequency << " MHz" << std::endl;
	myfile << "Pull Function generator frequency: " << pullFrequency << " MHz" << std::endl;
	myfile << "Push Function generator power: " << pushPower << " dBm" << std::endl;
	myfile << "Pull Function generator power: " << pullPower << " dBm" << std::endl;
	myfile << "push pull low frequency serrodyne" << std::endl;
	*/
	myfile << "timeVector, pressureVector" << std::endl;
	for(unsigned int i = 0; i < timeVector.size(); i++)
	{
		myfile << timeVector.at(i) << ", " << pressureVector.at(i) << std::endl;
	}
	myfile.close();
}