/*! \file STF_DA_FAST_Device.cpp
 *  \author David M.S. Johnson 
 *  \brief Source-file for the class STF_DA_FAST_Device
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

#include "stf_da_fast_device.h"
#include <iostream>

STF_DA_FAST_Device::STF_DA_FAST_Device(ORBManager* orb_manager, std::string DeviceName, 
							   std::string IPAddress, unsigned short ModuleNumber) : 
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
}

STF_DA_FAST_Device::~STF_DA_FAST_Device()
{
}

bool STF_DA_FAST_Device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void STF_DA_FAST_Device::defineAttributes()
{
}

void STF_DA_FAST_Device::refreshAttributes()
{
}

bool STF_DA_FAST_Device::updateAttribute(std::string key, std::string value)
{
	return false;
}

void STF_DA_FAST_Device::defineChannels()
{
	addOutputChannel(0, ValueNumber);
	addOutputChannel(1, ValueNumber);
}

void STF_DA_FAST_Device::definePartnerDevices()
{
}
std::string STF_DA_FAST_Device::execute(int argc, char **argv)
{
	return "";
}
void STF_DA_FAST_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	
	RawEventMap::const_iterator events;
	uInt32 value = 0;
	uInt32 command_bits = 0;
	double holdoffTime = 500; // backup required to make output occur at desired time
	double eventSpacing = 500; //minimum time between events 
	double time_update = 0;
	double event_time = 0;
			uInt32 loadSecondChannelCommand = 0;
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
}

