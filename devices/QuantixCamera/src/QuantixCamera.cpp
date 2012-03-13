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

#include "QuantixCamera.h"


QuantixCamera::QuantixCamera(int16 handle): cameraHandle(handle)
{
	initialized = false;
	notDestructed = false;
	extension = ".tif";

	eventMetadata = NULL;

	pauseCameraMutex = new omni_mutex();
	pauseCameraCondition = new omni_condition(pauseCameraMutex);
	stopEventMutex = new omni_mutex();
	stopEventCondition = new omni_condition(stopEventMutex);
	stopEvent = false;
	numAcquiredMutex = new omni_mutex();
	numAcquiredCondition = new omni_condition(numAcquiredMutex);
	waitForEndOfAcquisitionMutex = new omni_mutex();
	waitForEndOfAcquisitionCondition = new omni_condition(waitForEndOfAcquisitionMutex);
	waitForCleanupEventMutex = new omni_mutex();
	waitForCleanupEventCondition = new omni_condition(waitForCleanupEventMutex);
	bool cleanupEvent = false;

	//Initialize necessary parameters
/*	clockingMode_t = new ClockingMode(this, "Clocking mode");
	clearMode_t = new ClearMode(this, "Clear Cycle Mode");
	shutterMode_t = new ShutterMode(this, "Shutter mode");
	triggerMode_t = new TriggerMode(this, "**Trigger mode (EE)");
	gain_t = new Gain(this, "*Gain Index");
	readoutSpeed_t = new ReadoutSpeed(this, "*Readout Speed");
*/
/*	clockingMode_t->name = "Clocking mode"; //Normal v. MPP v. Frame transfer
	clearMode_t->name = "Clear Cycle Mode";
	shutterMode_t->name = "Shutter mode";
	triggerMode_t->name = "**Trigger mode (EE)";
	gain_t->name = "*Gain Index";
	gain = PREAMP_BLANK;

	readoutSpeed_t->name = "*Readout Speed";
*/

/*	clearCycles		=	0;
	exposureTime	=	(float) 0.05; // in seconds
	shutterMode		=	SHUTTERMODE_IGNORE;
//	triggerMode		=	TRIGGERMODE_EXTERNAL_EXPOSURE; //will be set by InitializeCamera	
	coolerSetpt		=  -25;
	coolerStat		=	CAMERA_ON;
	cameraTemp		=	20;

	readoutSpeed = 0;
*/
	filePath		=	createFilePath();

	try {
//		InitializeCamera();
		cameraState = new QuantixState(cameraHandle);
		initialized = false; // change to True eventually
	}
	catch(CameraException &e)
	{
		std::cerr << "Camera Initialization Error: " << e.what() << std::endl;
		initialized = false;
	}

	if (initialized){
		notDestructed = true;

//		omni_thread::create(playCameraWrapper, (void*) this, omni_thread::PRIORITY_HIGH);
	}
}

