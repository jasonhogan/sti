/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief source for the class STF_DDS_Device
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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

#include "stf_dds_device.h"

STF_DDS_Device::STF_DDS_Device(ORBManager* orb_manager, std::string configFilename) :
FPGA_Device(orb_manager, "DDS", configFilename)
{

	dds_parameters.push_back(DDS_Parameters());
	dds_parameters.push_back(DDS_Parameters());
	dds_parameters.push_back(DDS_Parameters());
	dds_parameters.push_back(DDS_Parameters());

	updateDDS = false;
	IOUpdate = true;
	notInitialized = true;

	initialized = false;
	sweepMode = false;
	sweepOnLastCommand = false;
	
	ExternalClock = false;
	extClkFreq = 25.0; // in MHz

	crystalFreq = 25.0; // in MHz
	sampleFreq = 500.0; //in MSPS
	SYNC_CLK = sampleFreq / 4;	//In MHz, even though sampleFreq is in MSPS
	PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / crystalFreq)); // valid values are 4-20. Multiplier for the input clock. 20*25 MHz crystal = 500 MHz -> 0x80000000 = 250 MHz
	
	activeChannel = 0; //corresponds to channel 0
	VCOEnable = true;

	ModulationLevel = 0; // set to 0 for now

	eventSpacing = 800; //minimum time between events
	holdOff = 326;

	minimumAbsoluteStartTime = 50000;

}

	
void STF_DDS_Device::defineAttributes()
{
	//addAttribute("Initialized", "true", "true, false");
	addAttribute("External Clock", "false", "true, false"); //Use external clock?
	addAttribute("External Clock Frequency", extClkFreq); //External Clock Frequency in MHz
}

void STF_DDS_Device::refreshAttributes()
{
	//setAttribute("Initialized", (initialized ? "true" : "false"));
	setAttribute("External Clock", (ExternalClock ? "true" : "false")); //Use external clock?
	setAttribute("External Clock Frequency", extClkFreq); //External Clock Frequency in MHz
}

bool STF_DDS_Device::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	if(key.compare("Initialized") == 0)
	{
		if(value.compare("true") == 0 && !initialized)
		{
			initialized = true;
			restoreDefaults();
			for(unsigned i = 0; i < 4; i++)
			{
				RawEvent rawEvent(50000, i, 0);
				rawEvent.setValue( "Initialize" );
				write(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
			}
		}
		else if(value.compare("false") == 0)
			initialized = false;
		success = true;
	}
	else if(key.compare("External Clock") == 0)
	{
		success = true;
		if(value.compare("false") == 0 && ExternalClock)
			ExternalClock = false;
		else if(value.compare("true") == 0 && !ExternalClock)
			ExternalClock = true;

		if(ExternalClock)
		{
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / extClkFreq)); 
			RawEvent rawEvent(50000, activeChannel, 0);
			rawEvent.setValue( "Initialize" );
			write(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
		}
		else
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / crystalFreq)); 

		
		
	}		
	else if(key.compare("External Clock Frequency") == 0 && successDouble)
	{
		success = true;
		if(extClkFreq != tempDouble && successDouble)
			extClkFreq = tempDouble;
		
		if(ExternalClock)
		{
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / extClkFreq)); 
			RawEvent rawEvent(50000, activeChannel, 0);
			rawEvent.setValue( "Initialize" );
			write(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
		}
	}
	else
		success = false;
		 
	return success;
}

void STF_DDS_Device::defineChannels()
{
	addOutputChannel(0, ValueVector);
	addOutputChannel(1, ValueVector);
	addOutputChannel(2, ValueVector);
	addOutputChannel(3, ValueVector);
}

