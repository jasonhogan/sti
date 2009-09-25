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


//#pragma argsused

ANDOR885_Device::ANDOR885_Device(
		ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//Initialize necessary parameters

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
	std::cerr << "*****************\n";
	std::cerr << "* DO NOT Ctrl-c *\n";
	std::cerr << "*****************\n";
//	std::cerr << "Press any key to cleanly shutdown camera and program... ";
//	std::cin >> message;
//	error = deviceExit();
	return error;
}
	
bool ANDOR885_Device::deviceExit()
{
	int errorValue;
	bool error = false;
	int temp;

	//Stop Acquisition
	error = !AbortIfAcquiring();

	//Free Buffer
	//FreeBuffers();

    //CloseShutter
	errorValue=SetShutter(ttl,shutterMode,closeTime,openTime);
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Shutter error\n";
		error = true;  
	}

	errorValue = GetTemperature(&temp);
	if(errorValue != DRV_TEMP_OFF){
		errorValue=CoolerOFF();        // Switch off cooler (if used)
		if(errorValue!=DRV_SUCCESS){
			std::cerr << "Error switching cooler off" << std::endl;
			error = true;
		}

		while(GetTemperature(&temp) < 5)
		{
			Sleep(1000);
			std::cerr << "Camera temperature rising...: " << temp << "deg C" << std::endl;
		}
	}

	errorValue=ShutDown();
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Error shutting down" << std::endl;
		error = true;
	}
	std::cerr << "Shutting down..." << std::endl;

	initialized = false;
//  FreeBuffers();      // frees memory used by image buffers in xxxxWndw.c
	return error;
}

void ANDOR885_Device::defineAttributes()
{

	//Attributes
	addAttribute("Camera status", "On", "On, Off");
	addAttribute("Acquisition status", "Off", "On, Off");
	addAttribute("Acquisition mode","Single scan","Single scan, Run 'til abort");
	addAttribute("Trigger mode", "External", "External, Internal"); //trigger mode
//	addAttribute("Read mode", "Image","Image, Multi-track, Random-track, Single-track"); //readout mode of data
	addAttribute("Read mode", "Image","Image"); //readout mode of data
	addAttribute("Spool mode", "Off", "On, Off"); //spooling of data
	
	addAttribute("Shutter mode", "Always open", "Auto, Always open, Always closed"); // Shutter control
	addAttribute("Shutter open time (ms)", openTime); //time it takes shutter to open
	addAttribute("Shutter close time (ms)", closeTime); //time it takes shutter to close
	
	addAttribute("Exposure time (s)", exposureTime); //length of exposure

	addAttribute("Folder Path for saved files", spoolPath);
	
}

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
/*		case READMODE_MULTI_TRACK:
			setAttribute("Read mode", "Multi-track");		
		case READMODE_RANDOM_TRACK:
			setAttribute("Read mode", "Random-track");		
		case READMODE_SINGLE_TRACK:
			setAttribute("Read mode", "Single-track");	*/
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

bool ANDOR885_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;
	char tempChar[MAX_PATH];
	std::string tempString;

	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;
	int error;

	//For labeling files
