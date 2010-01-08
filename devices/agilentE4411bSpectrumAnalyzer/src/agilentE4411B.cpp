/*************************************************************************
 *
 * Name:   agilentE4411B.cpp
 *
 * C++ Windows header for Stanford Timing Interface to control the agilentE4411B spectrum analyzer via the ENET_GPIB_device interface
 *
 * David Johnson 8/4/2009
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 



#include "ENET_GPIB_device.h"
#include "agilentE4411B.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




//===========================================================================

agilentE4411B::agilentE4411B(int primaryAddress)
{
	//stuff	
	primary_address = primaryAddress;
	secondary_address = 0;
	

}

//===========================================================================

agilentE4411B::~agilentE4411B()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}
std::string agilentE4411B::quickQuery(std::string query)
{
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(query.c_str()), buffer, 100);
	std::string result = std::string(buffer);
	return result;
}
bool agilentE4411B::quickCommand(std::string command)
{
	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command.c_str()), buffer, 100);
	return true;
}
void agilentE4411B::what_is_my_name()
{
	std::cerr << "My name is: " << quickQuery("*idn?");
}
bool agilentE4411B::setWaveformPoints(int numberPoints)
{
	std::ostringstream convertPoints;
	convertPoints << numberPoints;
	std::string pointsStr = convertPoints.str();
	std::string command_str = ":WAV:POINTS " + pointsStr;
	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	return true;
}

bool agilentE4411B::setupAcquisition()
{
	
	bool setupSuccess = false;
	
	setupSuccess = quickCommand(":FORMat:TRACe:DATA ASCII");
	setupSuccess = quickCommand(":TRACE1:MODE WRITE");
	setupSuccess = quickCommand(":AVERage OFF");

	
	return setupSuccess;
}
bool agilentE4411B::queryScalingInformation()
{
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
	
	return success;
}
std::string agilentE4411B::saveData()
{
	std::string result;
	
	bool dataSuccess = ENET_GPIB_device::readUntilNewLine (GPIBinterface, primary_address, secondary_address, ":TRACe:DATA? TRACE1", result);

	return result;
}
bool agilentE4411B::parseData(std::string &data, std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector)
{
	std::string dataString;
	double dataDouble;
	double freqValue;
	unsigned int dataPoint = 0;
	//std::stringstream convert;
	unsigned int i = 0;
	unsigned int j = 0;
	
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
		//std::cerr << dataPoint << std::endl;
		
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

