/*! \file testDevice.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class testDevice
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

#include "testDevice.h"

#include <string>
#include <map>
#include <iostream>

using std::string;
using std::map;
using namespace std;



void testDevice::defineAttributes() 
{
	addAttribute("BiasVoltage", "1.2");
	addAttribute("key2", "none", "2, 5, none, full open, true");
}


bool testDevice::updateAttribute(string key, string value)
{
	return false;
}

void testDevice::defineChannels()
{

	addInputChannel(2, DataNumber);

	addOutputChannel(22, ValueNumber);
	addOutputChannel(57, ValueNumber);
	addOutputChannel(58, ValueNumber);

	enableStreaming(2,"1e-1");

//	Attributes
//	Ch_2_Streaming
//	Ch_2_SamplePeriod
//	Ch_2_DataBuffer

}

bool testDevice::writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event)
{
	return true;
}

bool testDevice::readChannel(STI_Server_Device::TMeasurement & Measurement)
{
	switch(Measurement.channel)
	{
	case 2:
		Measurement.data.number(1e-6 * Measurement.time);
		break;
	default:
		break;
	}

	return true;
}

std::string testDevice::executeArgs(std::string args)
{
	return args;
}

bool testDevice::deviceMain(int argc, char **argv)
{
	int x;
	cin >> x;

	return true;
}

