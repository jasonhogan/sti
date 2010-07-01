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

STF_DA_FAST_Device::STF_DA_FAST_Device(ORBManager* orb_manager, std::string configFilename) : 
FPGA_Device(orb_manager, "Fast Analog Out", configFilename)
{
	activeChannel = 0;
	outputVoltage.push_back(0); //set channel 0 = 0V
	outputVoltage.push_back(0); //set channel 1 = 0V

	minimumEventSpacing = 320; //in nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	holdoff = minimumEventSpacing + 1000 + 8000 - 100 - 5000; //we assume the holdoff is equal to the minimum event spacing (to be verified)

}

STF_DA_FAST_Device::~STF_DA_FAST_Device()
{
}

bool STF_DA_FAST_Device::deviceMain(int argc, char **argv)
{
//	std::cout << "In Main" << endl;

//	int x;
//	cin >> x;
//	STI_Device::execute(string(""));

	return false;
}
	
void STF_DA_FAST_Device::defineAttributes()
{
	addAttribute("Ch. 0 Output Voltage", outputVoltage.at(0)); //set the output voltage to the value for channel 0 (at position 1)
	addAttribute("Ch. 1 Output Voltage", outputVoltage.at(1)); //set the output voltage to the value for channel 1 (at position 2)
}

void STF_DA_FAST_Device::refreshAttributes()
{
	//setAttribute("Ch. 0 Output Voltage", outputVoltage.at(0));
	//setAttribute("Ch. 1 Output Voltage", outputVoltage.at(1));
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
	//std::cerr << "My current active channel is:" << activeChannel << std::endl;
	rawEvent.setValue(outputVoltage.at(activeChannel));
	//std::cerr << "My planned output voltage is:" << outputVoltage.at(activeChannel) << std::endl;
	
	if(success)
	{
		write(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence
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
//	addPartnerDevice("digital","ep-timing1.stanford.edu",2,"Digital Out");
//	partnerDevice("digital").enablePartnerEvents();
}

std::string STF_DA_FAST_Device::execute(int argc, char **argv)
{
	std::vector<std::string> argvOutput;
	STI::Utils::convertArgs(argc, argv, argvOutput);

	//uInt32 time = 10000; //enough time to load events for a single line timing file
	uInt32 channel;
	double value;
	bool convertSuccess = true;

	if(argvOutput.size() == 3)
	{
		// expect channel, value
		convertSuccess = stringToValue(argvOutput.at(1), channel);
		if(!convertSuccess)
			return "Error: Unable to convert channel argument.";

		convertSuccess &= stringToValue(argvOutput.at(2), value);
		if(!convertSuccess)
			return "Error: Unable to convert value argument.";
	}
	else
		return "Error: Invalid argument list. Expecting 'channel' and 'value'."; // don't know what the user was trying to do

//	RawEvent rawEvent(time, channel, value, 1); //time channel value eventNumber

	if(write(channel, value)) //runs parseDeviceEvents on rawEvent and executes a short timing sequence
		return "";
	else
		return "Error: Failed when attempting to write.";
}

void STF_DA_FAST_Device::parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{
	
	RawEventMap::const_iterator events;
	
	uInt32 value = 0;
	bool A_WR = false;
	bool A_LOAD = false;
	bool B_WR = false;
	bool B_LOAD = false;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred
	
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{

		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
			previousTime = minimumAbsoluteStartTime - holdoff * events->second.size();

		
		eventTime = events->first - holdoff * events->second.size(); //need twice the holdoff if two events are being updated at the same time.
		
		


		if((events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The Fast Analog Out board needs " + valueToString(minimumEventSpacing * events->second.size()) + " ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The Fast Analog Out board needs " + valueToString(minimumAbsoluteStartTime)+ " ns at the beginning of the timing file.");
		}

		//check for too large voltages
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).numberValue() > 10 || events->second.at(i).numberValue() < -10)
			{
				throw EventParsingException(events->second.at(i),
					"The Fast Analog Out board only supports voltages between -10 and 10 Volts.");
			}
		}




		if(events->second.size() == 2) //both channels are trying to do something at the same time
		{
			if(events->second.at(0).channel() == events->second.at(1).channel())
			{
				throw EventConflictException(events->second.at(0), 
					events->second.at(1), 
					"The Fast Analog Out cannot currently have multiple events at the same time on the same channel." );
			}
			else
			{
				//first we write the first channel with double the holdoff, but do not load!
				eventTime = events->first - 2*holdoff;
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
					new FastAnalogOutEvent(eventTime, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );

				//second we write the second channel with a single offset, and load both!
				eventTime = events->first - holdoff;
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
					new FastAnalogOutEvent(eventTime, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );
			}
		}
		else //only one channel is trying to do something at the same time
		{
			//we write the channel with the appropriate offset, and load it!
			eventTime = events->first - holdoff;
			if(events->second.at(0).channel() == 0)
			{
				A_WR = true;
				B_WR = false;
				A_LOAD = true;
				B_LOAD = false;
				//std::cerr << "My planned channel:" << events->second.at(0).channel() << std::endl;
				//std::cerr << "I'm in the channel 0 loop." << std::endl;
			}
			else
			{
				A_WR = false;
				B_WR = true;
				A_LOAD = false;
				B_LOAD = true;
				//std::cerr << "My planned channel:" << events->second.at(0).channel() << std::endl;
				//std::cerr << "I'm in the channel 1 loop." << std::endl;
			}
			//std::cerr << "My planned output voltage is:" << events->second.at(0).numberValue() << std::endl;
			value =  static_cast<uInt32>( ( (events->second.at(0).numberValue()+10.0) / 20.0) * 65535.0 );
			eventsOut.push_back( 
				new FastAnalogOutEvent(eventTime, A_WR, A_LOAD, B_WR, B_LOAD, value, this) );





		}
	}
}

