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
	rotationAngle = 0;

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

	filePath		=	createFilePath();

	try {
		cameraState = new QuantixState(cameraHandle);
		initialized = true; // change to True eventually
	}
	catch(CameraException &e)
	{
		std::cerr << "Camera Initialization Error: " << e.what() << std::endl;
		initialized = false;
	}

	if (initialized){
		notDestructed = true;

		imageBuffer = (uns16 *) malloc(100);

		omni_thread::create(playCameraWrapper, (void*) this, omni_thread::PRIORITY_HIGH);
	}
}

QuantixCamera::~QuantixCamera()
{
	//clear the playCamera thread. Assumes an event is not currently playing
	notDestructed = false;
	pauseCameraMutex->lock();
		pauseCameraCondition->signal();
	pauseCameraMutex->unlock();
	
	cameraExit();
	if (imageBuffer != NULL)
		free(imageBuffer);

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
}

void QuantixCamera::playCameraWrapper(void* object)
{
	QuantixCamera* thisObject = static_cast<QuantixCamera*>(object);
	thisObject->playCamera();
}
void QuantixCamera::playCamera(){

	long imageSize = cameraState->imageHeight.getSize() * cameraState->imageWidth.getSize();
	long index;
	std::vector <unsigned short> singleImageVector(imageSize);
	int i;
	bool error;
	int numPlayCameraExp = 1; //define a local number of exposures in case the cleanup event gets played before acquisition ends

	int16 status;
    uns32 not_needed;

	ImageMagick::MyImage image;
	image.imageData.reserve(imageSize);

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
			isPlaying = true;
		pauseCameraMutex->unlock();

		//Make time string before returning to setup event acquisition
		timeStamp = imageWriter.makeTimeString();

		// If camera hasn't been destructed in the meanwhile, get the exposures
		if(notDestructed){

			imageWriter.imageVector.clear();
			imageSize = (eventMetadata->at(0).cropVector.at(2)+1)* (eventMetadata->at(0).cropVector.at(3)+1);
			image.imageData.resize(imageSize, 0);

			// Generate the temporary imageVector with a size large enough to hold all the exposures
			// Declaring it here ensures that the size will be correct even if the number of exposures
			// changes from event sequence to event sequence.
			
			numPlayCameraExp = eventMetadata->size();

			std::vector <unsigned short> tempImageVector(imageSize * numPlayCameraExp);

			//Get the camera temperature at the beginning of the acquisition for metadata purposes
			cameraState->get(cameraState->temperature);

			while(numAcquired < numPlayCameraExp && stopEvent == false)
			{

				//Saves data in one long array, tempImageVector
				std::cout << "Number of exposures: " << numPlayCameraExp << std::endl;
				
				if(!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
				{
					error = true;
					printError();
					break;
				}

				if (status == READOUT_FAILED)
				{
					error = true;
					std::cerr << "Readout failed" << std::endl;
					break;
				}

				if (status == READOUT_COMPLETE)
				{
					error = getCameraData(image.imageData);
					if (error)
						break;
					imageWriter.imageVector.push_back(image);
					numAcquired++;
				}
				else
				{
					//Sleep(100);
					pauseCameraMutex->lock();

						pauseCameraCondition->timedwait(0.1);

					pauseCameraMutex->unlock();
					continue;
				}

				std::cout << "Number acquired: " << numAcquired << std::endl;
				
				numAcquiredMutex->lock();
					numAcquiredCondition->broadcast();
				numAcquiredMutex->unlock();
			}


			std::cout << numAcquired << std::endl;

			waitForCleanupEventMutex->lock();
				while (!cleanupEvent)
				{
					waitForCleanupEventCondition->wait();
				}
			waitForCleanupEventMutex->unlock();

			// Save pictures as long as there are pictures to be taken
			if(numAcquired == imageWriter.imageVector.size() && !error) {
				filePath = createFilePath(); //update filepath

				ImageMagick::MyImage *tmpImage;
				for (i = 0; i < numAcquired; i++) {
					tmpImage = &(imageWriter.imageVector.at(i));
					setMetadata(tmpImage, eventMetadata->at(i));
					tmpImage->filename = filePath + eventMetadata->at(i).filename;
					
					tmpImage->extension = extension;
					if (eventMetadata->at(i).cropVector.empty()) {
						tmpImage->imageHeight = cameraState->imageHeight.getSize();
						tmpImage->imageWidth = cameraState->imageWidth.getSize();
					} else {
						tmpImage->imageHeight = eventMetadata->at(i).cropVector.at(3) + 1;
						tmpImage->imageWidth = eventMetadata->at(i).cropVector.at(2) + 1;
					}
					tmpImage->rotationAngle = rotationAngle;
					
				}

				imagePreprocessor.processImages(imageWriter.imageVector, cameraState->bitDepth.getSize());
				
				imageWriter.writeImageVector();
			}
			
		}
		else {
			isPlaying = false;
		}
	}
	return;

}
bool QuantixCamera::getCameraData(std::vector <unsigned short>& singleImageVector)
{
	bool error = false;

	void * address;
	if (pl_exp_get_oldest_frame(cameraHandle, &address))
	{
		singleImageVector.assign(((uns16*) address),((uns16*) address + singleImageVector.size()));
		pl_exp_unlock_oldest_frame(cameraHandle);
	}
	else
		error = true;

	return (error);
}
void QuantixCamera::setMetadata(ImageMagick::MyImage *image)
{
	ImageMagick::Metadatum metadatum;

	image->metadata.clear();

	setCommonMetadata(image);
}

