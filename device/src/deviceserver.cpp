/*! \file deviceserver.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the deviceserver test program
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

//This is a test platform for STI_Device interface.
//Combined with autoserver.cpp, deviceserver.cpp will automatically detect
//a Server and acquire a unique DeviceID from the Server. It then uses 
//this ID to register its servants and mount itself on the Server.



#include <ORBManager.h>
#include "testDevice.h"

#include <iostream>
using namespace std;

ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);
	
	unsigned short module;
	cerr << "Enter module: " << endl;
	cin >> module;

	// A derived class of STI_Device
	testDevice myDevice1(orbManager, "testDevice", "128.12.174.77", module);

//	cerr << orbManager->errMsg() << endl;

	orbManager->run();	//start the ORB
	
	return 0;
}

