/*! \file QuantixCamera.cpp
 *  \author Susannah Dickerson 
 *  \brief Source file for the Photometrics Quantix camera
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>\n
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

#include "QuantixState.h"

QuantixState::QuantixState(int16 handle) : cameraHandle(handle)
{
	coolerSetpoint.currentValue = "-25";
	binSize.currentValue = "1";

	binSize.name = "Bin Size (px)";
	clearMode.name = "Clear Cycle Mode";
	clockingMode.name = "Clocking Mode";
	coolerSetpoint.name = "Cooler Setpoint (oC)";
	gain.name = "Gain";
	readoutSpeed.name = "Readout Speed";
	shutterMode.name = "Shutter Mode";
	triggerMode.name = "*Trigger Mode";
	temperature.name = "Temperature (oC)";

	clearCycles.name = "Number of Clear Cycles";
	imageHeight.name = "Image Height";
	imageWidth.name = "Image Width";
	shutterOpenDelay.name = "Shutter Open Delay";
	shutterCloseDelay.name = "Shutter Close Delay";


	//Initialize camera; throw exceptions if not initialized
	InitializeCamera();

	//Gather attributes that the user can change
	std::vector <CameraAttribute*> tmp;

	tmp.push_back(&readoutSpeed);
	tmp.push_back(&clearMode);
	tmp.push_back(&clockingMode);
	tmp.push_back(&gain);
	tmp.push_back(&shutterMode);
	tmp.push_back(&triggerMode);
	tmp.push_back(&coolerSetpoint);
	tmp.push_back(&temperature);
	tmp.push_back(&binSize);

	std::vector <CameraAttribute*>::iterator it;
	for (it = tmp.begin(); it != tmp.end(); it++)
	{
		if (!((*it)->isAvailable))
			continue;

		guiAttributes.push_back(*it);
	}

}

void QuantixState::set(CameraAttribute &attribute, std::string newLabel)
{
	try 
	{
		attribute.set(newLabel, cameraHandle);
	}
	catch (CameraException &e)
	{
		std::cerr << "Error setting attribute " << attribute.name << " to " << newLabel << std::endl;
		std::cerr << e.what() << std::endl;
		
		char msg[ERROR_MSG_LEN];		// for error handling
		pl_error_message(pl_error_code(), msg);
		std::cout << "Quantix Camera error: " << msg << std::endl;
	}

	//Special cases

	// Readout speed resets gain to 1x
	if (attribute.name.compare(readoutSpeed.name) == 0)
	{
		gain.set(gain.currentValue, cameraHandle);
		bitDepth.get(cameraHandle);
	}
};

std::string QuantixState::get(CameraAttribute &attribute)
{
	if (attribute.name.compare(temperature.name) == 0)
		return temperature.get(cameraHandle);
	else
		return attribute.get();
}

std::string	QuantixState::Temperature::get(int16 cameraHandle)
{
	rs_bool available = FALSE;
	int temp = -99900;

	int16 currentTemp;
	pl_get_param(cameraHandle, PARAM_TEMP, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_TEMP, ATTR_CURRENT, (void *) &currentTemp);
	}

	currentValue = STI::Utils::valueToString(((double)currentTemp)/100);	// Camera reports temperature in degrees C x 100

	currentLabel = currentValue;

	return currentValue;
}

void QuantixState::ClearMode::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	uns32 mode;
	if (!STI::Utils::stringToValue(newValue, mode))
		throw CameraException("Error in converting string to value");
	if(!pl_set_param(cameraHandle, PARAM_CLEAR_MODE, (void *) &mode))
		throw CameraException("Error setting clear mode");
		
	currentValue = newValue;
	currentLabel = getLabel();
}

void QuantixState::ClockingMode::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	uns32 mode;
	if (!STI::Utils::stringToValue(newValue, mode))
		throw CameraException("Error in converting string to value");

	if(!pl_set_param(cameraHandle, PARAM_PMODE, (void *) &mode))
		throw CameraException("Error setting clocking mode");
		
	currentValue = newValue;
	currentLabel = getLabel();
}

void QuantixState::CoolerSetpoint::set(std::string setpt, int16 cameraHandle) 
{
	// Camera reports temperature in degrees C x 100

	int16 setptQ;
	if (!STI::Utils::stringToValue(setpt, setptQ))
		throw CameraException("Error in converting string to value");
	
	if (setptQ > maxTemp || setptQ < minTemp) {
		throw CameraException("Chosen temperature out of range.\n Temperature must be between " 
			+ STI::Utils::valueToString(minTemp/100) + " and " + STI::Utils::valueToString(maxTemp/100));
	}

	if(!pl_set_param(cameraHandle, PARAM_TEMP_SETPOINT, (void *) &setptQ))
		throw CameraException("Error setting cooler temperature");

	pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_CURRENT, (void *) &setptQ);
		
	currentValue = STI::Utils::valueToString(setpt);
	currentLabel = getLabel();
}


void QuantixState::Gain::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	int16 gainQ;
	if (!STI::Utils::stringToValue(newValue, gainQ))
		throw CameraException("Error in converting string to value");
	if(!pl_set_param(cameraHandle, PARAM_GAIN_INDEX, (void *) &gainQ))
		throw CameraException("Error setting gain");
		
	currentValue = newValue;
	currentLabel = getLabel();
}

void QuantixState::ShutterMode::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	int16 mode;
	if (!STI::Utils::stringToValue(newValue, mode))
		throw CameraException("Error in converting string to value");

	if(!pl_set_param (cameraHandle, PARAM_SHTR_OPEN_MODE, &mode))
		throw CameraException("Error setting shutter mode");
		
	currentValue = newValue;
	currentLabel = getLabel();
}

void QuantixState::TriggerMode::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	int16 mode;
	if (!STI::Utils::stringToValue(newValue, mode))
		throw CameraException("Error in converting string to value");
		
	currentValue = newValue;
	currentLabel = getLabel();
}

void QuantixState::ReadoutSpeed::set(std::string newLabel, int16 cameraHandle) 
{
	std::string newValue = inverseFind(newLabel);

	int16 speedQ;
	if (!STI::Utils::stringToValue(newValue, speedQ))
		throw CameraException("Error in converting string to value");

	if(!pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &speedQ))
		throw CameraException("Error setting readout speed");

	//Gain gets reset to 1x after a change in the Readout Speed. 
	//The gain will be reset in QuantixCamera::set
		
	currentValue = newValue;
	currentLabel = getLabel();
}
void QuantixState::BinSize::set(std::string newValue, int16 cameraHandle) 
{
	uns16 newSize;
	if (!STI::Utils::stringToValue(newValue, newSize))
		throw CameraException("Error in converting string to value");
	
	uns16 width;
	if (!pl_get_param(cameraHandle, PARAM_PAR_SIZE, ATTR_CURRENT, (void *) &width))
		throw CameraException("BinSize: Can't access CCD width");
	
	uns16 height;
	if (!pl_get_param(cameraHandle, PARAM_SER_SIZE, ATTR_CURRENT, (void *) &height))
		throw CameraException("BinSize: Can't access CCD width");


	if(newSize >= 0 && newSize <= width && newSize <= height)
	{
		currentValue = newValue;
		currentLabel = getLabel();
	}
	else
		throw CameraException("Bin size out of range");

	//Gain gets reset to 1x after a change in the Readout Speed. 
	//The gain will be reset in QuantixCamera::set
		
	
}
std::string	QuantixState::BitDepth::get(int16 cameraHandle)
{
	int16 bitDepthQ;
	rs_bool available = FALSE;

	//Set Bit depth (for image preprocessor, mainly)
	if (!pl_get_param (cameraHandle, PARAM_BIT_DEPTH, ATTR_CURRENT, &bitDepthQ))
		throw CameraException("Can't access bit depth");
	currentValue = STI::Utils::valueToString(bitDepthQ);
	currentLabel = currentValue;

	return currentValue;
}

int	QuantixState::BitDepth::getSize()
{
	int size;
	STI::Utils::stringToValue(currentValue, size);
	return size;
}

int	QuantixState::ImageSize::getSize()
{
	int size;
	STI::Utils::stringToValue(currentValue, size);
	return size;
}

std::string QuantixState::CameraAttribute::inverseFind(std::string value)
{
	std::map<std::string, std::string>::iterator iter;

	//Find string associated with flag
	for (iter = choices.begin(); iter != choices.end(); iter++ ){
		if (value.compare(iter->second) == 0){
			return iter->first;
		}
	}

	std::cerr << "Error in " << name << " selection: " << value << " not allowed." << std::endl;


	return "";
}

std::string QuantixState::CameraAttribute::getLabel()
{
	if (!choices.empty())
		return choices[currentValue];
	else
		return currentValue;
}
/*void QuantixState::CameraAttribute::set(std::string newValue, int16 cameraHandle)
{
	 currentValue = newValue;
}*/

