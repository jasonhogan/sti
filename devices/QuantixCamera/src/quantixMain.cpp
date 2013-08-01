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
	//string ipAddress = "epMezzanine1.stanford.edu";
	string ipAddress = "epdesktop2.stanford.edu";

	// for cameras
	int16 totalCameras = 0;
	char camName[CAM_NAME_LEN];
	std::string camNameStr;
	std::vector <std::string> cameraNames;
	std::vector <int16> cameraHandles;  
	int16 cameraHandle;
	rs_bool success = TRUE;
	bool initialized = false;
	
	QuantixDevice *quantixDevice;
	std::vector<QuantixDevice *> quantixDevices;
	try {
		success = pl_pvcam_init();
		if (!success)
			throw stringException("Could not initialize Quantix drivers");

		//Get the number of cameras and camera names
		pl_cam_get_total(&totalCameras);
		totalCameras = 1;
		for (int i = 0; i < totalCameras; i++) 
		{
			if(!pl_cam_get_name(i, camName))
				throw stringException("Could not get name of Quantix Camera " + STI::Utils::valueToString(i));
			else
				camNameStr = camName;
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

			quantixDevice = new QuantixDevice(orbManager, camNameStr, ipAddress, i, cameraHandle, "C:\\code\\sti\\devices\\QuantixCamera\\src\\quantix.ini");
			//quantixDevice = new QuantixDevice(orbManager, camNameStr, ipAddress, i, cameraHandle, "quantix.ini");
			initialized |= quantixDevice->initialized;
			if (quantixDevice->initialized)
				quantixDevices.push_back(quantixDevice);
			else 
				delete quantixDevice;
			
		}
	
		if (initialized) 
		{
			for (unsigned int i = 0; i < quantixDevices.size(); i++)
			{
				quantixDevices.at(i)->setSaveAttributesToFile(true);
			}

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

	for(unsigned int i = 0; i < quantixDevices.size(); i++)
	{
		delete quantixDevices.at(i);
	}

	//Shutdown cameras
	if (!cameraHandles.empty())
	{
		for (int i = 0; i < cameraHandles.size(); i++) 
		{
			if (pl_cam_check(cameraHandles.at(i)))
				pl_cam_close(cameraHandles.at(i));
		}
	}

    pl_pvcam_uninit();


	return 0;
}

void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}

