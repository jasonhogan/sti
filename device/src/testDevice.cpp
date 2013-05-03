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
//	addAttribute("BiasVoltage", "1.2");
//	addAttribute("key2", "none", "2, 5, none, full open, true");
}


bool testDevice::updateAttribute(string key, string value)
{
	return false;
}

void testDevice::defineChannels()
{

	addInputChannel(1, STI::Utils::Double);

	addOutputChannel(2, STI::Utils::String);
	addOutputChannel(3, STI::Utils::Double);
	addOutputChannel(4, STI::Utils::Double);

}


std::string testDevice::execute(int argc, char** argv)
{
	int x;
	cerr << "Remote Execution!" << endl;
	cin >> x;

	return "it worked!";
}

bool testDevice::deviceMain(int argc, char** argv)
{
//	char **argv;

	int testX = 1;

//	int x = 0;
	int choice;
//	cin >> x;

	cerr << "Enter choice: ";
	cin >> choice;

	string test = "test";
	while(choice == 0 && testX > 0)
	{
		testX++;
//		if( testX % 5000000 == 0)
		if( testX % 10000 == 0)
		{
			cout << "Sleeping" << endl;
			cin >> testX;
//			testThread->sleep(5);
		}
//		cout << testX << endl;
	}

//	if(testX == 1)
//		cerr << partnerDevice(string("test")).execute(string("")) << endl;

//	partnerDevice("lock").execute("-e1");
	return true;
}



bool testDevice::readChannel(unsigned short channel, 
	const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut)
{
	measurementOut = 3.14;
	return true;
}

bool testDevice::writeChannel(unsigned short channel, const STI::Utils::MixedValue& commandIn)
{
	cout << "write: " << channel << ", " << commandIn.print() << endl;
	return true;
}
