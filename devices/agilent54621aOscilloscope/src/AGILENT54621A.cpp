/*************************************************************************
 *
 * Name:   AGILENT54621A.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the AGILENT54621A oscilloscope via the ENET_GPIB_device interface
 *
 * David Johnson 7/16/2009
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 



#include "ENET_GPIB_device.h"
#include "AGILENT54621A.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




//===========================================================================

agilent54621A::agilent54621A()
{
	//stuff	
	primary_address = 7;
	secondary_address = 0;
	//maximumAllowedPower = maxPower;

	//output_off();

}

//===========================================================================

agilent54621A::~agilent54621A()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}
std::string agilent54621A::quickQuery(std::string query, int length)
{
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(query.c_str()), buffer, length);
	std::string result = std::string(buffer);
	return result;
}
bool agilent54621A::quickCommand(std::string command)
{
	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command.c_str()), buffer, 100);
	return true;
}
void agilent54621A::what_is_my_name()
{
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*idn?", buffer, 100);
	printf ("%s\n\n", buffer);
	//ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*opt?", buffer, 100);
	//printf ("%s\n\n", buffer);
}
bool agilent54621A::setWaveformPoints(int numberPoints)
{
	std::ostringstream convertPoints;
	convertPoints << numberPoints;
	std::string pointsStr = convertPoints.str();
	std::string command_str = ":WAV:POINTS " + pointsStr;
	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	return true;
}
bool agilent54621A::setupTrigger(std::string source, std::string mode, std::string slope)
{
	if( (source.compare("Channel1") == 0) || (source.compare("Channel2") == 0) || (source.compare("External") == 0) )
	{
		std::string command_str = ":TRIGger:SOURce " + source;
		ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
		ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, ":TRIGger:MODE EDGE", buffer, 100);
		ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, ":TRIG:Slope POS", buffer, 100);
		//check
		ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, ":TRIGger:SOURce?", buffer, 100);
		printf ("%s\n\n", buffer);
		
		bool generalSuccess = quickCommand(":TRIGGER:SWEEP AUTO; LEVEL 1.0");

		std::cerr << "Trigger Mode: " << quickQuery(":TRIG:MODE?", 100) << std::endl;
		return true;
	}
	else
	{
		std::cerr << "Invalid Trigger Source" << std::endl;
		return false;
	}
}
bool agilent54621A::setupAcquisition()
{
	bool generalSuccess = false;
	generalSuccess = quickCommand(":CHANNEL1:PROBE 1; RANGE 0.2; OFFSET 0.00; COUPLING DC");
	generalSuccess = quickCommand(":TIMEBASE:MODE NORMAL; RANGE 1E-2; DELAY -5.0E-3");
	
	bool timebaseSuccess = quickCommand(":TIMebase:REF CENT");
	bool acqTypeSuccess = quickCommand(":ACQUIRE:TYPE normal");
	acqTypeSuccess = quickCommand(":ACQUIRE:COUNT 1");
	std::cerr << "Timebase Ref: " << quickQuery(":TIMebase:REF?", 100) << std::endl;
	std::cerr << "Acquire Type: " << quickQuery(":ACQUIRE:TYPE?", 100) << std::endl;
	std::cerr << "Acquire Count: " << quickQuery(":ACQUIRE:COUNT?", 100) << std::endl;
	std::cerr << "Acquire Count: " << quickQuery(":ACQUIRE:POINTS?", 100) << std::endl;

	bool setupSuccess = false;
	//Digitize the Waveform that is showing on display
	
	bool digitizeSuccess = quickCommand(":ACQuire:COMPlete");
	std::cerr << "Acquire Status? " << quickQuery(":ACQuire:COMPlete?", 100) << std::endl;
	digitizeSuccess = quickCommand(":ACQuire:MODE ETIMe");
	std::cerr << "Acquire Status? " << quickQuery(":ACQuire:MODE?", 100) << std::endl;

	//Setup waveform transfer
	
	setupSuccess = quickCommand(":WAV:SOURCE CHANnel1");	
	setupSuccess = quickCommand(":WAVeform:FORMat ASCII");
	setupSuccess = quickCommand(":WAVeform:Unsigned 1");
	setupSuccess = quickCommand(":WAV:BYT LSBFIRST");
	setupSuccess = setWaveformPoints(2000);
	//setupSuccess = quickCommand(":WAV:POINTS MAX");

	std::cerr << "Waveform Preamble: " << quickQuery(":WAV:PREAMBLE?", 100) << std::endl;

	return true;
}
bool agilent54621A::queryScalingInformation()
{
	bool success = true;
	std::stringstream convert;
	
	convert << quickQuery(":WAV:YREFerence?", 100);
	convert >> yRef;
	std::cerr << ":WAV:YREFerence? " << yRef << std::endl;

	convert << quickQuery(":WAV:YORigin?", 100);
	convert >> yOrigin;
	std::cerr << ":WAV:YORigin? " << yOrigin << std::endl;

	convert << quickQuery(":WAV:YINCrement?", 100);
	convert >> yIncrement;
	std::cerr << ":WAV:YINCrement? " << yIncrement << std::endl;

	convert << quickQuery(":WAV:XREFerence?", 100);
	convert >> xRef;
	std::cerr << ":WAV:XREFerence? " << xRef << std::endl;

	convert << quickQuery(":WAV:XORigin?", 100);
	convert >> xOrigin;
	std::cerr << ":WAV:XORigin? " << xOrigin << std::endl;

	convert << quickQuery(":WAV:XINCrement?", 100);
	convert >> xIncrement;
	std::cerr << ":WAV:XINCrement? " << xIncrement << std::endl;
	
	return success;
}
std::string agilent54621A::saveData()
{
	bool setupSuccess = false;

	bool digitizeSuccess = quickCommand(":DIGitize CHANnel1");
	
	std::string result;
	
	bool dataSuccess = ENET_GPIB_device::readUntilTerminationCharacter (GPIBinterface, primary_address, secondary_address, ":WAV:DATA?", result);

	digitizeSuccess = quickCommand(":RUN");
	
	return result;
}
bool agilent54621A::parseData(std::string &data, std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector)
{
	std::string dataString;
	double dataDouble;
	double xValue;
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
			//convert << dataString;
			//convert >> dataDouble;
			dataDouble = atof( const_cast<char*>(dataString.c_str()) );
			dataPoint++;

			//dataDouble = (dataDouble - yRef)*yIncrement + yOrigin;
			xValue = (dataPoint - xRef)*xIncrement + xOrigin;

			FREQ_vector.push_back(xValue);
			DAQ_vector.push_back(dataDouble);
		}

		i++;
		//std::cerr << dataPoint << std::endl;
	}

	return true;
}

