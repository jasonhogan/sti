

#include "ENET_GPIB_device.h"
#include "AGILENT8648A.h"
#include "AGILENT54621A.h"
#include "Matlab.h"

#include <windows.h>
#include <iostream> //cin & cout commands
#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage

int main(int argc, char* argv[])

{
	//define data containers
	std::vector <double> timeVectorOff;
	std::vector <double> signalVectorOff;
	std::vector <double> timeVectorSerrodyne;
	std::vector <double> signalVectorSerrodyne;
	std::string data;


	AGILENT8648A agilent8648a(-6);
	agilent54621A Agilent54621A;
	MATLABPLOTTER matlabplotter;

	//Agilent54621A.quickCommand("*RST");
	Agilent54621A.what_is_my_name();
	bool triggerSuccess = Agilent54621A.setupTrigger("External", "Edge", "Pos");
	bool acquisitionSetupSuccess = Agilent54621A.setupAcquisition();
	bool scalingInformationSuccess = Agilent54621A.queryScalingInformation();
	
	bool parseSuccess = false;
	double frequency;
	double power;

	double outputPower = -12.4;
	double initialPower = outputPower;
	double powerIncrement = 0.2;

	double outputFrequency = 610;
	double frequencyIncrement = 10;
	
	for(unsigned int i = 0; i < 300; i++)
	{
		outputPower = initialPower;
		for(unsigned int j = 0; j < 13; j++)
		{
			agilent8648a.set_power(outputPower);
			agilent8648a.set_frequency(outputFrequency);

			data = Agilent54621A.saveData();
			parseSuccess = Agilent54621A.parseData(data, timeVectorOff, signalVectorOff);
	
			agilent8648a.output_on();
			agilent8648a.get_output_state();
	
			data = Agilent54621A.saveData();
			parseSuccess = Agilent54621A.parseData(data, timeVectorSerrodyne, signalVectorSerrodyne);
	
			agilent8648a.output_off();
			agilent8648a.get_output_state();

			matlabplotter.plotData(timeVectorOff, signalVectorOff, true);
			matlabplotter.plotData(timeVectorSerrodyne, signalVectorSerrodyne, false);

			frequency = agilent8648a.get_frequency();
			power = agilent8648a.get_power();

			matlabplotter.savedata(i*25 + j, frequency, power, timeVectorOff, signalVectorOff, timeVectorSerrodyne, signalVectorSerrodyne);
	
			timeVectorOff.clear();
			signalVectorOff.clear();
			timeVectorSerrodyne.clear();
			signalVectorSerrodyne.clear();

			outputPower = outputPower + powerIncrement;
		}
		outputFrequency = outputFrequency + frequencyIncrement;
	}

	


//	std::cout << "Continue?...";
//	std::cin >> cont;
//	if (!cont) {return 0;}

	return 0;
};

//	std::vector <std::string> recipients;
//	recipients.push_back("david.m.johnson@stanford.edu");
//	recipients.push_back("sdickers@stanford.edu");
