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



#include "arroyo.h"


arroyo::arroyo(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	serialController  = new rs232Controller("COM" + valueToString(comPort), 9600,8,"None",1);

	//Check that we can talk to the device
	if (!serialController->initialized)
	{
		initialized = false;
		return;
	}

	return;
}

arroyo::~arroyo()
{
	delete serialController;
}


bool arroyo::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	return false;
}

void arroyo::defineAttributes() 
{
	/*
	addAttribute("Attribute Name v1", "Initial Option", "Option 1, Option 2, ...");
	addAttribute("Attribute Name v2", some_initial_number);
	*/
}

void arroyo::refreshAttributes() 
{

	/*
	setAttribute("Attribute Name v1", "Option to set");   //figuring out which option to set 
														//often requires a switch on some parameter
	setAttribute("Attribute Name v2", some_other_number); 
	*/

}

bool arroyo::updateAttribute(string key, string value)
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

void arroyo::defineChannels()
{
	//one output channel per frequency channel
	addOutputChannel(0, ValueVector);
	addOutputChannel(1, ValueVector);
	addOutputChannel(2, ValueVector);
	addOutputChannel(3, ValueVector);
	addInputChannel(4, STI::Types::DataVector); //for reading settings
}

bool arroyo::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::vector <std::vector<double> > freqAmpPhases;
	std::vector <double> freqAmpPhase;

	if (channel == 4)
	{
		for (unsigned int i = 0; i < frequencyChannels.size(); i++)
		{
			freqAmpPhase.clear();

			freqAmpPhase.push_back(frequencyChannels.at(i).frequency);
			freqAmpPhase.push_back(frequencyChannels.at(i).amplitude);
			freqAmpPhase.push_back(frequencyChannels.at(i).phase);

			freqAmpPhases.push_back(freqAmpPhase);
		}
		dataOut.setValue(freqAmpPhases);
	}
	else
	{
		std::cerr << "Expecting channel 4 for a read command" << std::endl;
		return false;
	}

	return true;
}

bool arroyo::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	std::string queryResult;
	MixedValueVector tempVec;
	enum error {TYPE, CHANNEL, RANGE};

	double frequency;
	int amplitude;
	int phase;

	try {
		if (channel >= 0 && channel < 4)
		{
			if (valuet.getType() == MixedValue::Vector)
			{
				tempVec = valuet.getVector();
				if (tempVec.size() != 3)
					throw TYPE;

				frequency = tempVec.at(0).getDouble();
				amplitude = tempVec.at(1).getInt();
				phase = tempVec.at(2).getInt();

				if (frequency < 0 || frequency > 171.1276031)
				{
					std::cerr << "Frequency out of range." << std::endl;
					throw RANGE;
				}
				if (amplitude < 0 || amplitude > 1023)
				{
					std::cerr << "Amplitude out of range." << std::endl;
					throw RANGE;
				}
				if (phase < 0 || phase > 16383)
				{
					std::cerr << "Phase out of range." << std::endl;
					throw RANGE;
				}

			}
			else
				throw TYPE;	
		}
		else
			throw CHANNEL;
	}
	catch (error e)
	{
		if (e == CHANNEL)
			std::cerr << this->getDeviceName() << " expects channel 0, 1, 2,or 3 for a write command" << std::endl;
		if (e == TYPE)
			std::cerr << this->getDeviceName() << " requires a triplet of doubles: (frequency, amplitude, phase)" << std::endl;
		if (e == RANGE)
			std::cerr << this->getDeviceName() << " allows ranges of (0-171.1276031, 0-1023, 0-16383)" << std::endl;

		return false;
	}


	std::string frequencyString, amplitudeString, phaseString;

	frequencyString = "f" + valueToString(channel) + " " + valueToString(frequency, "", ios::dec, 10);
	amplitudeString = "v" + valueToString(channel) + " " + valueToString(amplitude);
	phaseString = "p" + valueToString(channel) + " " + valueToString(phase);


	//Set frequency
	queryResult = serialController->queryDevice(frequencyString);
	if (queryResult.find("OK") == std::string::npos)
	{
		std::cerr << "Unable to set frequency of " << this->getDeviceName() << " channel " << channel << std::endl;
		return false;
	}
	frequencyChannels.at(channel).frequency = frequency;

	queryResult = serialController->queryDevice(amplitudeString);
	if (queryResult.find("OK") == std::string::npos)
	{
		std::cerr << "Unable to set amplitude of " << this->getDeviceName() << " channel " << channel << std::endl;
		return false;
	}
	frequencyChannels.at(channel).amplitude = amplitude;

	queryResult = serialController->queryDevice(phaseString);
	if (queryResult.find("OK") == std::string::npos)
	{
		std::cerr << "Unable to set phase of " << this->getDeviceName() << " channel " << channel << std::endl;
		return false;
	}
	frequencyChannels.at(channel).phase = phase;


	return true;
}

void arroyo::definePartnerDevices()
{
}

std::string arroyo::execute(int argc, char **argv)
{
	return "";
}

void arroyo::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}

void arroyo::stopEventPlayback()
{
}

void arroyo::parseQUE(std::string queOutput)
{
	std::string chInfo;

	size_t locBegin = 0;
	size_t locEnd = 0;

	FrequencyChannel tempFC;
	int tempVal;

	for (int i = 0; i < 4; i++)
	{
		locBegin = queOutput.find_first_of('\n', locEnd);
		if (locBegin == std::string::npos)
			return;

		locEnd = queOutput.find_first_of('\n',locBegin+1);
		if (locEnd == std::string::npos)
			return;

		//return if the substring isn't be long enough to contain the required information
		if (locEnd - locBegin < 20)
			return;

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

		frequencyChannels.push_back(tempFC);
	}

	return;
}