void QuantixCamera::setMetadata(ImageMagick::MyImage *image, EventMetadatum &eventMetadatum)
{
	ImageMagick::Metadatum metadatum;

	image->metadata.clear();

	metadatum.tag = "Event Exposure Time";
	metadatum.value = STI::Utils::valueToString(eventMetadatum.exposureTime);
	image->metadata.push_back(metadatum);

	metadatum.tag = "Description";
	metadatum.value = eventMetadatum.description;
	image->metadata.push_back(metadatum);

	metadatum.tag = "Crop Vector";
	if (eventMetadatum.cropVector.empty())
	{
		metadatum.value = "Uncropped";
	}
	else 
	{
		metadatum.value = "( " + STI::Utils::valueToString(eventMetadatum.cropVector.at(0) + 1) + ", " + 
			STI::Utils::valueToString(eventMetadatum.cropVector.at(1) + 1) + ", " +
			STI::Utils::valueToString(eventMetadatum.cropVector.at(2) + 1) + ", " +
			STI::Utils::valueToString(eventMetadatum.cropVector.at(3) + 1) + " )";
	}
	image->metadata.push_back(metadatum);

	setCommonMetadata(image);
}

void QuantixCamera::setCommonMetadata(ImageMagick::MyImage *image)
{
	ImageMagick::Metadatum metadatum;

	cameraState->guiAttributes.size();
	for (unsigned int i = 0; i < cameraState->guiAttributes.size(); i++)
	{
		metadatum.tag = cameraState->guiAttributes.at(i)->name;
		metadatum.value = cameraState->get(*(cameraState->guiAttributes.at(i)));
		image->metadata.push_back(metadatum);
	}

}


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
void QuantixCamera::playCameraAcquisition(std::vector <EventMetadatum> *eM)
{
	eventMetadata = eM;

	waitForCleanupEventMutex->lock();
			cleanupEvent = false;
	waitForCleanupEventMutex->unlock();

	//Signal playCamera to start acquiring data
	std::cout << "Signaling..." << std::endl;

	int16 status;
    uns32 not_needed;
	pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed );
	
	//signal play camera

	pauseCameraMutex->lock();
		pauseCameraCondition->signal();
	pauseCameraMutex->unlock();
	
	std::cout << "Done Signaling..." << std::endl;
}