QuantixCamera::~QuantixCamera()
{
	//clear the playCamera thread. Assumes an event is not currently playing
	notDestructed = false;
	pauseCameraMutex->lock();
		pauseCameraCondition->signal();
	pauseCameraMutex->unlock();
	
	deviceExit();

	delete pauseCameraMutex;
	delete pauseCameraCondition;
	delete stopEventMutex;
	delete stopEventCondition;
	delete numAcquiredMutex;
	delete numAcquiredCondition;
	delete waitForEndOfAcquisitionMutex;
	delete waitForEndOfAcquisitionCondition;
	delete waitForCleanupEventMutex;
	delete waitForCleanupEventCondition;

/*	delete clockingMode_t;
	delete clearMode_t;
	delete shutterMode_t;
	delete triggerMode_t;
	delete gain_t;
	delete readoutSpeed_t;*/
}
/*
void QuantixCamera::playCameraWrapper(void* object)
{
	QuantixCamera* thisObject = static_cast<QuantixCamera*>(object);
	thisObject->playCamera();
}
void QuantixCamera::playCamera(){

	long imageSize = imageWidth*imageHeight;
	int errorValue;
	long index;
	std::vector <WORD> singleImageVector(imageSize);
	int i;
	bool error;
	int numPlayCameraExp = 1; //define a local number of exposures in case the cleanup event gets played before acquisition ends

//#ifndef _DEBUG
	ImageMagick::MyImage image;
	image.imageData.reserve(imageSize);
//#endif

	while(notDestructed){

		//initializion for a new acquisition
		numAcquired = 0;
		index = 0;
		error = false;

		//Signal camera to play. the extra lock/unlock is for refresh Attributes to know
		// whether it should signal the camera to play or not.
		pauseCameraMutex->lock();
			waitForEndOfAcquisitionMutex->lock();
				isPlaying = false;
				waitForEndOfAcquisitionCondition->broadcast();
			waitForEndOfAcquisitionMutex->unlock();

			pauseCameraCondition->wait();
			stopEvent = false;
			//Make time string before returning to setup event acquisition
			timeStamp = imageWriter.makeTimeString();
			isPlaying = true;
		pauseCameraMutex->unlock();

		// If camera hasn't been destructed in the meanwhile, get the exposures
		if(notDestructed){

			//Flag used by stopPlayEvent to stop acquisition
			// this flag is reset to 
//			eventStatMutex->lock();
//				eventStat = ANDOR_ON;
//			eventStatMutex->unlock();

			// Generate the temporary imageVector with a size large enough to hold all the exposures
			// Declaring it here ensures that the size will be correct even if the number of exposures
			// changes from event sequence to event sequence.
			
			numPlayCameraExp = eventMetadata->size();

			std::vector <WORD> tempImageVector(imageSize * numPlayCameraExp);

			//Get the camera temperature at the beginning of the acquisition for metadata purposes
			getCameraTemp();

			while(numAcquired < numPlayCameraExp)
			{
				//Check to make sure the acquisiton hasn't been stopped
//				eventStatMutex->lock();
//					if (eventStat == ANDOR_OFF) {
//						break;
//					}
//				eventStatMutex->unlock();
				

				//Saves data in one long array, tempImageVector
				//See if an acquisition has occurred before settling in to wait
				error = getCameraData(&numAcquired, numPlayCameraExp, tempImageVector);

				std::cout << "Number of exposures: " << numPlayCameraExp << std::endl;
				// If there are still exposures to take, wait for them
				// OR, if we're taking the sacrificial image in External Exposure trigger mode and we haven't already gotten an image...
				if ((!takeSaturatedPic && numAcquired < numPlayCameraExp) || 
					(takeSaturatedPic && numAcquired == 0))
				{
					std::cout<<"Waiting"<<std::endl;
					errorValue = WaitForAcquisition();
					std::cout<<"Done waiting"<<std::endl;
					if (errorValue != DRV_SUCCESS){
						break;
					} else {
						//Saves data in one long array, tempImageVector
						error = getCameraData(&numAcquired, numPlayCameraExp, tempImageVector);
					}
				}
				std::cout << "Number acquired: " << numAcquired << std::endl;
				
				numAcquiredMutex->lock();
					numAcquiredCondition->broadcast();
				numAcquiredMutex->unlock();
				
				if (takeSaturatedPic && numAcquired > 0)
					break;
			}


//			eventStatMutex->lock();
//				eventStat = ANDOR_OFF;
//				pauseCleanupCondition->signal();
//			eventStatMutex->unlock();

			std::cout << numAcquired << std::endl;


			waitForCleanupEventMutex->lock();
				while (!cleanupEvent)
				{
					waitForCleanupEventCondition->wait();
				}
			waitForCleanupEventMutex->unlock();

			// Save pictures as long as there are pictures to be taken
			if(numAcquired != 0 && !error && !takeSaturatedPic) {
#ifndef _DEBUG
				filePath = createFilePath(); //update filepath
				imageWriter.imageVector.clear();

				for (i = 0; i < numAcquired; i++) {
					cropImageData(image.imageData, tempImageVector, i, eventMetadata->at(i).cropVector);
					setMetadata(image, eventMetadata->at(i));
					image.filename = filePath + eventMetadata->at(i).filename;
					//std::cout << image.filename << std::endl;
					image.extension = extension;
					if (eventMetadata->at(i).cropVector.empty()) {
						image.imageHeight = imageHeight;
						image.imageWidth = imageWidth;
					} else {
						image.imageHeight = eventMetadata->at(i).cropVector.at(3) + 1;
						image.imageWidth = eventMetadata->at(i).cropVector.at(2) + 1;
					}
					image.rotationAngle = rotationAngle;
					imageWriter.imageVector.push_back(image);
				}

				imagePreprocessor.processImages(imageWriter.imageVector, bitDepth);
				
				imageWriter.writeImageVector();
#endif
			}
			
		}
		else {
			isPlaying = false;
		}
	}
	return;

}
*/
void QuantixCamera::cropImageData(std::vector <unsigned short> &imageData, std::vector <WORD> & tempImageVector, int imageIndex, std::vector <int> cropVector)
{
	long iWidth, iHeight;
	STI::Utils::stringToValue(cameraState->get(cameraState->imageWidth), iWidth);
	STI::Utils::stringToValue(cameraState->get(cameraState->imageHeight), iHeight);
	long fullImageSize = iWidth*iHeight;
	int i, j;

	imageData.clear();

	if (!cropVector.empty())
	{
		int cropStartX = cropVector.at(0);
		int cropStartY = cropVector.at(1);
		int cropWidth = cropVector.at(2) + 1;
		int cropHeight = cropVector.at(3) + 1;
		imageData.reserve((cropVector.at(2) + 1)*(cropVector.at(3) + 1));
		std::cout << "ImageIndex: " << imageIndex << std::endl;
		//std::cout << "Crop Height: " << cropHeight << std::endl;
		//std::cout << "Crop Width: " << cropWidth << std::endl;
		for (i = 0; i < cropHeight; i++)
		{
			//std::cout << i << std::endl;
			for (j = 0; j < cropWidth; j++)
			{
				//std::cout << j << std::endl;
				//std::cout << j + cropStartX + imageWidth * (cropStartY + i) + imageIndex * fullImageSize << std::endl;
				imageData.push_back(tempImageVector.at(j + cropStartX + iWidth * (cropStartY + i) + imageIndex * fullImageSize));
			}
		}
	}
	else {
		std::cout << "ImageIndex: " << imageIndex << std::endl;
		//std::cout << "No crop" << std::endl;
		imageData.assign(tempImageVector.begin() + imageIndex*fullImageSize, tempImageVector.begin() + (imageIndex + 1)*fullImageSize);
	}
}
std::string QuantixCamera::timeStampFilename(std::string fn)
{
	if (fn.compare("")==0)
	{
		return (timeStamp);
	} else
	{
		return (timeStamp + " " + fn);
	}
}

