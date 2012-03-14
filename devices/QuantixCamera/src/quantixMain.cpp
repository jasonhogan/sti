/*! \file main.cpp
 *  \author Susannah Dickerson 
 *  \brief main file for Quantix Camera Device
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah M Dickerson <sdickers@stanford.edu>\n
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
#include <iostream>
#include <exception>

#include <ORBManager.h>
#include "QuantixDevice.h"

using namespace std;

class stringException: public exception
{
public:
	std::string errString;
	stringException(std::string inString) : errString(inString) {}
	const char* what() const throw() {return errString.c_str();}
};

ORBManager* orbManager;
void printError();

int main(int argc, char* argv[])
{
	// for STI
	orbManager = new ORBManager(argc, argv);    
	string ipAddress = "epdesktop2.stanford.edu";

	// for cameras
	int16 totalCameras;
	char camName[CAM_NAME_LEN];
	std::vector <std::string> cameraNames;
	std::vector <int16> cameraHandles;  
	int16 cameraHandle;
	rs_bool success = TRUE;
	bool initialized = true;
	
	QuantixDevice *quantixDevice;
	std::vector<QuantixDevice *> quantixDevices;
	try {
		success = pl_pvcam_init();
		if (!success)
			throw stringException("Could not initialize Quantix drivers");

		//Get the number of cameras and camera names
		pl_cam_get_total(&totalCameras);
		for (int i = 0; i < totalCameras; i++) 
		{
			pl_cam_get_name(i, camName);
			if(!pl_cam_open(camName, &cameraHandle, OPEN_EXCLUSIVE ))
				throw stringException("Could not open Quantix Camera " + STI::Utils::valueToString(i));
			if (!pl_cam_get_diags(cameraHandle))
				throw stringException("Errors in diagnostics for Quantix Camera");
			
			rs_bool availFlag;
			pl_get_param(cameraHandle, PARAM_HEAD_SER_NUM_ALPHA, ATTR_AVAIL, (void *) &availFlag);
			pl_get_param(cameraHandle, PARAM_CHIP_NAME, ATTR_AVAIL, (void *) &availFlag);
			
			int16 type;
			pl_get_param(cameraHandle, PARAM_HEAD_SER_NUM_ALPHA, ATTR_TYPE, (void *) &type);

			char serialNum[CCD_NAME_LEN + 1];
			pl_get_param(cameraHandle, PARAM_CHIP_NAME, ATTR_CURRENT, (void *) &serialNum);
			cameraHandles.push_back(cameraHandle);

			quantixDevice = new QuantixDevice(orbManager, "Quantix " + STI::Utils::valueToString(i), ipAddress, 0, cameraHandle);
			quantixDevices.push_back(quantixDevice);

			initialized &= quantixDevice->initialized;
		}
	

		if (initialized) 
		{
			orbManager->run();
		} else {
			throw stringException("Error initializing Quantix camera(s)");
		}
	}
	catch (stringException& e)
	{
		//Print my message, then Quantix's message (which are somewhat obscure)
		std::cout << e.what() << std::endl;
		printError();
	}
	catch (...)
	{
		std::cout << "Unhandled exception; shutting down..." << std::endl;
	}

	//Shutdown cameras
	for (int i = 0; i < totalCameras; i++) 
	{
		if (pl_cam_check(cameraHandles.at(i)))
			pl_cam_close(cameraHandles.at(i));
	}

    pl_pvcam_uninit();

	for(unsigned int i = 0; i < quantixDevices.size(); i++)
	{
		delete quantixDevices.at(i);
	}



	return 0;
}

void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}