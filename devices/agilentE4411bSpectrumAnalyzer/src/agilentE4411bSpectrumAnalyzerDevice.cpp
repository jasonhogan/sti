/*! \file agilentE4411bSpectrumAnalyzerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class agilentE4411bSpectrumAnalyzerDevice
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

#include "agilentE4411bSpectrumAnalyzerDevice.h"

agilentE4411bSpectrumAnalyzerDevice::agilentE4411bSpectrumAnalyzerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber) : 
GPIB_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory, GCipAddress, GCmoduleNumber)
{ 
	//primaryAddress = primaryGPIBAddress; //normally 1
	//secondaryAddress = 0;
	//gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	numberPoints = 0;
	startFrequency = 0;
	stopFrequency = 500;
	referenceLevel = -50; //dBm
	peakLocation = 0;
	enableAveraging = false;
	//initialized = false;

	//":WAV:POINTS " + pointsStr;
	//setup acquisition
	//setupSuccess = quickCommand(":FORMat:TRACe:DATA ASCII");
	//setupSuccess = quickCommand(":TRACE1:MODE WRITE");
	//setupSuccess = quickCommand(":AVERage OFF");
	/* Scaling information
	bool success = true;
	std::stringstream convert;
	
	convert << quickQuery(":DISPlay:WINDow:TRACe:Y:NRLevel?");
	convert >> normalizedReferenceLevel;
	std::cerr << "Y:NRLevel? " << normalizedReferenceLevel << std::endl;

	convert << quickQuery(":DISPlay:WINDow:TRACe:Y:RLEVel:OFFSet?");
	convert >> referenceLevel;
	std::cerr << "Y:RLevel? " << referenceLevel << std::endl;

	convert << quickQuery(":DISPlay:WINDow:TRACe:Y:NRPosition?");
	convert >> referenceLevelPosition; //0-10, 10 is standard
	std::cerr << "Y:NRPosition? " << referenceLevelPosition << std::endl;

	convert << quickQuery(":DISPlay:WINDow:TRACe:Y:PDIVision?");
	convert >> powerPerDivision;
	std::cerr << "Y:PDIVision? " << powerPerDivision << std::endl;

	convert << quickQuery(":FREQuency:CENTer?");
	convert >> centerFrequency;
	std::cerr << "Center Frequency? " << centerFrequency << std::endl;

	convert << quickQuery(":FREQuency:SPAN?");
	convert >> frequencySpan;
	std::cerr << "Frequency Span? " << frequencySpan << std::endl;

	convert << quickQuery(":FREQuency:STARt?");
	convert >> startFrequency;
	std::cerr << "Start Frequency? " << startFrequency << std::endl;

	convert << quickQuery(":FREQuency:STOP?");
	convert >> stopFrequency;
	std::cerr << "Stop Frequency? " << stopFrequency << std::endl;

	//freq offset
	std::cerr << "Frequency Offset? " << quickQuery(":DISPlay:WINDow:TRACe:X:OFFSet?") << std::endl;

	//freq scaling
	std::cerr << "Frequency Scaling? " << quickQuery(":DISPlay:WINDow:TRACe:Y:PDIVision:FREQuency?") << std::endl;
	
	//log vs. linear vertical
	std::cerr << "Vertical Axis (log or linear)? " << quickQuery(":DISPlay:WINDow:TRACe:Y:SPACing?") << std::endl;
	
	//log vs. linear horizontal
	std::cerr << "Horizontal Axis (log or linear)? " << quickQuery(":SWEep:SPACing?") << std::endl;

	//byte order normal vs. swapped
	std::cerr << "Byte Order? " << quickQuery(":FORMat:BORDer?") << std::endl;

	//numeric data format
	std::cerr << "Numeric Data Format? " << quickQuery(":FORMat:TRACe:DATA?") << std::endl;

	//which mode?
	std::cerr << "Trace mode? " << quickQuery(":TRACE1:MODE?") << std::endl;

	//averaging?
	std::cerr << "Averaging? " << quickQuery(":AVERage?") << std::endl;
	*/
	
	//readUntilNewLine (primary_address, secondary_address, ":TRACe:DATA? TRACE1", result);
	
}
void agilentE4411bSpectrumAnalyzerDevice::defineGpibAttributes()
{
	addGpibAttribute("Start Frequency (Hz)", ":FREQuency:STARt");
	addGpibAttribute("Stop Frequency (Hz)", ":FREQuency:STOP");
	addGpibAttribute("Reference Level (dBm)", ":DISPlay:WINDow:TRACe:Y:RLEVel");
	addGpibAttribute("Peak Location (Hz)", ":CALCulate:MARKer:MAXimum; :CALCulate:MARKer:X", "", true);
}
void agilentE4411bSpectrumAnalyzerDevice::defineAttributes() 
{
	//addAttribute("GPIB ID", gpibID); //response to the IDN? query
	//addAttribute("Start Frequency (MHz)", startFrequency);
	//addAttribute("Stop Frequency (MHz)", stopFrequency);
	//addAttribute("Reference Level (dBm)", referenceLevel);
	//addAttribute("Peak Location (MHz)", peakLocation);

}

