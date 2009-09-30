//MUST INCLUDE ATMCD32M.lib among linker files. Keep this lib files and the atmcd32d.h files in the same directory as these files


/*! \file ANDOR885_device.cpp
 *  \author Susannah Dickerson 
 *  \brief Source file for the iXon DV-885 camera
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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

#include "andor885_device.h"


ANDOR885_Device::ANDOR885_Device(
		ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	int index = 0;

	initialized = false;

	//Initialize necessary parameters
	readMode_t.name = "Read mode";
	readMode_t.choices.push_back("Image");
	readMode_t.choiceFlags.push_back(READMODE_IMAGE);

	shutterMode_t.name = "Shutter mode";
	char *shutterModeChoices[] = {"Auto","Open","Closed"};
	int   shutterModeFlags  [] = {SHUTTERMODE_AUTO, SHUTTERMODE_OPEN, SHUTTERMODE_CLOSE};
	shutterMode_t.choices.assign(shutterModeChoices,shutterModeChoices + 3);
	shutterMode_t.choiceFlags.assign(shutterModeFlags,shutterModeFlags + 3);

	acquisitionMode_t.name = "Acquisition mode";
	char *acquisitionModeChoices[] = {"Single scan","Kinetic series","Run 'til abort"};
	int   acquisitionModeFlags  [] = {ACQMODE_SINGLE_SCAN, ACQMODE_KINETIC_SERIES, ACQMODE_RUN_TILL_ABORT};
	acquisitionMode_t.choices.assign(acquisitionModeChoices,acquisitionModeChoices + 3);
	acquisitionMode_t.choiceFlags.assign(acquisitionModeFlags,acquisitionModeFlags + 3);

	pImageArray = NULL;

	cameraStat		=	ON;
	acquisitionStat	=	OFF;
	acquisitionMode	=	ACQMODE_SINGLE_SCAN;
	readMode		=	READMODE_IMAGE;
	exposureTime	=	0.05;
	accumulateTime	=	0;
	kineticTime		=	0;
	ttl				=	TTL_OPEN_LOW;
	shutterMode		=	SHUTTERMODE_OPEN;
	closeTime		=	SHUTTER_CLOSE_TIME;
	openTime		=	SHUTTER_OPEN_TIME;
	triggerMode		=	TRIGGERMODE_EXTERNAL;
	frameTransfer	=	ON;
	spoolMode		=	OFF;
	numExposures	=	10;					//initialize number of exposures to 10. Kinetic mode only
	coolerTemp		=  -50;
	coolerStat		=	OFF;

	readMode_t.initial = findToggleAttribute(readMode_t, readMode);
	shutterMode_t.initial = findToggleAttribute(shutterMode_t, shutterMode);

	//Name of path to which files should be saved
	spoolPath		=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\";
	palPath			=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\GREY.PAL";

	initialized = !InitializeCamera();

}

ANDOR885_Device::~ANDOR885_Device()
{
}

bool ANDOR885_Device::deviceMain(int argc, char **argv)
{
	bool error = false;
	int message;

	ofstream cerrLog;
	char tempChar[MAX_PATH];
	std::string tempString;

	tempString = spoolPath + "cerr.log";
	strcpy(tempChar,tempString.c_str());
	
	cerrLog.open(tempChar);

	streambuf* cerrBuffer = cerr.rdbuf(); // save cerr's output buffer

	cerr.rdbuf (cerrLog.rdbuf()); // redirect output into the file	

	std::cout << "*****************\n";
	std::cout << "* DO NOT Ctrl-c *\n";
	std::cout << "*****************\n";
	std::cout << "Press any key to cleanly shutdown camera and program... ";
	std::cin >> message;
	error = deviceExit();

	cerr.rdbuf (cerrBuffer); // restore old output buffer

	cerrLog.close();

	return error;
}
	
bool ANDOR885_Device::deviceExit()
{
	int errorValue;
	bool error = false;
	int temp;

	//Stop Acquisition
	error = !AbortIfAcquiring();

    //CloseShutter
	errorValue=SetShutter(ttl,shutterMode,closeTime,openTime);
	printError(errorValue, "Shutter error", &error, ANDOR_ERROR);
/*	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Shutter error\n";
		error = true;  
	}
*/
	errorValue = GetTemperature(&temp);
	if(errorValue != DRV_TEMP_OFF){
		errorValue=CoolerOFF();        // Switch off cooler (if used)
		printError(errorValue, "Error switching cooler off", &error, ANDOR_ERROR);
	}

	std::cerr << "Camera temperature is: " << temp << " deg C" << std::endl;
	while(GetTemperature(&temp) < 5)
	{
		Sleep(1000);
		std::cerr << "Camera temperature rising...: " << temp << " deg C" << std::endl;
	}

	errorValue = ShutDown();
	printError(errorValue, "Error shutting down", &error, ANDOR_ERROR);

	std::cerr << "Shutting down..." << std::endl;

	initialized = false;
    FreeBuffers();      // frees memory used by image buffers in xxxxWndw.c
	return error;
}