/*
omni_mutex *testCameraMutex;
omni_condition *testCameraCondition;

int16 cameraHandle;

class PictureInfo
{
public:
	double exposureTime;
	std::string description;
	std::string filename;
	std::vector <int> cropVector;
} pictureInfo;

void printError();
void prepareToTakePictures() throw (std::exception);
void takePictures();

void AbortIfAcquiring();
double getMinExposureTime();
double getFrameRefreshTime();
void setupCameraAcquisition(std::vector <PictureInfo> *eM);
void waitForImage();
void getImage(PictureInfo &eventMetadatum);
void saveImages();
std::string	getTemperature(int16 cameraHandle);
void checkQuantixStatus();
void checkQuantixStatus(int16 status);
void checkCCSStatus();

int numberOfPicturesToBeTaken;
int binSize = 4;
int imageHeight = 1000;
int imageWidth = 1000;
bool isAcquiring = false;
bool stopEvent = false;
uns16 *imageBuffer;
ImageMagick imageWriter;

int main(int argc, char **argv)
{
	testCameraMutex = new omni_mutex();
	testCameraCondition = new omni_condition(testCameraMutex);

    char cam_name[CAM_NAME_LEN];    // camera name                    
    int16 hCam;                     // camera handle                  
    rs_bool avail_flag;             // ATTR_AVAIL, param is available 
	char a;

    // Initialize the PVCam Library and Open the First Camera 
    pl_pvcam_init();
    pl_cam_get_name( 0, cam_name );
    pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE );

	cameraHandle = hCam;
    // check for circular buffer support 
    if( pl_get_param( hCam, PARAM_CIRC_BUFFER, ATTR_AVAIL, &avail_flag ) &&
		avail_flag )
	{
		std::cerr << "Number of pictures to take?";
		std::cin >> numberOfPicturesToBeTaken;
		if (!pl_exp_init_seq())
			std::cerr << "Could not init Quantix for acquisition" << std::endl;
		else
		{
			while (numberOfPicturesToBeTaken > 0)
			{
				try{
					unsigned long waitS, waitNS;

					prepareToTakePictures();
					takePictures();
					//std::cerr << "Number of pictures to take?";
					//std::cin >> numberOfPicturesToBeTaken;
				//	testCameraMutex->lock();
				//		omni_thread::get_time(&waitS, &waitNS, 3, 0);
				//		testCameraCondition->timedwait(waitS,waitNS);
				//	testCameraMutex->unlock();
				}
				catch(CameraException &e)
				{
					std::cerr << e.what() << std::endl;
					printError();
					numberOfPicturesToBeTaken = -1;
				}

			}
		}
	}
    else
        printf( "circular buffers not supported\n" );

	AbortIfAcquiring();
    pl_cam_close( hCam );

    pl_pvcam_uninit();

	delete testCameraMutex;
	delete testCameraCondition;

    return 0;
}

void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}

// Get information from user and setup the acquisition
void prepareToTakePictures() throw (std::exception)
{
	double exposureTime;
	double timeBetweenPictures = 1500000000; //1.5 seconds

	//AbortIfAcquiring(); // finish the last acquisition

	//Sleep(1000);

	//if (isAcquiring)
//	{
//		if (!pl_exp_stop_cont(cameraHandle, CCS_HALT))
//		{
//			std::cerr << "Could not stop acquisition" << std::endl;
//			printError();
//		}
//		if(!pl_exp_finish_seq(cameraHandle, &imageBuffer, 0))
//		{
//			std::cerr << "Could not finish acquisition" << std::endl;
//			printError();
//		}
//		isAcquiring = false;
//	}

	pictureInfo.filename = "\\\\epsrv1\\ep\\data\\2012\\4\\4\\data\\mainPicture";
	pictureInfo.description = "pic";
	exposureTime = pictureInfo.exposureTime = 1000000; //in nanoseconds
	pictureInfo.cropVector.push_back(1);
	pictureInfo.cropVector.push_back(1);
	pictureInfo.cropVector.push_back(imageHeight);
	pictureInfo.cropVector.push_back(imageWidth);

	std::vector <PictureInfo> pictureInfos;
	pictureInfos.push_back(pictureInfo);

	double minExposureTime = getMinExposureTime(); //Check that the exposure time requested is reasonable
	if (exposureTime < minExposureTime)
		throw (CameraException("exposure time too short"));
	double frameRefreshTime = getFrameRefreshTime(); //Check that the time between pictures is not too short
	if (timeBetweenPictures < frameRefreshTime)
		throw (CameraException("Images taken too close to each other"));

	//Sleep(1000);
	setupCameraAcquisition(&pictureInfos); //Sets up acquisition in continuous mode
}

void takePictures()
{
	for (int i = 0; i < numberOfPicturesToBeTaken; i++)
	{
		waitForImage();
		getImage(pictureInfo);
	}

	saveImages();
	//checkCCSStatus();
}

//
//				Subfunctions 
//


//Stops acquisition if an acquisition is taking place
void AbortIfAcquiring()
{
//	checkQuantixStatus();
//	checkCCSStatus();
	Sleep(1000);
	if (isAcquiring)
	{
		if (!pl_exp_stop_cont(cameraHandle, CCS_HALT))
		{
			std::cerr << "Could not stop acquisition" << std::endl;
			printError();
		}
		if(!pl_exp_finish_seq(cameraHandle, &imageBuffer, 0))
		{
			std::cerr << "Could not finish acquisition" << std::endl;
			printError();
		}
		if (!pl_exp_uninit_seq())
		{
			std::cerr << "Could not uninit acquisition" << std::endl;
			printError();
		}
		isAcquiring = false;
	}
	else
	{
		std::cerr << "Not Acquiring" << std::endl;
	}
	return;
}

//Get the minimum exposure time the camera can support
double getMinExposureTime()
{
	//Best Case Scenario: get minimum exposure time from camera
	//Typical behavior for this camera: PARAM_EXP_MIN_TIME is not 
	//    available and minExpTimeS gets set to 0
	flt64 minExpTimeS;

	if(!pl_get_param(cameraHandle, PARAM_EXP_MIN_TIME, ATTR_CURRENT, (void *)&minExpTimeS))
	{
			minExpTimeS = 0;
	}

	return (double) (1000000000 * minExpTimeS);
}

//Get the minimum time between frames that the camera can support
double getFrameRefreshTime()
{
	uns16 pixelTimeNS;
	//Get the pixel readout time
	if(!pl_get_param(cameraHandle, PARAM_PIX_TIME, ATTR_CURRENT, (void *) &pixelTimeNS))
	{
		char msg[ERROR_MSG_LEN];
		pl_error_message(pl_error_code(), msg);
		std::cerr << "Pixel readout time error: " << msg << std::endl;
		throw CameraException("Error getting pixel readout time.");
	}

	//Estimates frame rate by calculating the time it would take
	//to read each pixel, multiplied by the total number of binned pixels
	//As I'm writing this comment, I see that I really should divide
	//by binSize again. Oops.
	return pixelTimeNS*(imageHeight)*(imageWidth)/binSize; //binSize is a global variable
}

//Sets up camera to take pictures given the region size and binning requested
void setupCameraAcquisition(std::vector <PictureInfo> *eM)
{
	uns16 s1, p1, s2, p2, sBin, pBin;

	//Check camera temperature for metadata purposes
	//   Does not immediately use result, but the value
	//   gets stored elsewhere
	//This DOES poll the camera. See subfunction at the end
	getTemperature(cameraHandle);
	
	p1 = eM->at(0).cropVector.at(0);
	s1 = eM->at(0).cropVector.at(1);
	p2 = p1 + eM->at(0).cropVector.at(2);
	s2 = s1 + eM->at(0).cropVector.at(3);

	pBin = sBin = binSize;

	rgn_type region = {s1, s2, sBin, p1, p2, pBin};

	//PSEUDO: This accesses a value stored when the
	//camera is initialized; it does not poll the camera
	uns16 currentTriggerMode = STROBED_MODE;
	//Almost invariably, this will be BULB_MODE

	//This gets ignored, since we're in BULB_MODE, but it's here for safety
	uns32 exposureTime = (uns32) (eM->at(0).exposureTime/1000000); 

	//Initialize the sequence
	uns32 bufferSize, frameSize = 1;
	//if (!pl_exp_init_seq())
	//	throw CameraException("Could not init Quantix for acquisition");
	if (!pl_exp_setup_cont(cameraHandle, 1, &region, currentTriggerMode, exposureTime, &frameSize, CIRC_OVERWRITE))
		throw CameraException("Could not setup Quantix for acquisition");

	//Prints out frame size and buffer size for error checking;
	std::cerr << "frame size " << cameraHandle << ": " << frameSize << std::endl;
	bufferSize = frameSize * numberOfPicturesToBeTaken;
	std::cerr << "Buffer Size " << cameraHandle << ": " << bufferSize << std::endl;

	//Allocate memory; I believe that I could reduce bufferSize by a factor of 2, since
	//    bufferSize assumes two bytes per pixel and "new" takes care of that, but I've
	//    left this in for the moment
	try
	{
		delete [] imageBuffer;
		imageBuffer = new uns16[bufferSize/2];
	}
	catch(...)
	{
		throw CameraException("Error allocating memory to image buffer");
	}

	//Start the acquisition in continuous mode
	if (!pl_exp_start_cont(cameraHandle, imageBuffer, bufferSize))
		throw CameraException("Could not start Quantix acquisition");

	//This just checks pl_exp_check_cont_status and reports the result
	checkQuantixStatus();

	isAcquiring = true;
}

//Wait for an image to be acquired; stop acquisition if an error has occurred.
void waitForImage()
{
	int16 status;
    uns32 not_needed;
	int i = 0;

	unsigned long waitS, waitNS;

	while(stopEvent == false)  //Check whether the user has interruped the acquisition
	{
		i++;
		if(!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
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
			//Sleep for half a second before checking again
			testCameraMutex->lock();
				omni_thread::get_time(&waitS, &waitNS, 0, 500000000);
				testCameraCondition->timedwait(waitS,waitNS);

			testCameraMutex->unlock();
			continue;
		}

	}
	
	stopEvent = true;
	return;

}
//Get an image that has been acquired. This ALWAYS occurs after 
//    waitForImage has returned with a positive response
void getImage(PictureInfo &eventMetadatum)
{

	long binnedHeight, binnedWidth;
	binnedHeight = imageHeight/binSize;
	binnedWidth = imageWidth/binSize;

	std::cerr << "Getting Image" << std::endl;

	long imageSize = binnedHeight * binnedWidth;
	ImageMagick::MyImage image;
	image.imageData.reserve(imageSize);

	//This just checks pl_exp_check_cont_status and reports the result
	checkQuantixStatus();
	Sleep(100);

	void * address;
	if (pl_exp_get_oldest_frame(cameraHandle, &address) && !stopEvent)
	{
		checkQuantixStatus();

		image.imageData.assign(((uns16*) address),((uns16*) address + imageSize));
		
		checkQuantixStatus();
		
		if (!pl_exp_unlock_oldest_frame(cameraHandle))
			printError();
		
		checkQuantixStatus();
		image.extension = ".tif";

		//Stores all images on a vector
		imageWriter.imageVector.push_back(image);
	}
	else
		stopEvent = true;

	return;
}
//Saves Images. Does not poll camera, but is here for completeness
void saveImages()
{

	if(!imageWriter.imageVector.empty() && !stopEvent)
	{
		//imagePreprocessor.processImages(imageWriter.imageVector, bitDepth);
					
		imageWriter.writeImageVector();
	}

	//This just checks pl_exp_check_cont_status and reports the result
	checkQuantixStatus();
}

//Gets the current camera temperature
std::string	getTemperature(int16 cameraHandle)
{
	//ADDED
	std::string currentValue;
	std::string currentLabel;

	rs_bool available = FALSE;
	int temp = -99900;

	int16 currentTemp;
	pl_get_param(cameraHandle, PARAM_TEMP, ATTR_AVAIL, (void *) &available);
	if (available){		
		pl_get_param(cameraHandle, PARAM_TEMP, ATTR_CURRENT, (void *) &currentTemp);
	}

	// Camera reports temperature in degrees C x 100
	// Convert the temperature to a string because the GUI uses strings
	currentValue = STI::Utils::valueToString(((double)currentTemp)/100);	

	currentLabel = currentValue;

	return currentValue;
}

void checkQuantixStatus()
{
	int16 status;
	uns32 byteCount, bufferCount;
	pl_exp_check_cont_status(cameraHandle, &status, &byteCount, &bufferCount);
	std::cerr << "Byte Count: " << byteCount << std::endl;
	std::cerr << "Buffer Count: " << bufferCount << std::endl;

	checkQuantixStatus(status);

}
void checkQuantixStatus(int16 status)
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

void checkCCSStatus()
{
	int16 status;

	if (!pl_get_param(cameraHandle, PARAM_CCS_STATUS, ATTR_CURRENT, (void *) &status))
		std::cerr << "Can't access CCS Status" << std::endl;

	switch (status)
	{
	case 0:
		std::cerr << "CCS idle" << std::endl;
		break;
	case 1:
		std::cerr << "CCS initializing" << std::endl;
		break;
	case 2:
		std::cerr << "CCS running" << std::endl;
		break;
	case 3:
		std::cerr << "CCS continuously clearing" << std::endl;
		break;
	default:
		std::cerr << "CCS status default: " << status << std::endl;
		break;
	}

}
*/



