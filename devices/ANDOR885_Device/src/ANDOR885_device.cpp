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

	preAmpGain_t.name = "Preamp Gain";
//	preAmpGain_t.choices.push_back("");
//	preAmpGain_t.choiceFlags.push_back(PREAMP_BLANK);
	preAmpGain = PREAMP_BLANK;
	preAmpGainPos = PREAMP_BLANK;

//	pImageArray = NULL;

	cameraStat		=	ON;
	acquisitionStat	=	OFF;
	acquisitionMode	=	ACQMODE_SINGLE_SCAN;
	readMode		=	READMODE_IMAGE;
	exposureTime	=	(float) 0.05;
	accumulateTime	=	0;
	kineticTime		=	0;
	ttl				=	TTL_OPEN_HIGH;
	shutterMode		=	SHUTTERMODE_OPEN;
	closeTime		=	SHUTTER_CLOSE_TIME;
	openTime		=	SHUTTER_OPEN_TIME;
	triggerMode		=	TRIGGERMODE_EXTERNAL;
	frameTransfer	=	OFF;
//	spoolMode		=	OFF;
	numExposures	=	3;					
	coolerSetpt		=  -50;
	coolerStat		=	OFF;
	cameraTemp		=	20;
	saveMode		=	OFF;

	numPerFile		=	2;

	readMode_t.initial = findToggleAttribute(readMode_t, readMode);
	shutterMode_t.initial = findToggleAttribute(shutterMode_t, shutterMode);

	//Name of path to which files should be saved
	filePath		=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\";
	palPath			=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\GREY.PAL";

	initialized = !InitializeCamera();

}

ANDOR885_Device::~ANDOR885_Device()
{
	deviceExit();
}

bool ANDOR885_Device::deviceMain(int argc, char **argv)
{
	bool error = false;
	int message;

	ofstream cerrLog;
	char tempChar[MAX_PATH];
	std::string tempString;

	tempString = filePath + "cerr.log";
	strcpy(tempChar,tempString.c_str());
	
	cerrLog.open(tempChar);

	streambuf* cerrBuffer = cerr.rdbuf(); // save cerr's output buffer

	cerr.rdbuf (cerrLog.rdbuf()); // redirect output into the file	

	std::cout << "*****************\n";
	std::cout << "* DO NOT Ctrl-c *\n";
	std::cout << "*****************\n";
	std::cout << "Press any key to cleanly shutdown camera and program... ";
	std::cin >> message;
	
	cerr.rdbuf (cerrBuffer); // restore old output buffer
	cerrLog.close();

	error = deviceExit();

	std::cout << "Camera off" << std::endl;

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
	shutterMode = SHUTTERMODE_CLOSE;
	errorValue=SetShutter(ttl,shutterMode,closeTime,openTime);
	printError(errorValue, "Shutter error", &error, ANDOR_ERROR);

	int i = -1;
	errorValue = IsCoolerOn(&i);
	printError(errorValue, "Error determing cooler status", &error, ANDOR_ERROR);
	if (!error) {
		if (i == 1) {
			GetTemperature(&temp);
			if(temp < 5) {
				coolerSetpt = 10;
			}
			errorValue = SetTemperature(coolerSetpt);
			printError(errorValue, "Error setting cooler temperature", &error, ANDOR_ERROR);
		}

		if (!error) {
			GetTemperature(&temp);
			while(temp < 5) {
				std::cerr << "Camera temperature rising...: " << temp << " deg C" << std::endl;
				Sleep(2000);
				GetTemperature(&temp);
			}
		}

		if (i == 1){
			errorValue=CoolerOFF();        // Switch off cooler (if used)
			printError(errorValue, "Error switching cooler off", &error, ANDOR_ERROR);
			if (!error) {
				coolerStat = OFF;
			}
		}

	}

	errorValue = ShutDown();
	printError(errorValue, "Error shutting down", &error, ANDOR_ERROR);

	std::cerr << "Shutting down..." << std::endl;

	initialized = false;
	return error;
}


bool ANDOR885_Device::isInitialized()
{
	return initialized;
}