void ANDOR885_Device::defineAttributes()
{

	//Attributes
	addAttribute("Camera status", "On", "On, Off");
	addAttribute("Acquisition status", "Off", "On, Off");
	addAttribute(acquisitionMode_t.name,"Single scan", makeString(acquisitionMode_t.choices));
	addAttribute("Trigger mode", "External", "External, Internal"); //trigger mode
//	addAttribute("Read mode", "Image","Image, Multi-track, Random-track, Single-track"); //readout mode of data
	addAttribute("Read mode", "Image","Image"); //readout mode of data
	addAttribute("Spool mode", "Off", "On, Off"); //spooling of data
	
	addAttribute(shutterMode_t.name, "Always open", makeString(shutterMode_t.choices)); // Shutter control
	addAttribute("Shutter open time (ms)", openTime); //time it takes shutter to open
	addAttribute("Shutter close time (ms)", closeTime); //time it takes shutter to close
	
	addAttribute("Exposure time (s)", exposureTime); //length of exposure

	addAttribute("Folder Path for saved files", spoolPath);
	addAttribute("Number of exposures", numExposures);

	addAttribute("Cooler setpoint", coolerTemp);
	addAttribute("Cooler status", "Off", "On, Off");
	
}

void ANDOR885_Device::refreshAttributes()
{
	
	// All attributes are stored in c++, none are on the fpga
	//Attributes not set in serial commands

	setAttribute("Camera status", (cameraStat == ON) ? "On" : "Off");

	setAttribute("Acquisition status", (acquisitionStat == ON) ? "On" : "Off");

	setAttribute(acquisitionMode_t.name, findToggleAttribute(acquisitionMode_t, acquisitionMode));	

	setAttribute("Trigger mode", ((triggerMode == TRIGGERMODE_EXTERNAL) ? "External" : "Internal")); //trigger mode?

	setAttribute(readMode_t.name, findToggleAttribute(readMode_t, readMode));

	setAttribute("Spool mode", (spoolMode == ON) ? "On" : "Off");

	setAttribute(shutterMode_t.name, findToggleAttribute(shutterMode_t, shutterMode));	
	setAttribute("Shutter open time (ms)", openTime); 
	setAttribute("Shutter close time (ms)", closeTime); 

	setAttribute("Exposure time (s)", exposureTime);

	setAttribute("Folder Path for saved files", spoolPath);
	setAttribute("Number of exposures", numExposures);

	setAttribute("Cooler setpoint", coolerTemp);
	setAttribute("Cooler status", (coolerStat == ON) ? "On" : "Off");

}