//	time_t rawTime;
//	struct tm * localTime;
	std::string localTimeString;
	size_t found;

	struct tm localTime;
	__int64 rawTime;
	char time_buf[26];
	errno_t err;

	if(key.compare("Camera status") == 0) 
	{
		success = true;
		if (value.compare("On") == 0)
		{
			cameraStat = ON;
			if (!initialized){
				std::cerr << "1. Initializing..." << std::endl;
				initialized = !InitializeCamera();
			}

			if (!initialized){
				std::cerr << "1. Error initializing camera" << std::endl;
			} else {
				std::cerr << "1. Camera on" << std::endl;
			}
		}
		else if (value.compare("Off") == 0)
		{
			if (cameraStat != OFF) {
				success = !deviceExit();
				cameraStat = OFF;
			}

			if (!success) {
				std::cerr << "1. Error shutting down camera" << std::endl;
			} else {
				std::cerr << "1. Camera off" << std::endl;
			}
		}
		else{
			success = false;
			std::cerr << "1. Camera status error" << std::endl;
		}
	}
	else if(key.compare("Acquisition status") == 0)
	{
		success = true;

		if (value.compare("Off") == 0)
		{
			if (acquisitionStat != OFF){
				acquisitionStat = OFF;
				
				success = AbortIfAcquiring();
				if(success)
				{
					std::cerr << "2. Acquisition status: off" << std::endl;
				}
				else
				{
					std::cerr << "2. Acquisition status error" << std::endl;
				}
			}
			else
			{
				std::cerr << "2. Acquisition status already off" << std::endl;
			}
		}
		else if (value.compare("On") == 0)
		{
			//check to see if camera is idle
			GetStatus(&error);

			if (acquisitionStat != ON && error == DRV_IDLE)
			{
				acquisitionStat = ON;

			  	if(spoolMode == ON && acquisitionMode == ACQMODE_RUN_TILL_ABORT){
					strcpy(tempChar,spoolPath.c_str());
					SetSpool(1,0,tempChar,10); //Enabled; 10 images can be stored in RAM before error
					std::cerr << "2. Spooling Enabled\r\n";
  				}else{
					SetSpool(0,0,NULL,10);  //Disabled
					std::cerr << "2. Spooling Disabled\r\n";
				}

				// Starting the acquisition also starts a timer which checks the card status
				// When the acquisition is complete the data is read from the card and
				// displayed in the paint area.
				error = StartAcquisition();
				if(error!=DRV_SUCCESS){
					std::cerr << "2. Error starting acquisition" << std::endl;
					success = false;
				} else {
					std::cerr << "2. Starting acquisition." << std::endl;
				}	


				//Save data and reset acquisition mode if performing a single scan
				if(acquisitionMode == ACQMODE_SINGLE_SCAN)
				{
					GetStatus(&error);
					while(error == DRV_ACQUIRING)
					{
						Sleep(10);
						GetStatus(&error);
					}

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

//					time(&rawTime);
//					localTime = localtime(&rawTime);
//					localTimeString = asctime(localTime);

					//Credit goes to Dave Johnson for the date manipulations
					//Repackage in a function?
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

					//the 0,0 at the end means the image will scale across full range of values.
					tempString = spoolPath+"\\image_"+localTimeString+".bmp";
					strcpy(tempChar,tempString.c_str());
					error = SaveAsBmp(tempChar,palPath,0,0);
					if(error!=DRV_SUCCESS){
						std::cerr << "Error saving image" << std::endl;
						std::cerr << error;
						success = false;
					} else {
						std::cerr << "Saved!" << std::endl;
					}
					acquisitionStat = OFF;
					refreshAttributes();
				}
			}
			else
			{
			std::cerr << "2. Acquisition status already on or camera is not idle" << std::endl;
			}
		}
		else
		{
			success = false;
			std::cerr << "2. Error setting Acquisition Status" << std::endl;
		}

	}
	else if(key.compare("Acquisition mode") == 0)
	{
		success = true;

		if(value.compare("Single scan") == 0)
		{
			acquisitionMode = ACQMODE_SINGLE_SCAN;

			AbortIfAcquiring();
			std::cerr << "3. Acquisition mode Single Scan" << std::endl;
		}
		else if (value.compare("Run 'til abort") == 0)
		{
			acquisitionMode = ACQMODE_RUN_TILL_ABORT;

			std::cerr << "3. Acquisition mode Run 'til abort" << std::endl;
		}
		else
		{
			success = false;
			std::cerr << "3. Error setting Acquisition Mode" << std::endl;
		}
	
		// Set acquisition mode to required setting
	    error=SetAcquisitionMode(acquisitionMode);
		if(error!=DRV_SUCCESS){
			std::cerr << "3. Set Acquisition Mode Error\n";
			success = false;
		}
	}
	else if(key.compare("Trigger mode") == 0)
	{
		success = true;
		
		if(value.compare("Internal") == 0)
		{
			triggerMode = TRIGGERMODE_INTERNAL;
			std::cerr << "4. Trigger mode is Internal" << std::endl;
		}
		else if(value.compare("External") == 0)
		{
			triggerMode = TRIGGERMODE_EXTERNAL;
			std::cerr << "4. Trigger mode is External" << std::endl;
		}
		else
		{
			success = false;
			std::cerr << "4. Error setting Trigger mode" << std::endl;
		}

		error=SetTriggerMode(triggerMode);
		if(error!=DRV_SUCCESS){
			std::cerr << "4. Error setting trigger" << std::endl;
			success = false;
		}
	}

	else if(key.compare("Read mode") == 0)
	{
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
		if(value.compare("Image") == 0)
		{
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
		else
		{
			success = false;
			std::cerr << "5. Error in read mode value" << std::endl;
		}

		error=SetReadMode(readMode);
		if(error!=DRV_SUCCESS){
			std::cerr << "5. Error setting read mode" << std::endl;
			success = false;
		} else {
			std::cerr << "5. Read mode set" << std::endl;
		}
	}
	else if(key.compare("Spool mode") == 0)
	{
		success = true;
		if (value.compare("On") == 0)
		{
			if (spoolMode != ON){
				spoolMode = ON;
				if(acquisitionMode == ACQMODE_RUN_TILL_ABORT){
					strcpy(tempChar,spoolPath.c_str());
					error = SetSpool(1,0,tempChar,10); //Enabled; 10 images can be stored in RAM before error
					if (error == DRV_SUCCESS) {
						std::cerr << "6. Spooling Enabled\r\n";
					} else {
						std::cerr << "6. Error enabling spooling" << std::endl;
					}
  				}else{
					SetSpool(0,0,NULL,10);  //Disabled
					std::cerr << "6. Cannot spool in single scan read mode\n";
					spoolMode = OFF;
				}
			} else {
				std::cerr << "6. Spooling already enabled" << std::endl;
			}
		}
		else if (value.compare("Off") == 0)
		{
			if (spoolMode != OFF) {
				SetSpool(0,0,NULL,10);  //Disabled
				spoolMode = OFF;
				std::cerr << "6. Spooling Disabled" << std::endl;
			} else {
				std::cerr << "6. Spooling already disabled" << std::endl;
			}
		}
		else
		{
			success = false;
			std::cerr << "6. Error setting Spool mode" << std::endl;
		}
	}
	else if(key.compare("Shutter mode") == 0)
	{
		success = true;
		if(value.compare("Auto") == 0)
		{
			shutterMode = SHUTTERMODE_AUTO;
			std::cerr << "7. Shutter mode is auto" << std::endl;
		}
		else if(value.compare("Always open") == 0)
		{
			shutterMode = SHUTTERMODE_OPEN;
			std::cerr << "7. Shutter mode is always open" << std::endl;
		}
		else if(value.compare("Always closed") == 0)
		{
			shutterMode = SHUTTERMODE_CLOSE;
			std::cerr << "7. Shutter mode is always closed" << std::endl;
		}
		else
		{
			success = false;
			std::cerr << "7. Error setting Shutter mode" << std::endl;
		}

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "7. Error setting shutter" << std::endl;
			success = false;
		}
	}		

	else if(key.compare("Shutter open time (ms)") == 0 && successDouble)
	{
		success = true;

		openTime = (int) tempDouble;

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "8. Error setting shutter opening time" << std::endl;
			success = false;
		}
		else {
			std::cerr << "8. Shutter opening time set" << std::endl;
		}
	}

	else if(key.compare("Shutter close time (ms)") == 0 && successDouble)
	{
		success = true;

		closeTime = (int) tempDouble;

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "9. Error setting shutter closing time" << std::endl;
			success = false;
		}
		else {
			std::cerr << "9. Shutter closing time set" << std::endl;
		}
	}

	else if(key.compare("Exposure time (s)") == 0 && successDouble)
	{
		success = true;
		
		exposureTime = (float) tempDouble;

		error=SetExposureTime(exposureTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "10. Error setting exposure time" << std::endl;
			success = false;
		}

		// It is necessary to get the actual times as the system will calculate the
		// nearest possible time. eg if you set exposure time to be 0, the system
		// will use the closest value (around 0.01s)
		GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
		std::cerr << "10. Actual Exposure Time is " << exposureTime << " s.\n";


	}
	else if(key.compare("Folder Path for saved files") == 0)
	{
		success = true;
		spoolPath = value;
		std::cerr << "11. Folder path set" << std::endl;
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
    if(errorValue!=DRV_SUCCESS){
		std::cerr << "Initialize error" << std::endl;
		errorFlag = true;
    }

    // Get camera capabilities
    errorValue=GetCapabilities(&caps);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Get Andor Capabilities information Error\n";
      errorFlag = true;
    }

    // Get Head Model
    errorValue=GetHeadModel(model);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Get Head Model information Error\n";
      errorFlag = true;
    }

    // Get detector information
    errorValue=GetDetector(&gblXPixels,&gblYPixels);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Get Detector information Error\n";
      errorFlag = true;
    }

	// Set frame transfer mode
	errorValue=SetFrameTransferMode((frameTransfer == ON) ? 1 : 0);
	if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Frame Transfer Mode Error\n";
      errorFlag = true;
    }

    // Set acquisition mode to required setting specified in xxxxWndw.c
    errorValue=SetAcquisitionMode(acquisitionMode);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Acquisition Mode Error\n";
      errorFlag = true;
    }

	if(readMode == READMODE_IMAGE) {
    	// This function only needs to be called when acquiring an image. It sets
		// the horizontal and vertical binning and the area of the image to be
		// captured. In this example it is set to 1x1 binning and is acquiring the
		// whole image
  		SetImage(1,1,1,gblXPixels,1,gblYPixels);
	}

    // Set read mode to required setting specified in xxxxWndw.c
    errorValue=SetReadMode(readMode);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Read Mode Error\n";
      errorFlag = true;
    }

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
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Vertical Speed Error\n";
      errorFlag = true;
    }

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
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set AD Channel Error\n";
      errorFlag = true;
    }
    errorValue=SetHSSpeed(0,HSnumber);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Horizontal Speed Error\n";
      errorFlag = true;
    }
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
  if (errorValue != DRV_SUCCESS){
	  std::cerr << "Exposure time error\r\n" << std::endl;
	  errorFlag = true;
  }

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
  if(errorValue!=DRV_SUCCESS){
	std::cerr << "Set Trigger Mode Error\n";
	errorFlag = true;
  }

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
/*
int ANDOR885_Device::AllocateBuffers(void)
{

	int 	size;

	FreeBuffers();

	size=gblXPixels*gblYPixels;  // Needs to hold full image

  // only allocate if necessary
	if(!pImageArray)
  	pImageArray=malloc(size*sizeof(long));

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
*/
bool ANDOR885_Device::AbortIfAcquiring()
{
	int error;
	bool success = true;

	//Check to see if the camera is acquiring. If it is, stop
	GetStatus(&error);
	if(error == DRV_ACQUIRING){
		error = AbortAcquisition();
		if(error != DRV_SUCCESS){
			std::cerr << "Error aborting acquisition" << std::endl;
			success = false;
		}
	}
	else
		std::cerr << "Camera not acquiring" << std::endl;

	return success;
}