void agilentE4411bSpectrumAnalyzerDevice::refreshAttributes() 
{
	//setAttribute("GPIB ID", gpibID); //will send the IDN? query
	//setAttribute("Start Frequency (MHz)", startFrequency);
	//setAttribute("Stop Frequency (MHz)", stopFrequency);
	//setAttribute("Reference Level (dBm)", referenceLevel);
	//setAttribute("Peak Location (MHz)", peakLocation);

}

bool agilentE4411bSpectrumAnalyzerDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	/*
	double tempDouble;
	double tempResult;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;
	string result;
	string command;


	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("*idn?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("Start Frequency (MHz)") == 0)
	{
		tempResult = updateGPIBAttribute(":FREQuency:STARt", tempDouble * 1000000, initialized);
		if(tempResult != -1)
		{
			startFrequency = tempResult / 1000000;
			success = true;
		}

	}
	else if(key.compare("Stop Frequency (MHz)") == 0)
	{
		tempResult = updateGPIBAttribute(":FREQuency:STOP", tempDouble * 1000000, initialized);
		if(tempResult != -1)
		{
			stopFrequency = tempResult / 1000000;
			success = true;
		}
	}
	else if(key.compare("Reference Level (dBm)") == 0)
	{
		tempResult = updateGPIBAttribute(":DISPlay:WINDow:TRACe:Y:RLEVel", tempDouble, initialized);
		if(tempResult != -1)
		{
			referenceLevel = tempResult;
			success = true;
		}
	}
	else if(key.compare("Peak Location (MHz)") == 0)
	{
		result = commandDevice(":CALCulate:MARKer:MAXimum");
		result = queryDevice(":CALCulate:MARKer:X?");
		success = stringToValue(result, tempResult);
		if(success)
			peakLocation = tempResult / 1000000;
		std::cerr << "Peak Location: " << peakLocation << std::endl;
	}

	return success;
	*/
	return false;
}

void agilentE4411bSpectrumAnalyzerDevice::defineChannels()
{
}


/*
bool agilentE4411bSpectrumAnalyzerDevice::saveData(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector)
{
	std::string queryString;
	std::string data;
	std::string dataString;
	double dataDouble;
	double freqValue;
	unsigned int dataPoint = 0;
	unsigned int i = 0;
	unsigned int j = 0;

	//runs the appropriate readUntilNewLine command
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + ":TRACe:DATA? TRACE1" + " 2"; //read until new line
	data = partnerDevice("gpibController").execute(queryString.c_str()); 

	while(i < data.length())
	{
		j = 0;
		while( (data.at(i) != ',') & (i < data.length() - 1) )
		{
			j++;
			i++;
		}
		if(i < (data.length()-1) )
		{
			dataString = data.substr(i-j, j);
			dataDouble = atof( const_cast<char*>(dataString.c_str()) );
			DAQ_vector.push_back(dataDouble);
		}
		i++;
	}

	double dataLength = static_cast<double>(DAQ_vector.size());
	double fractionLength = 0;

	for(double k = 1; k <= dataLength; k++)
	{
		fractionLength = (k/dataLength);
		freqValue = fractionLength*(stopFrequency - startFrequency) + startFrequency;
		FREQ_vector.push_back(freqValue);
	}

	return true;
}
*/