bool ANDOR885_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;
	int tempInt;
	char tempChar[MAX_PATH];
	std::string tempString;
	int temperature;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = false;
	int error;

	// When the camera is acquiring, the user is prohibited from changing the attributes,
	// except to turn off the acquisition.
	if(acquisitionStat != ON || (key.compare("Acquisition status") == 0 && value.compare("Off") == 0)) {

		if(key.compare("Camera status") == 0) {
			success = true;

			if (value.compare("On") == 0){

				// Turn on camera, if it's not already on
				if (!initialized){
					std::cerr << "1. Initializing..." << std::endl;
					initialized = !InitializeCamera();
				}

				if (!initialized){
					std::cerr << "1. Error initializing camera" << std::endl;
					success = false;
				} else {
					std::cerr << "1. Camera on" << std::endl;
					cameraStat = ON;
				}
			} 
			
			else if (value.compare("Off") == 0) {

				// Turn off camera, if it's not already off.
				if (initialized) {
					success = !deviceExit();
				}

				if (!success) {
					std::cerr << "1. Error shutting down camera" << std::endl;
				} else {
					std::cerr << "1. Camera off" << std::endl;
					cameraStat = OFF;
				}
			} 
			
			else {
				success = false;
				std::cerr << "1. Camera status selection error" << std::endl;
			}
		} 
		
		else if(key.compare("Acquisition status") == 0) {
			success = true;

			if (value.compare("Off") == 0) {

				// Turn off acquisition, aborting if necessary
				if (acquisitionStat != OFF){
					acquisitionStat = OFF;	
					success = AbortIfAcquiring();
					if(success) {
						std::cerr << "2. Acquisition status: off" << std::endl;
					} else {
						std::cerr << "2. Acquisition status error" << std::endl;
					}
				} else {
					std::cerr << "2. Acquisition status already off" << std::endl;
				}
			} 
			
			else if (value.compare("On") == 0) {

				// Check to see if camera is idle before trying to turn on acquisition
				GetStatus(&error);

				if (error != DRV_IDLE || (acquisitionMode == ACQMODE_KINETIC_SERIES && triggerMode != TRIGGERMODE_EXTERNAL)) {
				
					if (error == DRV_IDLE) {
						std::cerr << "2. Acquisition status already on or camera is not idle" << std::endl;
					}

					if (acquisitionMode == ACQMODE_KINETIC_SERIES && triggerMode != TRIGGERMODE_EXTERNAL) {
						std::cerr << "Kinetic series must be used with external trigger mode" << std::endl;
					}
				
				}
				else {
					acquisitionStat = ON;

					if (acquisitionMode == ACQMODE_KINETIC_SERIES && triggerMode == TRIGGERMODE_EXTERNAL) {
						SetNumberAccumulations(1);
						SetNumberKinetics(numExposures);
					}

					// Start acquisition
					error = StartAcquisition();
					printError(error, "2. Error starting acquisition", &success, ANDOR_SUCCESS); 
					
					if(success) {
						std::cerr << "2. Starting acquisition." << std::endl;
					}	


					//Save data and reset acquisition mode if performing a single scan
					if (acquisitionMode == ACQMODE_SINGLE_SCAN) {
						SaveSingleScan();
					} 
					else if (acquisitionMode == ACQMODE_RUN_TILL_ABORT || 
						acquisitionMode == ACQMODE_KINETIC_SERIES) {
						omni_thread::create(saveContinuousDataWrapper, (void*) this, omni_thread::PRIORITY_NORMAL);
					}
					else 
						std::cerr << "2. Unknown acquisition mode" << std::endl;
				} 
			}
			
			else {
				success = false;
				std::cerr << "2. Error setting Acquisition Status" << std::endl;
			}
		}

		else if(key.compare("Acquisition mode") == 0) {
			success = true;

			if(value.compare("Single scan") == 0) {
				acquisitionMode = ACQMODE_SINGLE_SCAN;

				// If the camera is still in run 'til abort mode, stop.
				AbortIfAcquiring();
				std::cerr << "3. Acquisition mode Single Scan" << std::endl;
			}

			else if (value.compare ("Kinetic series") == 0) {
				acquisitionMode = ACQMODE_KINETIC_SERIES;

				std::cerr << "3. Acquisition mode Kinetic series" << std::endl;
			}

			else if (value.compare("Run 'til abort") == 0) {
				acquisitionMode = ACQMODE_RUN_TILL_ABORT;

				std::cerr << "3. Acquisition mode Run 'til abort" << std::endl;
			}

			else {
				success = false;
				std::cerr << "3. Error setting Acquisition Mode" << std::endl;
			}
		
			// Set acquisition mode to required setting
			if (success) {
				error=SetAcquisitionMode(acquisitionMode);
				printError(error, "3. Set Acquisition Mode Error", &success, ANDOR_SUCCESS);
			}
		}

		else if (key.compare("Trigger mode") == 0) {
			success = true;
			
			if (value.compare("Internal") == 0) {
				triggerMode = TRIGGERMODE_INTERNAL;
				std::cerr << "4. Trigger mode is Internal" << std::endl;
			}
			else if (value.compare("External") == 0) {
				triggerMode = TRIGGERMODE_EXTERNAL;
				std::cerr << "4. Trigger mode is External" << std::endl;
			}
			else {
				success = false;
				std::cerr << "4. Error setting Trigger mode" << std::endl;
			}

			if (success) {
				error=SetTriggerMode(triggerMode);
				printError(error, "4. Error setting trigger", &success, ANDOR_SUCCESS);
			}
		}

		else if(key.compare("Read mode") == 0) {
			success = true;

	/*		if(value.compare("Multi-track") == 0)
			{
				readMode=READMODE_MULTI_TRACK;
				std::cerr << "Read mode is multi-track" << std::endl;
			}
			else if(value.compare("Random-track") == 0)
			{
				readMode=READMODE_RANDOM_TRACK;
				std::cerr << "Read mode is random-track" << std::endl;
			}
			else if(value.compare("Single-track") == 0)
			{
				readMode=READMODE_SINGLE_TRACK;
				std::cerr << "Read mode is single-track" << std::endl;
			}
			else	*/
			if(value.compare("Image") == 0) {
				if(readMode != READMODE_IMAGE) {
					readMode=READMODE_IMAGE;
					std::cerr << "5. Read mode is image" << std::endl;

					// This function only needs to be called when acquiring an image. It sets
					// the horizontal and vertical binning and the area of the image to be
					// captured. In this example it is set to 1x1 binning and is acquiring the
					// whole image
  					SetImage(1,1,1,gblXPixels,1,gblYPixels);
				}
				else {
					std::cerr << "5. Read mode is already image" << std::endl;
				}
			}
			else {
				success = false;
				std::cerr << "5. Error in read mode value" << std::endl;
			}

			if (success) {
				error=SetReadMode(readMode);
				if(error!=DRV_SUCCESS){
					std::cerr << "5. Error setting read mode" << std::endl;
					success = false;
				} else {
					std::cerr << "5. Read mode set" << std::endl;
				}
			}
		}

		else if(key.compare("Spool mode") == 0)
		{
			success = true;
			if (value.compare("On") == 0) {
				if (acquisitionMode == ACQMODE_SINGLE_SCAN){
					error = SetSpool(0,0,NULL,10);  //Disabled
					std::cerr << "6. Cannot spool in single scan read mode\n";
					if (error == DRV_SUCCESS) {
						std::cerr << "6. Spooling disabled\r\n";
						spoolMode = OFF;
					} else {
						std::cerr << "6. Error disabling spooling" << std::endl;
					}
				}
				else if (spoolMode != ON) {
					strcpy(tempChar,spoolPath.c_str());
					error = SetSpool(1,0,tempChar,10); //Enabled; 10 images can be stored in RAM before error
					if (error == DRV_SUCCESS) {
						std::cerr << "6. Spooling Enabled\r\n";
						spoolMode = ON;
					} else {
						std::cerr << "6. Error enabling spooling" << std::endl;
					}
				} else {
					std::cerr << "6. Spooling already enabled" << std::endl;
				}
			}

			else if (value.compare("Off") == 0) {
				if (spoolMode != OFF) {
					spoolMode = OFF;
					SetSpool(0,0,NULL,10);  //Disabled
					std::cerr << "6. Spooling Disabled" << std::endl;
				} else {
					std::cerr << "6. Spooling already disabled" << std::endl;
				}
			} 
			
			else {
				success = false;
				std::cerr << "6. Error selecting Spool mode" << std::endl;
			}
		}

		else if(key.compare("Shutter mode") == 0) {
			success = true;
			if(value.compare("Auto") == 0) {
				shutterMode = SHUTTERMODE_AUTO;
				std::cerr << "7. Shutter mode is auto" << std::endl;
			}
			else if(value.compare("Always open") == 0) {
				shutterMode = SHUTTERMODE_OPEN;
				std::cerr << "7. Shutter mode is always open" << std::endl;
			}
			else if(value.compare("Always closed") == 0) {
				shutterMode = SHUTTERMODE_CLOSE;
				std::cerr << "7. Shutter mode is always closed" << std::endl;
			}
			else {
				success = false;
				std::cerr << "7. Error setting Shutter mode" << std::endl;
			}

			if (success) {
				error=SetShutter(ttl, shutterMode, closeTime, openTime);
				printError(error, "7. Error setting shutter", &success, ANDOR_SUCCESS);
			}
		}		

		else if(key.compare("Shutter open time (ms)") == 0 && successInt)
		{
			success = true;

			openTime = tempInt;

			if (openTime < 1)
			{
				openTime = 1;
				std::cerr << "8. Shutter opening time minimum: 1 ms" << std::endl;
			}

			error = SetShutter(ttl, shutterMode, closeTime, openTime);
			if(error!=DRV_SUCCESS){
				std::cerr << "8. Error setting shutter opening time" << std::endl;
				success = false;
			}
			else {
				std::cerr << "8. Shutter opening time set" << std::endl;
			}
		}

		else if(key.compare("Shutter close time (ms)") == 0 && successInt)
		{
			success = true;

			closeTime = tempInt;

			if (closeTime < 1) {
				closeTime = 1;
				std::cerr << "8. Shutter closing time minimum: 1 ms" << std::endl;

			}

			error=SetShutter(ttl, shutterMode, closeTime, openTime);
			if(error!=DRV_SUCCESS){
				std::cerr << "9. Error setting shutter closing time" << std::endl;
				success = false;
			}
			else {
				std::cerr << "9. Shutter closing time set" << std::endl;
			}
		}

		else if(key.compare("Exposure time (s)") == 0 && successDouble) {
			success = true;
			
			exposureTime = (float) tempDouble;

			error=SetExposureTime(exposureTime);
			if(error!=DRV_SUCCESS){
				std::cerr << "10. Error setting exposure time" << std::endl;
				success = false;
			}

			// It is necessary to get the actual times as the system will calculate the
			// nearest possible time. eg if you set exposure time to be 0, the system
			// will use the closest value (around 10 us in FrameTransfer mode)
			GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
			std::cerr << "10. Actual Exposure Time is " << exposureTime << " s.\n";
		}

		else if(key.compare("Folder Path for saved files") == 0) {
			success = true;
			spoolPath = value;
			std::cerr << "11. Folder path set" << std::endl;
		}

		else if (key.compare("Number of exposures") == 0 && successInt) {
			success = true;
			numExposures = tempInt;
			if (numExposures < 1) {
				numExposures = 1;
				std::cerr << "12. Number of Exposures should be 1 or more." << std::cerr;
			}

			std::cerr << "12. Number of exposures set" << std::endl;
		}

		else if (key.compare("Cooler setpoint") == 0 && successInt) {
			success = true;
			coolerTemp = tempInt;

			if (coolerStat == ON) {
				error = SetTemperature(coolerTemp);
				printError(error, "13. Error setting cooler temperature", &success, ANDOR_SUCCESS);
				
				if(success){
					std::cerr << "13. Cooler temperature set to: " << coolerTemp << std::endl;
				}
			}
		}

		else if (key.compare("Cooler status") == 0) {
			success = true;
			if (value.compare("On") == 0)
			{
/*				SetTemperature(coolerTemp);
				printError(error, "14. Error setting cooler temperature", &success, ANDOR_SUCCESS);
				if (success) 
				{
					error = CoolerON();
					printError(error, "14. Error turning on cooler", &success, ANDOR_SUCCESS);
				}

				while(GetTemperature(&temperature) == DRV_TEMP_NOT_REACHED)
				{
					Sleep(1000);
					std::cerr << "14. Camera temperature dropping...: " << tempearture << " deg C" << std::endl;
				}

				std::cerr << "14. Camera temperature set: " << temperature << " deg C" << std::endl;
*/
				std::cerr << "14. Cooler on...kinda " << std::endl;
			} 
			
			else if (value.compare("Off") == 0) {
				error = CoolerOFF();
				printError(error, "14. Error turning off cooler", &success, ANDOR_SUCCESS);
				if (success)
					std::cerr << "14. Cooler off...definitely" << std::endl;
			}
			else {
				success = false;
				std::cerr << "14. Error setting cooler status" << std::endl;
			}
		}
	}

	else {
		std::cerr << "Stop acquisition before changing the parameters" << std::endl;
	}
	
	return success;

}