void QuantixCamera::setupCameraAcquisition(std::vector <EventMetadatum> *eM)
{
	uns16 s1, p1, s2, p2, sBin, pBin;


	if (eM == NULL)
		throw CameraException("No events provided");;

	if (eM->at(0).cropVector.size() == 4)
	{
		s1 = (uns16) eM->at(0).cropVector.at(0);
		p1 = (uns16) eM->at(0).cropVector.at(1);
		s2 = s1 + (uns16) eM->at(0).cropVector.at(2);
		p2 = p1 + (uns16) eM->at(0).cropVector.at(3);
	}
	else
		throw CameraException("Could not format crop vector (ROI)");;

	if (!STI::Utils::stringToValue(cameraState->binSize.get(), sBin))
		throw CameraException("Error transforming string to value");;
	pBin = sBin;

	rgn_type region = {s1, s2, sBin, p1, p2, pBin};


	uns16 currentTriggerMode;
	if (!STI::Utils::stringToValue(cameraState->triggerMode.get(), currentTriggerMode))
		throw CameraException("Error transforming string to value");

	uns32 exposureTime = (uns32) eM->at(0).exposureTime;

	//Only one region per image. What do multiple regions do? Multiple images, same exposure?
	uns32 bufferSize, frameSize;
	if (!pl_exp_init_seq())
		throw CameraException("Could not setup Quantix for acquisition");
	if (!pl_exp_setup_cont(cameraHandle, 1, &region, currentTriggerMode, exposureTime, &frameSize, CIRC_OVERWRITE))
		throw CameraException("Could not setup Quantix for acquisition");

	bufferSize = frameSize * eM->size();
//	imageBuffer.resize(bufferSize, 0);
	imageBuffer = (uns16 *) realloc(imageBuffer, bufferSize);

	if (!pl_exp_start_cont(cameraHandle, imageBuffer, bufferSize))
		throw CameraException("Could not start acquisition");

	int16 status;
    uns32 not_needed;
	pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed );

}


void QuantixCamera::cleanupCameraAcquisition()
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

}
bool QuantixCamera::AbortIfAcquiring()
{
	bool error = false;

	//Check to see if the camera is acquiring. If it is, stop
/*	GetStatus(&errorValue);
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

	}*/

	int16 status;
	uns32 byteCount, bufferCount;
	pl_exp_check_cont_status(cameraHandle, &status, &byteCount, &bufferCount);
/*	switch(status)
	{
	case READOUT_NOT_ACTIVE:
		std::cerr << "Readout not active" <<std::endl;
		break;
	case EXPOSURE_IN_PROGRESS:
		std::cerr << "EXPOSURE_IN_PROGRESS" <<std::endl;
		break;
	case READOUT_IN_PROGRESS:
		std::cerr << "READOUT_IN_PROGRESS" <<std::endl;
		break;
	case READOUT_COMPLETE:
		std::cerr << "READOUT_COMPLETE" <<std::endl;
		break;
	case READOUT_FAILED:
		std::cerr << "READOUT_FAILED" <<std::endl;
		break;
	case ACQUISITION_IN_PROGRESS:
		std::cerr << "ACQUISITION_IN_PROGRESS" <<std::endl;
		break;
	case MAX_CAMERA_STATUS:
		std::cerr << "MAX_CAMERA_STATUS" <<std::endl;
		break;
	default:
		std::cerr << "Default" <<std::endl;
		break;
	}
	printError();*/
	if (status != READOUT_NOT_ACTIVE)
	{

		if (status == READOUT_FAILED) {
			std::cerr << "Readout failed" << std::endl;
			printError();
			error = true;
		}

		pl_exp_stop_cont(cameraHandle, CCS_HALT);
		pl_exp_finish_seq(cameraHandle, &imageBuffer, 0);
		pl_exp_uninit_seq();
	}
	return error;
}
bool QuantixCamera::cameraExit()
{
	bool error = false;

	AbortIfAcquiring();

	initialized = false;
	return error;
}

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

double QuantixCamera::getMinExposureTime()
{
	//Best Case Scenario: get minimum exposure time from Camera
	flt64 minExpTimeS;

	if(!pl_get_param(cameraHandle, PARAM_EXP_MIN_TIME, ATTR_CURRENT, (void *)&minExpTimeS))
	{
			minExpTimeS = 0;
	}

	return (double) (1000000000 * minExpTimeS);
}

double QuantixCamera::getFrameRefreshTime()
{
	uns16 pixelTimeNS;
	if(!pl_get_param(cameraHandle, PARAM_PIX_TIME, ATTR_CURRENT, (void *) &pixelTimeNS))
	{
		char msg[ERROR_MSG_LEN];		// for error handling
		pl_error_message(pl_error_code(), msg);
		std::cerr << "Pixel readout time error: " << msg << std::endl;
		throw CameraException("Error getting pixel readout time.");
	}

	int binSize;
	if (!STI::Utils::stringToValue(cameraState->binSize.get(), binSize))
		throw CameraException("Error transforming string to value");

	return pixelTimeNS*(cameraState->imageHeight.getSize())*(cameraState->imageWidth.getSize())/binSize;
}
void QuantixCamera::printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}