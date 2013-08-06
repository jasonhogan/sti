/*! \file main.cpp
 *  \author Alex Sugarbaker 
 *  \brief main file for pplnTempArroyo.cpp
 *  \section license License
 *
 *  Copyright (C) 2012 Alex Sugarbaker <sugarbak@stanford.edu>
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>, David Johnson <david.m.johnson@stanford.edu>
 *  and Susannah Dickerson <sdickers@stanford.edu>
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
#include "pplnTempArroyo.h"

using namespace std;

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string ipAddress = "EPMezzanine1.stanford.edu";

	int comPortNumber2 = 7; //Laser Path 2
	int comPortNumber1 = 8; //Laser Path 1

	pplnTempArroyo arroyoTemp1(orbManager, "PPLN Temp Arroyo 1", ipAddress, comPortNumber1, comPortNumber1);
	pplnTempArroyo arroyoTemp2(orbManager, "PPLN Temp Arroyo 2", ipAddress, comPortNumber2, comPortNumber2);

	if (arroyoTemp1.initialized && arroyoTemp2.initialized) {
		orbManager->run();
	} else {
		std::cerr << "Error initializing Arroyo " << std::endl;
	}

	return 0;
}

