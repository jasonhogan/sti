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
	rotationAngle = 270;  // makes the height the right direction
	isAcquiring = false;

	imageBuffer = NULL;

	pauseCameraMutex = new omni_mutex();
	pauseCameraCondition = new omni_condition(pauseCameraMutex);

	acquisitionMutex = new omni_mutex();
	acquisitionCondition = new omni_condition(acquisitionMutex);

	bool cleanupEvent = false;

	filePath		=	createFilePath();

	try {
		cameraState = new QuantixState(cameraHandle);
		//Initialize data collection libraries
		if (!pl_exp_init_seq())
			throw CameraException("Could not init Quantix for acquisition");
		initialized = true; // change to True eventually
	}
	catch(CameraException &e)
	{
		std::cerr << "Camera Initialization Error: " << e.what() << std::endl;
		initialized = false;
	}

	if (initialized){
		notDestructed = true;

		//imageBuffer = (uns16 *) malloc(100);

		//omni_thread::create(playCameraWrapper, (void*) this, omni_thread::PRIORITY_HIGH);
	}
}

QuantixCamera::~QuantixCamera()
{
	//clear the playCamera thread. Assumes an event is not currently playing
	notDestructed = false;
	stopEvent = true;

	pauseCameraMutex->lock();
		pauseCameraCondition->signal();
	pauseCameraMutex->unlock();
	
	cameraExit();
	delete[] imageBuffer;

	delete pauseCameraMutex;
	delete pauseCameraCondition;

	delete acquisitionMutex;
	delete acquisitionCondition;
}

void QuantixCamera::waitForImage()
{
	int16 status;
    uns32 not_needed;
	int i = 0;

	unsigned long waitS, waitNS;

	//Wait for the acquisition to start before checking for data
	acquisitionMutex->lock();
		if (!isAcquiring)
			acquisitionCondition->wait();
	acquisitionMutex->unlock();

	//Wait for an image to be acquired; stop acquisition if an error has occurred.

	while(!stopEvent)
	{

		acquisitionMutex->lock();
			if (!isAcquiring)
				break;
		acquisitionMutex->unlock();

		i++;
		if(!pl_exp_check_status( cameraHandle, &status, &not_needed))
		{
			printError();
			break;
		}

		if (status == READOUT_FAILED)
		{
			std::cerr << "Readout failed" << std::endl;
			break;
		}

		if (status == READOUT_COMPLETE)
		{
			std::cerr << "Waited " << i << "times" << std::endl;
			return;
		}
		else
		{
			//Sleep(500);
			pauseCameraMutex->lock();
				omni_thread::get_time(&waitS, &waitNS, 0, 500000000);
				pauseCameraCondition->timedwait(waitS,waitNS);

			pauseCameraMutex->unlock();
			continue;
		}

	}
	
	stopEvent = true;
	return;

}
void QuantixCamera::getImage(EventMetadatum &eventMetadatum, unsigned int imageIndex)
{
	int binSize;
	STI::Utils::stringToValue(cameraState->binSize.get(), binSize);

	//NB: image magick and the camera have flipped definitions of height and width.
	// more accurately, flipped definitions of serial v. parallel
	long binnedHeight, binnedWidth;
	if (eventMetadatum.cropVector.empty())
	{
		binnedWidth = cameraState->imageHeight.getSize()/binSize;
		binnedHeight = cameraState->imageWidth.getSize()/binSize;
	}
	else
	{
		//use integer division
		binnedWidth = (eventMetadatum.cropVector.at(3)+1)/binSize;
		binnedHeight = (eventMetadatum.cropVector.at(2)+1)/binSize;
	}

	std::cerr << "Getting Image" << std::endl;

	long imageSize = binnedHeight * binnedWidth;
	ImageMagick::MyImage image;
	image.imageData.reserve(imageSize);

	checkQuantixStatus();
	uns16 * imageAddress;
	if (!stopEvent)
	{
		checkQuantixStatus();

		imageAddress = imageBuffer + (imageIndex*imageSize);
		image.imageData.assign(imageAddress, imageAddress + imageSize);

		checkQuantixStatus();

		setMetadata(&image, eventMetadatum);
		image.filename = filePath + eventMetadatum.filename;
		
		image.extension = extension;
		image.imageHeight = binnedHeight;
		image.imageWidth = binnedWidth;
		image.rotationAngle = rotationAngle;

		imageWriter.imageVector.push_back(image);
	}
	else
		stopEvent = true;

	return;
}

