/*! \file main.cpp
 *  \author Alex Sugarbaker 
 *  \brief main file for arroyo.cpp
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
#include "arroyo.h"

using namespace std;

ORBManager* orbManager;

int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	string ipAddress = "171.64.56.140";

	arroyo arroyoTest(orbManager, "Arroyo Test", ipAddress, 0, 9);	//comPort on ep server is 3

	if (arroyoTest.initialized) {
		orbManager->run();
	} else {
		std::cerr << "Error initializing Arroyo" << std::endl;
	}

	return 0;
}