void STF_DDS_Device::parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut) 
throw(std::exception)
{
	RawEventMap::const_iterator events;
	
	double lastEventTime = 10*eventSpacing;

	vector<int> commandList; 

	//always initialze dds before every timing file, just in case. 
	
	for(unsigned k = 0; k < 4; k++)
	{
		activeChannel = k;
		setNormalMode(activeChannel);
		for (unsigned j = 0; j < 11; j++ )
		{
			if(j == 10)
				IOUpdate = true;
			else
				IOUpdate = false;

			lastEventTime = lastEventTime + eventSpacing;
			eventsOut.push_back( generateDDScommand( lastEventTime, j) );
		}

		dds_parameters.at(k).ClearPhase = false; // IOUpdate automatically resets this to false, so we want to keep internal code state to be the same as external device state

	}



	
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		commandList.clear();
		
		if(events->second.size() > 1) // only one event at a time
			throw EventConflictException(events->second.at(0), events->second.at(1),
						"The DDS currently only supports one event at each time.");
		
		if(events->second.at(0).channel() != activeChannel) // check the channel and change it if necessary
		{
			activeChannel = events->second.at(0).channel();
			commandList.push_back(0x00);
		}

		// check what type of event it is
		if(events->second.at(0).getValueType() == MixedValue::Vector)	//three values given
			parseVectorType( events->second.at(0), &commandList);
		else if(events->second.at(0).getValueType() == MixedValue::String)
			parseStringType( events->second.at(0), &commandList);
		else
			throw EventParsingException(events->second.at(0),
						"The DDS does not support that data type.");

		if( (events->first - eventSpacing * commandList.size() ) < lastEventTime) // trying to make events happen before the last event has finished
			throw EventConflictException(events->second.at(0), "There is not enough time since the previous event." );
		
		for(unsigned int i = 0; i < commandList.size(); i++)
		{
			if( i < (commandList.size() - 1) )
				IOUpdate = false;
			else
			{
				IOUpdate = true;
				dds_parameters.at(activeChannel).profilePin = dds_parameters.at(activeChannel).sweepOnLastCommand;
				
			}

			eventsOut.push_back( generateDDScommand( events->first - eventSpacing * (commandList.size() - i + dds_parameters.at(activeChannel).sweepUpFast) + holdOff, commandList.at(i)) );
		}

		if(dds_parameters.at(activeChannel).sweepUpFast)
		{
			dds_parameters.at(activeChannel).sweepOnLastCommand = false;
			dds_parameters.at(activeChannel).profilePin = dds_parameters.at(activeChannel).sweepOnLastCommand;
			eventsOut.push_back( generateDDScommand( events->first - eventSpacing + holdOff, 0x0c) );

			dds_parameters.at(activeChannel).risingDeltaWord  = dds_parameters.at(activeChannel).fallingDeltaWord;
			dds_parameters.at(activeChannel).risingDeltaWordInMHz = dds_parameters.at(activeChannel).fallingDeltaWordInMHz;
			dds_parameters.at(activeChannel).sweepUpFast = false;

			eventsOut.push_back( generateDDScommand( events->first + holdOff, 0x08) );

		}


		lastEventTime = events->first;

	}
	/*
	for(unsigned i = 0; i < 4; i++)
	{		
		if(dds_parameters.at(i).profilePin)
		{
			std::cerr << "DDS left at end of a sweep. Set to function generator mode and frequency moved to that position." << std::endl;
			activeChannel = i;
			dds_parameters.at(activeChannel).Frequency = dds_parameters.at(activeChannel).sweepEndPoint;
			dds_parameters.at(activeChannel).FrequencyInMHz = dds_parameters.at(activeChannel).sweepEndPointInMHz;
			setNormalMode(activeChannel);
			for (unsigned j = 0; j < 11; j++ )
			{
				if(j == 10)
					IOUpdate = true;
				else
					IOUpdate = false;

				lastEventTime = lastEventTime + eventSpacing;
				eventsOut.push_back( generateDDScommand( lastEventTime, j) );
			}
		}
	}
	*/
}
bool STF_DDS_Device::checkSettings()
{
	// check that values are within range
	if(dds_parameters.at(activeChannel).FrequencyInMHz > sampleFreq || dds_parameters.at(activeChannel).FrequencyInMHz < 0)
	{
		errorMessage = "Frequency must be between 0 and " + valueToString(sampleFreq) + " MHz";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	else
		dds_parameters.at(activeChannel).Frequency = generateDDSfrequency(dds_parameters.at(activeChannel).FrequencyInMHz);

	if(dds_parameters.at(activeChannel).AmplitudeInPercent > 100.0 || dds_parameters.at(activeChannel).AmplitudeInPercent < 0)
	{
		errorMessage = "Amplitude must be between 0 and 100%";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	else
		dds_parameters.at(activeChannel).Amplitude = generateDDSamplitude(dds_parameters.at(activeChannel).AmplitudeInPercent);
	
	if(dds_parameters.at(activeChannel).PhaseInDegrees > 360.0 || dds_parameters.at(activeChannel).PhaseInDegrees < 0)
	{
		errorMessage = "Phase must be between 0 and 360 degrees";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	else
		dds_parameters.at(activeChannel).Phase = generateDDSphase(dds_parameters.at(activeChannel).PhaseInDegrees);

	return true;
}
bool STF_DDS_Device::parseVectorType( RawEvent eventVector, vector<int> * commandList)
{
	bool sweep = false;
	double startVal, endVal, rampTime;
	unsigned sizeOfTuple = eventVector.value().getVector().size();

	if(sizeOfTuple > 3)
	{
		throw EventParsingException(eventVector, "DDS commands must be a tuple with 1 to 3 doubles (freq,ampl*,phase*)");
		return false;
	}

	for(unsigned i = 0; i < sizeOfTuple; i++)
	{
		if( eventVector.value().getVector().at(i).getType() != MixedValue::Vector )
		{
			double doubleValue = eventVector.value().getVector().at(i).getDouble();
			if(i == 0)
				dds_parameters.at(activeChannel).FrequencyInMHz = doubleValue;
			else if(i == 1)
				dds_parameters.at(activeChannel).AmplitudeInPercent = doubleValue;
			else if(i == 2)
				dds_parameters.at(activeChannel).PhaseInDegrees = doubleValue;
		}
		else //means it is a MixedValue::Vector
		{
			if(sweep) //means more than one sweep at a time...
			{
				throw EventParsingException(eventVector, "Can't sweep two variables at once");
				return false;
			}
			else // this means we're good! We can start parsing the sweep
			{
				sweep = true;
				unsigned sizeOfSweep = eventVector.value().getVector().at(i).getVector().size();
				if(sizeOfSweep != 3)
				{
					throw EventParsingException(eventVector, "Sweep command should be (startVal, endVal, rampTime)");
					return false;
				}
				startVal = eventVector.value().getVector().at(i).getVector().at(0).getDouble();
				endVal = eventVector.value().getVector().at(i).getVector().at(1).getDouble();
				rampTime = eventVector.value().getVector().at(i).getVector().at(2).getDouble();

				if(i != 0)
				{
					throw EventParsingException(eventVector, "we only parse frequency sweeps currently");
					return false; // we only parse frequency sweeps currently
				}
				
				if( !parseFrequencySweep(startVal, endVal, rampTime) )
				{
					throw EventParsingException(eventVector, errorMessage);
					return false; //this sets the required settings for the sweep
				}
			}
		}
	}

	if(!checkSettings())
	{
		throw EventParsingException(eventVector, errorMessage);
		return false; //checks that amp, phase, freq have values within the allowed range and it converts MHz, %, degrees to ints.
	}
	if(!sweep)
	{	
		if(dds_parameters.at(activeChannel).sweepMode)
		{
			setNormalMode(activeChannel);
			for (unsigned j = 0; j < 11; j++ )
				commandList->push_back(j);
		}
		else
		{
			// push back into commandList...
			commandList->push_back(0x04); //set frequency
			commandList->push_back(0x05); //set phase
			commandList->push_back(0x06); // set amplitude
		}
	}
	else
	{
		//parse a sweep commands
		//std::cerr << "oh you're trying to sweep, are you?" << std::endl;
		
		dds_parameters.at(activeChannel).ClearSweep = true;
		commandList->push_back(0x02); //set sweep clear
		dds_parameters.at(activeChannel).ClearSweep = false;
		commandList->push_back(0x02); //set sweep clear
		
		if(!dds_parameters.at(activeChannel).sweepMode)
		{
			setSweepMode(activeChannel);
			for (unsigned j = 0; j < 11; j++ )
				commandList->push_back(j);

			
		}
		else
		{
			commandList->push_back(0x04); //set frequency
			commandList->push_back(0x05); //set phase
			commandList->push_back(0x07); //ramp rates
			commandList->push_back(0x08); //delta word up
			commandList->push_back(0x09); // delta word down
			commandList->push_back(0x0a); // end point
		}
		
		commandList->push_back(0x0c); //random address for starting the sweep
	}
				
	return true;
}

bool STF_DDS_Device::parseFrequencySweep(double startVal, double endVal, double rampTime)
{
	//SSR = ;
	//deltaWordInMHz = ;  //AD9959 spec sheet, page 25: deltaFreq = (DW / 2^32) * SYSCLK (Hz)
	//DW = ;  //AD9959 spec sheet, page 25: deltaPhase = (DW / 2^14) * 360 (degrees)
	//DW = ;  //AD9959 spec sheet, page 25: deltaAmpl = (DW / 2^10) * 1024 (DAC full scale current)	
	//deltaF = endVal - startVal;
	uInt32 RSRR;
	double numberOfPoints;
	double actualTime;
	uInt32 deltaWord;
	bool solution = false;

	if( startVal < 0 || startVal > sampleFreq || endVal < 0 || endVal > sampleFreq )
	{
		errorMessage = "Ramp frequencies must be between 0 and " + valueToString(sampleFreq) + " MHz.";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	if(rampTime < 0)
	{
		errorMessage = "The sweep time must be positive.";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	
	RSRR = 1;
	while(!solution)
	{
		
		numberOfPoints = (rampTime * SYNC_CLK) / (1000 * RSRR);		// rampTime (ns) * SYNC_CLK (MHz) --> 10^-3
		deltaWord = generateDDSfrequency( abs(endVal - startVal) / numberOfPoints );
		
		//std::cerr << "RSRR is: " << RSRR << std::endl;
		//std::cerr << "deltaWord is: " << deltaWord << std::endl;

		actualTime = abs( (4294967296 * RSRR * abs(startVal - endVal) ) /(deltaWord * sampleFreq * SYNC_CLK * 1000000))*1000000000;

		if(deltaWord == 0 || (abs(actualTime - rampTime) > 0.002 * rampTime) )
			RSRR++;
		else
			solution = true;

		if (RSRR > 255.0)
		{
			//errorMessage = string("The minimum sweep range is ") + valueToString( generateDDSfrequencyInMHz(1) * 1000000 ) + string(" Hz.");
			errorMessage = string("You broke the sweep range algorithm. Please don't sweep as far, or if you must, do it faster");
			std::cerr << errorMessage << std::endl;
			return false;
		}
	}

	dds_parameters.at(activeChannel).fallingSweepRampRate = RSRR;
	dds_parameters.at(activeChannel).fallingSweepRampRateInPercent = 100;
	dds_parameters.at(activeChannel).risingSweepRampRate = RSRR;
	dds_parameters.at(activeChannel).risingSweepRampRateInPercent = 100;


	if(numberOfPoints < 1)
	{
		errorMessage = string("The minimum sweep time is ") + valueToString(1000 / SYNC_CLK) + string(" ns.");
		std::cerr << errorMessage << std::endl;
		return false;
	}
	
	if(endVal > startVal)
	{
		dds_parameters.at(activeChannel).FrequencyInMHz = startVal;
		dds_parameters.at(activeChannel).Frequency = generateDDSfrequency(startVal);
		dds_parameters.at(activeChannel).sweepEndPointInMHz = endVal;
		dds_parameters.at(activeChannel).sweepEndPoint = generateDDSfrequency(endVal);
		dds_parameters.at(activeChannel).risingDeltaWord  = deltaWord;
		dds_parameters.at(activeChannel).risingDeltaWordInMHz = generateDDSfrequencyInMHz( deltaWord );
		dds_parameters.at(activeChannel).sweepUpFast = false;
	}
	else
	{
		//std::cerr << "Sweep fast up, then sweep down" << std::endl;
		dds_parameters.at(activeChannel).FrequencyInMHz = endVal;
		dds_parameters.at(activeChannel).Frequency = generateDDSfrequency(endVal);
		dds_parameters.at(activeChannel).sweepEndPointInMHz = startVal;
		dds_parameters.at(activeChannel).sweepEndPoint = generateDDSfrequency(startVal);
		dds_parameters.at(activeChannel).risingDeltaWord  = (uInt32)(2147483647.0 * 2 - 1);
		dds_parameters.at(activeChannel).risingDeltaWordInMHz = generateDDSfrequencyInMHz( dds_parameters.at(activeChannel).risingDeltaWord );
		dds_parameters.at(activeChannel).sweepUpFast = true;
	}

	dds_parameters.at(activeChannel).fallingDeltaWord  = deltaWord;
	dds_parameters.at(activeChannel).fallingDeltaWordInMHz = generateDDSfrequencyInMHz( deltaWord );

	
	dds_parameters.at(activeChannel).sweepOnLastCommand = true;
	
	return true;

}
bool STF_DDS_Device::parseStringType( RawEvent eventString, vector<int> * commandList)
{
	unsigned tempInt;
	activeChannel = eventString.channel();
	if(!checkSettings())
			return false; //check that freq, amp, phase are all correct and convert them to hex values
	if( eventString.stringValue().compare("Initialize") == 0 )
	{
		for (unsigned j = 0; j < 11; j++ )
			commandList->push_back(j);
	}
	else
	{
		bool success = stringToValue(eventString.stringValue(), tempInt);
		if(success)
			commandList->push_back(tempInt);
	}

	return true;
}
uInt32 STF_DDS_Device::generateDDSphase(double doublePhase)
{
	uInt32 hexPhase = 0;
	hexPhase = static_cast<uInt32>(floor((doublePhase / 360.0) * 16383.0));
	return hexPhase;
}
uInt32 STF_DDS_Device::generateDDSfrequency(double doubleFrequency)
{
	uInt32 hexFrequency = 0;
	if(ExternalClock)
		hexFrequency = static_cast<uInt32>(floor((doubleFrequency / sampleFreq) * 2147483647.0 * 2));
	else
		hexFrequency = static_cast<uInt32>(floor((doubleFrequency / sampleFreq) * 2147483647.0 * 2));
	
	return hexFrequency;
}

double STF_DDS_Device::generateDDSfrequencyInMHz(uInt32 hexFrequency)
{
	double doubleFrequency = 0;
	if(ExternalClock)
		doubleFrequency = static_cast<double>((hexFrequency * sampleFreq) / 2147483647.0 * 2);
	else
		doubleFrequency = static_cast<double>((hexFrequency * sampleFreq) / 2147483647.0 * 2);
	
	return doubleFrequency;
}

uInt32 STF_DDS_Device::generateDDSamplitude(double doubleAmplitude)
{
	uInt32 hexAmplitude = 0;
	hexAmplitude = static_cast<uInt32>(floor((doubleAmplitude / 100.0) * 1023.0)); //in percent
	return hexAmplitude;
}
uInt32 STF_DDS_Device::generateRampRate(double rampRateInPercent)
{
	return static_cast<uInt32>(floor(((100.0 - rampRateInPercent) / 100.0) * 255.0));
}
STF_DDS_Device::DDS_Event* STF_DDS_Device::generateDDScommand(double time, uInt32 addr)
{
	
	//DDSParameters& ddsCh = dds_parameters.at(activeChannel); //a shorthand way to use a reference

	DDS_Event* ddsCommand = new DDS_Event(time, 0, 0, this);
	ddsCommand->setBits(0);
	ddsCommand->setBits(addr, 32, 36);	//5 bit address
	ddsCommand->setBits(!IOUpdate, 48, 48); //if we do not want an IO Update, set this bit high
	ddsCommand->setBits(ExternalClock, 40, 40);
	
	//retain sweep state for all channels - so need to keep profile pins high if the channel is to stay at it's sweep end point
	for(unsigned i = 0; i < 4; i++)
		ddsCommand->setBits(dds_parameters.at(i).profilePin, 41 + i, 41 + i); //selects which channel to sweep based on active channel


	if (addr == 0x00)	//set active channel
	{
		ddsCommand->setBits(1, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(true, 28 + activeChannel, 28 + activeChannel);
	}
	else if (addr == 0x01)
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(VCOEnable, 31, 31); //turn on VCO control
		ddsCommand->setBits(PLLmultiplier, 26, 30); // set PLLmultiplier value (allowed 4-20)
		ddsCommand->setBits(ModulationLevel, 16, 17);
	}
	else if (addr == 0x02)	//???
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)	
	}
	else if (addr == 0x03)	//parameters...
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).AFPSelect, 30, 31);
		ddsCommand->setBits(dds_parameters.at(activeChannel).LSnoDwell, 23, 23);
		ddsCommand->setBits(dds_parameters.at(activeChannel).LinearSweepEnable, 22, 22);
		ddsCommand->setBits(dds_parameters.at(activeChannel).LoadSRR, 21, 21);
		ddsCommand->setBits(0, 18, 18); //Must be 0
		ddsCommand->setBits(dds_parameters.at(activeChannel).DACCurrentControl, 16, 17); //DAC full scale current control - set to default value of 0x03
		ddsCommand->setBits(dds_parameters.at(activeChannel).AutoclearSweep, 12, 12);
		ddsCommand->setBits(dds_parameters.at(activeChannel).ClearSweep, 11, 11);
		ddsCommand->setBits(dds_parameters.at(activeChannel).AutoclearPhase, 10, 10);
		ddsCommand->setBits(dds_parameters.at(activeChannel).ClearPhase, 9, 9);
		ddsCommand->setBits(dds_parameters.at(activeChannel).SinCos, 8, 8);		
	}
	else if (addr == 0x04)	//Set frequency
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).Frequency, 0, 31);
	}
	else if (addr == 0x05)	//Set phase
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).Phase, 16, 31);
	}
	else if (addr == 0x06)	//Set amplitude
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).AmplitudeEnable, 20, 20);
		ddsCommand->setBits(dds_parameters.at(activeChannel).Amplitude, 8, 17);
		ddsCommand->setBits(dds_parameters.at(activeChannel).LoadARR, 18, 18);
		ddsCommand->setBits(dds_parameters.at(activeChannel).RuRd, 19, 19);
	}
	else if (addr == 0x07)
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).fallingSweepRampRate, 24, 31); //RSRR has 8 bit resolution
		ddsCommand->setBits(dds_parameters.at(activeChannel).risingSweepRampRate, 16, 23); //FSRR has 8 bit resolution
	}
	else if (addr == 0x08)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).risingDeltaWord, 0, 31); //Frequency has 32 bit resolution
	}
	else if (addr == 0x09)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).fallingDeltaWord, 0, 31); //Frequency has 32 bit resolution
	}
	else if (addr == 0x0a)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(dds_parameters.at(activeChannel).sweepEndPoint, 0, 31); //Frequency has 32 bit resolution
	}
	else
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
	}

	/*
	std::cerr << "ddsCommand(" << addr << "): " ;
	
	for(int i = 63; i >= 0; i--)
	{
		std::cerr << ( ddsCommand->getBits(i,i) ? "1" : "0" );
	}
	std::cerr << std::endl;
	*/

	return ddsCommand;

}