void ANDOR885_Device::defineChannels()
{
	
}

bool ANDOR885_Device::readChannel(ParsedMeasurement& Measurement)
{
	//No provisions to read data from DDS device
	return false;
}

bool ANDOR885_Device::writeChannel(const RawEvent& Event)
{
	return false;
}


std::string ANDOR885_Device::execute(int argc, char **argv)
{
	return "";
}

void ANDOR885_Device::definePartnerDevices()
{
}


void ANDOR885_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception)
{
	return;
}


//------------------------------------------------------------------------------
//	FUNCTION NAME:	InitializeCamera()
//
//  RETURNS:				If the function terminates before entering the message loop,
//      						return FALSE.
//    							Otherwise, return the WPARAM value sent by the WM_QUIT
//									message.
//
//  LAST MODIFIED:	PMcK	11/11/98
//
//  DESCRIPTION:		calls initialization function, processes message loop
//
//                  Windows recognizes this function by name as the initial
//									entry point for the program.  This function calls the
//									application initialization routine, if no other instance of
//									the program is running, and always calls the instance
//									initialization routine.  It then executes a	message
//									retrieval and dispatch loop that is the top-level control
//    							structure for the remainder of execution.  The loop is
//									terminated when a WM_QUIT  message is received, at which
//									time this function exits the application instance by
//									returning the value passed by PostQuitMessage().
//
//    							If the function must abort before entering the message loop,
//									it returns the conventional value NULL.
//
//
//	ARGUMENTS: 			hInstance - The handle to the instance of this application
//									that is currently being executed.
//
//    							hPrevInstance - The handle to the instance of this
//									application that was last executed.  If this is the only
//									instance of this application executing, hPrevInstance is
//									NULL. In Win32 applications, this parameter is always NULL.
//
//    							lpCmdLine - A pointer to a null terminated string specifying
//									the command line of the application.
//
//    							nCmdShow - Specifies how the main window is to be diplayed.
//------------------------------------------------------------------------------
bool ANDOR885_Device::InitializeCamera()
{
	AndorCapabilities	caps;
	char 				aBuffer[256];
	int					errorValue;
	bool				errorFlag = false;
//	int 				test,test2; //need to pause while camera initializes

	float				speed, STemp;
	int					iSpeed, iAD, nAD, index;


	caps.ulSize = sizeof(AndorCapabilities);

    GetCurrentDirectoryA(256,aBuffer);// Look in current working directory
                                    // for driver files. Note: had to override usual mapping of GetCurrentDirectory to
									// GetCurrentDirectoryW because of mismatch of argument types.

    errorValue=Initialize(aBuffer);  // Initialize driver in current directory
	printError(errorValue, "Initialize error", &errorFlag, ANDOR_ERROR);
	if (errorFlag)
		return true;

    // Get camera capabilities
    errorValue=GetCapabilities(&caps);
	printError(errorValue, "Get Andor Capabilities information Error", &errorFlag, ANDOR_ERROR);

    // Get Head Model
    errorValue=GetHeadModel(model);
	printError(errorValue, "Get Head Model information Error", &errorFlag, ANDOR_ERROR);

    // Get detector information
    errorValue=GetDetector(&gblXPixels,&gblYPixels);
	printError(errorValue, "Get Detector information Error", &errorFlag, ANDOR_ERROR);

	// Set frame transfer mode
	errorValue=SetFrameTransferMode((frameTransfer == ON) ? 1 : 0);
	printError(errorValue, "Set Frame Transfer Mode Error", &errorFlag, ANDOR_ERROR);

    // Set acquisition mode to required setting specified in xxxxWndw.c
    errorValue=SetAcquisitionMode(acquisitionMode);
	printError(errorValue, "Set Acquisition Mode Error", &errorFlag, ANDOR_ERROR);

	if(readMode == READMODE_IMAGE) {
    	// This function only needs to be called when acquiring an image. It sets
		// the horizontal and vertical binning and the area of the image to be
		// captured. In this example it is set to 1x1 binning and is acquiring the
		// whole image
  		SetImage(1,1,1,gblXPixels,1,gblYPixels);
	}

    // Set read mode to required setting specified in xxxxWndw.c
    errorValue=SetReadMode(readMode);
	printError(errorValue, "Set Read Mode Error", &errorFlag, ANDOR_ERROR);

    // Set Vertical speed to max
    STemp = 0;
    VSnumber = 0;
    GetNumberVSSpeeds(&index);
    for(iSpeed=0; iSpeed<index; iSpeed++){
      GetVSSpeed(iSpeed, &speed);
      if(speed > STemp){
        STemp = speed;
        VSnumber = iSpeed;
      }
    }
    errorValue=SetVSSpeed(VSnumber);
	printError(errorValue, "Set Vertical Speed Error", &errorFlag, ANDOR_ERROR);

    // Set Horizontal Speed to max 
	//(scan over all possible AD channels; the 885 has only one 14-bit channel)
    STemp = 0;
    HSnumber = 0;
    ADnumber = 0;
    errorValue = GetNumberADChannels(&nAD);
    if (errorValue != DRV_SUCCESS){
	  std::cerr << "Get number AD Channel Error\n";
      errorFlag = true;
    }
    else {
      for (iAD = 0; iAD < nAD; iAD++) {
        GetNumberHSSpeeds(iAD, 0, &index);
        for (iSpeed = 0; iSpeed < index; iSpeed++) {
          GetHSSpeed(iAD, 0, iSpeed, &speed);
          if(speed > STemp){
            STemp = speed;
            HSnumber = iSpeed;
            ADnumber = iAD;
          }
        }
      }
    }

    errorValue=SetADChannel(ADnumber);
	printError(errorValue, "Set AD Channel Error", &errorFlag, ANDOR_ERROR);

    errorValue=SetHSSpeed(0,HSnumber);
	printError(errorValue, "Set Horizontal Speed Error", &errorFlag, ANDOR_ERROR);

    if(errorFlag)
    	//MessageBox(GetActiveWindow(),aBuffer,"Error!",MB_OK); SMD
	    std::cerr<<aBuffer<<std::endl;


  // Wait for 2 seconds to allow MCD to calibrate fully before allowing an
  // acquisition to begin
//  test=GetTickCount();
//  do{
//  	test2=GetTickCount()-test;
//  }while(test2<2000);

	Sleep(2000);

	errorValue = SetExposureTime(exposureTime);
	printError(errorValue, "Exposure time error", &errorFlag, ANDOR_ERROR);

	// It is necessary to get the actual times as the system will calculate the
	// nearest possible time. eg if you set exposure time to be 0, the system
	// will use the closest value (around 0.01s)
	  GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
	  std::cerr << "Actual Exposure Time is " << exposureTime << " s.\n";

  // Set Shutter is made up of ttl level, shutter and open close time

  //Check Get open close time
	if(openTime==0)
		openTime=1;
	if(closeTime==0)
		closeTime=1;

	// Set shutter
	errorValue=SetShutter(ttl,shutterMode,closeTime,openTime);
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Shutter error\n";
		errorFlag = true;  
	}
	else
		std::cerr << "Shutter set to specifications\n";

	// Set trigger selection
	errorValue=SetTriggerMode(triggerMode);
	printError(errorValue, "Set Trigger Mode Error", &errorFlag, ANDOR_ERROR);

	//Allocate buffers for external triggering;
