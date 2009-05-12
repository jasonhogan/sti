/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_AD_FAST
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


STF_DDS_Device::STF_DDS_Device(
		ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string IPAddress,
		unsigned short ModuleNumber) :
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{

	updateDDS = false;
	
	ExternalClock = false;
	extClkFreq = 25.0; // in MHz
	crystalFreq = 25.0; // inMHz
	PLLmultiplier = 20; // valid values are 4-20. Multiplier for the input clock. 20*25 MHz crystal = 500 MHz -> 0x80000000 = 250 MHz
	ChargePumpControl = 0; // higher values increase the charge pump current
	ProfilePinConfig = 0; // Determines how the profile pins are configured
	RuRd = 0; // Ramp Up / Ramp Down control
	ModulationLevel = 0; // set to 0 for now

	notInitialized = true;
	ActiveChannel = 0; //corresponds to channel 0
	VCOGainControl = true;
	AFPSelect = 0;
	LSnoDwell = false;
	LinearSweepEnable = false;
	LoadSRR = false;
	AutoclearSweep = false;
	ClearSweep = false;
	AutoclearPhase = false;
	ClearPhase = true;
	SinCos = false;
	//DACCurrentControl = 3;
	Phase = 0;
	PhaseInDegrees = 0;
	Frequency = 0;
	FrequencyInMHz = 0;
	Amplitude = 0;
	AmplitudeInPercent = 0;
	AmplitudeEnable = 0; // Default setting on DDS chip start-up
	risingDeltaWord = 0;
	fallingDeltaWord = 0;
	risingSweepRampRate = 0;
	fallingSweepRampRate = 0;
	startSweep = false;

}

STF_DDS_Device::~STF_DDS_Device()
{
}

bool STF_DDS_Device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void STF_DDS_Device::defineAttributes()
{

	addAttribute("Update DDS", "false", "true, false");


	//Use external clock?
	addAttribute("External Clock", "false", "true, false"); //what is the right syntax for a boolean attribute?
	//Active Channel
	addAttribute("Active Channel", "0", "0, 1, 2, 3"); // can set channel 0,1,2,3 or any combination i.e. 0xF = all channels
	// VCO gain control
	addAttribute("VCO Gain Control", "On", "On, Off"); //true activates VCO
	//PLL multiplier
	addAttribute("PLL Multiplier", PLLmultiplier ); //allowed values 4-25 - requires a timing sequence to be run
	//Set-up to sweep Amplitude, Frequency, or Phase
	addAttribute("Sweep Type", "Off", "Amplitude, Frequency, Phase, Off");
	
	//Charge pump control
	//addAttribute("Charge Pump Current (microAmps)", "75", "75, 100, 125, 150");
	//Profile Pin Configuration
	//addAttribute("Profile Pin Configuration", 0); // 3 bits
	//Ramp Up / Ramp Down
	//addAttribute("Ramp Up / Ramp Down", 0); // 2bits
	//Modulation Level
	//addAttribute("Modulation Level", 0); // 2 bits
	//Amplitude, Phase, Frequency Select
	//addAttribute("Amplitude, Phase, Frequency Select", "None", "Amplitude, Phase, Frequency, None"); //AFP, 2 bits
	
	//Linear Sweep Enable
	//addAttribute("Linear Sweep Enable", "off", "off, on");
	//DAC full scale current control
	//addAttribute("DAC full scale current control", 3); //2 bits, "11" default value
	
	//Linear Sweep, No Dwell
	addAttribute("Linear Sweep, No Dwell", "off", "off, on");
	//Autoclear sweep accumulator
	//addAttribute("Autoclear sweep accumulator", "false", "true, false");
	//Clear sweep accumulator
	//addAttribute("Clear sweep accumulator", "true", "true, false");
	//Autoclear phase accumulator
	//addAttribute("Autoclear phase accumulator", "false", "true, false");
	//Clear sweep accumulator
	//addAttribute("Clear phase accumulator", "false", "true, false");
	//Sine vs. Cosine
	//addAttribute("Sine vs. Cosine", "sin", "sin, cos");
	//Phase
	addAttribute("Phase", 0); //14 bits
	//Frequency
	addAttribute("Frequency", 0); //32 bits
	//Amplitude
	addAttribute("Amplitude", 0); //10 bits allowed range 0-1023
	//Amplitude Enable
	addAttribute("Amplitude Enable", "Off", "On, Off"); //required to be set to On before Amplitude control does anything
	//Amplitude Ramp Rate
	//addAttribute("Amplitude Ramp Rate", 0); //8 bits
	//Increment/Decrement Step Size
	//addAttribute("Amplitude Step Size", 0); //2 bits
	// Linear Sweep Rate
	addAttribute("Rising Sweep Ramp Rate(%)", 0); //8 bits
	addAttribute("Falling Sweep Ramp Rate(%)", 0); //8 bits
	// Rising Delta Word
	addAttribute("Rising Delta Word", 0); //32 bits
	// Falling Delta Word
	addAttribute("Falling Delta Word", 0); //32 bits
	// Sweep End Point
	addAttribute("Sweep End Point", 0); //32 bits
	// sweep go button
	addAttribute("Start Sweep", "false", "true, false");
	

}

