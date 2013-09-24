/*! \file main.cpp
 *  \author Alex Sugarbaker 
 *  \brief main file for arroyo.cpp
 *  \section license License
 *
 *  Copyright (C) 2013 Alex Sugarbaker <sugarbak@stanford.edu>
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

	typedef boost::shared_ptr<ConfigFile> ConfigFile_ptr;
	ConfigFile_ptr configFile;

	string configFilename = "ArroyoLaserAddresses.ini";
	configFile = ConfigFile_ptr(new ConfigFile(configFilename));

	string ipAddress = "";
	unsigned int arroyoLASCOM = 0;
	int arroyoTECCOM = 0;
	string arroyoName = "";

	configFile->getParameter("ipAddress", ipAddress);

	std::vector<arroyo*> deviceVector;
	arroyo* tempArroyo;

	bool okToRunOrbManager = true;

	int i = 1;
	string texti = boost::lexical_cast<string>(i);
	while(configFile->getParameter("arroyoName" + texti, arroyoName) &&
		  configFile->getParameter("arroyoLASCOM" + texti, arroyoLASCOM) &&
		  configFile->getParameter("arroyoTECCOM" + texti, arroyoTECCOM))
	{
		if(arroyoLASCOM == arroyoTECCOM) {
			tempArroyo = new arroyo(orbManager, arroyoName, ipAddress, arroyoLASCOM, arroyoLASCOM);
		} else {
			tempArroyo = new arroyo(orbManager, arroyoName, ipAddress, arroyoLASCOM, arroyoLASCOM, arroyoTECCOM);
		}
		deviceVector.push_back(tempArroyo);

		if(!(deviceVector.back())->initialized) {
			okToRunOrbManager = false;
			std::cerr << "Error initializing " + arroyoName << std::endl;
		}

		i++;
		texti = boost::lexical_cast<string>(i);
	}

	if (okToRunOrbManager) orbManager->run();

	return 0;
}