void ANDOR885_Device::defineAttributes()
{

	//Attributes
	addAttribute("Camera status", "On", "On, Off");
	addAttribute("Acquisition status", "Off", "On, Off");
	addAttribute(acquisitionMode_t.name,"Kinetic series", makeString(acquisitionMode_t.choices));
	addAttribute("Trigger mode", "External", "External, Internal"); //trigger mode
//	addAttribute("Read mode", "Image","Image, Multi-track, Random-track, Single-track"); //readout mode of data
	addAttribute("Read mode", "Image","Image"); //readout mode of data
//	addAttribute("Spool mode", "Off", "On, Off"); //spooling of data
	
	addAttribute(shutterMode_t.name, "Open", makeString(shutterMode_t.choices)); // Shutter control
//	addAttribute("Shutter open time (ms)", openTime); //time it takes shutter to open
//	addAttribute("Shutter close time (ms)", closeTime); //time it takes shutter to close
	
	addAttribute("Exposure time (s)", exposureTime); //length of exposure

	addAttribute("Folder Path for saved files", filePath);
	addAttribute("Number of exposures", numExposures);

	addAttribute("Cooler setpoint", coolerSetpt);
	addAttribute("Cooler status", "Off", "On, Off");
	addAttribute("Camera temperature", cameraTemp);

	addAttribute("Save mode", "On", "On, Off");

	addAttribute(preAmpGain_t.name,preAmpGain_t.choices.at(0),makeString(preAmpGain_t.choices)); //PreAmp gain
	
	addAttribute("Num exp per file", numPerFile);
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

//	setAttribute("Spool mode", (spoolMode == ON) ? "On" : "Off");

	setAttribute(shutterMode_t.name, findToggleAttribute(shutterMode_t, shutterMode));	
//	setAttribute("Shutter open time (ms)", openTime); 
//	setAttribute("Shutter close time (ms)", closeTime); 

	setAttribute("Exposure time (s)", exposureTime);

	setAttribute("Folder Path for saved files", filePath);
	setAttribute("Number of exposures", numExposures);

	setAttribute("Cooler setpoint", coolerSetpt);
	setAttribute("Cooler status", (coolerStat == ON) ? "On" : "Off");
	setAttribute("Camera temperature", cameraTemp);

	setAttribute("Save mode", (saveMode == ON) ? "On" : "Off");

	setAttribute(preAmpGain_t.name,findToggleAttribute(preAmpGain_t, preAmpGain));

	setAttribute("Num exp per file", numPerFile);

}

bool ANDOR885_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;
	int tempInt;
