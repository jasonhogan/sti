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

}

	
void STF_DDS_Device::defineAttributes()
{

	addAttribute("Initialized", "false", "true, false");
	//Attributes not set in serial commands
	//addAttribute("External Clock", "false", "true, false"); //Use external clock?
	//addAttribute("External Clock Frequency", extClkFreq); //External Clock Frequency in MHz
	
	//addAttribute("Active Channel", "0", "0, 1, 2, 3");
	// can set channel 0,1,2,3 or any combination i.e. 0xF = all channels
	//addAttribute("DAC Current", "High", "Off, Low, Medium, High"); //DAC full scale current control, 2 bits, "11" default, "10" best noise
//	addAttribute("Mode", "Function Generator", "Function Generator, Sweep, 2 Level Modulation"); //main selector switch
	
	//stuff needed for sweeps
	// Linear Sweep Rate
	//addAttribute("Rising Sweep Ramp Rate(%)", 0); //8 bits
	//addAttribute("Falling Sweep Ramp Rate(%)", 0); //8 bits
	// Rising Delta Word
	//addAttribute("Rising Delta Word", dds_parameters.at(activeChannel).risingDeltaWordInMHz); //32 bits
	// Falling Delta Word
	//addAttribute("Falling Delta Word", dds_parameters.at(activeChannel).fallingDeltaWordInMHz); //32 bits
	// Sweep End Point
	//addAttribute("Sweep End Point", dds_parameters.at(activeChannel).sweepEndPointInMHz); //32 bits
	// sweep go button
	//addAttribute("Start Sweep", "down", "up, down");
	
	//Phase
	//addAttribute("Phase", dds_parameters.at(activeChannel).PhaseInDegrees); //14 bits
	//Frequency
	//addAttribute("Frequency", dds_parameters.at(activeChannel).FrequencyInMHz); //32 bits
	//Amplitude
	//addAttribute("Amplitude", dds_parameters.at(activeChannel).AmplitudeInPercent); //10 bits allowed range 0-1023
}

void STF_DDS_Device::refreshAttributes()
{
	setAttribute("Initialized", (initialized ? "true" : "false"));
	// All attributes are stored in c++, none are on the fpga
	//Attributes not set in serial commands
	//setAttribute("External Clock", (ExternalClock ? "true" : "false")); //Use external clock?
	//setAttribute("External Clock Frequency", extClkFreq); //External Clock Frequency in MHz
	
	//setAttribute("Active Channel", activeChannel);
	// can set channel 0,1,2,3 or any combination i.e. 0xF = all channels
	//setAttribute("DAC Current", dds_parameters.at(activeChannel).DACCurrentControl); //DAC full scale current control, 2 bits, "11" default, "10" best noise
	/*
	if(dds_parameters.at(activeChannel).mode == 0)
		setAttribute("Mode", "None"); //main selector switch
	else if(dds_parameters.at(activeChannel).mode == 1)
		setAttribute("Mode", "Function Generator"); //main selector switch
	else if(dds_parameters.at(activeChannel).mode == 2)
		setAttribute("Mode", "Sweep"); //main selector switch
	else if(dds_parameters.at(activeChannel).mode == 3)
		setAttribute("Mode", "2 Level Modulation"); //main selector switch
	else
		setAttribute("Mode", "None"); //main selector switch
		*/
	
	//stuff needed for sweeps
	// Linear Sweep Rate
//	setAttribute("Rising Sweep Ramp Rate(%)", dds_parameters.at(activeChannel).risingSweepRampRateInPercent); //8 bits
//	setAttribute("Falling Sweep Ramp Rate(%)", dds_parameters.at(activeChannel).fallingSweepRampRateInPercent); //8 bits
	// Rising Delta Word
//	setAttribute("Rising Delta Word", dds_parameters.at(activeChannel).risingDeltaWordInMHz); //32 bits
	// Falling Delta Word
//	setAttribute("Falling Delta Word", dds_parameters.at(activeChannel).fallingDeltaWordInMHz); //32 bits
	// Sweep End Point
//	setAttribute("Sweep End Point", dds_parameters.at(activeChannel).sweepEndPointInMHz); //32 bits
	// sweep go button
//	setAttribute("Start Sweep", (dds_parameters.at(activeChannel).startSweep ? "up" : "down"));
	
	//Phase
	//setAttribute("Phase", dds_parameters.at(activeChannel).PhaseInDegrees); //14 bits
	//Frequency
	//setAttribute("Frequency", dds_parameters.at(activeChannel).FrequencyInMHz); //32 bits
	//Amplitude
	//setAttribute("Amplitude", dds_parameters.at(activeChannel).AmplitudeInPercent); //10 bits allowed range 0-1023
}

