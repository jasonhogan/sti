/*! 
 *  \author David M.S. Johnson 
 *  \brief main-file for the class FPGA_daughter_board::STF_DA_FAST
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

#include <string>
#include <ORBManager.h>
#include "stf_da_fast_device.h"

#include <iostream>
using namespace std;

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);

//	std::string ipAddress = "ep-timing1.stanford.edu";
//	unsigned short module = 6;
//	unsigned int etraxMemoryAddress = 0x90000048;

//	cout << "MODIFIED Module (6): " << endl;
//	cin >> module;

	//FPGA Trigger Device
//	STF_DA_FAST_Device fastAnalogOutMod6(orbManager, "Fast Analog Out", ipAddress, 6);

	std::string configFilename = "fastAnalogOut.ini"; //default

	if(argc > 1)
	{
		configFilename = string( argv[1] );
	}

	STF_DA_FAST_Device fastAnalogOutMod1(orbManager, configFilename);

//	fastAnalogOutMod1.setChannelNameFilename("fast_channels.ini");

	orbManager->run();

	return 0;
}

