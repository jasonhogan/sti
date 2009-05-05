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

	ExternalClock = false;
	PLLmultiplier = 10; // valid values are 4-20. Multiplier for the input clock. 10*25 MHz crystal = 250 MHz -> 0x80000000 = 250 MHz
	ChargePumpControl = 0; // higher values increase the charge pump current
	ProfilePinConfig = 0; // Determines how the profile pins are configured
	RuRd = 0; // Ramp Up / Ramp Down control
	ModulationLevel = 0; // set to 0 for now

	ActiveChannel = 1;
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

	bool success = false;

	if(key.compare("External Clock") == 0)
	{
		success = true;
		if(value.compare("Off") == 0)
			ExternalClock = false;
		else if(value.compare("On") == 0)
			ExternalClock = true;
		else
			success = false;
	}	else if(key.compare("Active Channel") == 0)	{		success = true;		if(value.compare("None") == 0)			ActiveChannel = 0;		else if(value.compare("0") == 0)			ActiveChannel = 0x1;		else if(value.compare("1") == 0)			ActiveChannel = 0x2;		else if(value.compare("2") == 0)			ActiveChannel = 0x4;		else if(value.compare("3") == 0)			ActiveChannel = 0x8;		else if(value.compare("All") == 0)			ActiveChannel = 0xF;	}	else if(key.compare("VCO Enable") == 0)
	{
		success = true;
		if(value.compare("On") == 0)
			VCOGainControl = true;
		else if(value.compare("Off") == 0)
			VCOGainControl = false;
		else
			success = false;
	}	else if(key.compare("PLL Multiplier") == 0 && successDouble)	{		success = true;
		PLLmultiplier = static_cast<uInt8>(tempDouble); //need safeguarges to make sure this is between 4 & 20
	}
	else if(key.compare("Ramp Up / Ramp Down") == 0 && successDouble)	{		success = true;
		RuRd = static_cast<uInt8>(tempDouble); // can be changed to a discrete list 
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
	}
	else if(key.compare("Phase") == 0 && successDouble)
	{
		success = true;
		Phase = static_cast<uInt32>(tempDouble);
	}
	else if(key.compare("Frequency") == 0 && successDouble)
	{
		success = true;
		Frequency = static_cast<uInt32>(tempDouble);
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
	}
	else if(key.compare("Amplitude") == 0 && successDouble)
	{
		success = true;
		Amplitude = static_cast<uInt32>(tempDouble);
	}
	else
		success = false;

	return success;
}

void STF_DDS_Device::defineChannels()
{
	addOutputChannel(0, ValueNumber);
	addOutputChannel(1, ValueNumber);
	addOutputChannel(2, ValueNumber);
	addOutputChannel(3, ValueNumber);
}

bool STF_DDS_Device::readChannel(ParsedMeasurement& Measurement)
{
	//No provisions to read data from DDS device
	return false;
}

bool STF_DDS_Device::writeChannel(const RawEvent& Event)
{
	//What does this do?
	return false;
}

std::string STF_DDS_Device::execute(int argc, char **argv)
{
	return "";
}

void STF_DDS_Device::definePartnerDevices()
{
}

void STF_DDS_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	
	//RawEventMap::const_iterator events;

	//double holdoffTime = 500; // backup required to make output occur at desired time
	double eventSpacing = 500; //minimum time between events 
	//double time_update = 0;
	double event_time = 0;
	//uInt32 loadSecondChannelCommand = 0;
	
	
	// add initialization commands at the head of the timing sequence
	// set function register : addr = 0x01;
	eventsOut.push_back( 
					(new DDS_Event(event_time, 0, 0, this))
					->setBits(generateDDScommand(0x01, 0), 0, 63)
					);
	// set channel addr = 0x00;
	event_time = event_time + eventSpacing;
	eventsOut.push_back( 
					(new DDS_Event(event_time, 0, 0, this))
					->setBits(generateDDScommand(0x00, 0), 0, 63)
					);
	// set frequency addr = 0x04;
	event_time = event_time + eventSpacing;
	eventsOut.push_back( 
					(new DDS_Event(event_time, 0, 0, this))
					->setBits(generateDDScommand(0x04, 0), 0, 63)
					);
	
/*
	



	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		
		time_update = events->first - holdoffTime; // compute the time to start the board to get the output at the desired time

		if(events->second.size() > 1)	//we only want one event per time per channel
		{
			if(events->second.at(0).channel() == events->second.at(1).channel())
			{
				//std::cout << "The Fast Analog Out cannot currently have multiple events at the same time on the same channel." << std::endl;
				throw EventConflictException(events->second.at(0), 
					events->second.at(1), 
					"The Fast Analog Out cannot currently have multiple events at the same time on the same channel." );
			}
			else
			{
				event_time = time_update - eventSpacing;

				command_bits = static_cast<uInt32>( 
					(events->second.at(1).channel() + 1) * 0x10000);
				value =  static_cast<uInt32>( 
					command_bits + ( (events->second.at(1).numberValue()+10.0) / 20.0) * 65535.0 );

				loadSecondChannelCommand = (events->second.at(1).channel() + 1) * 0x40000;

				eventsOut.push_back( 
					new FastAnalogOutEvent(event_time, value, this) );
			}
		}

		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).numberValue() > 10 || events->second.at(i).numberValue() < -10)
			{
				//std::cout << "The Fast Analog Out board only supports voltages between -10 and 10 Volts." << std::endl;
				throw EventParsingException(events->second.at(i),
					"The Fast Analog Out board only supports voltages between -10 and 10 Volts.");
			}
			if(events->second.at(i).channel() > 1)
			{
				//std::cout << "The Fast Analog Out board only has channels 0 & 1." << std::endl;
				throw EventParsingException(events->second.at(i),
					"The Fast Analog Out board only has channels 0 & 1.");
			}
		}
		
		
		event_time = time_update;
		command_bits = static_cast<uInt32>( 
			(events->second.at(0).channel() + 1) * 0x50000 + loadSecondChannelCommand);
		value =  static_cast<uInt32>( 
			command_bits + ( (events->second.at(0).numberValue()+10.0) / 20.0) * 65535.0 );
		
		eventsOut.push_back( 
				new FastAnalogOutEvent(event_time, value, this) );
	

	}
	*/
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
	timeAddress  = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() );
	commandAddress = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() + 1 );
	valueAddress = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() + 2 ); // this won't work - Jason
}

void STF_DDS_Device::DDS_Event::loadEvent()
{
	//write the event to RAM
	device_f->ramBus->writeDataToAddress( time32, timeAddress );
	device_f->ramBus->writeDataToAddress( getBits(32, 63), commandAddress );
	device_f->ramBus->writeDataToAddress( getBits(0, 31), valueAddress );
}