//	char tempChar[MAX_PATH];
	std::string tempString;
	int temperature;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = false;
	int error;

	int i;

	// When the camera is acquiring, the user is prohibited from changing the attributes,
	// except to turn off the acquisition.
	if(acquisitionStat != ON || 
		(key.compare("Acquisition status") == 0 && value.compare("Off") == 0) ||
		(key.compare("Save mode") == 0)) {

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

/*		else if(key.compare("Spool mode") == 0)
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
					strcpy(tempChar,filePath.c_str());
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
*/
		else if(key.compare("Shutter mode") == 0) {
			success = true;
			if(value.compare("Auto") == 0) {
				shutterMode = SHUTTERMODE_AUTO;
				std::cerr << "7. Shutter mode is auto" << std::endl;
			}
			else if(value.compare("Open") == 0) {
				shutterMode = SHUTTERMODE_OPEN;
				std::cerr << "7. Shutter mode is always open" << std::endl;
			}
			else if(value.compare("Closed") == 0) {
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

/*		else if(key.compare("Shutter open time (ms)") == 0 && successInt)
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
*/
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
			filePath = value;
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

			if (tempInt > maxTemp || tempInt < minTemp) {
				std::cerr << "13. Chosen temperature out of range." << std::endl;
				std::cerr << "13. Temperature must be between " << minTemp << " and " << maxTemp << std::endl;
			} else {
				coolerSetpt = tempInt;

				error = SetTemperature(coolerSetpt);
				printError(error, "13. Error setting cooler temperature", &success, ANDOR_SUCCESS);
				
				if(success){
					std::cerr << "13. Cooler temperature set to: " << coolerSetpt << std::endl;
				}
			}
		}

		else if (key.compare("Cooler status") == 0) {
			success = true;
			if (value.compare("On") == 0)
			{
				if (coolerStat != ON) {
					error = SetTemperature(coolerSetpt);
					printError(error, "14. Error setting cooler temperature", &success, ANDOR_SUCCESS);
					if (success) 
					{
						error = CoolerON();
						printError(error, "14. Error turning on cooler", &success, ANDOR_SUCCESS);
					}

					if (success) {
						coolerStat = ON;
						GetTemperature(&temperature);
//						while(GetTemperature(&temperature) == DRV_TEMP_NOT_REACHED)
//						{
//							std::cerr << "14. Camera temperature dropping...: " << temperature << " deg C" << std::endl;
//							Sleep(1000);
//						}

						std::cerr << "14. Camera temperature is: " << temperature << " deg C" << std::endl;
					}
				}
				else 
					std::cerr << "14. Cooler already on" << std::endl;
			} 
			
			else if (value.compare("Off") == 0) {
				i = -1;
				error = IsCoolerOn(&i);
				printError(error, "14. Error determining cooler status", &success, ANDOR_SUCCESS);
				// Turn of cooler if it's not already off
				if (i != 0) {
					error = CoolerOFF();
					printError(error, "14. Error turning off cooler", &success, ANDOR_SUCCESS);
					if (success)
						coolerStat = OFF;
				}
				
				if (success) 
					std::cerr << "14. Cooler off...definitely" << std::endl;
			}
			else {
				success = false;
				std::cerr << "14. Error setting cooler status" << std::endl;
			}
		}

		else if (key.compare("Camera temperature") == 0){
			success = true;
			tempInt = -999;
			GetTemperature(&tempInt);
			cameraTemp = tempInt;
			std::cerr << "Camera Temperature is " << cameraTemp << std::endl;
		} 

		else if (key.compare("Save mode") == 0){
			success = true;
			if (value.compare("On") == 0) {
				saveMode = ON;
				std::cerr << "15. Save mode is on" << std::endl;
			}
			else if (value.compare("Off") == 0) {
				saveMode = OFF;
				std::cerr << "15. Save mode is off" << std::endl;
			}
			else {
				success = false;
				std::cerr << "15. Error in Save mode selection" << std::endl;
			}
		}
		else if (key.compare(preAmpGain_t.name)==0){
			success = true;
			
			for(i = 0; (unsigned) i < preAmpGain_t.choices.size(); i++) {
				if (value.compare(preAmpGain_t.choices.at(i))==0){
					preAmpGain = preAmpGain_t.choiceFlags.at(i);
					preAmpGainPos = i;
					error = SetPreAmpGain(preAmpGain);
					printError(error, "16. Error selecting preamp gain", &success, ANDOR_SUCCESS);
					break;
				}
			}
			if (i == preAmpGain_t.choices.size()){
				std::cerr << "16. Unrecognized gain setting selected";
			}
		}

		else if(key.compare("Num exp per file") == 0 && successInt) {
			success = true;
			
			numPerFile = tempInt;
		}

	}

	else {
		std::cerr << "Stop acquisition before changing the parameters" << std::endl;
	}
	
	return success;

}

void ANDOR885_Device::defineChannels()
{
	//this->add
	addOutputChannel(0, ValueVector);
}

bool ANDOR885_Device::readChannel(DataMeasurement& Measurement)
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
	double minimumEventSpacing = 50000000; //50 ms in nanoseconds - this is approximate, based on experiments
	double minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	double holdoff = minimumEventSpacing; //we assume the holdoff is equal to the minimum event spacing (to be verified)
	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator previousEvents;

	Andor885Event* andor885Event;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events != eventsIn.begin())
		{
			previousEvents = events--;
			previousTime = events->first;
			events++;
		}
		else
		{
			previousEvents = events;
			previousTime = minimumAbsoluteStartTime - holdoff * events->second.size();
		}
		
		eventTime = events->first;

		if( (events->first - minimumEventSpacing) < previousTime )
	//	if (eventTime-previousTime < holdoff)
		{
			throw EventConflictException(previousEvents->second.at(0), 
				events->second.at(0), 
				"The camera cannot take pictures faster than 50 ms" );
		}
		

		if (events->second.at(0).getValueType() == MixedValue::Vector)
		{
			unsigned sizeOfTuple = events->second.at(0).value().getVector().size();

			const std::vector <MixedValue>& eVector = events->second.at(0).value().getVector();

			//Check that each type in tuple is correct. The first two switch statements
			// are deliberately un-break'd.
/*			switch(sizeOfTuple)
			{
			case 3:
				if(eVector.at(2).getType() != MixedValue::String)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera image description must be a string");
				}
			case 2:
				if(eVector.at(1).getType() != MixedValue::String)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera filename must be a string");
				}
			case 1:
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera exposure time must be a double");
				}
				break;

			default:
				throw EventParsingException(events->second.at(0),
					"Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}
*/

			switch(sizeOfTuple)
			{
			case 3:
				if(eVector.at(2).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera number of exposures per file must be an integer.");
				}
			case 2:
				if(eVector.at(1).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera number of exposures must be an integer.");
				}
			case 1:
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera exposure time must be a double.");
				}
				break;

			default:
				throw EventParsingException(events->second.at(0),
					"Andor camera commands must be a tuple in the form (double exposureTime, int numExposures, int numExpPerFile)");
				break;
			}

			andor885Event = new Andor885Event(eventTime, this);

