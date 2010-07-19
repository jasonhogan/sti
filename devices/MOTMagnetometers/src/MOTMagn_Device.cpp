/*! \file MOTMagn_Device.cpp
 *  \author Susannah Dickerson
 *  \brief Template for STI_Devices
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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



#include "MOTMagn_Device.h"


MOTMagn_Device::MOTMagn_Device(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, 
					   unsigned short comPort, std::string logDirectory) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	//Initialization of device
	std::string IDstring;
	int pos;

	initialized = true;

	myRS485Controller = new rs232Controller("COM" + valueToString(comPort));

	//Check that we can talk to the device
	if (!myRS485Controller->initialized)
	{
		initialized = false;
		return;
	}

	//Determine the number and IDs of all magnetometers attached
	IDstring = myRS485Controller->queryDevice("*99ID");

	pos = IDstring.find("ID= ");
	while(std::string::npos != pos)
	{
		magnIDs.push_back(IDstring.substr(pos + 4,2));
		IDstring.erase(0,7);
		pos = IDstring.find("ID= ");
	};
}

MOTMagn_Device::~MOTMagn_Device()
{
	delete myRS485Controller;
}


bool MOTMagn_Device::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	MixedData data1;
	MixedData data2;
	MixedData data3;
	std::string str = "str";
	MixedData data4 = str;
	std::vector <double> intVec1;
	std::vector <int> intVec2;

	intVec1.push_back(1.1);
	intVec1.push_back(2);
	intVec1.push_back(3);

	intVec2.push_back(2);
	intVec2.push_back(2);
	intVec2.push_back(2);

	data1.addValue(1);
	data1.addValue(intVec1);
	data2.addValue(2);
	data2.addValue(intVec2);

	data3 = data1 + data2;

	std::cerr << data3.print() << std::endl;

	data3 = data1 - data2;

	std::cerr << data3.print() << std::endl;

	data3 = data1 * data2;

	std::cerr << data3.print() << std::endl;

	data3 = data1 / data2;

	std::cerr << data3.print() << std::endl;

	return false;
}

void MOTMagn_Device::defineAttributes() 
{
	std::vector<std::string>::iterator it;

	for (it = magnIDs.begin(); it != magnIDs.end(); it++)
	{
		addAttribute(*it + " x", 0);
		addAttribute(*it + " y", 0);
		addAttribute(*it + " z", 0);
	}

	addAttribute("Enable Data Logging", "Off", "On, Off");
}

void MOTMagn_Device::refreshAttributes() 
{
	std::vector<std::string>::iterator it;
	std::vector <double> measurement;
	bool error;

	for (it = magnIDs.begin(); it != magnIDs.end(); it++)
	{
		error = measureField(*it, measurement);
		setAttribute(*it + " x", measurement.at(0));
		setAttribute(*it + " y", measurement.at(1)); 
		setAttribute(*it + " z", measurement.at(2)); 
		measurement.clear();
	}
	
	setAttribute("Enable Data Logging", (enableDataLogging ? "On" : "Off")); //response to the IDN? query
}

bool MOTMagn_Device::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	std::vector<std::string>::iterator it;

	//Doesn't actually do anything; this device monitors only
	for (it = magnIDs.begin(); it != magnIDs.end(); it++) {

		if(key.compare(*it + " x") == 0)
		{
			success = true;
		}
		else if(key.compare(*it + " y") == 0)
		{
			success = true;
		}
		else if(key.compare(*it + " z") == 0)
		{
			success = true;
		}
	}

	if(key.compare("Enable Data Logging") == 0)
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

void MOTMagn_Device::defineChannels()
{
	unsigned int i;
	for (i = 0; i < magnIDs.size(); i++)
	{
		addInputChannel((short) i, DataString, ValueNumber);
		addLoggedMeasurement((short) i, 5, 5);
	}
}

bool MOTMagn_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::vector<std::string>::iterator it;
	std::vector <double> measurement;
	bool success = true;

	if (channel < 0 || channel >= magnIDs.size())
	{
		std::cerr << "Expect a channel between 0 and " << valueToString(magnIDs.size()) << ", not " << valueToString(channel) << std::endl;
		return false;
	}

	success = !measureField(magnIDs.at(channel), measurement);

	std::cerr << measurement.at(0) << std::endl;
	dataOut.setValue(measurement.at(0));

	measurement.clear();

	return success;
}

bool MOTMagn_Device::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	return false;
}

void MOTMagn_Device::definePartnerDevices()
{
}

std::string MOTMagn_Device::execute(int argc, char **argv)
{
	return "";
}

void MOTMagn_Device::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void MOTMagn_Device::stopEventPlayback()
{
}

bool MOTMagn_Device::measureField(std::string ID, std::vector <double> & measurement)
{    	
	bool error = false;
	std::string error_msg;
	
	error_msg = myRS485Controller->queryDevice("*" + ID + "WE");
	if (error_msg.compare("OK") != 0)
		error = true;

	error_msg = myRS485Controller->queryDevice("*" + ID + "B");
	if (error_msg.compare("BINARY ON") != 0)
		error = true;
	
	std::vector <int> output = myRS485Controller->binaryQueryDevice("*" + ID + "P");  //binaryQueryDevice

	int signedResult;
	unsigned int shortResult;
	double result;

	for(int i=0;i<3;i++){
		signedResult = (signed int)output[2*i];
		shortResult = (unsigned int) (unsigned char) output[2*i+1];

		if(signedResult<0)
				result = -(double)(( abs( signedResult))*256 + shortResult);
		else
				result = (double)((abs( signedResult))*256 +  shortResult);
		
		measurement.push_back(result/15);	
	};
   
    return error; 	
	
}