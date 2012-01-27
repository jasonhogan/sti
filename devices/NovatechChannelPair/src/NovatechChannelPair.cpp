/*! \file NovatechChannelPair.cpp
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



#include "NovatechChannelPair.h"


NovatechChannelPair::NovatechChannelPair(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber, 
					   unsigned short lowFreqChannel_, 
					   unsigned short highFreqChannel_, 
					   std::string partnerName_) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialization of device
	centerFreq = 100;	//this should be overwritten by the values set on the novatech
	deltaFreq = 6;		//this should be overwritten by the values set on the novatech
	
	lowFreqChannel = lowFreqChannel_;
	highFreqChannel = highFreqChannel_;
	partnerName = partnerName_;
	freqRampRate = 0;
	maxResolution = 2;


	if (partnerName.compare("Big Table Novatech")==0 || partnerName.compare("Little Table Novatech")==0)
		initialized = true;
	else
		initialized = false;
}

NovatechChannelPair::~NovatechChannelPair()
{
}


bool NovatechChannelPair::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	return false;
}

void NovatechChannelPair::defineAttributes() 
{
	//addAttribute("Center Frequency (MHz)", centerFreq);
	//addAttribute("Delta Frequency (MHz)", deltaFreq);

	

	addAttribute("Frequency Ramp Rate (MHz/ms)", freqRampRate);
	addAttribute("Min Ramp Resolution (MHz)", maxResolution);
	addAttribute("Low Freq Channel", valueToString(lowFreqChannel, ""), "0, 1, 2, 3");
	addAttribute("High Freq Channel", valueToString(highFreqChannel, ""), "0, 1, 2, 3");

	MixedValue valueIn("");
	MixedData	dataOut;
	double lowFreq = 100, highFreq = 100;

	if (partnerDevice("Novatech").read(lowFreqChannel + 10, valueIn, dataOut))
	{
		lowFreq = dataOut.getVector().at(0).getDouble();
	}
	else
		std::cerr << "Error reading Novatech" << std::endl;

	dataOut.clear();
	if (partnerDevice("Novatech").read(highFreqChannel + 10, valueIn, dataOut))
	{
		highFreq = dataOut.getVector().at(0).getDouble();
	}
	else
		std::cerr << "Error reading Novatech" << std::endl;

	centerFreq = (lowFreq + highFreq)/2;
	deltaFreq = highFreq - lowFreq;
	
}

void NovatechChannelPair::refreshAttributes() 
{

	//setAttribute("Center Frequency (MHz)", centerFreq);   //figuring out which option to set 
														//often requires a switch on some parameter
	//setAttribute("Delta Frequency (MHz)", deltaFreq); 
	setAttribute("Frequency Ramp Rate (MHz/ms)", freqRampRate);
	setAttribute("Min Ramp Resolution (MHz)", maxResolution);
	setAttribute("Low Freq Channel", valueToString(lowFreqChannel, ""));
	setAttribute("High Freq Channel", valueToString(highFreqChannel, ""));

}

bool NovatechChannelPair::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	if(key.compare("Frequency Ramp Rate (MHz/ms)") == 0 && successDouble)
	{
		success = true;

		if (tempDouble >= 0)
			freqRampRate = tempDouble;
		else
			success = false;
	}
	else if(key.compare("Min Ramp Resolution (MHz)") == 0 && successDouble)
	{
		success = true;

		if (tempDouble > 0)
			maxResolution = tempDouble;
		else
			success = false;
	}
	else if(key.compare("Low Freq Channel") == 0 && successInt)
	{
		success = true;

		if (tempInt >= 0 && tempInt < 4)
			lowFreqChannel = tempInt;
		else
			success = false;
	}
	else if(key.compare("High Freq Channel") == 0 && successInt)
	{
		success = true;

		if (tempInt >= 0 && tempInt < 4)
			highFreqChannel = tempInt;
		else
			success = false;
	}
	else
		success = false;

	return success;
}

void NovatechChannelPair::defineChannels()
{
	addOutputChannel(0, ValueNumber); //center frequency
	addOutputChannel(1, ValueNumber); //delta frequency
}

bool NovatechChannelPair::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return false;
}

bool NovatechChannelPair::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	bool success = true;
	double cFreq, dFreq, lFreq, hFreq;


	if (valuet.getType() != MixedValue::Double)
	{
		std::cerr << "Expecting a double for the input value of write" << std::endl;
		return false;
	}

	if (channel == 0) //center frequency
	{
		cFreq = valuet.getDouble();
		dFreq = deltaFreq;

	}
	else if (channel == 1)
	{
		dFreq = valuet.getDouble();
		cFreq = centerFreq;
	}
	else
		return false;

	lFreq = cFreq - dFreq/2;
	hFreq = cFreq + dFreq/2;


	double lowFreq, highFreq;
	int numSteps, numStepsLow, numStepsHigh;
	double lStepSize, hStepSize;
	MixedValue novatechValueIn;

	lowFreq = centerFreq - deltaFreq/2;
	highFreq = centerFreq + deltaFreq/2;

	numStepsLow = 1 + (int) abs(lFreq - lowFreq)/maxResolution;
	numStepsHigh = 1 + (int) abs(hFreq - highFreq)/maxResolution;
	numSteps = max(numStepsLow, numStepsHigh);

	lStepSize = (lFreq - lowFreq)/numSteps;
	hStepSize = (hFreq - highFreq)/numSteps;
	MixedData dataOutLow, dataOutHigh;
	MixedValue valueIn("");

	//Does not require a serial query to read. Novatech keeps track
	success &= partnerDevice("Novatech").read(lowFreqChannel + 10, valueIn, dataOutLow);
	success &= partnerDevice("Novatech").read(highFreqChannel + 10, valueIn, dataOutHigh);


	for (int i = 0; i < numSteps && success; i++)
	{
		novatechValueIn.clear();

		novatechValueIn.addValue(lowFreq + (i+1)*lStepSize);						//Frequency
		novatechValueIn.addValue((int) dataOutLow.getVector().at(1).getDouble());	//Amplitude
		novatechValueIn.addValue((int) dataOutLow.getVector().at(2).getDouble());	//Phase

		success &= partnerDevice("Novatech").write(lowFreqChannel, novatechValueIn);

		novatechValueIn.clear();

		novatechValueIn.addValue(highFreq + (i+1)*hStepSize);						//Frequency
		novatechValueIn.addValue((int) dataOutHigh.getVector().at(1).getDouble());	//Amplitude
		novatechValueIn.addValue((int) dataOutHigh.getVector().at(2).getDouble());	//Phase

		success &= partnerDevice("Novatech").write(highFreqChannel, novatechValueIn);

		Sleep((int) (abs(max(lStepSize,hStepSize))*freqRampRate) );
	}

	if(partnerDevice("Novatech").read(lowFreqChannel + 10, valueIn, dataOutLow))
	{
		lowFreq = dataOutLow.getVector().at(0).getDouble();

		if(partnerDevice("Novatech").read(highFreqChannel + 10, valueIn, dataOutHigh))
		{
			highFreq = dataOutHigh.getVector().at(0).getDouble();
			centerFreq = (highFreq + lowFreq)/2;
			deltaFreq = highFreq - lowFreq;
		}
	}


	return success;
}

void NovatechChannelPair::definePartnerDevices()
{
	if (partnerName.compare("Big Table Novatech") == 0)
	{
		addPartnerDevice("Novatech", "eplittletable.stanford.edu", 1, "Big Table Novatech");
	}
	else
	{
		addPartnerDevice("Novatech", "eplittletable.stanford.edu", 0, "Little Table Novatech");
	}
}

std::string NovatechChannelPair::execute(int argc, char **argv)
{
	return "";
}

void NovatechChannelPair::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void NovatechChannelPair::stopEventPlayback()
{
}