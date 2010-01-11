/*! \file stf_ad_fast_main.cpp
 *  \author David M.S. Johnson 
 *  \brief main() entry point for FPGA_daughter_board::STF_AD_FAST
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
#include "stf_ad_fast_device.h"
#include <iostream>
using namespace std;

using std::string;

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

//	unsigned short module = 3;
//	cerr << "Enter module: " << endl;
//	cin >> module;

	unsigned int memAddress = 0x90000038;
//	string ipAddress = "ep-timing1.stanford.edu";

	std::string configFilename = "analogIn.ini"; //default

	if(argc > 0)
	{
		configFilename = string( argv[1] );
	}


	//"analog_in_ch3" on timing board ch3
	STF_AD_FAST::STF_AD_FAST_Device analog_in(orbManager, configFilename, memAddress);

	orbManager->run();

	return 0;
}

