/*! \file trigger_main.cpp
 *  \author Jason Michael Hogan
 *  \brief main() entry point for stf_da_slow_device
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
#include <ORBManager.h>
#include "stf_output_device.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);

//	std::string ipAddress = "ep-timing1.stanford.edu";
//	unsigned short module = 2;

	std::string configFilename = "digitalOut.ini"; //default

	if(argc > 1)
	{
		configFilename = string( argv[1] );
	}

	//FPGA Digital Out board
	stf_output_device digitalOut(orbManager, configFilename);

	orbManager->run();

	return 0;
}