void QuantixCamera::saveImages()
{

	if(!imageWriter.imageVector.empty() && !stopEvent)
	{
		//all images have been received, so clean up
		if(!pl_exp_finish_seq(cameraHandle, &imageBuffer, 0))
		{
			std::cerr << "Could not finish acquisition" << std::endl;
			printError();
		}
		else
		{
			acquisitionMutex->lock();
				isAcquiring = false;
			acquisitionMutex->unlock();
		}

		imagePreprocessor.processImages(imageWriter.imageVector, cameraState->bitDepth.getSize());
					
		imageWriter.writeImageVector();
	}

	checkQuantixStatus();
	std::cerr << std::endl << std::endl;
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
		if (metadatum.tag.compare(cameraState->temperature.name) == 0)
			metadatum.value = cameraState->temperature.getLabel();
		else
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
void QuantixCamera::initializeCameraAcquisition()
{
	cleanupEvent = false;
	stopEvent = false;

	if (!pl_exp_start_seq(cameraHandle, imageBuffer)) {
		stopEvent = true;
		std::cerr << "Could not start Quantix acquisition" << std::endl;
	}

	checkQuantixStatus();

	//Wait for the acquisition to start before checking for data
	acquisitionMutex->lock();
		isAcquiring = true;
		acquisitionCondition->broadcast(); //there should only be one thread waiting for this	
	acquisitionMutex->unlock();

	timeStamp = imageWriter.makeTimeString();
	imageWriter.imageVector.clear();

	//isPlaying = true;


}

/*void QuantixCamera::playCameraAcquisition()
{
	
	//Signal playCamera to start acquiring data
	std::cout << "Signaling..." << std::endl;


	//signal play camera

	pauseCameraMutex->lock();
		pauseCameraCondition->signal();
	pauseCameraMutex->unlock();
	
	std::cout << "Done Signaling..." << std::endl;


}*/

void QuantixCamera::setupCameraAcquisition(std::vector <EventMetadatum> *eM)
{
	uns16 s1, p1, s2, p2, sBin, pBin;


	//Check camera temperature for metadata purposes
	cameraState->get(cameraState->temperature);
	
	if (eM == NULL)
		throw CameraException("No events provided");

	if (eM->at(0).cropVector.size() == 4)
	{
		p1 = (uns16) eM->at(0).cropVector.at(0);
		s1 = (uns16) eM->at(0).cropVector.at(1);
		p2 = p1 + (uns16) eM->at(0).cropVector.at(2);  //width is parallel
		s2 = s1 + (uns16) eM->at(0).cropVector.at(3);  //height is serial
	}
	else
		throw CameraException("Could not format crop vector (ROI)");

	if (!STI::Utils::stringToValue(cameraState->binSize.get(), sBin))
		throw CameraException("Error transforming string to value");
	pBin = sBin;

	rgn_type region = {s1, s2, sBin, p1, p2, pBin};


	uns16 currentTriggerMode;
	if (!STI::Utils::stringToValue(cameraState->triggerMode.get(), currentTriggerMode))
		throw CameraException("Error transforming string to value");

	uns32 exposureTime = (uns32) (eM->at(0).exposureTime/1000000); //This gets ignored

	//Only one region per image. What do multiple regions do? Multiple images, same exposure?
	uns32 bufferSize, frameSize = 1;
	uns16 numberOfExposures = eM->size();
	std::cerr << "Number of exposures: " << numberOfExposures << std::endl;
	
	if (!pl_exp_setup_seq(cameraHandle, numberOfExposures, 1, &region, currentTriggerMode, exposureTime, &frameSize))
		throw CameraException("Could not setup Quantix for acquisition");

	bufferSize = (frameSize/2) * numberOfExposures; //Comes in bytes and an uns16 is 2 bytes

	std::cerr << "Buffer Size: " << bufferSize << std::endl;
	std::cerr << "Crop vector numbers: " << eM->at(0).cropVector.at(2) << " and " << eM->at(0).cropVector.at(3) << std::endl;

	try
	{
		delete [] imageBuffer;
		imageBuffer = new uns16[bufferSize];
	}
	catch(...)
	{
		throw CameraException("Error allocating memory to image buffer");
	}


}


/*void QuantixCamera::cleanupCameraAcquisition()
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

}*/
bool QuantixCamera::AbortIfAcquiring()
{
	bool error = false;

	acquisitionMutex->lock();
		if (isAcquiring)
		{
			if(!pl_exp_abort(cameraHandle, CCS_HALT))
			{
				std::cerr << "Could not abort acquisition" << std::endl;
				printError();
				error = true;
			}
			
			isAcquiring = false;
		}
	acquisitionMutex->unlock();
	return error;


}
bool QuantixCamera::cameraExit()
{
	bool error = false;

	AbortIfAcquiring();

	//Release libraries
	if (!pl_exp_uninit_seq())
	{
		std::cerr << "Could not uninit acquisition" << std::endl;
		printError();
		error = true;
	}

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

	char enumStr[100];
	int32 enumValue;
	uns32 currentMode = 0;
	//experimentally-determined min exposure time: 0.3*ms, but keep 1 ms to be safe

	if(!pl_get_param(cameraHandle, PARAM_EXP_MIN_TIME, ATTR_CURRENT, (void *)&minExpTimeS))
	{
		pl_get_param(cameraHandle, PARAM_EXP_RES, ATTR_CURRENT, (void *) &currentMode);
		pl_get_enum_param (cameraHandle, PARAM_EXP_RES, currentMode, &enumValue, enumStr, 100);
		if (enumValue == EXP_RES_ONE_MICROSEC)
			minExpTimeS = 0.000001;
		else if (enumValue == EXP_RES_ONE_MILLISEC)
			minExpTimeS = 0.001;
		else
			minExpTimeS = 0;
	}

	return (double) (1000000000 * minExpTimeS);
}

double QuantixCamera::getFrameRefreshTime(EventMetadatum &eventMetadatum)
{
	uns16 pixelWriteTimeNS;

	double parallelShiftTimeNS = 80000; // 80 microseconds
	double serialDiscardRateNS = 100;   // 0.1 microseconds

	

	if(!pl_get_param(cameraHandle, PARAM_PIX_TIME, ATTR_CURRENT, (void *) &pixelWriteTimeNS))
	{
		char msg[ERROR_MSG_LEN];		// for error handling
		pl_error_message(pl_error_code(), msg);
		std::cerr << "Pixel readout time error: " << msg << std::endl;
		throw CameraException("Error getting pixel readout time.");
	}

	int parallelRows = (cameraState->imageWidth.getSize());
	int totalPixels = (cameraState->imageHeight.getSize())*(cameraState->imageWidth.getSize());
	int recordedRows = eventMetadatum.cropVector.at(2) + 1;
	int recordedPixels = recordedRows*(eventMetadatum.cropVector.at(3) + 1);
	int binSize;
	if (!STI::Utils::stringToValue(cameraState->binSize.get(), binSize))
		throw CameraException("Error transforming string to value");


	//Based off the 6303 data sheet
	double refreshTime = 0;
	refreshTime += recordedRows*parallelShiftTimeNS;			//Time to shift relevant rows.
	refreshTime += recordedPixels*serialDiscardRateNS / binSize / binSize; //Time to record (and discard?) relevent pixels
	refreshTime += (totalPixels - recordedPixels)*serialDiscardRateNS; //Time to discard non-relevant pixels

	
	std::cerr << "Frame Rate (s): " << refreshTime/1000000000 << std::endl; 
	return refreshTime;
}
void QuantixCamera::printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}
void QuantixCamera::checkQuantixStatus()
{
	int16 status;
	uns32 byteCount;
	pl_exp_check_status(cameraHandle, &status, &byteCount);

	checkQuantixStatus(status);

}
void QuantixCamera::checkQuantixStatus(int16 status)
{
	switch(status)
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
}