/*	if (acquisitionMode == ACQMODE_SINGLE_SCAN){
		bufferSize = 1;
	} else if (acquisitionMode == ACQMODE_RUN_TILL_ABORT) {
	  bufferSize = numExposures;
	}
	AllocateBuffers();
*/
	// Returns the value from PostQuitMessage
	return errorFlag;
}


//------------------------------------------------------------------------------
//	FUNCTION NAME:	AllocateBuffers()
//
//  RETURNS:				int size:  size of the image buffer
//
//  LAST MODIFIED:	PMcK	03/11/98
//
//  DESCRIPTION:    This function allocates enough memory for the buffers (if not
//									allocated already).
//
//	ARGUMENTS: 			NONE
//------------------------------------------------------------------------------

int ANDOR885_Device::AllocateBuffers(void)
{

	int 	size;

	FreeBuffers();

	size=bufferSize*gblXPixels*gblYPixels;  // Needs to hold full image

  // only allocate if necessary
	if(!pImageArray)
  	pImageArray = (long *) malloc(size*sizeof(long));

  return size;
}

//------------------------------------------------------------------------------
//	FUNCTION NAME:	FreeBuffers()
//
//  RETURNS:				NONE
//
//  LAST MODIFIED:	PMcK	03/11/98
//
//  DESCRIPTION:    This function frees the memory allocated each buffer.
//
//	ARGUMENTS: 			NONE
//------------------------------------------------------------------------------