bool STF_DDS_Device::updateAttribute(std::string key, std::string value)
{
	bool success = false;
/*
	TDDS ddsValue;

	ddsValue.ampl.setValue( dds_parameters.at(activeChannel).AmplitudeInPercent );
	ddsValue.freq.setValue( dds_parameters.at(activeChannel).FrequencyInMHz );
	ddsValue.phase.setValue( dds_parameters.at(activeChannel).PhaseInDegrees );
*/
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
				playSingleEvent(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence

			}
		}
		if(value.compare("false") == 0)
			initialized = false;

		success = true;
	}
/*
	if(key.compare("Active Channel") == 0)
	{
		success = true;
		//stateChange = true;

		if(value.compare("0") == 0 && activeChannel != 0)
			rawEvent.setChannel(0);
		else if(value.compare("1") == 0 && activeChannel != 1)
			rawEvent.setChannel(1);
		else if(value.compare("2") == 0 && activeChannel != 2)
			rawEvent.setChannel(2);
		else if(value.compare("3") == 0 && activeChannel != 3)
			rawEvent.setChannel(3);
		else
			rawEvent.setChannel(activeChannel); //set the channel to the current active channel

		rawEvent.setValue( "Switch Mode" ); //	addr = 0x03 to change DAC Current, so just run switch mode
	}
	else if(key.compare("Mode") == 0)
	{
		success = true;
		dds_parameters.at(activeChannel).startSweep = false; //just make sure we don't have a profile pin set high
		dds_parameters.at(activeChannel).ClearSweep = true; //always want to keep the sweep counter cleared, unless we're actively sweeping

		if(value.compare("Function Generator") == 0)
		{
			dds_parameters.at(activeChannel).AFPSelect = 0;
			dds_parameters.at(activeChannel).LinearSweepEnable = false;
			dds_parameters.at(activeChannel).AmplitudeEnable = true;
			std::cerr << "My mode is Function Generator" << std::endl;
			dds_parameters.at(activeChannel).mode = 1;
		}
		else if(value.compare("Sweep") == 0)
		{
			dds_parameters.at(activeChannel).AFPSelect = 2;
			dds_parameters.at(activeChannel).LinearSweepEnable = true;
			dds_parameters.at(activeChannel).AmplitudeEnable = false;
			std::cerr << "My mode is Sweep" << std::endl;
			dds_parameters.at(activeChannel).mode = 2;
		}
		else if(value.compare("2 Level Modulation") == 0)
		{
			dds_parameters.at(activeChannel).AFPSelect = 2;
			dds_parameters.at(activeChannel).LinearSweepEnable = false;
			dds_parameters.at(activeChannel).AmplitudeEnable = false;
			std::cerr << "My mode is 2 Level Modulation" << std::endl;
			dds_parameters.at(activeChannel).mode = 3;
		}
		else
			success = false;

		rawEvent.setValue( "Switch Mode" ); //this will write all set up commands
	}
	else if(key.compare("Start Sweep") == 0)
	{
		success = true;
		dds_parameters.at(activeChannel).ClearSweep = false;


		if(value.compare("down") == 0 && dds_parameters.at(activeChannel).startSweep) //only sets parameter if it is different
		{
			dds_parameters.at(activeChannel).startSweep = false;
			stateChange = true;
		}
		else if(value.compare("up") == 0 && !dds_parameters.at(activeChannel).startSweep) //only sets parameter if it is different
		{
			dds_parameters.at(activeChannel).startSweep = true;
			stateChange = true;
		}
		else
			success = false;

		rawEvent.setValue( "0x03" ); // 
		//rawEvent.setValue( "Non Register Command" ); //this will create a dummy event that doesn't write to DDS registers
	}
	
	else if(key.compare("External Clock") == 0)
	{
		success = true;
		if(value.compare("false") == 0 && ExternalClock)
		{
			ExternalClock = false;
			stateChange = true;
		}
		else if(value.compare("true") == 0 && !ExternalClock)
		{
			ExternalClock = true;
			stateChange = true;
		}
		else
			success = false;

		if(ExternalClock)
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / extClkFreq)); 
		else
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / crystalFreq)); 

		rawEvent.setValue( "Switch Mode" ); //	addr = 0x01 to change PLL Multiplier, so just run switch mode
	}		
	else if(key.compare("External Clock Frequency") == 0 && successDouble)
	{
		success = true;
		if(extClkFreq != tempDouble)
		{
			extClkFreq = tempDouble;
			stateChange = true;
		}
		
		if(ExternalClock)
		{
			PLLmultiplier = static_cast<uInt32>(floor(sampleFreq / extClkFreq)); 
			rawEvent.setValue( "Switch Mode" ); //	addr = 0x01 to change PLL Multiplier, so just run switch mode
		}
	}
	else if(key.compare("Phase") == 0 && successDouble)
	{
		success = true;
		if(tempDouble >= 0 && dds_parameters.at(activeChannel).PhaseInDegrees != tempDouble)
		{
//			dds_parameters.at(activeChannel).PhaseInDegrees = tempDouble;
			ddsValue.phase.setValue( tempDouble );
			stateChange = true;
		}
		else
		{
			ddsValue.phase.setValue( dds_parameters.at(activeChannel).PhaseInDegrees );
		}
	
		ddsValue.ampl.setValue( dds_parameters.at(activeChannel).AmplitudeInPercent );
		ddsValue.freq.setValue( dds_parameters.at(activeChannel).FrequencyInMHz );
		rawEvent.setValue(ddsValue.getMixedValue());
	}
	else if(key.compare("Frequency") == 0 && successDouble)
	{
		success = true;
		if(tempDouble >= 0.0 && tempDouble <= 250.0 && dds_parameters.at(activeChannel).FrequencyInMHz != tempDouble)
		{
//			dds_parameters.at(activeChannel).FrequencyInMHz = tempDouble;
			ddsValue.freq.setValue( tempDouble );
			stateChange = true;
		}
		else
		{
			ddsValue.freq.setValue( dds_parameters.at(activeChannel).FrequencyInMHz );
		}
		
		ddsValue.ampl.setValue( dds_parameters.at(activeChannel).AmplitudeInPercent );
		ddsValue.phase.setValue( dds_parameters.at(activeChannel).PhaseInDegrees );
		rawEvent.setValue(ddsValue.getMixedValue());
	}
	
	else if(key.compare("Amplitude") == 0 && successDouble)
	{
		success = true;
		if(tempDouble >= 0.0 && tempDouble <= 100.0 && dds_parameters.at(activeChannel).AmplitudeInPercent != tempDouble)
		{
//			dds_parameters.at(activeChannel).AmplitudeInPercent = tempDouble;
			ddsValue.ampl.setValue( tempDouble );
			stateChange = true;
		}
		else
		{
			ddsValue.ampl.setValue( dds_parameters.at(activeChannel).AmplitudeInPercent );
		}
		
		ddsValue.freq.setValue( dds_parameters.at(activeChannel).FrequencyInMHz );
		ddsValue.phase.setValue( dds_parameters.at(activeChannel).PhaseInDegrees );
		rawEvent.setValue(ddsValue.getMixedValue());
	}
	
	else if(key.compare("Rising Sweep Ramp Rate(%)") == 0 && successDouble)
	{
		success = true;
		if(tempDouble <= 99.0 && tempDouble >= 0.0)
		{
			if(dds_parameters.at(activeChannel).risingSweepRampRateInPercent != tempDouble)
			{
				dds_parameters.at(activeChannel).risingSweepRampRate = static_cast<uInt32>(floor(((100.0 - tempDouble) / 100.0) * 255.0));
				dds_parameters.at(activeChannel).risingSweepRampRateInPercent = tempDouble;
				stateChange = true;
			}
		}
		else
		{
			std::cerr << "Enter a value between 0 and 100 (non-inclusive)" << std::endl;
			//std::cerr << "temp double is: " << tempDouble << std::endl;
		}
		
		rawEvent.setValue( "Set Sweep Parameters" ); //	sets all sweep parameters
	}
	
	else if(key.compare("Falling Sweep Ramp Rate(%)") == 0 && successDouble)
	{
		success = true;
		if(tempDouble <= 99.0 && tempDouble >= 0.0)
		{
			if(dds_parameters.at(activeChannel).fallingSweepRampRateInPercent != tempDouble)
			{
				dds_parameters.at(activeChannel).fallingSweepRampRate = static_cast<uInt32>(floor(((100.0 - tempDouble) / 100.0) * 255.0));
				dds_parameters.at(activeChannel).fallingSweepRampRateInPercent = tempDouble;
				stateChange = true;
			}
		}
		else
		{
			std::cerr << "Enter a value between 0 and 100 (non-inclusive)" << std::endl;
			//std::cerr << "temp double is: " << tempDouble << std::endl;
		}
		
		rawEvent.setValue( "Set Sweep Parameters" ); //	sets all sweep parameters
	}
	
	else if(key.compare("Rising Delta Word") == 0 && successDouble)
	{
		success = true;
		if(dds_parameters.at(activeChannel).risingDeltaWordInMHz != tempDouble)
		{
			dds_parameters.at(activeChannel).risingDeltaWord  = generateDDSfrequency(tempDouble);
			dds_parameters.at(activeChannel).risingDeltaWordInMHz = tempDouble;
			stateChange = true;
		}

		rawEvent.setValue( "Set Sweep Parameters" ); //	sets all sweep parameters
	}
	
	else if(key.compare("Falling Delta Word") == 0 && successDouble)
	{
		success = true;
		if(dds_parameters.at(activeChannel).fallingDeltaWordInMHz != tempDouble)
		{
			dds_parameters.at(activeChannel).fallingDeltaWord = generateDDSfrequency(tempDouble);
			dds_parameters.at(activeChannel).fallingDeltaWordInMHz = tempDouble;
			stateChange = true;
		}
		
		rawEvent.setValue( "Set Sweep Parameters" ); //	sets all sweep parameters
	}
	
	else if(key.compare("Sweep End Point") == 0 && successDouble)
	{
		success = true;
		
		if((tempDouble > dds_parameters.at(activeChannel).FrequencyInMHz && tempDouble <= 250.0) || (tempDouble < dds_parameters.at(activeChannel).FrequencyInMHz && tempDouble > 0.0 && dds_parameters.at(activeChannel).LinearSweepEnable == false)) //for sweep mode, can't have end point < start point
		{
			if(dds_parameters.at(activeChannel).sweepEndPointInMHz != tempDouble)
			{
				dds_parameters.at(activeChannel).sweepEndPoint = generateDDSfrequency(tempDouble);
				dds_parameters.at(activeChannel).sweepEndPointInMHz = tempDouble;
				stateChange = true;
			}
		}
			

		rawEvent.setValue( "Set Sweep Parameters" ); //	sets all sweep parameters
	}
	else if(key.compare("DAC Current") == 0)
	{
		success = true;

		if(value.compare("Off") == 0 && dds_parameters.at(activeChannel).DACCurrentControl != 0)
		{
			dds_parameters.at(activeChannel).DACCurrentControl = 0;
			stateChange = true;
		}
		else if(value.compare("Low") == 0 && dds_parameters.at(activeChannel).DACCurrentControl != 1)
		{
			dds_parameters.at(activeChannel).DACCurrentControl = 1;
			stateChange = true;
		}
		else if(value.compare("Medium") == 0 && dds_parameters.at(activeChannel).DACCurrentControl != 2)
		{
			dds_parameters.at(activeChannel).DACCurrentControl = 2;
			stateChange = true;
		}
		else if(value.compare("High") == 0 && dds_parameters.at(activeChannel).DACCurrentControl != 3)
		{
			dds_parameters.at(activeChannel).DACCurrentControl = 3;
			stateChange = true;
		}
		else
			success = false;

		rawEvent.setValue( "Switch Mode" ); //	addr = 0x03 to change DAC Current, so just run switch mode
	}
	*/
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