void STF_DDS_Device::refreshAttributes()
{
	// All attributes are stored in c++ - can't read any of them from DDS

}

bool STF_DDS_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;

	bool successDouble = stringToValue(value, tempDouble);
	

if(successDouble)
{
successDouble = true;
}
//	uInt8 tempUInt8;
//	bool successUInt8 = stringToValue(value, tempUInt8);

	uInt32 tempUInt32;
	bool successUInt32 = stringToValue(value, tempUInt32);

	bool success = false;

	STI_Server_Device::TDDS ddsValue;
	ddsValue.ampl = Amplitude;
	ddsValue.freq = Frequency;
	ddsValue.phase = Phase;

	RawEvent rawEvent(5000, 0, 0);
	rawEvent.setChannel(ActiveChannel); //set the channel to the current active channel

	if(key.compare("Update DDS") == 0)
	{
		success = true;
		if(value.compare("false") == 0)
			updateDDS = false;
		else if(value.compare("true") == 0)
			updateDDS = true;
		else
			success = false;
	}
	else if(key.compare("Sweep Type") == 0)
	{
		success = true;
		LinearSweepEnable = true;
		ModulationLevel = 0;
		if(value.compare("Amplitude") == 0)
			AFPSelect = 1;
		else if(value.compare("Frequency") == 0)
			AFPSelect = 2;
		else if(value.compare("Phase") == 0)
			AFPSelect = 3;
		else if(value.compare("Off") == 0)
		{
			LinearSweepEnable = false; //Sweep Enable
			AFPSelect = 0;
		}
		else
			success = false;

		//	addr = 0x03 for channel function registers
		rawEvent.setValue( valueToString(0x03) );

	}
	else if(key.compare("Start Sweep") == 0)
	{
		success = true;
		if(value.compare("false") == 0)
			startSweep = false;
		else if(value.compare("true") == 0)
			startSweep = true;
		else
			success = false;

		//	addr = 0x03 for channel function registers
		rawEvent.setValue( valueToString(0x03) );
	}
	else if(key.compare("External Clock") == 0)
	{
		success = true;
		if(value.compare("false") == 0)
			ExternalClock = false;
		else if(value.compare("true") == 0)
			ExternalClock = true;
		else
			success = false;
		//this can't do anything yet as there is no provision for actively modifying ext_clk
	}	else if(key.compare("Active Channel") == 0)	{		success = true;		if(value.compare("0") == 0)			//newActiveChannel = 0;
			rawEvent.setChannel(0);		else if(value.compare("1") == 0)			//newActiveChannel = 1;
			rawEvent.setChannel(1);		else if(value.compare("2") == 0)			rawEvent.setChannel(2);			
			//newActiveChannel = 2;		else if(value.compare("3") == 0)			//newActiveChannel = 3;
			rawEvent.setChannel(3);		//	addr = 0x00 for channel registers
		rawEvent.setValue( valueToString(0x00) );	}	else if(key.compare("VCO Enable") == 0)
	{
		success = true;
		if(value.compare("On") == 0)
			VCOGainControl = true;
		else if(value.compare("Off") == 0)
			VCOGainControl = false;
		else
			success = false;
		//	addr = 0x01 for function register 1
		rawEvent.setValue( valueToString(0x01) );
	}	else if(key.compare("PLL Multiplier") == 0 && successUInt32)	{		success = true;

		if(successUInt32)
			PLLmultiplier = tempUInt32;
		else
			success = false;
		
		//	addr = 0x01 for function register 1
		rawEvent.setValue( valueToString(0x01) );
	}
	else if(key.compare("Ramp Up / Ramp Down") == 0 && successUInt32)	{		success = true;
		RuRd = tempUInt32; // can be changed to a discrete list 
		//	addr = 0x01 for function register 1
		rawEvent.setValue( valueToString(0x01) );
	}
	else if(key.compare("Amplitude, Phase, Frequency Select") == 0)
	{
		success = true;
		if(value.compare("Amplitude") == 0)
			AFPSelect = 1;
		else if(value.compare("Frequency") == 0)
			AFPSelect = 2;
		else if(value.compare("Phase") == 0)
			AFPSelect = 3;
		else if(value.compare("None") == 0)
			AFPSelect = 0;
		else
			success = false;

		//	addr = 0x03 for channel function registers
		rawEvent.setValue( valueToString(0x03) );
	}
	else if(key.compare("Modulation Level") == 0 && successUInt32)
	{
		success = true;
		ModulationLevel = tempUInt32;

		//	addr = 0x03 for function register 1
		rawEvent.setValue( valueToString(0x01) );
	}
	else if(key.compare("Phase") == 0 && successDouble)
	{
		success = true;
		PhaseInDegrees = tempDouble;

		ddsValue.ampl = AmplitudeInPercent;
		ddsValue.freq = FrequencyInMHz;
		ddsValue.phase = PhaseInDegrees;
		rawEvent.setValue(ddsValue);
	}
	else if(key.compare("Frequency") == 0 && successDouble)
	{
		success = true;
		FrequencyInMHz = tempDouble;
	
		ddsValue.ampl = AmplitudeInPercent;
		ddsValue.freq = FrequencyInMHz;
		ddsValue.phase = PhaseInDegrees;
		rawEvent.setValue(ddsValue);
	}
	else if(key.compare("Amplitude Enable") == 0)
	{
		success = true;
		if(value.compare("On") == 0)
			AmplitudeEnable = true;
		else if(value.compare("Off") == 0)
			AmplitudeEnable = false;
		else
			success = false;
		//	addr = 0x06 for amplitude control register
		rawEvent.setValue( valueToString(0x06) );
	}
	else if(key.compare("Amplitude") == 0 && successDouble)
	{
		success = true;
		AmplitudeInPercent = tempDouble;
		ddsValue.ampl = AmplitudeInPercent;
		ddsValue.freq = FrequencyInMHz;
		ddsValue.phase = PhaseInDegrees;
		rawEvent.setValue(ddsValue);
	}
	else if(key.compare("Rising Sweep Ramp Rate(%)") == 0 && successDouble)
	{
		success = true;
		risingSweepRampRate = static_cast<uInt32>(floor((tempDouble / 100.0) * 255.0));

		//	addr = 0x07 for linear sweep ramp rate register
		rawEvent.setValue( valueToString(0x07) );
	}
	else if(key.compare("Falling Sweep Ramp Rate(%)") == 0 && successDouble)
	{
		success = true;
		fallingSweepRampRate = static_cast<uInt32>(floor((tempDouble / 100.0) * 255.0));

		//	addr = 0x07 for linear sweep ramp rate register
		rawEvent.setValue( valueToString(0x07) );
	}
	else if(key.compare("Rising Delta Word") == 0 && successDouble)
	{
		success = true;

		if(AFPSelect == 1)
			risingDeltaWord = static_cast<uInt32>(floor((tempDouble / 100.0) * 1023.0));
		else if(AFPSelect == 2)
		{
			if(ExternalClock)
				risingDeltaWord = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * extClkFreq)) * 2147483647.0 * 2));
			else
				risingDeltaWord = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * crystalFreq)) * 2147483647.0 * 2));
		}
		else if(AFPSelect == 3)
			risingDeltaWord = static_cast<uInt32>(floor((tempDouble / 360.0) * 16383.0));
		else if(AFPSelect == 0)
			risingDeltaWord = 0;
		else
			success = false;

		//	addr = 0x08 for rising delta word
		rawEvent.setValue( valueToString(0x08) );
	}
	else if(key.compare("Falling Delta Word") == 0 && successDouble)
	{
		success = true;

		if(AFPSelect == 1)
			fallingDeltaWord = static_cast<uInt32>(floor((tempDouble / 100.0) * 1023.0));
		else if(AFPSelect == 2)
		{
			if(ExternalClock)
				fallingDeltaWord = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * extClkFreq)) * 2147483647.0 * 2));
			else
				fallingDeltaWord = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * crystalFreq)) * 2147483647.0 * 2));
		}
		else if(AFPSelect == 3)
			fallingDeltaWord = static_cast<uInt32>(floor((tempDouble / 360.0) * 16383.0));
		else if(AFPSelect == 0)
			fallingDeltaWord = 0;
		else
			success = false;

		//	addr = 0x09 for falling delta word
		rawEvent.setValue( valueToString(0x09) );
	}
	else if(key.compare("Sweep End Point") == 0 && successDouble)
	{
		success = true;

		if(AFPSelect == 1)
			sweepEndPoint = static_cast<uInt32>(floor((tempDouble / 100.0) * 1023.0));
		else if(AFPSelect == 2)
		{
			if(ExternalClock)
				sweepEndPoint = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * extClkFreq)) * 2147483647.0 * 2));
			else
				sweepEndPoint = static_cast<uInt32>(floor((tempDouble / (PLLmultiplier * crystalFreq)) * 2147483647.0 * 2));
		}
		else if(AFPSelect == 3)
			sweepEndPoint = static_cast<uInt32>(floor((tempDouble / 360.0) * 16383.0));
		else if(AFPSelect == 0)
			sweepEndPoint = 0;
		else
			success = false;

		//	addr = 0x0A for falling delta word
		rawEvent.setValue( valueToString(0x0a) );
	}
	else
		success = false;

	if(success && updateDDS)
	{
		writeChannel(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
	}

	return success;

}