void QuantixCamera::EventMetadatum::assign(double e, std::string d, std::string f) 
{
	exposureTime = e; 
	description = d; 
	filename = f; 
	cropVector.clear();
}
void QuantixCamera::EventMetadatum::assign(double e, std::string d, std::string f, std::vector <int> cV) 
{
	exposureTime = e; 
	description = d; 
	filename = f; 
	cropVector = cV;
}
/*void QuantixCamera::setupEventAcquisition(std::vector <EventMetadatum> *eM)
{
	eventMetadata = eM;

	waitForCleanupEventMutex->lock();
			cleanupEvent = false;
	waitForCleanupEventMutex->unlock();

	try {

		// Open the shutter
		origShutterMode = getShutterMode();
		if (origShutterMode != SHUTTERMODE_OPEN) {
			setShutterMode(SHUTTERMODE_OPEN);
		}

		//Signal playCamera to start acquiring data
		std::cout << "Signaling..." << std::endl;
		
		//signal play camera

		pauseCameraMutex->lock();
			pauseCameraCondition->signal();
		pauseCameraMutex->unlock();
		
		std::cout << "Done Signaling..." << std::endl;
	}
	catch (ANDOR885_Exception &e) {
		std::cerr << e.printMessage() << std::endl;
	}

}

void QuantixCamera::cleanupEventAcquisition()
{

	waitForCleanupEventMutex->lock();
		cleanupEvent = true;
		waitForCleanupEventCondition->signal();
	waitForCleanupEventMutex->unlock();

	// Wait for the end of the acquistion
	waitForEndOfAcquisitionMutex->lock();
	while (isPlaying)
	{
		waitForEndOfAcquisitionCondition->wait();
	}
	waitForEndOfAcquisitionMutex->unlock();

	eventMetadata = NULL;

	try {
		// Return the shutter to original setting
		if (origShutterMode != SHUTTERMODE_OPEN) {
			setShutterMode(origShutterMode);
		}

	}
	catch (ANDOR885_Exception &e)
	{
		std::cerr << e.printMessage() << std::endl;
	}

}*/
/*bool QuantixCamera::AbortIfAcquiring()
{
	int errorValue;
	bool error = false;

	//Check to see if the camera is acquiring. If it is, stop
	GetStatus(&errorValue);
	if(errorValue == DRV_ACQUIRING){
		//Check to see if the camera is playing. If it is, stop
		if (isPlaying)
			CancelWait();
		errorValue = AbortAcquisition();
		if(errorValue != DRV_SUCCESS){
			std::cerr << "QuantixCamera: Error aborting acquisition" << std::endl;
			error = true;
		} else {
			//Wait until camera stops playing. (necessary? Yes, if called in play event.)
			waitForEndOfAcquisitionMutex->lock();
				while (isPlaying) {
					waitForEndOfAcquisitionCondition->wait();
				}
			waitForEndOfAcquisitionMutex->unlock();
		}

	}

	return error;
}*/
bool QuantixCamera::deviceExit()
{
	bool error = false;

	initialized = false;
	return error;
}