void STF_DDS_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	double eventSpacing = 800; //minimum time between events
	double lastEventTime = 0;

	vector<int> commandList; 

	if(!initialized)
		throw EventParsingException(eventsIn.begin()->second.at(0), "The DDS needs to be initialized before use.");

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
				dds_parameters.at(activeChannel).startSweep = sweepOnLastCommand;
			}

			eventsOut.push_back( generateDDScommand( events->first - eventSpacing * (commandList.size() - i), commandList.at(i)) );
		}

		lastEventTime = events->first;

	}
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

	//uInt32 eventTypeSize = 1;
	//uInt32 sweepModeChangeSize = 5+4+1;

	//bool successOutputAddr = false;
	//uInt32 outputAddr = 0;

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
		dds_parameters.at(activeChannel).AFPSelect = 0;
		dds_parameters.at(activeChannel).LinearSweepEnable = false;
		dds_parameters.at(activeChannel).AmplitudeEnable = true;
		dds_parameters.at(activeChannel).ClearSweep = true; //always keep the sweep counter cleared, unless we're actively sweeping
		dds_parameters.at(activeChannel).startSweep = false;
		sweepOnLastCommand = false;

		// push back into commandList...
		commandList->push_back(0x04); //set frequency
		commandList->push_back(0x05); //set phase
		commandList->push_back(0x06); // set amplitude
	}
	else
	{
		//parse a sweep commands
		std::cerr << "oh you're trying to sweep, are you?" << std::endl;
		dds_parameters.at(activeChannel).AFPSelect = 2;
		dds_parameters.at(activeChannel).LinearSweepEnable = true;
		dds_parameters.at(activeChannel).AmplitudeEnable = false;
		dds_parameters.at(activeChannel).ClearSweep = false; //don't clear the sweep counter since we're actively sweeping
		dds_parameters.at(activeChannel).startSweep = false;	//don't start a sweep yet...
		sweepOnLastCommand = true;

		commandList->push_back(0x03); //set function registers
		commandList->push_back(0x04); //set frequency
		commandList->push_back(0x05); //set phase
		commandList->push_back(0x06); //set amplitude enable
		commandList->push_back(0x07); //set ramp rates
		commandList->push_back(0x08); //set rising delta word
		commandList->push_back(0x09); //set falling delta word
		commandList->push_back(0x0a); //set sweep end point
		commandList->push_back(0x0c); //random address for starting the sweep
	}
				
	return true;

	
