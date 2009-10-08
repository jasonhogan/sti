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
	activeChannel = 0;
	outputVoltage.push_back(0); //set channel 0 = 0V
	outputVoltage.push_back(0); //set channel 1 = 0V
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
	addAttribute("Ch. 0 Output Voltage", outputVoltage.at(0)); //set the output voltage to the value for channel 0 (at position 1)
	addAttribute("Ch. 1 Output Voltage", outputVoltage.at(1)); //set the output voltage to the value for channel 1 (at position 2)
}

void STF_DA_FAST_Device::refreshAttributes()
{
	setAttribute("Ch. 0 Output Voltage", outputVoltage.at(0));
	setAttribute("Ch. 1 Output Voltage", outputVoltage.at(1));
}

bool STF_DA_FAST_Device::updateAttribute(std::string key, std::string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;

	RawEvent rawEvent(50000, 0, 0);
	

	if(key.compare("Ch. 0 Output Voltage") == 0)
	{
		if(successDouble)
		{
			activeChannel = 0;
			outputVoltage.at(activeChannel) = tempDouble;
			success = true;
		}
	}
	else if(key.compare("Ch. 1 Output Voltage") == 0)
	{
		if(successDouble)
		{
			activeChannel = 1;
			outputVoltage.at(activeChannel) = tempDouble;
			success = true;
		}
	}

	rawEvent.setChannel(activeChannel); //set the channel to the current active channel
	rawEvent.setValue(outputVoltage.at(activeChannel));
	
	if(success)
	{
		playSingleEvent(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
	}

	return success;
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
	double holdoffTime = 500; // backup required to make output occur at desired time
	double eventSpacing = 500; //minimum time between events 
	double time_update = 0;
	double event_time = 0;

	bool A_WR = false;
	bool A_LOAD = false;
	bool B_WR = false;
	bool B_LOAD = false;
	
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		
		time_update = events->first - holdoffTime; // compute the time to start the board to get the output at the desired time
		if(time_update < 0)
		{
			throw EventConflictException(events->second.at(0),  
					"Not Enough Time!! Need more time at the beginning of the timing sequence to account for Fast Analog Out setup time." );
		}
		//check for too large voltages
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).numberValue() > 10 || events->second.at(i).numberValue() < -10)
			{
				//std::cout << "The Fast Analog Out board only supports voltages between -10 and 10 Volts." << std::endl;
				throw EventParsingException(events->second.at(i),
					"The Fast Analog Out board only supports voltages between -10 and 10 Volts.");
			}
		}
		//deal with the number of events
		if(events->second.size() > 2)	//we only have two channels
		{
			//really, the timing file writer must be stupid
			throw EventConflictException(events->second.at(0), 
					events->second.at(1), 
					"The Fast Analog Out only has 2 channels, stupid." );
		}
		else if(events->second.size() == 2) //both channels are trying to do something at the same time
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
				//first we write the first channel with the appropriate offset, but do not load!
				event_time = time_update - eventSpacing;
				if(events->second.at(0).channel() == 0)
				{
					A_WR = true;
					B_WR = false;
					A_LOAD = false;
					B_LOAD = false;
				}
				else
				{
					A_WR = false;
					B_WR = true;
					A_LOAD = false;
					B_LOAD = false;
				}
				value =  static_cast<uInt32>( ( (events->second.at(0).numberValue()+10.0) / 20.0) * 65535.0 );
				eventsOut.push_back( 
					new FastAnalogOutEvent(event_time, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );

				//second we write the second channel with the appropriate offset, and load both!
				event_time = time_update;
				if(events->second.at(1).channel() == 0)
				{
					A_WR = true;
					B_WR = false;
					A_LOAD = true;
					B_LOAD = true;
				}
				else
				{
					A_WR = false;
					B_WR = true;
					A_LOAD = true;
					B_LOAD = true;
				}
				value =  static_cast<uInt32>( ( (events->second.at(1).numberValue()+10.0) / 20.0) * 65535.0 );
				eventsOut.push_back( 
					new FastAnalogOutEvent(event_time, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );
			}
		}
		else //only one channel is trying to do something at the same time
		{
			//we write the channel with the appropriate offset, and load it!
			event_time = time_update;
			if(events->second.at(0).channel() == 0)
			{
				A_WR = true;
				B_WR = false;
				A_LOAD = true;
				B_LOAD = false;
			}
			else
			{
				A_WR = false;
				B_WR = true;
				A_LOAD = false;
				B_LOAD = true;
			}
			value =  static_cast<uInt32>( ( (events->second.at(0).numberValue()+10.0) / 20.0) * 65535.0 );
			eventsOut.push_back( 
				new FastAnalogOutEvent(event_time, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );
		}
	}
}