/*void QuantixCamera::InitializeCamera() throw(std::exception)
{
	rs_bool success = true;
	rs_bool available = false;

	char enumStr[100];
	int32 enumValue;
	uns32 currentMode = 0;
	uns32 numModes = 0;
    
	//camera has been initialized by main program


//	*********************************
//	* Get detector size information *
//	*********************************
    pl_get_param(cameraHandle, PARAM_PAR_SIZE, ATTR_AVAIL, (void *) &available);
	if (available) 
	{
		if (!pl_get_param(cameraHandle, PARAM_PAR_SIZE, ATTR_CURRENT, (void *) &imageWidth))
			throw CameraException("Can't access CCD width");
	}

	pl_get_param(cameraHandle, PARAM_SER_SIZE, ATTR_AVAIL, (void *) &available);
	if (available)
	{
		if (!pl_get_param(cameraHandle, PARAM_SER_SIZE, ATTR_CURRENT, (void *) &imageHeight))
			throw CameraException("Can't access CCD height");
	}

//	*******************************
//	* Get parallel clocking modes *
//	*******************************
	pl_get_param(cameraHandle, PARAM_PMODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_PMODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_PMODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_PMODE, mode, &enumValue, enumStr, 100);
			if (!pl_set_param (cameraHandle, PARAM_PMODE, &mode)) {
				continue;
			}
			
			clockingMode_t->choices[(int) mode] = enumStr;
		}

		//reset initial mode
		pl_set_param (cameraHandle, PARAM_PMODE, &currentMode);
		clockingMode_t->currentString = clockingMode_t->choices[(int) currentMode];
		clockingMode_t->currentValue = (int) currentMode;
	}

	/*clockingMode_t->choices[PMODE_NORMAL] = "Normal";
	pl_get_param(cameraHandle, PARAM_MPP_CAPABLE, ATTR_AVAIL, (void *) &available);
	if (available)
		clockingMode_t->choices[PMODE_MPP] = "Multi-Pinned Phase";

	rs_bool tmpAvail = available;
	pl_get_param(cameraHandle, PARAM_FRAME_CAPABLE, ATTR_AVAIL, (void *) &available);
	if (available) 
	{
		clockingMode_t->choices[PMODE_FT] = "Frame Transfer";
		if (tmpAvail)
			clockingMode_t->choices[PMODE_FT_MPP] = "FT and MPP";
	}
	uns32 pmode;
	clockingMode_t->currentString = clockingMode_t->choices[PMODE_NORMAL];
	pmode = (uns32) clockingMode_t->currentString;
	if (!pl_set_param(cameraHandle, PARAM_PMODE, (void *) &pmode))
		throw CameraException("Can't set clocking mode to Normal.");
	clockingMode = PMODE_NORMAL;*/