/*			switch(sizeOfTuple)
			{
			case 3:
				andor885Event.exposureTimes.push_back(eVector.at(0));
				andor885Event.descriptions.push_back(eVector.at(1));
				andor885Event.filenames.push_back(eVector.at(2));
				break;
			case 2:
				andor885Event.exposureTimes.push_back(eVector.at(0));
				andor885Event.descriptions.push_back(eVector.at(1));
				andor885Event.filenames.push_back("");
				break;
			case 1:
				andor885Event.exposureTimes.push_back(eVector.at(0));
				andor885Event.descriptions.push_back("");
				andor885Event.filenames.push_back("");
				break;
				
			default:
				delete andor885Event;
				throw EventParsingException(events->second.at(0), "Never should get here, but Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}
			 */
			switch(sizeOfTuple)
			{
			case 3:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = eVector.at(1).getDouble();
				andor885Event->eventNumExpPerFile = eVector.at(2).getDouble();
				break;
			case 2:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = eVector.at(1).getDouble();
				andor885Event->eventNumExpPerFile = numPerFile;
				break;
			case 1:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = numExposures;
				andor885Event->eventNumExpPerFile = numPerFile;
				break;
				
			default:
				delete andor885Event;
				throw EventParsingException(events->second.at(0), "Never should get here, but Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}

			eventsOut.push_back( andor885Event );

		}
		else
		{
			std::cerr << "The Andor camera does not support that data type" << std::endl;
			throw EventParsingException(events->second.at(0),
						"The Andor camera does not support that data type.");
		}
		
	}

}

