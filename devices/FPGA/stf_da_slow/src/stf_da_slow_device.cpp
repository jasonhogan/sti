/*! \file stf_da_slow_device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class stf_da_slow_device
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
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


#include "stf_da_slow_device.h"


stf_da_slow_device::stf_da_slow_device(ORBManager* orb_manager, std::string DeviceName, 
							   std::string IPAddress, unsigned short ModuleNumber) : 
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
}

stf_da_slow_device::~stf_da_slow_device()
{
}

bool stf_da_slow_device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void stf_da_slow_device::defineAttributes()
{
}

void stf_da_slow_device::refreshAttributes()
{
}

bool stf_da_slow_device::updateAttribute(std::string key, std::string value)
{
	return false;
}

void stf_da_slow_device::defineChannels()
{
	for( int i = 0; i < 40; i++ )
	{
	addOutputChannel(i, ValueNumber);
	}
}

bool stf_da_slow_device::readChannel(ParsedMeasurement& Measurement)
{	
	return false;
}

bool stf_da_slow_device::writeChannel(const RawEvent& Event)
{
	return false;
}

void stf_da_slow_device::definePartnerDevices()
{
}

std::string stf_da_slow_device::execute(int argc, char **argv)
{
	return "";
}

void stf_da_slow_device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	uInt32 value = 0;
	uInt32 channel = 0;
	uInt32 bits = 0;
	uInt32 combined = 0;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1)	//we only want one event per time
		{
			throw EventConflictException(events->second.at(0), 
				events->second.at(1), 
				"The Slow Analog Out cannot currently have multiple events at the same time." );
		}

		if(events->second.at(0).numberValue() > 10 || events->second.at(0).numberValue() < -10)
		{
			throw EventConflictException(events->second.at(0),
				"The Slow Analog Out board only supports voltages between -10 and 10 Volts.");
		}
		if(events->second.at(0).channel() > 39)
		{
			throw EventConflictException(events->second.at(0),
				"The Slow Analog Out board only has channels 0-39.");
		}
		else
		{
			channel = events->second.at(0).channel();
			value = static_cast<int>((-1*(events->second.at(0).numberValue()) + 10.)*16383./20.);
			bits = (channel << 1) + 1 + (1 << 7) + (1 << 8);
			combined = (bits << 14) + value;

			eventsOut.push_back( 
				new SlowAnalogOutEvent(events->first, combined, this) );
		}


	}
}