/*
//	*******************************
//	* CCD Readout Port and Speeds *
//	*******************************
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
		short	bitDepth;
		short	pixTime;
		if (pl_get_param (cameraHandle, PARAM_SPDTAB_INDEX, ATTR_MAX, &maxSpeeds))
		{
			pl_get_param(cameraHandle, PARAM_SPDTAB_INDEX, ATTR_CURRENT, (void *) &currentMode);

			maxSpeeds++;
			for (short speed = 0; speed < maxSpeeds; speed++) {
				if (!pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &speed)) {
					continue;
				}
				if (!pl_get_param (cameraHandle, PARAM_BIT_DEPTH, ATTR_CURRENT, &bitDepth))
					throw CameraException("Can't access bit depth");
			
				if (!pl_get_param (cameraHandle, PARAM_PIX_TIME, ATTR_CURRENT, &pixTime))
					throw CameraException("Can't access pixel time");
				
				readoutSpeed_t->choices[(int) speed] = STI::Utils::valueToString(bitDepth) + " bits, " +
					STI::Utils::valueToString(pixTime) + " ns/px";

			}

			//reset initial mode
			pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &currentMode);
			readoutSpeed_t->currentString = readoutSpeed_t->choices[(int) currentMode];
			readoutSpeed = (int) currentMode;
		}
	}

//	********************
//	*     CCD Gain     *
//	********************
	int16 maxGain;
    if (pl_get_param (cameraHandle, PARAM_GAIN_INDEX, ATTR_MAX, (void *)&maxGain)) 
	{
		for (int16 i = 1; i <= maxGain; i++)
		{
			gain_t->choices[(int) i] = STI::Utils::valueToString(i);
			//Can't get actual gain with the Quantix 6303E; the parameter would be PARAM_ACTUAL_GAIN.
		}

		int16 currentGain;
		if(!pl_get_param(cameraHandle, PARAM_GAIN_INDEX, ATTR_CURRENT, (void *) &currentGain))
			throw CameraException("Can't access current gain");

		gain_t->currentString = gain_t->choices.find((int) currentGain)->second;
    }

//	*******************
//	*     Shutter     *
//	*******************
	
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
			
			shutterMode_t->choices[(int) mode] = enumStr;
		}

		//reset initial mode, or set to Ignore mode
		pl_set_param (cameraHandle, PARAM_SHTR_OPEN_MODE, &currentMode);
//		pl_get_enum_param (cameraHandle, PARAM_SHTR_OPEN_MODE, currentMode, &enumValue, enumStr, 100);
		shutterMode_t->currentString = shutterMode_t->choices[(int) currentMode];

	}
	
	// Set up delays
	pl_get_param(cameraHandle, PARAM_SHTR_OPEN_DELAY, ATTR_AVAIL, (void *) &available);
	if (available){
		unsigned short shutterOpenTime = SHUTTER_OPEN_TIME;
		pl_set_param(cameraHandle, PARAM_SHTR_OPEN_DELAY, (void *) &shutterOpenTime);
	}

	pl_get_param(cameraHandle, PARAM_SHTR_CLOSE_DELAY, ATTR_AVAIL, (void *) &available);
	if (available){
		unsigned short shutterCloseTime = SHUTTER_CLOSE_TIME;
		pl_set_param(cameraHandle, PARAM_SHTR_CLOSE_DELAY, (void *) &shutterCloseTime);
	}

//	***********************
//	*     Temperature     *
//	***********************
	pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_AVAIL, (void *) &available);
	if (available){
		int16 maxTempS;
		pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_MAX, (void *) &maxTempS);
		maxTemp = (int) maxTempS/100;
		int16 minTempS;
		pl_get_param(cameraHandle, PARAM_TEMP_SETPOINT, ATTR_MIN, (void *) &minTempS);
		minTemp = (int) minTempS/100;
	}
	else 
		throw CameraException("Program expects camera to be cooled");

	setCoolerSetpt(coolerSetpt);

//	************************
//	*     Clear Cycles     *
//	************************
	uns16 numCycles;
	pl_get_param(cameraHandle, PARAM_CLEAR_CYCLES, ATTR_CURRENT, (void *) &numCycles);
	clearCycles = (int) numCycles;

	pl_get_param(cameraHandle, PARAM_CLEAR_MODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_CLEAR_MODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_CLEAR_MODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_CLEAR_MODE, mode, &enumValue, enumStr, 100);
			if (!pl_set_param (cameraHandle, PARAM_CLEAR_MODE, &mode)) {
				continue;
			}
			
			clearMode_t->choices[(int) mode] = enumStr;
		}

		//reset initial mode
		pl_set_param (cameraHandle, PARAM_CLEAR_MODE, &currentMode);
		clearMode_t->currentString = clearMode_t->choices[(int) currentMode];
		clearMode = currentMode;
	}

//	*************************
//	*     Exposure Mode     *
//	*************************

	pl_get_param(cameraHandle, PARAM_EXPOSURE_MODE, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_EXPOSURE_MODE, ATTR_CURRENT, (void *) &currentMode);

		pl_get_param (cameraHandle, PARAM_EXPOSURE_MODE, ATTR_COUNT, &numModes);
		for (uns32 mode = 0; mode < numModes; mode++) {
			pl_get_enum_param (cameraHandle, PARAM_EXPOSURE_MODE, mode, &enumValue, enumStr, 100);
			
			if (enumValue == TRIGGERMODE_EXTERNAL)
				triggerMode_t->choices[TRIGGERMODE_EXTERNAL] = enumStr;

			if (enumValue == TRIGGERMODE_EXTERNAL_EXPOSURE)
				triggerMode_t->choices[TRIGGERMODE_EXTERNAL_EXPOSURE] = enumStr;
		}

		//Cannot set exposure mode through PARAM, so just store the value
		//it will get set vefore taking a picture through pl_exp_set_time
		if (triggerMode_t->choices.count(TRIGGERMODE_EXTERNAL_EXPOSURE) > 0)
		{
			triggerMode = TRIGGERMODE_EXTERNAL_EXPOSURE;
			triggerMode_t->currentString = triggerMode_t->choices.find(triggerMode)->second;
		}
		else
			throw CameraException("Program requires camera to have external exposure (a.k.a bulb) mode");
	}

	return;
}

int	QuantixCamera::getCameraTemp()
{
	rs_bool available = FALSE;
	int temp = -99900;

	int16 currentTemp;
	pl_get_param(cameraHandle, PARAM_TEMP, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_TEMP, ATTR_CURRENT, (void *) &currentTemp);
	}

	cameraTemp = temp/100;	// Camera reports temperature in degrees C x 100

	return cameraTemp;
}
int	QuantixCamera::getClearMode()
{
	return clearMode;
}
void QuantixCamera::setClearMode(int newMode) 
{
	uns32 mode = (uns32) newMode;
	if(!pl_set_param(cameraHandle, PARAM_CLEAR_MODE, (void *) &mode))
		throw CameraException("Error setting clear mode");
		
	clearMode = newMode;
}
/*int	QuantixCamera::getClockingMode()
{
	return clockingMode;
}
void QuantixCamera::setClockingMode(int newMode) 
{
	uns32 mode = (uns32) newMode;
	if(!pl_set_param(cameraHandle, PARAM_PMODE, (void *) &mode))
		throw CameraException("Error setting clocking mode");
		
	clockingMode = newMode;
}
int	QuantixCamera::getCoolerSetpt()
{
	return coolerSetpt;
}
void QuantixCamera::setCoolerSetpt(int setpt) 
{
	if (setpt > maxTemp || setpt < minTemp) {
		throw CameraException("Chosen temperature out of range.\n Temperature must be between " 
			+ STI::Utils::valueToString(minTemp) + " and " + STI::Utils::valueToString(maxTemp));
	}

	int16 coolerSetptS = (int16) 100*coolerSetpt; // Camera reports temperature in degrees C x 100
	if(!pl_set_param(cameraHandle, PARAM_TEMP_SETPOINT, (void *) &coolerSetptS))
		throw CameraException("Error setting cooler temperature");
		
	coolerSetpt = setpt;
}
std::string	QuantixCamera::getFilePath()
{
	return filePath;
}*/
/*
void QuantixCamera::ClockingMode::set(int newMode) 
{
	uns32 mode = (uns32) newMode;
	if(!pl_set_param(camera->cameraHandle, PARAM_PMODE, (void *) &mode))
		throw CameraException("Error setting clocking mode");
		
	currentValue = newMode;
}
int	QuantixCamera::getCoolerSetpt()
{
	return coolerSetpt;
}
void QuantixCamera::setCoolerSetpt(int setpt) 
{
	if (setpt > maxTemp || setpt < minTemp) {
		throw CameraException("Chosen temperature out of range.\n Temperature must be between " 
			+ STI::Utils::valueToString(minTemp) + " and " + STI::Utils::valueToString(maxTemp));
	}

	int16 coolerSetptS = (int16) 100*coolerSetpt; // Camera reports temperature in degrees C x 100
	if(!pl_set_param(cameraHandle, PARAM_TEMP_SETPOINT, (void *) &coolerSetptS))
		throw CameraException("Error setting cooler temperature");
		
	coolerSetpt = setpt;
}*/
std::string	QuantixCamera::getFilePath()
{
	return filePath;
}