/*

				//amplitude sweep
				if( ddsValue.ampl.getType() == ParsedDDSValue::DDSSweep )
				{
					amplEvent = Sweep;
					eventTypeSize += sweepModeChangeSize;

					startVal = ddsValue.ampl.getStartValue();
					endVal   = ddsValue.ampl.getEndValue();
					rampTime = ddsValue.ampl.getRampTime();

					if(dds_parameters.at(activeChannel).AmplitudeInPercent != startVal)
						eventTypeSize++;
				}
				//phase sweep
				if( ddsValue.phase.getType() == ParsedDDSValue::DDSSweep )
				{
					phaseEvent = Sweep;
					eventTypeSize += sweepModeChangeSize;
											
					startVal = ddsValue.phase.getStartValue();
					endVal   = ddsValue.phase.getEndValue();
					rampTime = ddsValue.phase.getRampTime();

					if(dds_parameters.at(activeChannel).PhaseInDegrees != startVal)
						eventTypeSize++;
				}

				// Make sure nothing else changes during a ramp
				if( (freqEvent  == Sweep && (amplEvent != None && phaseEvent != None)) ||
					(amplEvent  == Sweep && (freqEvent != None && phaseEvent != None)) ||
					(phaseEvent == Sweep && (amplEvent != None && freqEvent  != None))
					)
				{
					throw EventParsingException(events->second.at(i),
						"Only the sweeping parameter can change during a DDS sweep event. The other parameters must be ''.");
				}

				if(freqEvent == Sweep || amplEvent == Sweep || phaseEvent == Sweep)
				{
					if(rampTime < 0)
						throw EventParsingException(events->second.at(i),
						"The DDS sweep time must be positive.");

			//		dds_parameters.at(events->second.at(i).channel()).ClearSweep = false;
			//		dds_parameters.at(events->second.at(i).channel()).startSweep = true;		
				}
			}
			*/
}