void STF_DDS_Device::defineChannels()
{
	addOutputChannel(0, ValueDDSTriplet);
	addOutputChannel(1, ValueDDSTriplet);
	addOutputChannel(2, ValueDDSTriplet);
	addOutputChannel(3, ValueDDSTriplet);
}

bool STF_DDS_Device::readChannel(ParsedMeasurement& Measurement)
{
	//No provisions to read data from DDS device
	return false;
}


std::string STF_DDS_Device::execute(int argc, char **argv)
{
	return "";
}

void STF_DDS_Device::definePartnerDevices()
{
}

short STF_DDS_Device::wordsPerEvent()
{
	return 3;
}


void STF_DDS_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	
	RawEventMap::const_iterator events;

	double eventSpacing = 800; //minimum time between events, same as holdoff time
	double eventTime = 0;
	double holdoffTime = 0;
	uInt32 eventTypeSize = 1;

	bool successOutputAddr = false;
	uInt32 outputAddr = 0;
	
	
	

	std::cerr << "Number of Synched Events: " << eventsIn.size() << std::endl;

	//main loop over rawEvents
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		for(unsigned i = 0; i < events->second.size(); i++) //step through all channels at this time
		{
			//really needs 3 spaces for each DDS triplet & need control over IOUpdate on DDS Board...
			if(events->second.at(i).type() == ValueDDSTriplet)
				eventTypeSize = 3;
			else
				eventTypeSize = 1;

			holdoffTime = events->first - (events->second.size() - i) * eventSpacing * eventTypeSize - notInitialized * eventSpacing * 2; // compute the time to start the board to get the output at the desired time
			std::cerr << "The start time is: " << holdoffTime << std::endl;

			if(events->second.at(i).channel() != ActiveChannel)
			{
				holdoffTime = holdoffTime - eventSpacing; //add time space to allow for a channel change
				if(holdoffTime < eventTime) // if we have to change channels, this becomes the main check if there's enough time to execute the sequence
				{
					throw EventParsingException(events->second.at(i),
						"There is not enough time allowed between events. Make sure at least 10 microseconds are allowed before the 1st event for initialization.");
				}
				else
					eventTime = holdoffTime;

				ActiveChannel = events->second.at(i).channel();
				eventsOut.push_back( 
					generateDDScommand(eventTime, 0x00)
					);

				std::cerr << "I changed my channel because it wasn't correct." << std::endl;

				holdoffTime = holdoffTime + eventSpacing; //set holdoffTime for next event
			}

			if(holdoffTime < eventTime) //check to see if the start is at negative time
			{
				throw EventParsingException(events->second.at(i),
					"There is not enough time allowed between events. Make sure at least 10 microseconds are allowed before the 1st event for initialization.");
			}
			else
				eventTime = holdoffTime;

			
			// add initialization commands at the head of the timing sequence
			if(notInitialized)
			{
				// set function register : addr = 0x01;
				eventsOut.push_back( 
					generateDDScommand(eventTime, 0x01)
					);

				// set channel addr = 0x00;
				eventTime = eventTime + eventSpacing;
				eventsOut.push_back( 
					generateDDScommand(eventTime, 0x00)
					);
				eventTime = eventTime + eventSpacing;
				//notInitialized = false;
				std::cerr << "I initialized myself." << std::endl;

			}
			
			
			
			
			switch(events->second.at(i).type())
			{
				case ValueNumber:
					throw EventParsingException(events->second.at(i),
						"The DDS does not support ValueNumber events.");
					break;
				case ValueString:
					successOutputAddr = stringToValue(events->second.at(i).stringValue(), outputAddr);
					if(successOutputAddr)
					{
						eventsOut.push_back( 
							generateDDScommand(eventTime, outputAddr)
							);
					}
					std::cerr << "I updated an attribute via a synchronous event." << std::endl;
					break;
				case ValueDDSTriplet:
					Frequency = generateDDSfrequency(events->second.at(i).ddsValue().freq);
					Phase = generateDDSphase(events->second.at(i).ddsValue().phase);
					Amplitude = generateDDSamplitude(events->second.at(i).ddsValue().ampl);
					
					//set Amplitude @ addr 0x06
					eventsOut.push_back( 
							generateDDScommand(eventTime, 0x06)
							);
					eventTime = eventTime + eventSpacing;
						
					//set Frequency @ addr 0x04
					eventsOut.push_back( 
							generateDDScommand(eventTime, 0x04)
							);
					eventTime = eventTime + eventSpacing;

					//set Phase @ addr 0x05
					eventsOut.push_back( 
							generateDDScommand(eventTime, 0x05)
							);

					std::cerr << "I created an event using a dds triplet. I set all 3 values, ampl, freq, phase." << std::endl;
					break;
				case ValueMeas:
					throw EventParsingException(events->second.at(i),
						"The DDS does not support ValueMeas events.");
					break;
				default:
					throw EventParsingException(events->second.at(i),
						"The DDS does not support whatever you tried to give it.");
					break;
			}
		}
	}
	
	notInitialized = false;

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
		hexFrequency = static_cast<uInt32>(floor((doubleFrequency / (PLLmultiplier * extClkFreq)) * 2147483647.0 * 2));
	else
		hexFrequency = static_cast<uInt32>(floor((doubleFrequency / (PLLmultiplier * crystalFreq)) * 2147483647.0 * 2));
	
	return hexFrequency;
}
uInt32 STF_DDS_Device::generateDDSamplitude(double doubleAmplitude)
{
	uInt32 hexAmplitude = 0;
	hexAmplitude = static_cast<uInt32>(floor((doubleAmplitude / 100.0) * 1023.0)); //in percent
	return hexAmplitude;
}
STF_DDS_Device::DDS_Event* STF_DDS_Device::generateDDScommand(double time, uInt32 addr)
{
	
	DDS_Event* ddsCommand = new DDS_Event(time, 0, 0, this);
	ddsCommand->setBits(0);
	ddsCommand->setBits(addr, 32, 36);	//5 bit address
	ddsCommand->setBits(ExternalClock, 40, 40);
	ddsCommand->setBits(startSweep, 41, 41);

	
	
	std::cerr << "addr is: " << addr << " before if: " ;
	for(int i = 63; i >= 0; i--)
	{
		std::cerr << ( ddsCommand->getBits(i,i) ? "1" : "0" );
	}
	std::cerr << std::endl;

	if (addr == 0x00)
	{
		ddsCommand->setBits(1, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(true, 28 + ActiveChannel, 28 + ActiveChannel);
	}
	else if (addr == 0x01)
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(1, 31, 31); //turn on VCO control
		ddsCommand->setBits(PLLmultiplier, 26, 30); // set PLLmultiplier value (allowed 4-20)
		ddsCommand->setBits(ModulationLevel, 16, 17);
	}
	else if (addr == 0x02)
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)	
	}
	else if (addr == 0x03)
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(AFPSelect, 30, 31);
		ddsCommand->setBits(LSnoDwell, 23, 23);
		ddsCommand->setBits(LinearSweepEnable, 22, 22);
		ddsCommand->setBits(LoadSRR, 21, 21);
		ddsCommand->setBits(0, 18, 18); //Must be 0
		ddsCommand->setBits(3, 16, 17); //DAC full scale current control - set to default value of 0x03
		ddsCommand->setBits(AutoclearSweep, 12, 12);
		ddsCommand->setBits(ClearSweep, 11, 11);
		ddsCommand->setBits(AutoclearPhase, 10, 10);
		ddsCommand->setBits(ClearPhase, 9, 9);
		ddsCommand->setBits(SinCos, 8, 8);		
	}
	else if (addr == 0x04)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(Frequency, 0, 31);
	}
	else if (addr == 0x05)
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(Phase, 16, 31);
	}
	else if (addr == 0x06)
	{
		ddsCommand->setBits(3, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(AmplitudeEnable, 20, 20);
		ddsCommand->setBits(Amplitude, 8, 17);
	}
	else if (addr == 0x07)
	{
		ddsCommand->setBits(2, 45, 47);		//3 bit length (number of bytes in command)
		ddsCommand->setBits(risingSweepRampRate, 24, 31); //RSRR has 8 bit resolution
		ddsCommand->setBits(fallingSweepRampRate, 16, 23); //FSRR has 8 bit resolution
	}
	else if (addr == 0x08)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		if(AFPSelect == 1)
			ddsCommand->setBits(risingDeltaWord, 21, 31); //Amplitude has 10 bit resolution
		else if(AFPSelect == 2)
			ddsCommand->setBits(risingDeltaWord, 0, 31); //Frequency has 32 bit resolution
		else if(AFPSelect == 3)
			ddsCommand->setBits(risingDeltaWord, 17, 31); //Phase has 14 bit resolution
	}
	else if (addr == 0x09)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		if(AFPSelect == 1)
			ddsCommand->setBits(fallingDeltaWord, 21, 31); //Amplitude has 10 bit resolution
		else if(AFPSelect == 2)
			ddsCommand->setBits(fallingDeltaWord, 0, 31); //Frequency has 32 bit resolution
		else if(AFPSelect == 3)
			ddsCommand->setBits(fallingDeltaWord, 17, 31); //Phase has 14 bit resolution
	}
	else if (addr == 0x0a)
	{
		ddsCommand->setBits(4, 45, 47);		//3 bit length (number of bytes in command)
		if(AFPSelect == 1)
			ddsCommand->setBits(sweepEndPoint, 21, 31); //Amplitude has 10 bit resolution
		else if(AFPSelect == 2)
			ddsCommand->setBits(sweepEndPoint, 0, 31); //Frequency has 32 bit resolution
		else if(AFPSelect == 3)
			ddsCommand->setBits(sweepEndPoint, 17, 31); //Phase has 14 bit resolution
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
BitLineEvent<64>(time, device), 
device_f(device)
{
	this->setBits(command, 32, 63);
	this->setBits(value, 0, 31);
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

