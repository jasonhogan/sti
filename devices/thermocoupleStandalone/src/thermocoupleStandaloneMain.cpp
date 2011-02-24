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

#include <string>
#include <iostream>

/*
#include <ORBManager.h>
#include "usb1408fsDevice.h"


using namespace std;


ORBManager* orbManager;


int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    

	unsigned short module = 31;

	usb1408fsDevice USB1408fsDevice(orbManager, "usb1408fs", "eplittletable.stanford.edu", module);
	
	orbManager->run();
	
	return 0;
}
*/


#include "thermocoupleScanner.h"

#include "Matlab.h"

#include <iostream> //cin & cout commands


#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage


int main(int argc, char* argv[])

{
	std::vector <double> thermocoupleVoltages;
	//define scan variables

	bool save_data = true;

	thermocoupleScanner ThermocoupleScanner;

	MATLABPLOTTER matlabplotter;

	std::string dataFilename = matlabplotter.createFilename();

	while(1)
	{
		thermocoupleVoltages.clear();
		ThermocoupleScanner.scanThermocouples(thermocoupleVoltages);

		matlabplotter.plotVector(thermocoupleVoltages);
		matlabplotter.saveVector(dataFilename);

		Sleep(1000 * 60);

	}

/*
	matlabplotter.plotfreqscan(FREQ_vector, DAQ_vector);

	matlabplotter.plotlockpoints(FITFREQ_vector, FITDAQ_vector);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);

	FREQ_vector.clear();
	DAQ_vector.clear();
	FITFREQ_vector.clear();
	FITDAQ_vector.clear();
*/
	return 0;
};