STF_DDS_Device::DDS_Event::DDS_Event(double time, uInt32 command, uInt32 value, FPGA_Device* device) : 
FPGA_BitLineEvent<64>(time, device), device_f(device)
{
	setBits(command, 32, 63);
	setBits(value, 0, 31);
}

void STF_DDS_Device::DDS_Event::setupEvent()
{
	time32 = static_cast<uInt32>( getTime() / 10 );	//in clock cycles! (1 cycle = 10 ns)
	timeAddress  = device_f->ramBlock.getWrappedAddress( 3*getEventNumber() );
	commandAddress = device_f->ramBlock.getWrappedAddress( 3*getEventNumber() + 1 );
	valueAddress = device_f->ramBlock.getWrappedAddress( 3*getEventNumber() + 2 );
}

void STF_DDS_Device::DDS_Event::loadEvent()
{
	//write the event to RAM
	device_f->ramBus->writeDataToAddress( time32, timeAddress );
	device_f->ramBus->writeDataToAddress( getBits(32, 63), commandAddress );
	device_f->ramBus->writeDataToAddress( getBits(0, 31), valueAddress );
}

//void STF_DDS_Device::DDS_Event::waitBeforePlay()
//{
//	device_f->waitForEvent( getEventNumber() );
//	cerr << "DDS waitBeforePlay() is finished " << getEventNumber() << endl;
//}