void QuantixCamera::setFilePath(std::string path)
{
	filePath = path;
}
std::string QuantixCamera::createFilePath()
{
	std::string filePathStem = "\\\\epsrv1\\EP\\data\\";
	std::string year;
	std::string month;
	std::string day;

	struct tm localTime;
	__int64 rawTime;
	errno_t err;

	_time64( &rawTime );

	// Obtain coordinated universal time: 
	err = _localtime64_s( &localTime, &rawTime );
	if (err)
	{
		std::cerr << "Invalid Argument to _gmtime64_s." << std::endl;
	}

	year = STI::Utils::valueToString(localTime.tm_year + 1900);
	month = STI::Utils::valueToString(localTime.tm_mon + 1);
	day = STI::Utils::valueToString(localTime.tm_mday);


	return filePathStem + year + "\\" + month + "\\" + day + "\\data\\";
}
/*
void QuantixCamera::Gain::set(int newGain) 
{
	int16 gainQ = (int16) newGain;
	if(!pl_set_param(camera->cameraHandle, PARAM_GAIN_INDEX, (void *) &gainQ))
		throw CameraException("Error setting gain");
		
	currentValue = newGain;
}

void QuantixCamera::ShutterMode::set(int newMode) 
{
	int16 mode = (int16) newMode;
	if(!pl_set_param (camera->cameraHandle, PARAM_SHTR_OPEN_MODE, &mode))
		throw CameraException("Error setting shutter mode");
		
	currentValue = newMode;
}

void QuantixCamera::ReadoutSpeed::set(int newSpeed) 
{
	int16 speed = (int16) newSpeed;
	if(!pl_set_param (camera->cameraHandle, PARAM_SPDTAB_INDEX, &speed))
		throw CameraException("Error setting readout speed");

	//Gain gets reset to 1x after a change in the Readout Speed. Reset the gain
	camera->gain_t->set(camera->gain_t->currentValue);
		
	currentValue = newSpeed;
}*/
/*
int	QuantixCamera::getGain()
{
	return gain;
}
void QuantixCamera::setGain(int newGain) 
{
	int16 gainQ = (int16) newGain;
	if(!pl_set_param(cameraHandle, PARAM_GAIN_INDEX, (void *) &gainQ))
		throw CameraException("Error setting gain");
		
	gain = newGain;
}
int	QuantixCamera::getShutterMode()
{
	return shutterMode;
}
void QuantixCamera::setShutterMode(int newMode) 
{
	int16 mode = (int16) newMode;
	if(!pl_set_param (cameraHandle, PARAM_SHTR_OPEN_MODE, &mode))
		throw CameraException("Error setting shutter mode");
		
	shutterMode = newMode;
}
int	QuantixCamera::ReadoutSpeed::get()
{
	return currentValue;
}
void QuantixCamera::ReadoutSpeed::set(int newSpeed) 
{
	int16 speed = (int16) newSpeed;
	if(!pl_set_param (cameraHandle, PARAM_SPDTAB_INDEX, &speed))
		throw CameraException("Error setting readout speed");

	//Gain gets reset to 1x after a change in the Readout Speed. Reset the gain
	setGain(gain_t->currentValue);
		
	readoutSpeed = newSpeed;
}
int	QuantixCamera::getTriggerMode()
{
	return triggerMode;
}
void QuantixCamera::setTriggerMode(int newTriggerMode) 
{
	//trigger mode gets set before each picture sequence
	triggerMode = newTriggerMode;
}*/
/*int QuantixCamera::CameraAttribute::inverseFind(std::string value)
{
	int i = -1;

	std::map<int, std::string>::iterator iter;

	//Find string associated with flag
	for (iter = choices.begin(); iter != choices.end(); iter++ ){
		if (value.compare(iter->second) == 0){
			return iter->first;
		}
	}

	std::cerr << "Error in " << name << " selection: " << value << " not allowed." << std::endl;


	return i;
}*/

// Makes the string of attribute choices for addAttribute 