void ANDOR885_Device::Andor885Event::playEvent()
{
	ANDORdevice_->exposureTime = eventExposureTime;
	ANDORdevice_->numExposures = eventNumExposures;
	ANDORdevice_->numPerFile = eventNumExpPerFile;

	ANDORdevice_->refreshAttributes();

	ANDORdevice_->acquisitionStat = ON;

	ANDORdevice_->refreshAttributes();

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

	float				speed, STemp, gain;
	int					iSpeed, iAD, nAD, nAmp, nPreAmp, index, IsPreAmpAvailable;

	int i;


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

	errorValue = GetNumberAmp(&nAmp);
	printError(errorValue, "Get Number Amplifiers Error", &errorFlag, ANDOR_ERROR);

	errorValue = GetNumberPreAmpGains(&nPreAmp);
	printError(errorValue, "Get Number Preamplifiers Error", &errorFlag, ANDOR_ERROR);

	if (nAmp == 1 && nAD == 1) {
		for (i = 0; i < nPreAmp; i++) {
			errorValue = GetPreAmpGain(i, &gain);
			errorValue = IsPreAmpGainAvailable(0,0,HSnumber,i,&IsPreAmpAvailable);
			if (IsPreAmpAvailable == 1) {
				preAmpGain_t.choices.push_back(valueToString(gain));
				preAmpGain_t.choiceFlags.push_back(i);
			}
		}
		if (!preAmpGain_t.choices.empty()) {
			preAmpGain = preAmpGain_t.choiceFlags.at(0);
			preAmpGainPos = 0;
			errorValue = SetPreAmpGain(preAmpGain_t.choiceFlags.at(0));
			printError(errorValue, "Set AD Channel Error", &errorFlag, ANDOR_ERROR);
		} else {
			std::cerr << "No gains available at this speed. Weird.";
			errorFlag = true;
		}
	} else {
		std::cerr << "Unexpected number of A/D's or output amps" << std::endl;
		std::cerr << "Expected A/D's:       1 \t Measured: " << nAD << std::endl;
		std::cerr << "Expected output Amps: 1 \t Measured: " << nAmp << std::endl;
		errorFlag = true;
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

	errorValue = GetTemperatureRange(&minTemp, &maxTemp);
	if (errorValue != DRV_SUCCESS){
		std::cerr << "Error finding temperature range or camera is not on" << std::endl;
		errorFlag = true;
	}
	else {
		std::cerr << "Temperature must be between " << minTemp << " and " << maxTemp << std::endl;
		std::cerr << "Warning: Water cooling is required for temperatures < -58 deg C" << std::endl;

		//Set temperature
		if (coolerSetpt > maxTemp || coolerSetpt < minTemp) {
			std::cerr << "Chosen temperature out of range." << std::endl;
			if (coolerSetpt > maxTemp)
				coolerSetpt = maxTemp;
			else
				coolerSetpt = minTemp;
			std::cerr << "Resetting temp to nearest acceptable value " << std::endl;
		} 

		int i;
		errorValue = IsCoolerOn(&i);
		if (i == 0) {
			std::cerr << "Cooler is currently off." << std::endl;
			errorValue = SetTemperature(coolerSetpt);
			printError(errorValue, "Error setting cooler temperature", &errorFlag, ANDOR_ERROR);
		} else if (i == 1) {
			std::cerr << "Cooler is on." << std::endl;
			errorValue = GetTemperature(&i);
			switch(errorValue){
				case DRV_TEMP_STABILIZED:
					std::cerr << "Cooler temp has stabilized at " << i << " deg C" << std::endl;
					break;
				case DRV_TEMP_NOT_REACHED:
					std::cerr << "Cooler temp is " << i << " deg C" << std::endl;
					std::cerr << "Cooler setpoint has not been reached." << std::endl;
					std::cerr << "This may be because water cooling is required for setpoints < -58 deg C" << std::endl;
					std::cerr << "Either wait or try resetting cooler setpoint" << std::endl;
					break;
				case DRV_TEMP_DRIFT:
					std::cerr << "Cooler temp is " << i << " deg C" << std::endl;
					std::cerr << "Cooler temperature has drifted. Try resetting setpoint" << std::endl;
					break;
				case DRV_TEMP_NOT_STABILIZED:
					std::cerr << "Cooler temp is " << i << " deg C" << std::endl;
					std::cerr << "Temperature has been reached, but cooler has not stabilized" << std::endl;
					std::cerr << "Either wait or try resetting cooler setpoint" << std::endl;
					break;
				default:
					std::cerr << "Unrecognized error sequence. Camera may be off or acquiring" << std::endl;
					break;
			}
		}
		
		
		if(!errorFlag){
			std::cerr << "Cooler temperature set to: " << coolerSetpt << std::endl;
		}
	
	}


	errorValue = SetSpool(0,0,NULL,10);  //Disable spooling
	printError(errorValue, "Spool mode error", &errorFlag, ANDOR_ERROR);
	std::cerr << "Spooling Disabled" << std::endl;

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

int ANDOR885_Device::AllocateBuffers(at_32 *pImageArray, int bufferSize)
{
	int 	size;

	FreeBuffers(pImageArray);

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

void ANDOR885_Device::FreeBuffers(at_32 *pImageArray)
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

	std::string tempString;

	std::string localTimeString;
	int imageSize = gblXPixels*gblYPixels;
	images.imageData.reserve(imageSize);

	//Don't save until the camera has stopped acquiring
	WaitForAcquisitionTimeOut(10000);

	//Use the date to label images
	localTimeString = makeTimeString();
	
	error = GetAcquiredData16(&(images.imageData[0]), imageSize);
	printError(error, "Error in acquiring data", &success, ANDOR_SUCCESS);

	if (saveMode == ON && success) {

#ifdef _DEBUG
		pImageVector.push_back(images.imageData);
		saveImageVector();
		pImageVector.clear();
#else
		tempString = filePath+"image_"+localTimeString+".tif";

		images.filename = tempString;
		images.imageWidth = gblXPixels;
		images.imageHeight = gblYPixels;
	
		images.saveImageGrey();
#endif
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
		std::cerr << "Invalid Argument to _gmtime64_s." << std::endl;
	}

	// Convert to an ASCII representation 
	err = asctime_s(time_buf, 26, &localTime);
	if (err)
	{
		std::cerr << "Invalid Argument to asctime_s." << std::endl;
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
	int errorValue = DRV_SUCCESS;
	bool error = false;
	long first = 1;
	long last = 1;
	long imageSize = gblXPixels*gblYPixels;
	long validFirst = 1;
	long validLast = 1;
	int numAcquired = 0;
	int tempAcq = 0;
	int excess = 0;
	int end;
	long index = 0;
	std::vector <WORD> tempImageVector(imageSize * numExposures);
	std::vector <WORD> singleImageVector(imageSize);
	int i, j;
	bool overwritten = false;
	ImageMagick::Metadata singleImageMetadata;


	while(acquisitionStat == ON && (numAcquired < numExposures || acquisitionMode == ACQMODE_RUN_TILL_ABORT))
	{
		if (numAcquired >= numExposures) {
			numAcquired = 0;
			overwritten = true;
			index++;
		}
//		errorValue = WaitForAcquisition();
//		printError(errorValue, "Error acquiring data", &error, ANDOR_ERROR);
//		Sleep(1000);
		omni_thread::yield();
//		if (!error) {
			error = false;
			errorValue = GetNumberNewImages(&first, &last);
			if (errorValue != DRV_NO_NEW_DATA)
				printError(errorValue, "Error acquiring number of new images", &error, ANDOR_ERROR);
			else
				error = true;
//		}

		if (!error){
			if(numAcquired + last - first + 1 > numExposures) {
				excess = numAcquired + last - first + 1 - numExposures;
				last -= excess;
				std::cerr << "More images acquired than expected number of exposures" << std::endl;
				std::cerr << "Ignored extra images" << std::cerr;
			}
//			bufferSize = last - first + 1;
//			AllocateBuffers(pImageArray);
//			errorValue = GetImages(first, last, pImageArray, (last - first + 1)*imageSize, &validFirst, &validLast);
			errorValue = GetImages16(first, last, &tempImageVector[numAcquired*imageSize], (last - first + 1)*imageSize, &validFirst, &validLast);
			printError(errorValue, "Error acquiring images", &error, ANDOR_ERROR);
			tempAcq = last - first + 1;
			numAcquired += tempAcq;
		}

//		error = false;
	}

//	FreeBuffers(pImageArray);

	if(numAcquired != 0 || overwritten) {
		if (overwritten){
			j = numAcquired;
			end = numExposures;
			std::cerr << "Buffer overwritten " << index << " times" << std::endl;
		} else {
			j = 0;
			end = numAcquired;
		}

		for (i = 0; i < end; i++) {
			singleImageVector.assign(tempImageVector.begin() + ((j + i) % numExposures)*imageSize, tempImageVector.begin() + ((i + j) % numExposures + 1)*imageSize);
			images.imageDataVector.push_back(singleImageVector);

			addMetadata(singleImageMetadata);
			images.metadata.push_back(singleImageMetadata);
		}
		
		if (saveMode == ON) {
			saveImageVector();
		}
	}

	images.imageDataVector.clear();
	images.metadata.clear();

	acquisitionStat = OFF;
}

void ANDOR885_Device::addMetadata(ImageMagick::Metadata &metadata)
{
	metadata.tags.clear();
	metadata.values.clear();

	metadata.tags.push_back("Exposure Time");
	metadata.values.push_back(valueToString(exposureTime));

	metadata.tags.push_back("Camera Temp");
	metadata.values.push_back(valueToString(cameraTemp));

	metadata.tags.push_back("PreAmp Gain");
	metadata.values.push_back(valueToString(preAmpGain_t.choices.at(preAmpGainPos)));
}

void ANDOR885_Device::saveImageVector()
{
	int imageSize = gblXPixels*gblYPixels;
	std::string localTimeString;

	ofstream file;
	std::string tempString;

	localTimeString = makeTimeString();

#ifdef _DEBUG
	int index = 1;
	int i,j,k;
	char tempChar[MAX_PATH];
	
	for (i = 0; (unsigned) i < pImageVector.size(); i += 1)
	{
		tempString = filePath + localTimeString + "_" + valueToString(index) + ".dat";
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
#else

	images.filename = localTimeString;
	images.filepath = filePath;
	images.extension = ".tif";
	images.imageHeight = gblYPixels;
	images.imageWidth = gblXPixels;

	//images.saveToMultiPageGrey();
	images.saveToMultiMultiPageGrey(numPerFile);

#endif
}