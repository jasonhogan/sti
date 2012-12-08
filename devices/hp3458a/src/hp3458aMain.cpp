/*! \file hp3458aMain.cpp
 *  \author David M. S. Johnson
 *  \modified by Susannah Dickerson
 *  \brief main()
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>\n
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

#include <ORBManager.h>
#include "hp3458aDevice.h"


using namespace std;


ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    

	unsigned short gpibAddress = 5;

	//for eplittletable GPIB (uses USB->GPIB)
	//hp3458aDevice multimeter(orbManager, "multimeter hp3458a", "eplittletable.stanford.edu", gpibAddress);
	
	//for use with ENET (ethernet->GPIB)
	hp3458aDevice multimeter(orbManager, "multimeter hp3458a", "li-gpib.stanford.edu", gpibAddress, "hp3458a.ini");

	if (multimeter.initialized)
		orbManager->run();
	else
		std::cerr << "Failed to initialize multimeter" << std::endl;
	
	return 0;
}

