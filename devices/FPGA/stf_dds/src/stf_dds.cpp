/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Source-file for the class FPGA_daughter_board::STF_AD_FAST
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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "stf_dds.h"

#include <iostream>

EtraxBus* STF_DDS::dds::bus = NULL;

STF_DDS::dds::dds(unsigned int EtraxMemoryAddress) 

{
	// Etrax Memory Address
	if(bus == NULL)
	{
		bus = new EtraxBus(EtraxMemoryAddress);
	}

	currentChannel = all;
	currentMode = one_bit;
	operationTime = 40; //in clock cycles (10ns increments). i.e. 40 clock cycles = 400ns real time. 

}

STF_DDS::dds::~dds()
{
}


double STF_DDS::dds::read_data()
{
	int tempData = 64000;
	#ifdef HAVE_LIBBUS
		tempData = bus->readData();
	#endif

	return tempData*0.1;
}
bool STF_DDS::dds::write_data(unsigned int data)
{

	#ifdef HAVE_LIBBUS
		bus->writeData(data);
		return true;
	#else
	std::cerr << "libbus-error: writeData." << std::endl;
		return false;
	#endif

}
bool STF_DDS::dds::setChannel(which_channel channel, Int64 time)
{
	int address = 0x00;
	int chCommand = 0xF0; //sets all channels high
	int modeCommand = 0x00; //sets DDS in 3-wire, 1 bit serial mode
	int command = chCommand & modeCommand;

	switch(channel)
	{
		case one :
			chCommand = 0x80;
			break;
		case two :
			chCommand = 0x40;
			break;
		case three :
			chCommand = 0x20;
			break;
		case four :
			chCommand = 0x10;
			break;
		default :
			chCommand = 0xF0;
			break;
	}

	command = chCommand & modeCommand;

	addressList.push_back(address);
	commandList.push_back(command);

	if(!setTime(time))
	{
		std::cerr << "failed to set Channel" << std::endl;
		return false;
	}
	else
	{
		std::cerr << "Set Channel" << std::endl;
		currentChannel = channel; 
		return true;
	}



}

bool STF_DDS::dds::setFrequency(which_channel channel, uInt32 frequency, Int64 time)
{
	if (channel != currentChannel)
	{
		setChannel(channel, time-operationTime);
	}
	std::string command = u32_to_hex(frequency);

	std::cerr << "this is the frequency in hex: " << command << std::endl;
	std::cerr << "this is the frequency in decimal: " << frequency << std::endl;

	int address = 0x00 & 0x04;

	addressList.push_back(address);
	commandList.push_back(frequency);

	if(!setTime(time))
		std::cerr << "failed to set frequency" << std::endl;
	else
		std::cerr << "Set frequency" << std::endl;

	return true;
}
bool STF_DDS::dds::setAmplitude(which_channel channel, uInt32 amplitude, Int64 time)
{
	if (channel != currentChannel)
	{
		setChannel(channel, time-operationTime);
	}
	int address = 0x00 & 0x06;
	addressList.push_back(address);
	// set manual amplitude mode, if not already set
	uInt32 command = (0x1000 | amplitude);

	std::string command_str = u32_to_hex(command);

	std::cerr << "this is the command in hex: " << command_str << std::endl;
	std::cerr << "this is the command in decimal: " << command << std::endl;

	commandList.push_back(command);

	if(!setTime(time))
		std::cerr << "failed to set amplitude" << std::endl;
	else
		std::cerr << "Set amplitude" << std::endl;



	return true;
}
bool STF_DDS::dds::setPhase(which_channel channel, uInt32 phase, Int64 time)
{
	if (channel != currentChannel)
	{
		setChannel(channel, time-operationTime);
	}
	int address = 0x00 & 0x05;
	addressList.push_back(address);

	phase = phase << 16; // register is only 16 bits wide & always start with MSB first. 
	std::string phase_string = u32_to_hex(phase);


	std::cerr << "this is the command in hex: " << phase_string << std::endl;
	std::cerr << "this is the command in decimal: " << phase << std::endl;

	commandList.push_back(phase);

	if(!setTime(time))
		std::cerr << "failed to set phase" << std::endl;
	else
		std::cerr << "Set phase" << std::endl;

	return true;
}
bool STF_DDS::dds::setSweep(which_channel channel, sweep_type sweep, uInt32 startPoint, uInt32 endPoint, int delta, Int64 time)
{
	// for linear sweeps - start point (S0) is loaded into word 0 register
	// end point (e0) is loaded into word 1 register (0x0A) - MSB aligned as usual (ideally use setAmplitude, setPhase, etc... code)
	
	if (channel != currentChannel)
	{
		setChannel(channel, time-operationTime);
	}

	int address = 0x00 & 0x05;
	addressList.push_back(address);
	
	switch(sweep)
	{
		case amplitude :
			setAmplitude(channel, startPoint, time);
			break;
		case phase :
			setPhase(channel, startPoint, time);
			break;
		case frequency :
			setFrequency(channel, startPoint, time);
			break;
		default :
			std::cerr << "What did you want to do?" << std::endl;
			break;
	}
	return true;
}
bool STF_DDS::dds::masterReset()
{
	//stuff
	return true;
}
bool STF_DDS::dds::setTime(Int64 time)
{
	if (time < 0)
	{
		std::cerr << "Time less than 0! Give yourself more time!" << std::endl;
		return false;
	}

	if (timeList.size() != 0)
	{
		if (time - timeList.back() >= operationTime)
		{
			timeList.push_back(time);
			return true;
		}
		else
		{
			std::cerr << "Events too close together!" << std::endl;
			return false;
		}
	}
	else
	{
		timeList.push_back(time);
		return true;
	}
}
