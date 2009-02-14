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

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);

	std::string ipAddress = "ep-timing1.stanford.edu";
	unsigned short module = 6;
	unsigned int etraxMemoryAddress = 0x90000048;

	//FPGA Trigger Device
	STF_DA_FAST_Device fastAnalogOut(orbManager, "FPGA Fast Analog Out", ipAddress, module, etraxMemoryAddress);

	orbManager->run();

	return 0;
}