std::string QuantixState::CameraAttribute::get()
{
	return currentValue;
}

void QuantixState::InitializeCamera()
{
	rs_bool success = TRUE;
	rs_bool available = FALSE;

	char enumStr[100];
	int32 enumValue;
	uns32 currentMode = 0;
	uns32 numModes = 0;
    
	//camera has been initialized by main program


    /*********************************
	 * Get detector size information *
	 *********************************/
    pl_get_param(cameraHandle, PARAM_PAR_SIZE, ATTR_AVAIL, (void *) &available);
	if (available) 
	{
		uns16 imageWidthQ;
		if (!pl_get_param(cameraHandle, PARAM_PAR_SIZE, ATTR_CURRENT, (void *) &imageWidthQ))
			throw CameraException("Can't access CCD width");
		imageWidth.currentValue = STI::Utils::valueToString(imageWidthQ);
		imageWidth.isAvailable = true;
	}

	pl_get_param(cameraHandle, PARAM_SER_SIZE, ATTR_AVAIL, (void *) &available);
	if (available)
	{
		uns16 imageHeightQ;
		if (!pl_get_param(cameraHandle, PARAM_SER_SIZE, ATTR_CURRENT, (void *) &imageHeightQ))
			throw CameraException("Can't access CCD height");
		imageHeight.currentValue = STI::Utils::valueToString(imageHeightQ);
		imageHeight.isAvailable = true;
	}

	binSize.isAvailable = true;

	/*******************************
	 * Get parallel clocking modes *
	 *******************************/
	pl_get_param(cameraHandle, PARAM_PMODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_PMODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_PMODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_PMODE, mode, &enumValue, enumStr, 100);
			if (!pl_set_param (cameraHandle, PARAM_PMODE, &mode)) {
				continue;
			}
			
			clockingMode.choices[STI::Utils::valueToString(mode)] = enumStr;
			
		}

		//reset initial mode
		pl_set_param (cameraHandle, PARAM_PMODE, &currentMode);
		clockingMode.currentValue = STI::Utils::valueToString(currentMode);
		clockingMode.currentLabel = clockingMode.choices[clockingMode.currentValue];
		clockingMode.isAvailable = true;
	}


	/*******************************
	 * CCD Readout Port and Speeds *
	 *******************************/
	int numPorts;
	pl_get_param (cameraHandle, PARAM_READOUT_PORT, ATTR_AVAIL, &available);
	if (available)
	{
		if (pl_get_param (cameraHandle, PARAM_READOUT_PORT, ATTR_COUNT, &numPorts))
		{
			if (numPorts > 1)
				std::cerr << "Quantix Warning: multiple ports not supported; choosing current port" << std::endl;
			uns32 port;
			pl_get_param (cameraHandle, PARAM_READOUT_PORT, ATTR_CURRENT, &port);
			//pl_get_enum_param (cameraHandle, PARAM_READOUT_PORT, port, &enumValue, enumStr, 100);
			//std::cerr << "Current Readout Port: " << enumStr << std::endl;
		}
	

		short	maxSpeeds = 0;
		short	bitDepthQ;
		short	pixTime;
		if (pl_get_param (cameraHandle, PARAM_SPDTAB_INDEX, ATTR_MAX, &maxSpeeds))
		{
			pl_get_param(cameraHandle, PARAM_SPDTAB_INDEX, ATTR_CURRENT, (void *) &currentMode);

			maxSpeeds++;
			for (short speed = 0; speed < maxSpeeds; speed++) {
				if (!pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &speed)) {
					continue;
				}
				if (!pl_get_param (cameraHandle, PARAM_BIT_DEPTH, ATTR_CURRENT, &bitDepthQ))
					throw CameraException("Can't access bit depth");
			
				if (!pl_get_param (cameraHandle, PARAM_PIX_TIME, ATTR_CURRENT, &pixTime))
					throw CameraException("Can't access pixel time");
				
				readoutSpeed.choices[STI::Utils::valueToString(speed)] = STI::Utils::valueToString(bitDepthQ) + " bits; " +
					STI::Utils::valueToString(pixTime) + " ns/px";

			}

			//reset initial mode
			pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &currentMode);
			readoutSpeed.currentValue = STI::Utils::valueToString(currentMode);
			readoutSpeed.currentLabel = readoutSpeed.choices[readoutSpeed.currentValue];
			readoutSpeed.isAvailable = true;

			//Set Bit depth (for image preprocessor, mainly)
			bitDepth.get(cameraHandle);
		}
	}

	/********************
	 *     CCD Gain     *
	 ********************/
	int16 maxGain;
    if (pl_get_param (cameraHandle, PARAM_GAIN_INDEX, ATTR_MAX, (void *)&maxGain)) 
	{
		for (int16 i = 1; i <= maxGain; i++)
		{
			gain.choices[STI::Utils::valueToString(i)] = STI::Utils::valueToString(i);
			//Can't get actual gain with the Quantix 6303E; the parameter would be PARAM_ACTUAL_GAIN.
		}

		int16 currentGain;
		if(!pl_get_param(cameraHandle, PARAM_GAIN_INDEX, ATTR_CURRENT, (void *) &currentGain))
			throw CameraException("Can't access current gain");

		gain.currentValue = STI::Utils::valueToString(currentGain);
		gain.currentLabel = gain.choices.find(gain.currentValue)->second;
		gain.isAvailable = true;
    }

	/*******************
	 *     Shutter     *
	 *******************/
	
	pl_get_param(cameraHandle, PARAM_SHTR_OPEN_MODE, ATTR_AVAIL, (void *) &available);
	if (available){

		pl_get_param(cameraHandle, PARAM_SHTR_OPEN_MODE, ATTR_CURRENT, (void *) &currentMode);
		
		uns32	numModes = 0;
		pl_get_param (cameraHandle, PARAM_SHTR_OPEN_MODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_SHTR_OPEN_MODE, mode, &enumValue, enumStr, 100);
			if (!pl_set_param (cameraHandle, PARAM_SHTR_OPEN_MODE, &mode)) {
				continue;
			}

			if (enumValue == SHUTTERMODE_IGNORE)
				currentMode = mode;
			
			shutterMode.choices[STI::Utils::valueToString(mode)] = enumStr;
		}

		//reset initial mode, or set to Ignore mode
		pl_set_param (cameraHandle, PARAM_SHTR_OPEN_MODE, &currentMode);
