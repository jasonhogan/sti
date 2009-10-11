/*! \file stf_output_device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class stf_output_device
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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


#include "stf_output_device.h"


stf_output_device::stf_output_device(ORBManager* orb_manager, std::string DeviceName, 
							   std::string IPAddress, unsigned short ModuleNumber) : 
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//Digital out board event holdoff parameters
	minimumEventSpacing = 1050; //1.05*us in nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	holdoff = minimumEventSpacing + 1000; //the holdoff is equal to the minimum event spacing + 1*us - experimentally determined
}

stf_output_device::~stf_output_device()
{
}

bool stf_output_device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void stf_output_device::defineAttributes()
{
}

void stf_output_device::refreshAttributes()
{
}

bool stf_output_device::updateAttribute(std::string key, std::string value)
{
	return false;
}

void stf_output_device::defineChannels()
{
	for( int i = 0; i < 24; i++ )
	{
		addOutputChannel(i, ValueNumber);
	}
}

void stf_output_device::definePartnerDevices()
{
}

std::string stf_output_device::execute(int argc, char **argv)
{
	return "";
}

void stf_output_device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	unsigned char bit;
	unsigned short channel;

	DigitalOutEvent* digitalEvent;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
			previousTime = minimumAbsoluteStartTime - minimumEventSpacing;
		
		eventTime = events->first - holdoff; //we can put events closer together than this, but they don't happen until 2*us later

		if( (events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The Digital Out board needs " + valueToString(minimumEventSpacing) + " ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The Digital Out board needs " + valueToString(minimumAbsoluteStartTime)+ " ns at the beginning of the timing file.");
		}

		digitalEvent = new DigitalOutEvent(eventTime, this);	//eventsOut ptr_vector will handle deletion if it gets push_back'ed

		for(unsigned i = 0; i < events->second.size(); i++)
		{
			bit = static_cast<unsigned char>( events->second.at(i).numberValue() );
			channel = events->second.at(i).channel();

			if(bit == 0 || bit == 1)
			{
//*******************Check the correct bit pattern****************
				digitalEvent->setBits( bit, channel, channel + 1 );
			}
			else
			{
				delete digitalEvent;	//free memory before throwing exception

				throw EventParsingException(events->second.at(i),
					"The Digital Out board value must be either '1' or '0'.");
			}
		}

		eventsOut.push_back( digitalEvent );
	}
}

