/*! \file RAM_Controller_main.cpp
 *  \author Jason Michael Hogan
 *  \brief main() entry point for RAM_Controller
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
#include "RAM_Controller_Device.h"

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);

	std::string ipAddress = "ep-timing1.stanford.edu";
	unsigned short module = 9;

	//FPGA Trigger Device
	RAM_Controller_Device ram_controller1(orbManager, "RAM_Controller", "ep-timing1.stanford.edu", module);
	RAM_Controller_Device ram_controller2(orbManager, "RAM_Controller", "timing-test.stanford.edu", module);

	orbManager->run();

	return 0;
}