//		pl_get_enum_param (cameraHandle, PARAM_SHTR_OPEN_MODE, currentMode, &enumValue, enumStr, 100);
		shutterMode.currentValue = STI::Utils::valueToString(currentMode);
		shutterMode.currentLabel = shutterMode.choices[shutterMode.currentValue];
		shutterMode.isAvailable = true;

	}
	
	// Set up delays
	pl_get_param(cameraHandle, PARAM_SHTR_OPEN_DELAY, ATTR_AVAIL, (void *) &available);
	if (available){
		uns16 shutterOpenTime = SHUTTER_OPEN_TIME;
		pl_set_param(cameraHandle, PARAM_SHTR_OPEN_DELAY, (void *) &shutterOpenTime);
		shutterOpenDelay.currentValue = STI::Utils::valueToString(shutterOpenTime);
		shutterOpenDelay.currentLabel = shutterOpenDelay.currentValue;
		shutterOpenDelay.isAvailable = true;
	}

	pl_get_param(cameraHandle, PARAM_SHTR_CLOSE_DELAY, ATTR_AVAIL, (void *) &available);
	if (available){
		uns16 shutterCloseTime = SHUTTER_CLOSE_TIME;
		pl_set_param(cameraHandle, PARAM_SHTR_CLOSE_DELAY, (void *) &shutterCloseTime);
		shutterCloseDelay.currentValue = STI::Utils::valueToString(shutterCloseTime);
		shutterCloseDelay.currentLabel = shutterCloseDelay.currentValue;
		shutterCloseDelay.isAvailable = true;
	}

	/***********************
	 *     Temperature     *
	 ***********************/
	pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_AVAIL, (void *) &available);
	if (available){
		int16 maxTempQ;
		pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_MAX, (void *) &maxTempQ);
		coolerSetpoint.maxTemp = maxTempQ;
		int16 minTempQ;
		pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_MIN, (void *) &minTempQ);
		coolerSetpoint.minTemp = minTempQ;
		coolerSetpoint.isAvailable = true;
	}
	else 
		throw CameraException("Program expects camera to be cooled");

	//Current value is set in constructor
	set(coolerSetpoint,coolerSetpoint.currentValue);

	pl_get_param(cameraHandle, PARAM_TEMP, ATTR_AVAIL, (void *) &available);
	if (available){
		temperature.get(cameraHandle); //initialize temperature
		temperature.isAvailable = true;
	}
	else 
		throw CameraException("Program expects camera to be cooled");

	/************************
	 *     Clear Cycles     *
	 ************************/
	uns16 numCycles;
	pl_get_param(cameraHandle, PARAM_CLEAR_CYCLES, ATTR_CURRENT, (void *) &numCycles);
	clearCycles.currentValue = STI::Utils::valueToString(numCycles);
	clearCycles.currentLabel = clearCycles.currentValue;

	pl_get_param(cameraHandle, PARAM_CLEAR_MODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_CLEAR_MODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_CLEAR_MODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_CLEAR_MODE, mode, &enumValue, enumStr, 100);
			if (!pl_set_param (cameraHandle, PARAM_CLEAR_MODE, &mode)) {
				continue;
			}
			
			clearMode.choices[STI::Utils::valueToString(mode)] = enumStr;
		}

		//reset initial mode
		pl_set_param (cameraHandle, PARAM_CLEAR_MODE, &currentMode);
		clearMode.currentValue = STI::Utils::valueToString(currentMode);
		clearMode.currentLabel = clearMode.choices[clearMode.currentValue];
		clearMode.isAvailable = true;
		
	}

	/*************************
	 *     Exposure Mode     *
	 *************************/

	pl_get_param(cameraHandle, PARAM_EXPOSURE_MODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_EXPOSURE_MODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_EXPOSURE_MODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_EXPOSURE_MODE, mode, &enumValue, enumStr, 100);
			
			if (enumValue == TRIGGERMODE_EXTERNAL)
				triggerMode.choices[STI::Utils::valueToString(TRIGGERMODE_EXTERNAL)] = enumStr;

			if (enumValue == TRIGGERMODE_EXTERNAL_EXPOSURE)
				triggerMode.choices[STI::Utils::valueToString(TRIGGERMODE_EXTERNAL_EXPOSURE)] = enumStr;
		}

		//Cannot set exposure mode through PARAM, so just store the value
		//it will get set vefore taking a picture through pl_exp_set_time
		if (triggerMode.choices.count(STI::Utils::valueToString(TRIGGERMODE_EXTERNAL_EXPOSURE)) > 0)
		{
			triggerMode.currentValue = STI::Utils::valueToString(TRIGGERMODE_EXTERNAL_EXPOSURE);
			triggerMode.currentLabel = triggerMode.choices[triggerMode.currentValue];
			triggerMode.isAvailable = true;
		}
		else
			throw CameraException("Program requires camera to have external exposure (a.k.a bulb) mode");
	}

	/****************************
	 * Exposure Time Resolution *
	 ****************************/
	// Probably pointless, but try to set it to microsecond resolution. 
	// In Timed mode, this would limit exposure time to 65 ms.
	pl_get_param(cameraHandle, PARAM_EXP_RES, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_EXP_RES, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_EXP_RES, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_EXP_RES, mode, &enumValue, enumStr, 100);
			if (enumValue == EXP_RES_ONE_MICROSEC)
				pl_set_param (cameraHandle, PARAM_EXP_RES, (void *) &mode);
		}

		pl_get_param(cameraHandle, PARAM_EXP_RES, ATTR_CURRENT, (void *) &currentMode);
		pl_get_enum_param (cameraHandle, PARAM_EXP_RES, currentMode, &enumValue, enumStr, 100);
		std::cerr << "Exposure time \"resolution\" is nominally" << enumStr << std::endl;
	}

	
	/*************************
	 * Minimum Exposure Time *
	 *************************/
	pl_get_param(cameraHandle, PARAM_EXP_MIN_TIME, ATTR_AVAIL, (void *) &available);
	if (!available){
		std::cerr << "Function to calculate minimum exposure time is not available." << std::endl;
		std::cerr << "Camera will not bound the minimum exposure time." << std::endl;
	}

	return;
}