bool STF_DDS_Device::parseFrequencySweep(double startVal, double endVal, double rampTime)
{
	//SSR = ;
	//deltaWordInMHz = ;  //AD9959 spec sheet, page 25: deltaFreq = (DW / 2^32) * SYSCLK (Hz)
	//DW = ;  //AD9959 spec sheet, page 25: deltaPhase = (DW / 2^14) * 360 (degrees)
	//DW = ;  //AD9959 spec sheet, page 25: deltaAmpl = (DW / 2^10) * 1024 (DAC full scale current)	
	//deltaF = endVal - startVal;
	uInt32 RSSR;
	double numberOfPoints;
	uInt32 deltaWord;

	if( startVal < 0 || startVal > sampleFreq || endVal < 0 || endVal > sampleFreq )
	{
		errorMessage = "Ramp frequencies must be between 0 and " + valueToString(sampleFreq) + " MHz.";
		std::cerr << errorMessage << std::endl;
		return false;
	}
	else
	{
		dds_parameters.at(activeChannel).FrequencyInMHz = startVal;
		dds_parameters.at(activeChannel).Frequency = generateDDSfrequency(startVal);
		dds_parameters.at(activeChannel).sweepEndPointInMHz = endVal;
		dds_parameters.at(activeChannel).sweepEndPoint = generateDDSfrequency(endVal);
	}

	if(rampTime < 0)
	{
		errorMessage = "The sweep time must be positive.";
		std::cerr << errorMessage << std::endl;
		return false;
	}

	RSSR = 1;
	numberOfPoints = (rampTime * SYNC_CLK) / (RSSR * 1000);		// rampTime (ns) * SYNC_CLK (MHz) --> 10^-3

	if(numberOfPoints < 1)
	{
		errorMessage = string("The minimum sweep time is ") + valueToString(1000 / SYNC_CLK) + string(" ns.");
		std::cerr << errorMessage << std::endl;
		return false;
	}

	if (endVal > startVal)
		deltaWord = generateDDSfrequency( (endVal - startVal) / numberOfPoints );
	else
	{
		errorMessage = "Must sweep up before sweeping down.";
		std::cerr << errorMessage << std::endl;
		return false;
		//deltaWord = generateDDSfrequency( (startVal - endVal) / numberOfPoints );
	}

	if (deltaWord == 0)
	{
		errorMessage = string("The minimum sweep range is ") + valueToString( generateDDSfrequencyInMHz(1) * 1000000 ) + string(" Hz.");
		std::cerr << errorMessage << std::endl;
		return false;
	}
	
	dds_parameters.at(activeChannel).fallingSweepRampRate = RSSR;
	dds_parameters.at(activeChannel).fallingSweepRampRateInPercent = 100;

	if (endVal > startVal)
	{
		dds_parameters.at(activeChannel).risingDeltaWord  = deltaWord;
		dds_parameters.at(activeChannel).risingDeltaWordInMHz = generateDDSfrequencyInMHz( deltaWord );
		dds_parameters.at(activeChannel).fallingDeltaWord  = deltaWord;
		dds_parameters.at(activeChannel).fallingDeltaWordInMHz = generateDDSfrequencyInMHz( deltaWord );
	}
	else
	{
		errorMessage = "Must sweep up before sweeping down.";
		std::cerr << errorMessage << std::endl;
		return false;
	}

	return true;

}
bool STF_DDS_Device::parseStringType( RawEvent eventString, vector<int> * commandList)
{
	//
	if( eventString.stringValue().compare("Initialize") == 0 )
	{
		activeChannel = eventString.channel();
		if(!checkSettings())
			return false; //check that freq, amp, phase are all correct and convert them to hex values
		for (unsigned j = 0; j < 11; j++ )
			commandList->push_back(j);
	}
	else
		return false;

	return true;
}
/*
void STF_DDS_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	
	RawEventMap::const_iterator events;

	double eventSpacing = 800; //minimum time between events, same as holdoff time
	double eventTime = 0;
	double holdoffTime = 0;
	uInt32 eventTypeSize = 1;
	uInt32 sweepModeChangeSize = 5+4+1;

	bool successOutputAddr = false;
	uInt32 outputAddr = 0;

	string tempErr;

	uInt32 RSSR;
	double numberOfPoints;

	DDSEventType freqEvent = None;
	DDSEventType amplEvent = None;
	DDSEventType phaseEvent = None;

	double startVal, endVal, rampTime;
	double newFreq, newAmpl, newPhase;
	uInt32 deltaWord;

	TDDS ddsValue;


	//main loop over rawEvents
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1)
			throw EventConflictException(events->second.at(0), events->second.at(1),
						"The DDS currently only supports one event at each time.");

		// compute total setback needed
		holdoffTime = events->first; //start time before we add in the setbacks

		for(unsigned i = 0; i < events->second.size(); i++) //step through all channels at this time
		{
			
			else if(events->second.at(i).getValueType() == MixedValue::String)
			{
				if(events->second.at(i).stringValue() == "initialize")
					eventTypeSize = 7;
				else if(events->second.at(i).stringValue() == "Set Sweep Parameters")
					eventTypeSize = 4;
				else
					eventTypeSize = 1;
			}
			else
			{
				std::cerr << "The DDS does not support that data type." << std::endl;
				throw EventParsingException(events->second.at(i),
						"The DDS does not support that data type.");
			}


			holdoffTime = holdoffTime - eventSpacing * eventTypeSize;
		}

		holdoffTime = holdoffTime - eventSpacing * events->second.size(); //add in the time required to change channels
		if(events->second.at(0).channel() == activeChannel)
			holdoffTime = holdoffTime + eventSpacing; //shorten result if we only need i-1 channel changes

		//std::cerr << "The start time is: " << holdoffTime << std::endl;
		//std::cerr << "The event time is: " << eventTime << std::endl;


		if(holdoffTime < eventTime) //check to see if the start is at negative time
			{
				std::cerr << "There is not enough time allowed between events. Make sure there is at least 10 microseconds." << std::endl;
				throw EventParsingException(events->second.at(0),
					"There is not enough time allowed between events. Make sure at least 10 microseconds are allowed before the 1st event for initialization.");
			}
		else
			eventTime = holdoffTime;

		for(unsigned i = 0; i < events->second.size(); i++) //step through all channels at this time
		{
			//first set the channel
			if(events->second.at(i).channel() != activeChannel)
			{
				std::cerr << "Old Channel: " << activeChannel << std::endl;

				activeChannel = events->second.at(i).channel();

				std::cerr << "New Channel: " << activeChannel << std::endl;

				eventsOut.push_back( 
					generateDDScommand(eventTime, 0x00)
					);

				//std::cerr << "I changed my channel because it wasn't correct." << std::endl;

				eventTime = eventTime + eventSpacing; //set holdoffTime for next event
			}

			switch(events->second.at(i).getValueType())
			{
			case MixedValue::Double:
					std::cerr << "The value of a DDS event must be a tuple (freq, ampl, phase)." << std::endl;
					throw EventParsingException(events->second.at(i),
						"The value of a DDS event must be a tuple (freq, ampl, phase).");
					break;
			case MixedValue::String:
					
					if(events->second.at(i).stringValue() == "initialize")
					{
						//update Channel Registers, FR1, CFR, and Amplitude
						IOUpdate = false;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x00)
								); //update channel
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x01)
								); //update function registers
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x03)
								); // update channel function registers
						eventTime = eventTime + eventSpacing;
						
						IOUpdate = true;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x06)
								); // update amplitude enable
						eventTime = eventTime + eventSpacing;
					}
					else if(events->second.at(i).stringValue() == "Set Sweep Parameters")
					{
						//update RSRR & FSRR, RDW, 
						IOUpdate = false;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x07)
								); //update rising and falling ramp rates
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x08)
								); //update rising delta word
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x09)
								); //update falling delta word
						eventTime = eventTime + eventSpacing;
						
						IOUpdate = true;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x0A)
								); //update end word
						eventTime = eventTime + eventSpacing;
					}
					else if(events->second.at(i).stringValue() == "Non Register Command")
					{
						IOUpdate = false;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x0c)
								); //write to some random address, we don't care, just as long as an event gets written
						eventTime = eventTime + eventSpacing;
					}
					else
					{
						//must mean it's an address...
						successOutputAddr = stringToValue(events->second.at(i).stringValue(), outputAddr);
						if(successOutputAddr)
						{
							IOUpdate = true; //we probably want to update
							eventsOut.push_back( 
								generateDDScommand(eventTime, outputAddr)
								);
							eventTime = eventTime + eventSpacing;
						}
					}
					break;
			case MixedValue::Vector:

					IOUpdate = false;

					//simple freq change event
					if(freqEvent == Change)
					{
						//only IOUpdate here if there is no ampl or phase event
						IOUpdate = !( amplEvent == Change || phaseEvent == Change);

						//set Frequency @ addr 0x04
						eventsOut.push_back( generateDDScommand(eventTime, 0x04) );
						eventTime = eventTime + eventSpacing;
					}
					if(amplEvent == Change)
					{
						//only IOUpdate here if there is no phase event
						IOUpdate = !( phaseEvent == Change);

						//set Amplitude @ addr 0x06
						eventsOut.push_back( generateDDScommand(eventTime, 0x06) );
						eventTime = eventTime + eventSpacing;

					}
					if(phaseEvent == Change)
					{
						IOUpdate = true;

						//set Phase @ addr 0x05
						eventsOut.push_back( generateDDScommand(eventTime, 0x05) );
						eventTime = eventTime + eventSpacing;
					}
						
					if(freqEvent == Sweep)
					{
						if(dds_parameters.at(activeChannel).FrequencyInMHz != startVal)
						{
							//add an event to make the frequency equal the correct start value
							//set Frequency @ addr 0x04
							eventsOut.push_back( generateDDScommand(eventTime, 0x04) );
							eventTime = eventTime + eventSpacing;
						}



						//Switch Mode

						dds_parameters.at(activeChannel).AFPSelect = 2;
						dds_parameters.at(activeChannel).LinearSweepEnable = true;
						dds_parameters.at(activeChannel).AmplitudeEnable = false;
						dds_parameters.at(activeChannel).mode = 2;



						//update Channel Registers, FR1, CFR, and Amplitude
						IOUpdate = false;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x00)
								); //update channel
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x01)
								); //update function registers
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x03)
								); // update channel function registers
						eventTime = eventTime + eventSpacing;
						
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x06)
								); // update amplitude enable
						eventTime = eventTime + eventSpacing;







						//"Set Sweep Parameters"					
						//update RSRR & FSRR, RDW, 
						IOUpdate = false;
						eventsOut.push_back( 
							generateDDScommand(eventTime, 0x07)); //update rising and falling ramp rates
						eventTime = eventTime + eventSpacing;

						eventsOut.push_back( 
							generateDDScommand(eventTime, 0x08)); //update rising delta word
						eventTime = eventTime + eventSpacing;

						eventsOut.push_back( 
							generateDDScommand(eventTime, 0x09)); //update falling delta word
						eventTime = eventTime + eventSpacing;
		
						eventsOut.push_back( 
							generateDDScommand(eventTime, 0x0A)); //update end word
						eventTime = eventTime + eventSpacing;


						IOUpdate = true;
						dds_parameters.at(events->second.at(i).channel()).ClearSweep = false;
						
						eventsOut.push_back( 
							generateDDScommand(eventTime, 0x03)); //parameters...
						eventTime = eventTime + eventSpacing;


						dds_parameters.at(events->second.at(i).channel()).startSweep = true;		
												
						IOUpdate = false;
						eventsOut.push_back( 
								generateDDScommand(eventTime, 0x0c)
								); //write to some random address, we don't care, just as long as an event gets written
						eventTime = eventTime + eventSpacing;


					}



					////OLD
					//dds_parameters.at(activeChannel).Frequency = generateDDSfrequency(events->second.at(i).ddsValue().freq);
					//dds_parameters.at(activeChannel).Phase = generateDDSphase(events->second.at(i).ddsValue().phase);
					//dds_parameters.at(activeChannel).Amplitude = generateDDSamplitude(events->second.at(i).ddsValue().ampl);
					//
					////set Amplitude @ addr 0x06
					//IOUpdate = false;
					//eventsOut.push_back( 
					//		generateDDScommand(eventTime, 0x06)
					//		);
					//eventTime = eventTime + eventSpacing;
					//	
					////set Frequency @ addr 0x04
					//eventsOut.push_back( 
					//		generateDDScommand(eventTime, 0x04)
					//		);
					//eventTime = eventTime + eventSpacing;

					////set Phase @ addr 0x05
					//IOUpdate = true;
					//eventsOut.push_back( 
					//		generateDDScommand(eventTime, 0x05)
					//		);

					//std::cerr << "I created an event using a dds triplet. I set all 3 values, ampl, freq, phase." << std::endl;
					break;
			//	case ValueMeas:
			//		std::cerr << "The DDS does not support ValueMeas events." << std::endl;
			//		throw EventParsingException(events->second.at(i),
			//			"The DDS does not support ValueMeas events.");
			//		break;
				default:
					std::cerr << "The DDS does not support whatever you tried to give it." << std::endl;
					throw EventParsingException(events->second.at(i),
						"The DDS does not support whatever you tried to give it.");
					break;
			}
		}
	}
	
	notInitialized = false;
	//std::cerr << "The DDS parsed." << std::endl;

}
*/
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
STF_DDS_Device::DDS_Event* STF_DDS_Device::generateDDScommand(double time, uInt32 addr)
{
	
	//DDSParameters& ddsCh = dds_parameters.at(activeChannel); //a shorthand way to use a reference

	DDS_Event* ddsCommand = new DDS_Event(time, 0, 0, this);
	ddsCommand->setBits(0);
	ddsCommand->setBits(addr, 32, 36);	//5 bit address
	ddsCommand->setBits(!IOUpdate, 48, 48); //if we do not want an IO Update, set this bit high
	ddsCommand->setBits(ExternalClock, 40, 40);
	
	ddsCommand->setBits(dds_parameters.at(activeChannel).startSweep, 41 + activeChannel, 41 + activeChannel); //selects which channel to sweep based on active channel


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

	std::cerr << "ddsCommand: " ;
	
	for(int i = 63; i >= 0; i--)
	{
		std::cerr << ( ddsCommand->getBits(i,i) ? "1" : "0" );
	}
	std::cerr << std::endl;

	return ddsCommand;

}


