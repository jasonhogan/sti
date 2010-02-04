/*! \file usb1408fsMain.cpp
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

#include "digitelSPCDevice.h"
#include <ORBManager.h>



using namespace std;


ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    

	//unsigned short module = 17;
	//unsigned short comPort = 3;

	digitelSPCDevice gamma10L_S(orbManager, "Gamma 10 L_S Ion Pump", "eplittletable.stanford.edu", 100, 4, "\\\\atomsrv1\\EP\\Data\\pumpPressure\\");
	digitelSPCDevice gamma300L_S(orbManager, "Gamma 300 L_S Ion Pump", "eplittletable.stanford.edu", 100, 3, "\\\\atomsrv1\\EP\\Data\\pumpPressure\\");
	
	orbManager->run();
	
	return 0;
}