void ANDOR885_Device::FreeBuffers(void)
{
  // free all allocated memory
  if(pImageArray){
    free(pImageArray);
    pImageArray = NULL;
  }
}


bool ANDOR885_Device::AbortIfAcquiring()
{
	int error;
	bool success = true;

	//Check to see if the camera is acquiring. If it is, stop
	GetStatus(&error);
	if(error == DRV_ACQUIRING){
		error = AbortAcquisition();
		printError(error, "Error aborting acquisition", &success, ANDOR_SUCCESS);
	}
	else
		std::cerr << "Camera not acquiring" << std::endl;

	return success;
}

// In Single Scan mode, save the data in raw form as well as a bitmap
bool ANDOR885_Device::SaveSingleScan()
{
	int error;
	bool success = true;

	char tempChar[MAX_PATH];
	std::string tempString;

	std::string localTimeString;
	int imageSize = gblXPixels*gblYPixels;
	std::vector <at_32> tempImageVector (imageSize);
	
//	int size;

	//Don't save until the camera has stopped acquiring
	WaitForAcquisitionTimeOut(10000);
/*	GetStatus(&error);
	while(error == DRV_ACQUIRING)
	{
		Sleep(10);
		GetStatus(&error);
	}
*/
	//Use the date to label images
	localTimeString = makeTimeString();
	
/*	bufferSize = 1;
	size = AllocateBuffers();
	*/
//	error = GetAcquiredData(pImageArray, size);
	error = GetAcquiredData(&tempImageVector[0], imageSize);
	printError(error, "Error in acquiring data", &success, ANDOR_SUCCESS);

//	tempImageVector.assign(pImageArray , pImageArray + size);
	pImageVector.push_back(tempImageVector);

	saveImageVector();

	//Convert string to the necessary char *
	tempString = spoolPath+"image_"+localTimeString+".bmp";
	strcpy(tempChar,tempString.c_str());

	//the 0,0 at the end means the image will scale across full range of values.
	error = SaveAsBmp(tempChar,palPath,0,0);
	if(error!=DRV_SUCCESS){
		std::cerr << "Error saving image: "<< error << std::endl;
		success = false;
	} else {
		std::cerr << "Saved!" << std::endl;
	}

	acquisitionStat = OFF;

	return success;
}

