/*! \file offsetLockDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class offsetLockDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David M.S. Johnson <david.m.johnson@stanford.edu>\n
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



#include "offsetLockDevice.h"
#include "STI_Device.h"
#include <types.h>

#include <sstream>
#include <string>
#include <map>
using std::string;
using std::map;

#include <iostream>
using namespace std;


offsetLockDevice::offsetLockDevice(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//set serial address and circuit number defaults
	serialAddressVariable = 2;

	//set initial conditions
	pGain = 0;
	iGain = 0;
	servoFrequency = 10;
	enable = false;
	iEnable = false;
	
	//Initialize the Cs Lock board
	offsetLockBoard = new offsetLock("Offset Lock Board", serialAddressVariable);		//init address 0
}

offsetLockDevice::~offsetLockDevice()
{
}


void offsetLockDevice::defineAttributes() 
{
	addAttribute("P   [0,1]",     pGain );
	addAttribute("I   [0,1]",     iGain );
	addAttribute("Servo Frequency",     servoFrequency );
	addAttribute("Enable",        (enable ? "On" : "Off"), "Off, On");
	addAttribute("Enable I",      (iEnable ? "On" : "Off"), "Off, On");
}

void offsetLockDevice::refreshAttributes() 
{
	setAttribute("P   [0,1]",     pGain );
	setAttribute("I   [0,1]",     iGain );
	setAttribute("Servo Frequency",     servoFrequency );
	setAttribute("Enable",        (enable ? "On" : "Off") );
	setAttribute("Enable I",      (iEnable ? "On" : "Off") );

	offsetLockBoard->enableOffsetLock(enable, iEnable, servoFrequency, iGain, pGain);
	showTextMenu();
}

bool offsetLockDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);
	bool success = successDouble || successInt;

	if(key.compare("Enable") == 0)
	{
		if(value.compare("On") == 0)
			enable = true;
		else
			enable = false;

		success = true;
	}
	else if(key.compare("Enable I") == 0)
	{
		if(value.compare("On") == 0)
			iEnable = true;
		else
			iEnable = false;

		success = true;
	}
	else if(key.compare("P   [0,1]") == 0)
	{
		if( tempDouble <= 1 && tempDouble >= 0 )
		{
			pGain = tempDouble;
			success = true;
		}
		else
		{
			std::cerr << "Please choose a gain value between 0 and 1" << std::endl;
			success = false;
		}

	}
	else if(key.compare("I   [0,1]") == 0)
	{
		if( tempDouble <= 1 && tempDouble >= 0 )
		{
			iGain = tempDouble;
			success = true;
		}
		else
		{
			std::cerr << "Please choose a gain value between 0 and 1" << std::endl;
			success = false;
		}
	}
	else if(key.compare("Servo Frequency") == 0)
	{
		if( tempDouble <= 300 && tempDouble >= -300 )
		{
			servoFrequency = tempDouble;
			success = true;
		}
		else
		{
			std::cerr << "Please choose a frequency between -300 and 300 MHz" << std::endl;
			success = false;
		}
	}

	return success;
}
bool offsetLockDevice::deviceMain(int argc, char **argv)
{
	int selection = -1;
	std::string dummy;
	bool showMenu = true;
	double tempGain;
	
	showTextMenu();


	// Menu Interface
	while(showMenu)
	{
		while(selection != 6)
		{
			refreshAttributes();

			std::cin >> selection;

			if (cin.fail())
			{
				selection = 0;
				cin.clear();
				cin >> dummy;
			}

			switch(selection)
			{
			case 1:
				cout << "P = ";
				cin >> tempGain;
				if( tempGain <= 1 && tempGain >= 0 )
					pGain = tempGain;
				else
					std::cerr << "Please choose a gain value between 0 and 1" << std::endl;
				break;
			case 2:
				cout << "I = ";
				cin >> tempGain;
				if( tempGain <= 1 && tempGain >= 0 )
					iGain = tempGain;
				else
					std::cerr << "Please choose a gain value between 0 and 1" << std::endl;
				break;
			case 3:
				cout << "freq = ";
				cin >> tempGain;
				if( tempGain <= 300 && tempGain >= -300 )
					servoFrequency = tempGain;
				else
					std::cerr << "Please choose a frequency value between -300 and 300 MHz" << std::endl;
				break;
			case 4:
				enable = !enable;
				break;
			case 5:
				iEnable = !iEnable;
				break;
			case 6:
				showMenu = false;
				break;
			default:
				break;
			}
		}
	}

	deviceShutdown();
	
	return false;
}
void offsetLockDevice::showTextMenu()
{
	cout << endl 
		<< "-------------------" << endl;
	cout << "(1) P   [0,1]              : " << pGain << endl;
	cout << "(2) I   [0,1]              : " << iGain << endl;
	cout << "(3) Servo Frequency        : " << servoFrequency << " MHz" << endl;
	cout << "(4) Enable                 : " << (enable ? "On" : "Off") << endl;
	cout << "(5) Enable I               : " << (iEnable ? "On" : "Off") << endl;
	cout << "(6) Quit" << endl;
}
