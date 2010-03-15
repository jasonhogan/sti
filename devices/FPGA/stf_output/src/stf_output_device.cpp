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


stf_output_device::stf_output_device(ORBManager* orb_manager, std::string configFilename) : 
FPGA_Device(orb_manager, "Digital Out", configFilename),
bitState(0)
{
	//Digital out board event holdoff parameters
	minimumEventSpacing = 500; // 500 nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	holdoff = 0; //the holdoff of the digital board is zero by definition
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
//	addPartnerDevice("fast","ep-timing1.stanford.edu",1,"Fast Analog Out");
//	partnerDevice("fast").enablePartnerEvents();
}

std::string stf_output_device::execute(int argc, char **argv)
{
	std::vector<std::string> argvOutput;
	STI::Utils::convertArgs(argc, argv, argvOutput);

	uInt32 time = 10000; //enough time to load events for a single line timing file
	uInt32 channel;
	bool value;
	bool convertSuccess;

	if(argvOutput.size() == 2)
	{
		// just expect a 32 bit number
		// not supported yet
		return "failed";
	}
	else if(argvOutput.size() == 3)
	{
		// expect channel, bool
		convertSuccess = stringToValue(argvOutput.at(1), channel);
		convertSuccess = stringToValue(argvOutput.at(2), value);
	}
	else
		return "failed"; // don't know what the user was trying to do

	RawEvent rawEvent(time, channel, value, 1);


	write(channel, value);
//	write(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence

	return "worked";
}

void stf_output_device::parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
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
			//	bitState.at(static_cast<std::size_t>(channel)) = (bit == 0 ? false : true);
				bitState.set(channel, (bit == 0 ? false : true) );
	//			digitalEvent->setBits( bit, channel, channel + 1 );
			}
			else
			{
				delete digitalEvent;	//free memory before throwing exception

				throw EventParsingException(events->second.at(i),
					"The Digital Out board value must be either '1' or '0'.");
			}
		}

		for(unsigned j = 0; j < 24; j++)
		{
			digitalEvent->setBits( bitState.test(j), j, j );
		}

		eventsOut.push_back( digitalEvent );
	}
}