void STF_DDS_Device::setSweepMode(unsigned k)
{
	//sets an individual channel to be ready for sweeping
	dds_parameters.at(k).sweepMode = true;
	dds_parameters.at(k).AFPSelect = 2; //normally 0
	dds_parameters.at(k).LSnoDwell = false;
	dds_parameters.at(k).LinearSweepEnable = true; //false;
	dds_parameters.at(k).LoadSRR = true; //false;
	dds_parameters.at(k).AutoclearSweep = false;
	dds_parameters.at(k).ClearSweep = false;
	dds_parameters.at(k).ClearPhase = false; // Want to reset all channels to be phase coherent at the start of each DDS cycle
	dds_parameters.at(k).AmplitudeEnable = false; //normally true// We want to enable everything on initialization
	dds_parameters.at(k).LoadARR = false;
	dds_parameters.at(k).profilePin = false;

}
void STF_DDS_Device::setNormalMode(unsigned k)
{
	//sets an individual channel to be ready for sweeping
	dds_parameters.at(k).sweepMode = false;
	dds_parameters.at(k).AFPSelect = 0; //normally 0
	dds_parameters.at(k).LSnoDwell = false;
	dds_parameters.at(k).LinearSweepEnable = false;
	dds_parameters.at(k).LoadSRR = false;
	dds_parameters.at(k).AutoclearSweep = false;
	dds_parameters.at(k).ClearSweep = false;
	dds_parameters.at(k).ClearPhase = false; // Want to make all channels to be phase coherent at the start of each DDS cycle
	dds_parameters.at(k).AmplitudeEnable = true; // We want to enable everything on initialization
	dds_parameters.at(k).LoadARR = false;
	dds_parameters.at(k).profilePin = false;
	dds_parameters.at(k).sweepOnLastCommand = false;
	dds_parameters.at(k).sweepUpFast = false;

}
void STF_DDS_Device::restoreDefaults()
{
	
	//restores the device to its original state
	for(unsigned k = 0; k < 4; k++)
	{
		dds_parameters.at(k).sweepMode = false;
		dds_parameters.at(k).ChargePumpControl = 0;
		dds_parameters.at(k).ProfilePinConfig = 0;
		dds_parameters.at(k).RuRd = 0;
		dds_parameters.at(k).AFPSelect = 0; //normally 0 for function generator mode
		dds_parameters.at(k).LSnoDwell = false;
		dds_parameters.at(k).LinearSweepEnable = false;
		dds_parameters.at(k).LoadSRR = false;
		dds_parameters.at(k).AutoclearSweep = false;
		dds_parameters.at(k).ClearSweep = false;
		dds_parameters.at(k).AutoclearPhase = false;
		dds_parameters.at(k).ClearPhase = true; // Want to reset all channels to be phase coherent at the start of each DDS cycle
		dds_parameters.at(k).SinCos = false;
		dds_parameters.at(k).DACCurrentControl = 1; //set DAC current to low
		dds_parameters.at(k).AmplitudeEnable = true; // We want to enable everything on initialization
		dds_parameters.at(k).LoadARR = false;
		dds_parameters.at(k).PhaseInDegrees = 0;
		dds_parameters.at(k).FrequencyInMHz = 10;
		dds_parameters.at(k).AmplitudeInPercent = 100;
		dds_parameters.at(k).risingDeltaWordInMHz = 0.001;
		dds_parameters.at(k).fallingDeltaWordInMHz = 0.001;
		dds_parameters.at(k).sweepEndPointInMHz = 20;
		dds_parameters.at(k).risingSweepRampRateInPercent = 10;
		dds_parameters.at(k).fallingSweepRampRateInPercent = 10;
		dds_parameters.at(k).profilePin = false;
		dds_parameters.at(k).sweepOnLastCommand = false;
		dds_parameters.at(k).sweepUpFast = false;
		dds_parameters.at(k).Phase = generateDDSphase(dds_parameters.at(k).PhaseInDegrees);
		dds_parameters.at(k).Frequency = generateDDSfrequency(dds_parameters.at(k).FrequencyInMHz);
		dds_parameters.at(k).Amplitude = generateDDSamplitude(dds_parameters.at(k).AmplitudeInPercent);
		dds_parameters.at(k).risingDeltaWord = generateDDSfrequency(dds_parameters.at(k).risingDeltaWordInMHz);
		dds_parameters.at(k).fallingDeltaWord = generateDDSfrequency(dds_parameters.at(k).fallingDeltaWordInMHz);
		dds_parameters.at(k).sweepEndPoint = generateDDSfrequency(dds_parameters.at(k).sweepEndPointInMHz);
		dds_parameters.at(k).risingSweepRampRate = generateRampRate(dds_parameters.at(k).risingSweepRampRateInPercent);
		dds_parameters.at(k).fallingSweepRampRate = generateRampRate(dds_parameters.at(k).fallingSweepRampRateInPercent);
	}

}
STF_DDS_Device::DDS_Parameters::DDS_Parameters()
{
	//define initial values	
	sweepMode = false; //start out in function generator
	ChargePumpControl = 0; // higher values increase the charge pump current
	ProfilePinConfig = 0; // Determines how the profile pins are configured
	RuRd = 0; // Ramp Up / Ramp Down control
	AFPSelect = 0;
	LSnoDwell = false;
	LinearSweepEnable = false;
	LoadSRR = false;
	AutoclearSweep = false;
	ClearSweep = false;
	AutoclearPhase = false;
	ClearPhase = true; // Want to reset all channels to be phase coherent at the start of each DDS cycle
	SinCos = false;
	DACCurrentControl = 1; //set DAC current to low
	AmplitudeEnable = 1; // We want to enable everything on initialization
	LoadARR = false;
	PhaseInDegrees = 0;
	FrequencyInMHz = 10;
	AmplitudeInPercent = 100;
	risingDeltaWordInMHz = 0.001;
	fallingDeltaWordInMHz = 0.001;
	risingSweepRampRateInPercent = 10;
	fallingSweepRampRateInPercent = 10;
	sweepEndPointInMHz = 20;
	profilePin = false;
	sweepOnLastCommand = false;
	sweepUpFast = false;
	Phase = 0;//STF_DDS_Device::generateDDSphase(PhaseInDegrees);
	Frequency = 0;//STF_DDS_Device::generateDDSfrequency(FrequencyInMHz);
	Amplitude = 0;//STF_DDS_Device::generateDDSamplitude(AmplitudeInPercent);
	risingDeltaWord = 0;//STF_DDS_Device::generateDDSfrequency(risingDeltaWordInMHz);
	fallingDeltaWord = 0;//STF_DDS_Device::generateDDSfrequency(fallingDeltaWordInMHz);
	sweepEndPoint = 0;//STF_DDS_Device::generateDDSfrequency(sweepEndPointInMHz);
	risingSweepRampRate = 0;//STF_DDS_Device::generateRampRate(risingSweepRampRateInPercent);
	fallingSweepRampRate = 0;//STF_DDS_Device::generateRampRate(fallingSweepRampRateInPercent);
}
