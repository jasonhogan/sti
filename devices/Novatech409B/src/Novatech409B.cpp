/*! \file STI_Device_Template.cpp
 *  \author Susannah Dickerson
 *  \brief Template for STI_Devices
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah Dickerson <sdickers@stanford.edu>\n
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



#include "Novatech409B.h"


Novatech409B::Novatech409B(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 19200,8,"None",1);

	//Check that we can talk to the device
	if (!serialController->initialized)
	{
		initialized = false;
		delete serialController;
		return;
	}

	//read all the frequency channels

	std::string initialState = serialController->queryDevice("QUE", 200, 250);  //string, sleep time in ms for query, read length of 250 characters

	std::cerr << "COM " << valueToString(comPort) << ";  " << initialState << std::endl;

	parseQUE(initialState);

	initialized = true;
}

Novatech409B::~Novatech409B()
{
	delete serialController;
}

void Novatech409B::parseQUE(std::string queOutput)
{
	std::string chInfo;

	size_t locBegin = 0;
	size_t locEnd = 0;

	FrequencyChannel tempFC;
	int tempVal;

	for (int i = 0; i < 4; i++)
	{
		locBegin = queOutput.find_first_of('\n', locEnd);
		locEnd = queOutput.find_first_of('\n',locBegin+1);

		chInfo.assign(queOutput.begin()+ locBegin + 1, queOutput.begin()+locEnd-1);
		std::cerr << chInfo << std::endl;

		//Get the frequency (which comes in units of 0.1 Hz (so 1 Hz is written as 10 in decimal)
		stringToValue(chInfo.substr(0,8),tempVal,ios::hex);
		tempFC.frequency = tempVal/10/1000000;  //write frequency in MHz

		//Get the phase
		stringToValue(chInfo.substr(9,4),tempVal,ios::hex);
		tempFC.phase = tempVal;

		//Get the amplitude
		stringToValue(chInfo.substr(14,4),tempVal,ios::hex);
		tempFC.amplitude = tempVal;

		frequencyChannels.insert(pair<int, FrequencyChannel>(i, tempFC));
	}

	return;
}

bool Novatech409B::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	return false;
}

void Novatech409B::defineAttributes() 
{
	/*
	addAttribute("Attribute Name v1", "Initial Option", "Option 1, Option 2, ...");
	addAttribute("Attribute Name v2", some_initial_number);
	*/
}

void Novatech409B::refreshAttributes() 
{

	/*
	setAttribute("Attribute Name v1", "Option to set");   //figuring out which option to set 
														//often requires a switch on some parameter
	setAttribute("Attribute Name v2", some_other_number); 
	*/

}

bool Novatech409B::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	/*
	if(key.compare("Attribute Name v1") == 0)
	{
		success = true;
		
		if(value.compare("Option 1") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else if(value.compare("Option 2") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else
			success = false;
	}
	else if(key.compare("Attribute Name v2") == 0 && successDouble)
	{
		success = true;

		myVariable = tempDouble;

		set "Attribute Name v2" to myVariable
	}
	*/

	success = true;

	return success;
}

void Novatech409B::defineChannels()
{
}

bool Novatech409B::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return false;
}

bool Novatech409B::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	return false;
}

void Novatech409B::definePartnerDevices()
{
}

std::string Novatech409B::execute(int argc, char **argv)
{
	return "";
}

void Novatech409B::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}

void Novatech409B::stopEventPlayback()
{
}