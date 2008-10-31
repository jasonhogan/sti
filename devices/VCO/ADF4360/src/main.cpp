/*! \file main.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file containing main() for testing ADF4360 device code
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

#include "ADF4360.h"
#include <iostream>

int main(int argc, char* argv[])
{
	Analog_Devices_VCO::ADF4360 vco;

	std::cout << "printControlLatch() " << vco.printControlLatch() << std::endl;
	std::cout << "printNLatch()       " << vco.printNLatch() << std::endl;
	std::cout << "printRLatch()       " << vco.printRLatch() << std::endl;

	double fvco;

	vco.setFvco(1200);
	
	while(true)
	{
		std::cout << "Fvco = ";
		std::cin >> fvco;

		vco.setFvco(fvco);
		std::cout << "getFvco()  " << vco.getFvco()  
			<< "   N = " << vco.getN() 
			<< "   A = " << vco.getACounter()
			<< "   B = " << vco.getBCounter()
			<< std::endl;
		vco.sendLatches();

	//	std::cin >> fvco;
	//	vco.SynchronousPowerDown();

	//	std::cin >> fvco;
	//	vco.initialize();
	
	}
/*
	while(true)
	{
		for(int i=0; i<20; i++)
		{
			vco.setFvco(1195 + 0.5*i);
			vco.sendLatches();
		}
	}
*/
}