STF_DDS_Device::DDS_Event::DDS_Event(double time, uInt32 command, uInt32 value, FPGA_Device* device) : 
BitLineEvent<64>(time, device), device_f(device)
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
void STF_DDS_Device::restoreDefaults()
{
	sweepOnLastCommand = false;
	//restores the device to its original state
	for(unsigned k = 0; k < 4; k++)
	{
		dds_parameters.at(k).ChargePumpControl = 0;
		dds_parameters.at(k).ProfilePinConfig = 0;
		dds_parameters.at(k).RuRd = 0;
		dds_parameters.at(k).AFPSelect = 0;
		dds_parameters.at(k).LSnoDwell = false;
		dds_parameters.at(k).LinearSweepEnable = false;
		dds_parameters.at(k).LoadSRR = false;
		dds_parameters.at(k).AutoclearSweep = false;
		dds_parameters.at(k).ClearSweep = false;
		dds_parameters.at(k).AutoclearPhase = false;
		dds_parameters.at(k).ClearPhase = false;
		dds_parameters.at(k).SinCos = false;
		dds_parameters.at(k).DACCurrentControl = 1; //set DAC current to low
		dds_parameters.at(k).Phase = 0;
		dds_parameters.at(k).PhaseInDegrees = 0;
		dds_parameters.at(k).Frequency = 0;
		dds_parameters.at(k).FrequencyInMHz = 10;
		dds_parameters.at(k).Amplitude = 0;
		dds_parameters.at(k).AmplitudeInPercent = 100;
		dds_parameters.at(k).AmplitudeEnable = 1; // We want to enable everything on initialization
		dds_parameters.at(k).LoadARR = false;
		dds_parameters.at(k).risingDeltaWord = 0;
		dds_parameters.at(k).risingDeltaWordInMHz = 0;
		dds_parameters.at(k).fallingDeltaWord = 0;
		dds_parameters.at(k).fallingDeltaWordInMHz = 0;
		dds_parameters.at(k).sweepEndPoint = 0;
		dds_parameters.at(k).sweepEndPointInMHz = 0;
		dds_parameters.at(k).risingSweepRampRate = 0;
		dds_parameters.at(k).risingSweepRampRateInPercent = 0;
		dds_parameters.at(k).fallingSweepRampRate = 0;
		dds_parameters.at(k).fallingSweepRampRateInPercent = 0;
		dds_parameters.at(k).startSweep = false;
	}

}
STF_DDS_Device::DDS_Parameters::DDS_Parameters()
{
	//define initial values	
	mode = 1; //start out in function generator
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
	ClearPhase = false;
	SinCos = false;
	DACCurrentControl = 1; //set DAC current to low
	Phase = 0;
	PhaseInDegrees = 0;
	Frequency = 0;
	FrequencyInMHz = 10;
	Amplitude = 0;
	AmplitudeInPercent = 100;
	AmplitudeEnable = 1; // We want to enable everything on initialization
	LoadARR = false;
	risingDeltaWord = 0;
	risingDeltaWordInMHz = 0;
	fallingDeltaWord = 0;
	fallingDeltaWordInMHz = 0;
	sweepEndPoint = 0;
	sweepEndPointInMHz = 0;
	risingSweepRampRate = 0;
	risingSweepRampRateInPercent = 0;
	fallingSweepRampRate = 0;
	fallingSweepRampRateInPercent = 0;
	startSweep = false;
}
