/*! \file gpib_hub_main.cpp
 *  \author David M.S. Johnson
 *  \brief main()
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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
#include "vortex6000Device.h"


using namespace std;


ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    

	unsigned short gpibAddressMaster = 1;
	unsigned short gpibAddressSlave = 2;
	//unsigned short module = gpibAddress;
	

	vortex6000Device scanningVortex(orbManager, "Scanning Vortex", "eplittletable.stanford.edu", gpibAddressSlave, gpibAddressSlave);
	vortex6000Device masterVortex(orbManager, "Vortex6000", "eplittletable.stanford.edu", gpibAddressMaster, gpibAddressMaster, false);

	orbManager->run();
	
	return 0;
}