//Credit goes to Dave Johnson for the date manipulations
std::string ANDOR885_Device::makeTimeString()
{
	std::string localTimeString;
	size_t found;

	struct tm localTime;
	__int64 rawTime;
	char time_buf[26];
	errno_t err;

	_time64( &rawTime );

	// Obtain coordinated universal time: 
	err = _localtime64_s( &localTime, &rawTime );
	if (err)
	{
		std::cerr << "2. Invalid Argument to _gmtime64_s." << std::endl;
	}

	// Convert to an ASCII representation 
	err = asctime_s(time_buf, 26, &localTime);
	if (err)
	{
		std::cerr << "2. Invalid Argument to asctime_s." << std::endl;
	}		

	localTimeString = time_buf;

	found=localTimeString.find_first_of(":");

	while (found!=std::string::npos)
	{
		localTimeString[found]='_';
		found=localTimeString.find_first_of(":",found+1);
	}	

	found=localTimeString.find_first_of("\n");

	while (found!=std::string::npos)
	{
		localTimeString.erase(found, 1);
		found=localTimeString.find_first_of("\n",found+1);
	}

	return localTimeString;
}


// Makes the string of attribute choices for addAttribute 
std::string ANDOR885_Device::makeString(vector <std::string>& choices)
{
	std::string tempString;
	std::string filler = ", ";
	unsigned int len;
	unsigned int i;

	len = choices.size();

	for(i = 0; i < len-1; i++){
		tempString += choices.at(i);
		tempString += filler;
	}
	tempString += choices.at(i);

	return tempString;
}

// Finds the string associated with the flag in the toggleAttribute class
std::string ANDOR885_Device::findToggleAttribute(toggleAttribute &attr, int flag)
{
	unsigned int i;
	std::string tempString = "";

	//Find string associated with flag
	for (i = 0; i < attr.choiceFlags.size(); i++ ){
		if (attr.choiceFlags.at(i) == flag){
			tempString = attr.choices.at(i);
		}
	}

	if (tempString.compare("") == 0){
		std::cerr << "Error in " << attr.name << " selection." << std::endl;
	}

	return tempString;
}

// Determines if an error has occured and prints an error message. Sets the bool.
// Flag can be either ANDOR_ERROR or ANDOR_SUCCESS, depending on whether the calling function expects
//		a success bool or an error bool.
void ANDOR885_Device::printError(int errorValue, std::string errorMsg, bool *success, int flag)
{
	if(errorValue!=DRV_SUCCESS){
		std::cerr << errorMsg << std::endl;
		if (flag == ANDOR_ERROR) {
			*success = true;
		} else {
			*success = false;
		}
	}
}

void ANDOR885_Device::saveContinuousDataWrapper(void* object)
{
	ANDOR885_Device* thisObject = static_cast<ANDOR885_Device*>(object);
	thisObject->saveContinuousData();
}