/*static void AcquireContinuous( int16 hCam );
static void AcquireContinuousWithWait( int16 hCam );
static void AcquireContinuousThreaded( int16 hCam );
void acquireContinuousWrapper(void* object);
void waitForImage();
void waitForImagesWrapper(void* object);
void waitForImages(uns32 frame_size);
void printError();

omni_mutex *testCameraMutex;
omni_condition *testCameraCondition;

int16 cameraHandle;

int main(int argc, char **argv)
{
	testCameraMutex = new omni_mutex();
	testCameraCondition = new omni_condition(testCameraMutex);

    char cam_name[CAM_NAME_LEN];    // camera name                    
    int16 hCam;                     // camera handle                  
    rs_bool avail_flag;             // ATTR_AVAIL, param is available 
	char a;

    // Initialize the PVCam Library and Open the First Camera 
    pl_pvcam_init();
    pl_cam_get_name( 0, cam_name );
    pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE );

	cameraHandle = hCam;
    // check for circular buffer support 
    if( pl_get_param( hCam, PARAM_CIRC_BUFFER, ATTR_AVAIL, &avail_flag ) &&
		avail_flag )
	{
        AcquireContinuous( hCam );
		
		//omni_thread::create(acquireContinuousWrapper, &hCam, omni_thread::PRIORITY_LOW);
        //AcquireContinuousThreaded( hCam );
		//std::cout << "Press any key to shut down" << std::endl;
		//std::cin >> a;
		
	}
    else
        printf( "circular buffers not supported\n" );

    pl_cam_close( hCam );

    pl_pvcam_uninit();

	delete testCameraMutex;
	delete testCameraCondition;

    return 0;
}

void acquireContinuousWrapper(void* object)
{
	int16 *hCam = static_cast<int16*>(object);
	AcquireContinuous(*hCam);
}

void AcquireContinuous( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
    int16 status;
    uns32 not_needed;
    void_ptr address;
    uns16 numberframes = 5;
	bool error = false;
	int i = 0;
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, TIMED_MODE, 10, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames 
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

	std::cerr << "Frame size: " << frame_size << std::endl;
    // Start the acquisition 
    printf( "Collecting %i Frames\n", numberframes );
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
    while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
         //                                &not_needed ) && 
         //      (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		while(!error)
		{
			if (!pl_exp_check_cont_status( hCam, &status, &not_needed, &not_needed ))
				error = true;

			// Check Error Codes 
			if( status == READOUT_FAILED ) {
				printf( "Data collection error: %i\n", pl_error_code() );
				error = true;
			}

			if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( hCam, &address )) {
				// address now points to valid data 
				printf( "Center Three Points: %i, %i, %i\n", 
						*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
						*((uns16*)address + frame_size/sizeof(uns16)/2),
						*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
				numberframes--;
				printf( "Remaining Frames %i\n", numberframes );
				pl_exp_unlock_oldest_frame( hCam );
				printf("Slept %i times \n", i);
				i = 0;
				break;
			}

			Sleep(500);
			i++;
		}
    } // End while 

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    //Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}

void AcquireContinuousWithWait( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
    int16 status;
    uns32 not_needed;
    void_ptr address;
    uns16 numberframes = 5;
	bool error = false;
	int i = 0;
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, TIMED_MODE, 100, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames 
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

    // Start the acquisition 
    printf( "Collecting %i Frames\n", numberframes );
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
    while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
        //                                 &not_needed ) && 
        //       (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		
		waitForImage();

		if (!pl_exp_check_cont_status( hCam, &status, &not_needed, &not_needed ))
			error = true;

		// Check Error Codes 
		if( status == READOUT_FAILED ) {
			printf( "Data collection error: %i\n", pl_error_code() );
			error = true;
		}

		if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( hCam, &address )) {
			// address now points to valid data 
			printf( "Center Three Points: %i, %i, %i\n", 
					*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
					*((uns16*)address + frame_size/sizeof(uns16)/2),
					*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
			numberframes--;
			printf( "Remaining Frames %i\n", numberframes );
			pl_exp_unlock_oldest_frame( hCam );
			printf("Slept %i times \n", i);
			i = 0;
			break;
		}

		Sleep(500);
		i++;
		
    } // End while 

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    //Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}

void AcquireContinuousThreaded( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
   
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, BULB_MODE, 100, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

    // Start the acquisition 
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
	omni_thread::create(waitForImagesWrapper, &frame_size, omni_thread::PRIORITY_LOW);
    
	//waitForImages();
	testCameraMutex->lock();
		testCameraCondition->wait();
	testCameraMutex->unlock();

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    // Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}
void waitForImage()
{
	bool stopEvent = false;
	int16 status;
    uns32 not_needed;
	int i = 0;

	//Wait for an image to be acquired; stop acquisition if an error has occurred.
	while(stopEvent == false)
	{
		i++;
		if(!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
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
			Sleep(500);
//			pauseCameraMutex->lock();
				//omni_thread::get_time(&waitS, &waitNS, 2, 0);
				//pauseCameraCondition->timedwait(waitS,waitNS);

			//pauseCameraMutex->unlock();
			continue;
		}

	}

	
	stopEvent = true;
	return;

}

void waitForImagesWrapper(void* object)
{
	uns32 *frame_size = static_cast<uns32*>(object);
	waitForImages(*frame_size);
}
void waitForImages(uns32 frame_size)
{
	bool error = false;
	uns16 numberframes = 5;

	int16 status;
    uns32 not_needed;
    void_ptr address;
	int i = 0;

    printf( "Collecting %i Frames\n", numberframes );

	while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
        //                                 &not_needed ) && 
        //       (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		
		waitForImage();

		if (!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
			error = true;

		// Check Error Codes 
		if( status == READOUT_FAILED ) {
			printf( "Data collection error: %i\n", pl_error_code() );
			error = true;
		}

		if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( cameraHandle, &address )) {
			// address now points to valid data 
			printf( "Center Three Points: %i, %i, %i\n", 
					*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
					*((uns16*)address + frame_size/sizeof(uns16)/2),
					*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
			numberframes--;
			printf( "Remaining Frames %i\n", numberframes );
			pl_exp_unlock_oldest_frame( cameraHandle );
			printf("Slept %i times \n", i);
			i = 0;
		}
		i++;
		
    } // End while 

	testCameraMutex->lock();
		testCameraCondition->signal();
	testCameraMutex->unlock();

}
void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}*/