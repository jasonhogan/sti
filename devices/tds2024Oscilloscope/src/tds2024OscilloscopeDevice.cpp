/*! \file tds2024OscilloscopeDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class tds2024OscilloscopeDevice
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



#include "tds2024OscilloscopeDevice.h"
#include <iostream>
#include <fstream>
#include <time.h>

tds2024OscilloscopeDevice::tds2024OscilloscopeDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							unsigned short comPort, std::string logDirectory) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	std::string comportString = "COM" + valueToString(comPort);
	myVISA32Controller = new visa32Controller(comportString);

	enableDataLogging = false;

	deviceName = DeviceName;
	deviceID = "";
	
}
tds2024OscilloscopeDevice::~tds2024OscilloscopeDevice()
{
//	savedata(timeVector, pressureVector);
	delete myVISA32Controller;
}
void tds2024OscilloscopeDevice::defineAttributes() 
{
//	addAttribute("Pressure", pressure); // 10L/s ion pump pressure
//	addAttribute("Voltage", voltage); //
//	addAttribute("Current", current); //
//	addAttribute("Enable Data Logging", "On", "On, Off");
	addAttribute("ID", deviceID);

//	addLoggedMeasurement("Pressure", 1, 60);
//	addLoggedMeasurement("Voltage", 10, 60);
}

void tds2024OscilloscopeDevice::refreshAttributes() 
{
//	setAttribute("Pressure", pressure); // 10L/s ion pump pressure
//	setAttribute("Voltage", voltage); //
//	setAttribute("Current", current); //
//	setAttribute("Enable Data Logging", (enableDataLogging ? "On" : "Off")); 
	setAttribute("ID", deviceID);//response to the IDN? query
}

bool tds2024OscilloscopeDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	bool success = false;
	bool measureSuccess = false;
	size_t length;
	char buffer[30];

	if(key.compare("ID") == 0)
	{
		success = true;
		deviceID = myVISA32Controller->queryDevice("*IDN?\n");
	}


	return success;
}

void tds2024OscilloscopeDevice::defineChannels()
{
	
	addInputChannel(0,STI::Types::DataString);
}
bool tds2024OscilloscopeDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::string result;
	std::string mode;
	std::string data;
	
	myVISA32Controller->commandDevice("DATA:SOURCE CH1");
	myVISA32Controller->commandDevice("DATA:START 1");
	myVISA32Controller->commandDevice("DATA:STOP 2500");
	myVISA32Controller->commandDevice("DATa:WIDth 2");
	myVISA32Controller->commandDevice("DATa:ENCdg ASCII");
	//mode = myVISA32Controller->queryDevice("DATa:ENCdg?");
	//std::cerr << "Scope is in Data Encoding mode: " << mode << std::endl;
	mode = myVISA32Controller->queryDevice("WFMPRe?");
	//std::cerr << "Scope is in WFMPRE mode: " << mode << std::endl;


	data = myVISA32Controller->readData("CURVe?");

	//perform error checking

	result = mode.append(data);


	dataOut.setValue( result );

	return true;



}
std::string tds2024OscilloscopeDevice::execute(int argc, char **argv)
{
	return "";	
}
bool tds2024OscilloscopeDevice::deviceMain(int argc, char **argv)
{
	
	/*
	__int64 local_time;
	while(false)
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
	*/

	return false;
}
std::string tds2024OscilloscopeDevice::generateDate()
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
/*
void tds2024OscilloscopeDevice::savedata(std::vector <__int64> &timeVector, std::vector <double> &pressureVector)
{
	//generic data saving
	std::ofstream myfile;
	std::string filename_raw_data;

	filename_raw_data = "ScopeData " + deviceName + " " + generateDate() + ".csv"; 
	std::string path = "C:\\data\\TDS2024B\\";

	std::string fullPath = path + filename_raw_data;

	myfile.open(const_cast<char*>( fullPath.c_str()) );


	myfile << "timeVector, dataVector" << std::endl;
	for(unsigned int i = 0; i < timeVector.size(); i++)
	{
		myfile << timeVector.at(i) << ", " << pressureVector.at(i) << std::endl;
	}
	myfile.close();
}
*/