// Saves a kinetic series. In Run 'Till Abort mode, only the last numExposures images will be saved
void ANDOR885_Device::saveContinuousData()
{
	int errorValue;
	bool error = false;
	long first;
	long last;
	long imageSize = gblXPixels*gblYPixels;
	long validFirst;
	long validLast;
	int numAcquired = 0;
	int tempAcq = 0;
	int excess = 0;
	long index = 0;
	std::vector <at_32> tempImageVector(imageSize * numExposures);
	std::vector <at_32> singleImageVector(imageSize);
	int i, j;
	bool overwritten = false;


	while(acquisitionStat == ON && (numAcquired < numExposures || acquisitionMode == ACQMODE_RUN_TILL_ABORT)){

		if (numAcquired >= numExposures) {
			numAcquired = 0;
			overwritten = true;
		}
		errorValue = WaitForAcquisition();
		printError(errorValue, "Error acquiring data", &error, ANDOR_ERROR);

		errorValue = GetNumberNewImages(&first, &last);
		printError(errorValue, "Error acquiring number of new images", &error, ANDOR_ERROR);

		if (!error){
			if(numAcquired + last - first + 1 > numExposures) {
				excess = numAcquired + last - first + 1 - numExposures;
				last -= excess;
				std::cerr << "More images acquired than expected number of exposures" << std::endl;
				std::cerr << "Ignored extra images" << std::cerr;
			}
//			bufferSize = last - first + 1;
//			AllocateBuffers();
//			errorValue = GetImages(first, last, pImageArray, (last - first + 1)*imageSize, &validFirst, &validLast);
			errorValue = GetImages(first, last, &tempImageVector[numAcquired*imageSize], (last - first + 1)*imageSize, &validFirst, &validLast);
			printError(errorValue, "Error acquiring images", &error, ANDOR_ERROR);
			tempAcq = validLast - validFirst + 1;
			numAcquired += tempAcq;
		}

		error = false;
	}

	if(!error) {
		if (overwritten)
			j = numAcquired;
		else
			j = 0;

		for (i = 0; i < numAcquired; i++) {
//			tempImageVector.assign(pImageArray + i*imageSize, pImageArray + (i + 1) * imageSize);
			singleImageVector.assign(tempImageVector.begin() + (i + j % numExposures)*imageSize, tempImageVector.begin() + (i + j % numExposures + 1)*imageSize);
			pImageVector.push_back(singleImageVector);
		}
		saveImageVector();
	}
	pImageVector.clear();

	acquisitionStat = OFF;
	refreshAttributes();
}


void ANDOR885_Device::saveImageVector()
{
	int imageSize = gblXPixels*gblYPixels;
	std::string localTimeString;
	int index = 1;
	int i,j,k;

	ofstream file;
	char tempChar[MAX_PATH];
	std::string tempString;

	localTimeString = makeTimeString();
	
	for (i = 0; i < pImageVector.size(); i += 1)
	{
		tempString = spoolPath + localTimeString + "_" + valueToString(index) + ".dat";
		strcpy(tempChar,tempString.c_str());
		file.open(tempChar);
	
		for (j = 0; j < gblYPixels; j++)
		{
			for(k = 0; k < gblXPixels; k++)
			{
				file << pImageVector.at(i).at(j*gblXPixels + k) << "\t";
			}
			file << "\n";
		}
		file.close();
		index++;
	}
}

/*
void ANDOR885_Device::refreshAttributes()
{
	
	// All attributes are stored in c++, none are on the fpga
	//Attributes not set in serial commands

	setAttribute("Camera status", (cameraStat == ON) ? "On" : "Off");

	switch (acquisitionStat)
	{
		case ON:
			setAttribute("Acquisition status", "On");
			break;
		case OFF:
			setAttribute("Acquisition status", "Off");
			break;
		default:
			std::cerr << "Error in acquisition status selection" << std::endl;
			break;
	}

	setAttribute("Acquisition mode", (acquisitionMode == ACQMODE_SINGLE_SCAN) ? "Single scan" : "Run 'til abort");

	setAttribute("Trigger mode", ((triggerMode == TRIGGERMODE_EXTERNAL) ? "External" : "Internal")); //trigger mode?
	switch (readMode)
	{
//		case READMODE_MULTI_TRACK:
//			setAttribute("Read mode", "Multi-track");		
//		case READMODE_RANDOM_TRACK:
//			setAttribute("Read mode", "Random-track");		
//		case READMODE_SINGLE_TRACK:
//			setAttribute("Read mode", "Single-track");	
		case READMODE_IMAGE:
			setAttribute("Read mode", "Image");
			break;
		default:
			std::cerr << "Error in read mode selection" << std::endl;
			break;
	}

	setAttribute("Spool mode", (spoolMode == ON) ? "On" : "Off");

	switch (shutterMode)
	{
		case SHUTTERMODE_AUTO:
			setAttribute("Shutter mode", "Auto");
			break;
		case SHUTTERMODE_OPEN:
			setAttribute("Shutter mode", "Always open");		
			break;
		case SHUTTERMODE_CLOSE:
			setAttribute("Shutter mode", "Always closed");
			break;
		default:
			std::cerr << "Error in shutter mode selection" << std::endl;
			break;
	}
	
	setAttribute("Shutter open time (ms)", openTime); 
	setAttribute("Shutter close time (ms)", closeTime); 

	setAttribute("Exposure time (s)", exposureTime);

	setAttribute("Folder Path for saved files", spoolPath);
}
*/

/*
void ANDOR885_Device::saveImageArray()
{
	int imageSize = gblXPixels*gblYPixels;
	std::string localTimeString;
	int index = 1;
	int i,j,k;

	ofstream file;
	char tempChar[MAX_PATH];
	std::string tempString;

	localTimeString = makeTimeString();
	
	for (i = 0; i < nextImage/imageSize; i += imageSize)
	{
		tempString = spoolPath + localTimeString + "_" + valueToString(index) + ".dat";
		strcpy(tempChar,tempString.c_str());
		file.open(tempChar);
	
		for (j = 0; j < gblYPixels; j++)
		{
			for(k = 0; k < gblXPixels; k++)
			{
				file << pImageArray[i*imageSize + j*gblXPixels + k] << "\t";
			}
			file << "\n";
		}
		file.close();
		index++;
	}
}
*/