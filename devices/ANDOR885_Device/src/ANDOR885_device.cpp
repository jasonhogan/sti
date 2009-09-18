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
		std::string IPAddress,
		unsigned short ModuleNumber) :
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	//Initialize necessary parameters

	acquisitionMode	=	1;
	readMode		=	READMODE_IMAGE;
	exposureTime	=	EXPOSURE_DEFAULT;
	accumulateTime	=	0;
	kineticTime		=	0;
	ttl				=	TTL_OPEN_LOW;
	shutterMode		=	SHUTTERMODE_OPEN;
	closeTime		=	SHUTTER_CLOSE_TIME;
	openTime		=	SHUTTER_OPEN_TIME;
	triggerMode		=	TRIGGERMODE_EXTERNAL;

	InitializeCamera();
}

ANDOR885_Device::~ANDOR885_Device()
{
}

bool ANDOR885_Device::deviceMain(int argc, char **argv)
{
	bool error = false;
	int message;
	std::cerr << "press any key to exit... ";
	std::cin >> message;
	while (message!=0)
	{

	}
	error = deviceExit();
	return error;
}
	
bool ANDOR885_Device::deviceExit()
{
	int errorValue;
	bool error = false;

    //CloseShutter
	errorValue=SetShutter(ttl,shutterMode,closeTime,openTime);
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Shutter error\n";
		error = true;  
	}

	errorValue=CoolerOFF();        // Switch off cooler (if used)
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Error switching cooler off" << std::endl;
		error = true;
	}

	errorValue=ShutDown();
	if(errorValue!=DRV_SUCCESS){
		std::cerr << "Error shutting down" << std::endl;
		error = true;
	}

//  FreeBuffers();      // frees memory used by image buffers in xxxxWndw.c
	return error;
}

void ANDOR885_Device::defineAttributes()
{

	//Attributes
	addAttribute("Trigger mode", "External", "External, Internal"); //trigger mode
	addAttribute("Read mode", "Image","Image, Multi-track, Random-track, Single-track"); //readout mode of data
	
	addAttribute("Shutter mode", "Auto", "Auto, Always open, Always closed"); // Shutter control
	addAttribute("Shutter open time (ms)", openTime); //time it takes shutter to open
	addAttribute("Shutter close time (ms)", closeTime); //time it takes shutter to close
	
	addAttribute("Exposure time (s)", exposureTime); //length of exposure
	
}

void ANDOR885_Device::refreshAttributes()
{
	
	// All attributes are stored in c++, none are on the fpga
	//Attributes not set in serial commands
	setAttribute("Trigger mode", ((triggerMode == TRIGGERMODE_EXTERNAL) ? "External" : "Internal")); //trigger mode?
	switch (readMode)
	{
		case READMODE_MULTI_TRACK:
			setAttribute("Read mode", "Multi-track");		
		case READMODE_RANDOM_TRACK:
			setAttribute("Read mode", "Random-track");		
		case READMODE_SINGLE_TRACK:
			setAttribute("Read mode", "Single-track");
		case READMODE_IMAGE:
			setAttribute("Read mode", "Image");
		default:
			std::cerr << "Error in read mode selection" << std::endl;
	}

	switch (shutterMode)
	{
		case SHUTTERMODE_AUTO:
			setAttribute("Shutter mode", "Auto");
		case SHUTTERMODE_OPEN:
			setAttribute("Shutter mode", "Always open");		
		case SHUTTERMODE_CLOSE:
			setAttribute("Shutter mode", "Always closed");
		default:
			std::cerr << "Error in shutter mode selection" << std::endl;
	}
	
	setAttribute("Shutter open time (ms)", openTime); 
	setAttribute("Shutter close time (ms)", closeTime); 

	setAttribute("Exposure time (s)", exposureTime);
}

bool ANDOR885_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;

	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;
	unsigned int error;

	if(key.compare("Trigger mode") == 0)
	{
		success = true;
		
		if(value.compare("Internal") == 0)
		{
			triggerMode = TRIGGERMODE_INTERNAL;
			std::cerr << "Trigger mode is Internal" << std::endl;
		}
		else if(value.compare("External") == 0)
		{
			triggerMode = TRIGGERMODE_EXTERNAL;
			std::cerr << "Trigger mode is External" << std::endl;
		}
		else
			success = false;

		error=SetTriggerMode(triggerMode);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting trigger mode" << std::endl;
			success = false;
		}
	}

	else if(key.compare("Read mode") == 0)
	{
		success = true;

		if(value.compare("Multi-track") == 0)
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
		else if(value.compare("Image") == 0)
		{
			readMode=READMODE_IMAGE;
			std::cerr << "Read mode is image" << std::endl;
		}
		else
			success = false;

		error=SetReadMode(readMode);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting read mode" << std::endl;
			success = false;
		}
	}
	
	else if(key.compare("Shutter mode") == 0)
	{
		success = true;
		if(value.compare("Auto") == 0)
		{
			shutterMode = SHUTTERMODE_AUTO;
			std::cerr << "Shutter mode is auto" << std::endl;
		}
		else if(value.compare("Always open") == 0)
		{
			shutterMode = SHUTTERMODE_OPEN;
			std::cerr << "Shutter mode is always open" << std::endl;
		}
		else if(value.compare("Always closed") == 0)
		{
			shutterMode = SHUTTERMODE_CLOSE;
			std::cerr << "Shutter mode is always closed" << std::endl;
		}
		else
			success = false;

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting shutter mode" << std::endl;
			success = false;
		}
	}		

	else if(key.compare("Shutter open time (ms)") == 0 && successDouble)
	{
		success = true;

		openTime = (int) tempDouble;

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting shutter opening time" << std::endl;
			success = false;
		}
	}

	else if(key.compare("Shutter close time (ms)") == 0 && successDouble)
	{
		success = true;

		closeTime = (int) tempDouble;

		error=SetShutter(ttl, shutterMode, closeTime, openTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting shutter closing time" << std::endl;
			success = false;
		}
	}
	else if(key.compare("Exposure Time (s)") == 0 && successDouble)
	{
		success = true;
		
		exposureTime = (float) tempDouble;

		error=SetExposureTime(exposureTime);
		if(error!=DRV_SUCCESS){
			std::cerr << "Error setting exposure time" << std::endl;
			success = false;
		}
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


std::string ANDOR885_Device::execute(int argc, char **argv)
{
	return "";
}

void ANDOR885_Device::definePartnerDevices()
{
}

short ANDOR885_Device::wordsPerEvent()
{
	return 3;
}


void ANDOR885_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
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
	int 				test,test2; //need to pause while camera initializes

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

    // Set acquisition mode to required setting specified in xxxxWndw.c
    errorValue=SetAcquisitionMode(acquisitionMode);
    if(errorValue!=DRV_SUCCESS){
      std::cerr << "Set Acquisition Mode Error\n";
      errorFlag = true;
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
  test=GetTickCount();
  do{
  	test2=GetTickCount()-test;
  }while(test2<2000);


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
