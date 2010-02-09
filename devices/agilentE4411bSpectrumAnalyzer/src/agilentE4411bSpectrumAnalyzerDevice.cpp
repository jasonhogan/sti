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
							unsigned short primaryGPIBAddress) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	primaryAddress = primaryGPIBAddress; //normally 1
	secondaryAddress = 0;
	gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	numberPoints = 0;
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

agilentE4411bSpectrumAnalyzerDevice::~agilentE4411bSpectrumAnalyzerDevice()
{
}


void agilentE4411bSpectrumAnalyzerDevice::defineAttributes() 
{
	addAttribute("GPIB ID", gpibID); //response to the IDN? query
}

void agilentE4411bSpectrumAnalyzerDevice::refreshAttributes() 
{
	setAttribute("GPIB ID", gpibID); //will send the IDN? query
	setAttribute("Piezo Gain", (true ? "High" : "Low"));
}

bool agilentE4411bSpectrumAnalyzerDevice::updateAttribute(string key, string value)
{
	//converts desired command into GPIB command string and executes via gpib controller partner device
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);
	//bool commandSuccess;
	bool success = false;
	string result;

	if(key.compare("GPIB ID") == 0)
	{
		gpibID = queryDevice("*idn?");
		if(gpibID.compare("") == 0)
			success = false;
		else
			success = true;
		std::cerr << "Identification: " << gpibID << std::endl;
	}
	else if(key.compare("blah blah blah") == 0)
	{
		
	}
	else if(key.compare("Piezo Voltage (V)") == 0)
	{
		/*
		bool successPiezoVoltage = stringToValue(value, newPiezoVoltage);
		if(successPiezoVoltage && newPiezoVoltage < 117.5 && newPiezoVoltage > 0) 
		{
			std::string piezoCommand = ":SOUR:VOLT:PIEZ " + value;
			std::cerr << "piezo_command_str: " << piezoCommand << std::endl;
			commandSuccess = commandDevice(piezoCommand);
			std::cerr << "device successfully commanded"<< std::endl;
			if(commandSuccess)
			{
				result = queryDevice(":SOUR:VOLT:PIEZ?");
				if(result.compare("") == 0)
					success =  false;
				else
				{	
					successPiezoVoltage = stringToValue(result, piezoVoltage);
					success = true;
				}
			}
			else
				success = false;
			}
		else
		{
			std::cerr << "The desired voltage is outside of the allowed range." << std::endl;
			success = false;
		}
		*/
	}


	return success;
}

void agilentE4411bSpectrumAnalyzerDevice::defineChannels()
{
}

bool agilentE4411bSpectrumAnalyzerDevice::writeChannel(const RawEvent& Event)
{
	return false;
}

bool agilentE4411bSpectrumAnalyzerDevice::readChannel(DataMeasurement& Measurement)
{
	return false;
}

void agilentE4411bSpectrumAnalyzerDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void agilentE4411bSpectrumAnalyzerDevice::definePartnerDevices()
{
	addPartnerDevice("gpibController", "eplittletable.stanford.edu", 0, "gpib"); //local name (shorthand), IP address, module #, device name as defined in main function
}

void agilentE4411bSpectrumAnalyzerDevice::stopEventPlayback()
{
}

std::string agilentE4411bSpectrumAnalyzerDevice::execute(int argc, char **argv)
{
	string commandString;
	string commandValue;
	
	int query = 0; //true (1) or false (0) if the command is expecting a response
	double measuredValue = 0;
	//bool commandSuccess;
	//double commandValue;
	//bool outputSuccess;
	string result;

	//command comes as "attribute value query?"
	if(argc == 5)
	{
		commandValue = argv[4];
		commandString = ":SOUR:VOLT:PIEZ " + commandValue;
		result = commandDevice(commandString);
	}
	if(argc == 4)
	{
		result = queryDevice(":SOUR:VOLT:PIEZ?");
		return result;
	}
	else
		return "0"; //command needs to contain 2 pieces of information

	/*
	if(commandSuccess)
	{
		outputSuccess = setAttribute(attribute, commandValue); //will only work with attributes that take doubles
		
		if(outputSuccess)
			return "1";
		else
			return "0";
	}
	else
		return "0";	
	*/
}
bool agilentE4411bSpectrumAnalyzerDevice::deviceMain(int argc, char **argv)
{
	return false;
}
std::string agilentE4411bSpectrumAnalyzerDevice::queryDevice(std::string query)
{
	std::string queryString;
	std::string result;
	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + query + " 1";
	std::cerr << "query_str: " << queryString << std::endl;

	result = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

	return result;
}
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

	queryString = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + ":TRACe:DATA? TRACE1" + " 2"; //read until new line
	data = partnerDevice("gpibController").execute(queryString.c_str()); //usage: partnerDevice("lock").execute("--e1");

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
bool agilentE4411bSpectrumAnalyzerDevice::commandDevice(std::string command)
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