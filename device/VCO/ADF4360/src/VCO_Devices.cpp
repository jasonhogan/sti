/*! \file VCO_Devices.cpp
 *  \author Jason Michael Hogan
 *  \brief main() entry point for ADF4360_Device (for controling VCOs)
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

#include <string>
#include <iostream>

#include <server/src/corba/ORBManager.h>
#include "ADF4360_Device.h"

using namespace std;


ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	unsigned short module;
	cerr << "Enter module: " << endl;
	cin >> module;

	//"ADF4360"
	Analog_Devices_VCO::ADF4360_Device adf4360_device(orbManager, "ADF4360", "128.12.174.77", module);

	orbManager->run();

	return 0;
}