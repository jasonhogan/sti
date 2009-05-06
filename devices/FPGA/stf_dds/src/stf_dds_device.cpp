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
	PLLmultiplier = 10; // valid values are 4-20. Multiplier for the input clock. 10*25 MHz crystal = 250 MHz -> 0x80000000 = 250 MHz
	ChargePumpControl = 0; // higher values increase the charge pump current
	ProfilePinConfig = 0; // Determines how the profile pins are configured
	RuRd = 0; // Ramp Up / Ramp Down control
	ModulationLevel = 0; // set to 0 for now

	notInitialized = true;
	ActiveChannel = 0x01; //corresponds to channel 0
	VCOGainControl = true;
	AFPSelect = 0;
	LSnoDwell = false;
	LSenable = false;
	LoadSRR = false;
	AutoclearSweep = false;
	ClearSweep = false;
	AutoclearPhase = false;
	ClearPhase = false;
	SinCos = false;
	//DACCurrentControl = 3;
	Phase = 0;
	Frequency = 0;
	Amplitude = 0;
	AmplitudeEnable = 0; // Default setting on DDS chip start-up


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
	addAttribute("Active Channel", "None", "None, 0, 1, 2, 3, All"); // can set channel 0,1,2,3 or any combination i.e. 0xF = all channels
	// VCO gain control
	addAttribute("VCO Gain Control", "On", "On, Off"); //true activates VCO
	//PLL multiplier
	addAttribute("PLL multiplier", PLLmultiplier ); //allowed values 4-25 - requires a timing sequence to be run
	//Charge pump control
	addAttribute("Charge Pump Current (microAmps)", "75", "75, 100, 125, 150");
	//Profile Pin Configuration
	addAttribute("Profile Pin Configuration", 0); // 3 bits
	//Ramp Up / Ramp Down
	addAttribute("Ramp Up / Ramp Down", 0); // 2bits
	//Modulation Level
	addAttribute("Modulation Level", 0); // 2 bits
	//Amplitude, Phase, Frequency Select
	addAttribute("Amplitude, Phase, Frequency Select", "None", "Amplitude, Phase, Frequency, None"); //AFP, 2 bits
	//Linear Sweep, No Dwell
	addAttribute("Linear Sweep, No Dwell", "off", "off, on");
	//Linear Sweep Enable
	addAttribute("Linear Sweep Enable", "off", "off, on");
	//DAC full scale current control
	//addAttribute("DAC full scale current control", 3); //2 bits, "11" default value
	//Autoclear sweep accumulator
	addAttribute("Autoclear sweep accumulator", "false", "true, false");
	//Clear sweep accumulator
	addAttribute("Clear sweep accumulator", "false", "true, false");
	//Autoclear phase accumulator
	addAttribute("Autoclear phase accumulator", "false", "true, false");
	//Clear sweep accumulator
	addAttribute("Clear phase accumulator", "false", "true, false");
	//Sine vs. Cosine
	addAttribute("Sine vs. Cosine", "sin", "sin, cos");
	//Phase
	addAttribute("Phase", 0); //14 bits
	//Frequency
	addAttribute("Frequency", 0); //32 bits
	//Amplitude
	addAttribute("Amplitude", 0); //10 bits allowed range 0-1023
	//Amplitude Enable
	addAttribute("Amplitude Enable", "Off", "On, Off"); //required to be set to On before Amplitude control does anything
	//Amplitude Ramp Rate
	addAttribute("Amplitude Ramp Rate", 0); //8 bits
	//Increment/Decrement Step Size
	addAttribute("Amplitude Step Size", 0); //2 bits
	// Linear Sweep Rate
	addAttribute("Linear Rising Sweep Rate", 0); //8 bits
	addAttribute("Linear Falling Sweep Rate", 0); //8 bits
	// Rising Delta Word
	addAttribute("Rising Delta Word", 0); //32 bits
	// Falling Delta Word
	addAttribute("Falling Delta Word", 0); //32 bits
	

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
	uInt8 tempUInt8;
	bool successUInt8 = stringToValue(value, tempUInt8);

	uInt32 tempUInt32;
	bool successUInt32 = stringToValue(value, tempUInt32);

	bool success = false;

	STI_Server_Device::TDDS ddsValue;
	ddsValue.ampl = Amplitude;
	ddsValue.freq = Frequency;
	ddsValue.phase = Phase;

	RawEvent rawEvent(0, 0, 0);

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
	}	else if(key.compare("Active Channel") == 0)	{		success = true;		if(value.compare("None") == 0)			ActiveChannel = 0;		else if(value.compare("0") == 0)			ActiveChannel = 0x1;		else if(value.compare("1") == 0)			ActiveChannel = 0x2;		else if(value.compare("2") == 0)			ActiveChannel = 0x4;		else if(value.compare("3") == 0)			ActiveChannel = 0x8;		else if(value.compare("All") == 0)			ActiveChannel = 0xF;		//	addr = 0x00 for channel registers
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
	}	else if(key.compare("PLL Multiplier") == 0 && successUInt8)	{		success = true;

		if(successUInt8)
			PLLmultiplier = tempUInt8;
		else
			success = false;
		
		//	addr = 0x01 for function register 1
		rawEvent.setValue( valueToString(0x01) );


	}
	else if(key.compare("Ramp Up / Ramp Down") == 0 && successUInt8)	{		success = true;
		RuRd = tempUInt8; // can be changed to a discrete list 
		//	addr = 0x01 for function register 1
		rawEvent.setValue( valueToString(0x01) );
	}
	else if(key.compare("Amplitude, Phase, Frequency Select") == 0)
	{
		success = true;
		if(value.compare("Amplitude") == 0)
			AFPSelect = 1;
		else if(value.compare("Phase") == 0)
			AFPSelect = 2;
		else if(value.compare("Frequency") == 0)
			AFPSelect = 3;
		else if(value.compare("None") == 0)
			AFPSelect = 0;
		else
			success = false;

		//	addr = 0x03 for channel function registers
		rawEvent.setValue( valueToString(0x03) );
	}
	else if(key.compare("Phase") == 0 && successUInt32)
	{
		success = true;
		Phase = tempUInt32;

		ddsValue.phase = Phase;
		rawEvent.setValue(ddsValue);
	}
	else if(key.compare("Frequency") == 0 && successUInt32)
	{
		success = true;
		Frequency = tempUInt32;
	
		ddsValue.freq = Frequency;
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
	else if(key.compare("Amplitude") == 0 && successUInt32)
	{
		success = true;
		Amplitude = tempUInt32;
		ddsValue.ampl = Amplitude;
		rawEvent.setValue(ddsValue);
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
	addOutputChannel(1, ValueDDSTriplet);
	addOutputChannel(2, ValueDDSTriplet);
	addOutputChannel(3, ValueDDSTriplet);
	addOutputChannel(4, ValueDDSTriplet);
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

	double eventSpacing = 500; //minimum time between events, same as holdoff time
	double eventTime = 0;
	double holdoffTime = 0;

	bool successOutputAddr = false;
	uInt16 outputAddr = 0;
	
	// add initialization commands at the head of the timing sequence
	if(notInitialized)
	{
		// set function register : addr = 0x01;
		eventsOut.push_back( 
					(new DDS_Event(eventTime, 0, 0, this))
					->setBits(generateDDScommand(0x01, 0), 0, 63)
					);
		// set channel addr = 0x00;
		eventTime = eventTime + eventSpacing;
		eventsOut.push_back( 
					(new DDS_Event(eventTime, 0, 0, this))
					->setBits(generateDDScommand(0x00, 0), 0, 63)
					);
		notInitialized = false;
	}

	//main loop over rawEvents
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		for(unsigned i = 0; i < events->second.size(); i++) //step through all channels at this time
		{
			//really needs 3 spaces for each DDS triplet & need control over IOUpdate on DDS Board...
			holdoffTime = events->first - (events->second.size() - i) * eventSpacing; // compute the time to start the board to get the output at the desired time
			
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
							(new DDS_Event(eventTime, 0, 0, this))
							->setBits(generateDDScommand(0x00, 0), 0, 63)
							);

				holdoffTime = holdoffTime + eventSpacing; //set holdoffTime for next event
			}
			
			if(holdoffTime < eventTime) //this will be trivial if we had to change channels
			{
				throw EventParsingException(events->second.at(i),
					"There is not enough time allowed between events. Make sure at least 10 microseconds are allowed before the 1st event for initialization.");
			}
			else
				eventTime = holdoffTime;

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
							(new DDS_Event(eventTime, 0, 0, this))
							->setBits(generateDDScommand(outputAddr, 0), 0, 63)
							);
					}
					break;
				case ValueDDSTriplet:
					Frequency = generateDDSfrequency(events->second.at(i).ddsValue().freq);
					Phase = generateDDSphase(events->second.at(i).ddsValue().phase);
					if(AmplitudeEnable)
					{
						Amplitude = generateDDSamplitude(events->second.at(i).ddsValue().ampl);
						//only push back an amplitude change if it is going to do something
					}
					//set Frequency @ addr 0x04
					eventsOut.push_back( 
							(new DDS_Event(eventTime, 0, 0, this))
							->setBits(generateDDScommand(0x04, 0), 0, 63)
							);
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

}
uInt32 STF_DDS_Device::generateDDSphase(double doublePhase)
{
	uInt32 hexPhase = 0;
	hexPhase = static_cast<uInt32>((doublePhase / 360.0) * 16383.0);
	return hexPhase;
}
uInt32 STF_DDS_Device::generateDDSfrequency(double doubleFrequency)
{
	uInt32 hexFrequency = 0;
	if(ExternalClock)
		hexFrequency = static_cast<uInt32>((doubleFrequency / (PLLmultiplier * extClkFreq)) * 2147483647.0);
	else
		hexFrequency = static_cast<uInt32>((doubleFrequency / (PLLmultiplier * crystalFreq)) * 2147483647.0);
	
	return hexFrequency;
}
uInt32 STF_DDS_Device::generateDDSamplitude(double doubleAmplitude)
{
	uInt32 hexAmplitude = 0;
	hexAmplitude = static_cast<uInt32>((doubleAmplitude / 100.0) * 1023.0); //in percent
	return hexAmplitude;
}
uInt64 STF_DDS_Device::generateDDScommand(uInt32 addr, uInt32 p_registers)
{
	uInt64 command = 0;
	uInt64 value = 0;
	uInt64 dds_command = 0;
	uInt32 instruction_byte = (p_registers << 9) + addr;

	if (addr == 0x00)
	{
		command = (1 << 13) + instruction_byte;
		value = (ActiveChannel << 24);
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x01)
	{
		command = (3 << 13) + instruction_byte;
		value = (1 << 31) + (PLLmultiplier << 26) + (ChargePumpControl << 24) + (ProfilePinConfig << 20) + (RuRd << 18) + (ModulationLevel << 16);
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x02)
	{
		command = (2 << 13) + instruction_byte;
		value = 0;
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x03)
	{
		command = (3 << 13) + instruction_byte;
		value = (AFPSelect << 30) + (LSnoDwell << 23) + (LSenable << 22) + (LoadSRR << 21) + (AutoclearSweep << 12) + (ClearSweep << 11) + (AutoclearPhase << 10) + (ClearPhase << 9) + (SinCos << 8); // + (DACCurrentControl << 16);
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x04)
	{
		command = (4 << 13) + instruction_byte;
		value = Frequency;
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x05)
	{
		command = (2 << 13) + instruction_byte;
		value = (Phase << 16);
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x06)
	{
		command = (3 << 13) + instruction_byte;
		value = (AmplitudeEnable << 20) + (Amplitude << 8);
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x07)
	{
		command = (2 << 13) + instruction_byte;
		value = 0;
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x09)
	{
		command = (4 << 13) + instruction_byte;
		value = 0;
		dds_command = (command << 32) + value;
	}
	else if (addr == 0x09)
	{
		command = (4 << 13) + instruction_byte;
		value = 0;
		dds_command = (command << 32) + value;
	}
	else
	{
		command = (4 << 13) + instruction_byte;
		value = 0;
		dds_command = (command << 32) + value;
	}

	return dds_command;
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

