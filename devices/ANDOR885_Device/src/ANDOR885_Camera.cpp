//MUST INCLUDE ATMCD32M.lib among linker files. Keep this lib files and the atmcd32d.h files in the same directory as these files


/*! \file ANDOR885_Camera.cpp
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

#include "ANDOR885_Camera.h"


ANDOR885_Camera::ANDOR885_Camera()
{
	debugging = false;

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
	readMode_t.name = "Read mode"; //If ever there is more than one read mode, be sure to properly initialize this for playing events!
	readMode_t.choices[READMODE_IMAGE] = "Image";

	shutterMode_t.name = "Shutter mode";
	shutterMode_t.choices[SHUTTERMODE_AUTO] = "Auto";
	shutterMode_t.choices[SHUTTERMODE_OPEN] = "Open";
	shutterMode_t.choices[SHUTTERMODE_CLOSE] = "Closed";

	acquisitionMode_t.name = "**Acquisition mode (RTA)";
	acquisitionMode_t.choices[ACQMODE_SINGLE_SCAN] = "Single scan";
	acquisitionMode_t.choices[ACQMODE_KINETIC_SERIES] = "Kinetic series";
	acquisitionMode_t.choices[ACQMODE_RUN_TILL_ABORT] = "Run 'til abort";

	triggerMode_t.name = "**Trigger mode (EE)";
	triggerMode_t.choices[TRIGGERMODE_EXTERNAL] = "External";
	triggerMode_t.choices[TRIGGERMODE_EXTERNAL_EXPOSURE] = "External exposure";
	triggerMode_t.choices[TRIGGERMODE_INTERNAL] = "Internal";

	preAmpGain_t.name = "*Preamp Gain";
//	preAmpGain_t.choices.push_back("");
//	preAmpGain_t.choiceFlags.push_back(PREAMP_BLANK);
	preAmpGain = PREAMP_BLANK;
//	preAmpGainPos = PREAMP_BLANK;

	verticalShiftSpeed_t.name = "*Vertical Shift Speed (us/px)";
	verticalClockVoltage_t.name = "*Vertical Clock Voltage";
	horizontalShiftSpeed_t.name = "*Horizontal Shift Speed (MHz)";

//	pImageArray = NULL;

	cameraStat		=	ANDOR_ON;
	acquisitionMode	=	ACQMODE_RUN_TILL_ABORT;
	readMode		=	READMODE_IMAGE;
	exposureTime	=	(float) 0.05; // in seconds
	accumulateTime	=	0;
	kineticTime		=	0;
	ttl				=	TTL_OPEN_HIGH;
	shutterMode		=	SHUTTERMODE_OPEN;
	closeTime		=	SHUTTER_CLOSE_TIME;
	openTime		=	SHUTTER_OPEN_TIME;
	triggerMode		=	TRIGGERMODE_EXTERNAL_EXPOSURE;
	frameTransfer	=	ANDOR_OFF;
//	spoolMode		=	ANDOR_OFF;				
	coolerSetpt		=  -50;
	coolerStat		=	ANDOR_ON;
	cameraTemp		=	20;

	verticalShiftSpeed = 0;
	verticalClockVoltage = 0;
	horizontalShiftSpeed = 0;

	readMode_t.initial = readMode_t.choices.find(readMode)->second;
	shutterMode_t.initial = shutterMode_t.choices.find(shutterMode)->second;
	triggerMode_t.initial = triggerMode_t.choices.find(triggerMode)->second;
	acquisitionMode_t.initial = acquisitionMode_t.choices.find(acquisitionMode)->second;

	//Name of path to which files should be saved
	filePath		=	"\\\\atomsrv1\\EP\\Data\\epdata1\\";
	logPath			=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\";
	palPath			=	"C:\\Documents and Settings\\User\\My Documents\\My Pictures\\Andor_iXon\\GREY.PAL";

	initialized = !InitializeCamera();

	if (initialized){
		notDestructed = true;

		omni_thread::create(playCameraWrapper, (void*) this, omni_thread::PRIORITY_HIGH);
	
		if (debugging) {
			try {
				setAcquisitionMode(acquisitionMode);
				setTriggerMode(triggerMode);
				setReadMode(readMode);
				setExposureTime(exposureTime);
			} catch (ANDOR885_Exception& e){
				std::cerr << e.printMessage() << std::endl;
				initialized = false;
			}
		} 
	}
}

ANDOR885_Camera::~ANDOR885_Camera()
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
}

void ANDOR885_Camera::playCameraWrapper(void* object)
{
	ANDOR885_Camera* thisObject = static_cast<ANDOR885_Camera*>(object);
	thisObject->playCamera();
}
void ANDOR885_Camera::playCamera(){

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
/*				eventStatMutex->lock();
					if (eventStat == ANDOR_OFF) {
						break;
					}
				eventStatMutex->unlock();
*/				

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

/*
			eventStatMutex->lock();
				eventStat = ANDOR_OFF;
				pauseCleanupCondition->signal();
			eventStatMutex->unlock();
*/
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
					imageWriter.imageVector.push_back(image);
				}
				
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
void ANDOR885_Camera::cropImageData(std::vector <unsigned short> &imageData, std::vector <WORD> & tempImageVector, int imageIndex, std::vector <int> cropVector)
{
	long fullImageSize = imageWidth*imageHeight;
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
		std::cout << "Crop Height: " << cropHeight << std::endl;
		std::cout << "Crop Width: " << cropWidth << std::endl;
		for (i = 0; i < cropHeight; i++)
		{
			//std::cout << i << std::endl;
			for (j = 0; j < cropWidth; j++)
			{
				//std::cout << j << std::endl;
				//std::cout << j + cropStartX + imageWidth * (cropStartY + i) + imageIndex * fullImageSize << std::endl;
				imageData.push_back(tempImageVector.at(j + cropStartX + imageWidth * (cropStartY + i) + imageIndex * fullImageSize));
			}
		}
	}
	else {
		std::cout << "ImageIndex: " << imageIndex << std::endl;
		std::cout << "No crop" << std::endl;
		imageData.assign(tempImageVector.begin() + imageIndex*fullImageSize, tempImageVector.begin() + (imageIndex + 1)*fullImageSize);
	}
}
std::string ANDOR885_Camera::timeStampFilename(std::string fn)
{
	if (fn.compare("")==0)
	{
		return (timeStamp);
	} else
	{
		return (timeStamp + " " + fn);
	}
}

void ANDOR885_Camera::EventMetadatum::assign(double e, std::string d, std::string f) 
{
	exposureTime = e; 
	description = d; 
	filename = f; 
	cropVector.clear();
}
void ANDOR885_Camera::EventMetadatum::assign(double e, std::string d, std::string f, std::vector <int> cV) 
{
	exposureTime = e; 
	description = d; 
	filename = f; 
	cropVector = cV;
}
void ANDOR885_Camera::setupEventAcquisition(std::vector <EventMetadatum> *eM)
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

void ANDOR885_Camera::cleanupEventAcquisition()
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

}
bool ANDOR885_Camera::AbortIfAcquiring()
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
			std::cerr << "ANDOR885_Camera: Error aborting acquisition" << std::endl;
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
}
bool ANDOR885_Camera::deviceExit()
{
	int errorValue;
	bool error = false;
	int temp;

	//Stop Acquisition; ignore errors
	AbortIfAcquiring();

    //CloseShutter
	shutterMode = SHUTTERMODE_CLOSE;
	errorValue = SetShutter(ttl,shutterMode,closeTime,openTime);
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
				coolerStat = ANDOR_OFF;
			}
		}

	}

	errorValue = ShutDown();
	printError(errorValue, "Error shutting down", &error, ANDOR_ERROR);

	std::cerr << "Shutting down..." << std::endl;

	initialized = false;
	return error;
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
bool ANDOR885_Camera::InitializeCamera()
{
	AndorCapabilities	caps;
	char 				aBuffer[256];
	int					errorValue;
	bool				errorFlag = false;
//	int 				test,test2; //need to pause while camera initializes

	float				speed, STemp, gain;
	int					iSpeed, nAD, nAmp, nPreAmp, index, IsPreAmpAvailable;

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
    errorValue=GetDetector(&imageWidth,&imageHeight);
	printError(errorValue, "Get Detector information Error", &errorFlag, ANDOR_ERROR);

	// Set frame transfer mode
	errorValue=SetFrameTransferMode((frameTransfer == ANDOR_ON) ? 1 : 0);
	printError(errorValue, "Set Frame Transfer Mode Error", &errorFlag, ANDOR_ERROR);

    // Set acquisition mode to required setting specified in xxxxWndw.c
    errorValue=SetAcquisitionMode(acquisitionMode);
	printError(errorValue, "Set Acquisition Mode Error", &errorFlag, ANDOR_ERROR);

	if(readMode == READMODE_IMAGE) {
    	// This function only needs to be called when acquiring an image. It sets
		// the horizontal and vertical binning and the area of the image to be
		// captured. In this example it is set to 1x1 binning and is acquiring the
		// whole image
  		SetImage(1,1,1,imageWidth,1,imageHeight);
	}

    // Set read mode to required setting specified in xxxxWndw.c
    errorValue=SetReadMode(readMode);
	printError(errorValue, "Set Read Mode Error", &errorFlag, ANDOR_ERROR);

    // Set Vertical speed to max
/*    STemp = 0;
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
*/
	
	if (!notDestructed){
		STemp = 0;
		GetNumberVSSpeeds(&index);
		for(iSpeed=0; iSpeed < index; iSpeed++){
			GetVSSpeed(iSpeed, &speed);
			verticalShiftSpeed_t.choices[iSpeed] = STI::Utils::valueToString(speed);
			if(speed > STemp){
				STemp = speed;
				verticalShiftSpeed = iSpeed;
			}
		}
		verticalShiftSpeed_t.initial = verticalShiftSpeed_t.choices.begin()->second;
	}
    errorValue = SetVSSpeed(verticalShiftSpeed);
	printError(errorValue, "Set Vertical Speed Error", &errorFlag, ANDOR_ERROR);

	/* Set Vertical Clock Voltage; 
		note: only the fastest vertical shift speeds will benefit from the higher clock voltage;
			  increasing clock voltage adds noise.
	*/
	if (!notDestructed) {
		index = 0;
		GetNumberVSAmplitudes(&index);
		for (i = 0; i < index; i++){
			if (i == 0){
				verticalClockVoltage_t.choices[i] = "Normal";
			} else {
				verticalClockVoltage_t.choices[i] = STI::Utils::valueToString(i);
			}
		}
		verticalClockVoltage_t.initial = verticalClockVoltage_t.choices.begin()->second;
	}
	errorValue = SetVSAmplitude(0);
	printError(errorValue, "Set Vertical Clock Voltage Error", &errorFlag, ANDOR_ERROR);

    // Set Horizontal Speed to max 
	//(scan over all possible AD channels; although, the 885 has only one 14-bit channel)
		STemp = 0;
	//    HSnumber = 0;
		ADnumber = 0;
	if (!notDestructed) {
		errorValue = GetNumberADChannels(&nAD);
		if (errorValue != DRV_SUCCESS){
		  std::cerr << "Get number AD Channel Error\n";
		  errorFlag = true;
		}
		else if (nAD != 1) {
			std::cerr << "Expect 1 AD channel for this camera. The following code will miss channels\n";
			errorFlag = true;
		}
		else {
			GetNumberHSSpeeds(0, 0, &index);
			for (iSpeed = 0; iSpeed < index; iSpeed++) {
			  GetHSSpeed(0, 0, iSpeed, &speed);
			  horizontalShiftSpeed_t.choices[iSpeed] = STI::Utils::valueToString(speed);
			  if(speed > STemp){
				STemp = speed;
				horizontalShiftSpeed = iSpeed;
			  }
			}
			horizontalShiftSpeed_t.initial = horizontalShiftSpeed_t.choices.find(horizontalShiftSpeed)->second;
		}

		errorValue = GetNumberAmp(&nAmp);
		printError(errorValue, "Get Number Amplifiers Error", &errorFlag, ANDOR_ERROR);

		errorValue = GetNumberPreAmpGains(&nPreAmp);
		printError(errorValue, "Get Number Preamplifiers Error", &errorFlag, ANDOR_ERROR);

		if (nAmp == 1 && nAD == 1) {
			for (i = 0; i < nPreAmp; i++) {
				errorValue = GetPreAmpGain(i, &gain);
				errorValue = IsPreAmpGainAvailable(0,0,horizontalShiftSpeed,i,&IsPreAmpAvailable);
				if (IsPreAmpAvailable == 1) {
					preAmpGain_t.choices[i] = STI::Utils::valueToString(gain);
				}
			}
			if (!preAmpGain_t.choices.empty()) {
				preAmpGain = preAmpGain_t.choices.begin()->first;
				//preAmpGainPos = 0;
				preAmpGain_t.initial = preAmpGain_t.choices.begin()->second;
				errorValue = SetPreAmpGain(preAmpGain);
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
	}
	else {
		errorValue = SetPreAmpGain(preAmpGain);
		printError(errorValue, "Set AD Channel Error", &errorFlag, ANDOR_ERROR);
	}
	

    errorValue=SetADChannel(ADnumber);
	printError(errorValue, "Set AD Channel Error", &errorFlag, ANDOR_ERROR);


    errorValue=SetHSSpeed(0,horizontalShiftSpeed);
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

		errorValue = SetTemperature(coolerSetpt);
		printError(errorValue, "Error setting cooler temperature", &errorFlag, ANDOR_ERROR);

		int i;
		errorValue = IsCoolerOn(&i);
		if (i == 0) {
			// if it's off and it's supposed to be on, turn it on
			if (coolerStat == ANDOR_ON) {
				std::cerr << "Turning on cooler." << std::endl;
				errorValue = CoolerON();
				printError(errorValue, "Error turning on cooler", &errorFlag, ANDOR_ERROR);
			}
			
		} else if (i == 1) {
			std::cerr << "Cooler is on." << std::endl;
			//if it's on and it's supposed to be off, turn it off
			if (coolerStat == ANDOR_OFF)
			{
				errorValue = CoolerOFF();
				printError(errorValue, "Error turning off cooler", &errorFlag, ANDOR_ERROR);
			} else {
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
		}
		
		
		if(!errorFlag){
			std::cerr << "Cooler temperature set to: " << coolerSetpt << std::endl;
		}
	
	}


	errorValue = SetSpool(0,0,NULL,10);  //Disable spooling
	printError(errorValue, "Spool mode error", &errorFlag, ANDOR_ERROR);
	std::cerr << "Spooling Disabled" << std::endl;

	// Returns the value from PostQuitMessage
	return errorFlag;
}



void ANDOR885_Camera::throwError(int errorValue, std::string errorMsg) throw(std::exception)
{
	switch(errorValue)
	{
	case DRV_SUCCESS:
		break;
	case DRV_ACQUIRING:
		throw ANDOR885_Exception(errorMsg + ", perhaps because the camera is acquiring");
		break;
	case DRV_NOT_INITIALIZED:
		throw ANDOR885_Exception(errorMsg + ", perhaps because the camera is not initialized");
		break;
	default:
		throw ANDOR885_Exception(errorMsg + ". ANDOR error code is " + STI::Utils::valueToString(errorValue));
		break;
	}
}




void ANDOR885_Camera::printError(int errorValue, std::string errorMsg, bool *success, int flag)
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

// Finds the string associated with the flag in the toggleAttribute class
int ANDOR885_Camera::AndorAttribute::inverseFind(std::string value)
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
}

// Makes the string of attribute choices for addAttribute 
std::string ANDOR885_Camera::AndorAttribute::makeAttributeString()
{
	std::string tempString = "";
	std::string filler = ", ";
	std::map<int,std::string>::iterator iter;

	for(iter = choices.begin(); iter != choices.end(); iter++){
		tempString += iter->second;
		iter++;
		if (iter != choices.end()){
			tempString += filler;
		}
		iter--;
	}

	return tempString;
}
bool ANDOR885_Camera::getCameraData(int *numAcquired_p, int numExposures, std::vector <WORD>& tempImageVector)
{
	int errorValue = DRV_SUCCESS;
	bool error = false;
	long first = 1;
	long last = 1;
	long imageSize = imageWidth*imageHeight;
	long validFirst = 1;
	long validLast = 1;
	int tempAcq = 0;
	int excess = 0;

	errorValue = GetNumberNewImages(&first, &last);
	if (errorValue != DRV_NO_NEW_DATA)
		printError(errorValue, "Error acquiring number of new images", &error, ANDOR_ERROR);

	if (!error && errorValue != DRV_NO_NEW_DATA){
		if(*numAcquired_p + last - first + 1 > numExposures) {
			excess = *numAcquired_p + last - first + 1 - numExposures;
			last -= excess;
			std::cerr << "More images acquired than expected number of exposures" << std::endl;
			std::cerr << "Ignored extra images" << std::cerr;
		}
		errorValue = GetImages16(first, last, &tempImageVector[(*numAcquired_p)*imageSize], (last - first + 1)*imageSize, &validFirst, &validLast);
		printError(errorValue, "Error acquiring images", &error, ANDOR_ERROR);
		tempAcq = last - first + 1;
		*numAcquired_p += tempAcq;
	}

	return (error);
}
#ifndef _DEBUG
void ANDOR885_Camera::setMetadata(ImageMagick::MyImage &image)
{
	ImageMagick::Metadatum metadatum;

	image.metadata.clear();

	metadatum.tag = "Exposure Time";
	metadatum.value = STI::Utils::valueToString(exposureTime);
	image.metadata.push_back(metadatum);

	setCommonMetadata(image);
}

void ANDOR885_Camera::setMetadata(ImageMagick::MyImage &image, EventMetadatum &eventMetadatum)
{
	ImageMagick::Metadatum metadatum;

	image.metadata.clear();

	metadatum.tag = "Exposure Time";
	metadatum.value = STI::Utils::valueToString(eventMetadatum.exposureTime);
	image.metadata.push_back(metadatum);

	metadatum.tag = "Description";
	metadatum.value = eventMetadatum.description;
	image.metadata.push_back(metadatum);

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
	image.metadata.push_back(metadatum);

	setCommonMetadata(image);
}

void ANDOR885_Camera::setCommonMetadata(ImageMagick::MyImage &image)
{
	ImageMagick::Metadatum metadatum;

	metadatum.tag = "Vertical Shift Speed";
	metadatum.value = verticalShiftSpeed_t.choices.find(verticalShiftSpeed)->second;
	image.metadata.push_back(metadatum);

	metadatum.tag = "Vertical Clock Voltage";
	metadatum.value = verticalClockVoltage_t.choices.find(verticalClockVoltage)->second;
	image.metadata.push_back(metadatum);

	metadatum.tag = "Horizontal Shift Speed";
	metadatum.value = horizontalShiftSpeed_t.choices.find(horizontalShiftSpeed)->second;
	image.metadata.push_back(metadatum);

	metadatum.tag = "Camera Temp";
	metadatum.value = STI::Utils::valueToString(cameraTemp);
	image.metadata.push_back(metadatum);

	metadatum.tag = "PreAmp Gain";
	metadatum.value = preAmpGain_t.choices.find(preAmpGain)->second;
	image.metadata.push_back(metadatum);
}

#endif

bool ANDOR885_Camera::startAcquisition()
{
	int errorValue;
	bool success = true;


	// Check to see if camera is idle before trying to turn on acquisition
	GetStatus(&errorValue);

	if (errorValue != DRV_IDLE) {
	
		std::cerr << "Acquisition still running" << std::endl;
		success = false;
	
	}
	else {
		if (acquisitionMode == ACQMODE_KINETIC_SERIES) {
			//it is assumed that the triggermode is a flavor of external, as required in setAcquisitionMode and setTriggerMode
			// KINETIC SERIES ACQUISITION MODE IS NOT SUITABLE FOR PLAYING EVENTS. Probably should get rid of the option.
			SetNumberAccumulations(1);
			if (eventMetadata == NULL) {
				SetNumberKinetics(1);
			}
			else {
				SetNumberKinetics(eventMetadata->size());
			}
		}

		// Start acquisition
		errorValue = StartAcquisition();
		printError(errorValue,"Error starting acquisition", &success, ANDOR_SUCCESS); 

	} 

	return success;

}

//Get and Set Functions
int	ANDOR885_Camera::getImageWidth()
{
	return imageWidth;
}
int	ANDOR885_Camera::getImageHeight()
{
	return imageHeight;
}
std::string	ANDOR885_Camera::getFilePath()
{
	return filePath;
}

void	ANDOR885_Camera::setFilePath(std::string path)
{
	filePath = path;
}
char *	ANDOR885_Camera::getPalPath()
{
	return palPath;
}

int	ANDOR885_Camera::getCameraStat()
{
	return cameraStat;
}
void ANDOR885_Camera::setCameraStat(int cStat) throw(std::exception)
{
	bool error;

	if (cStat == ANDOR_ON){

		// Turn on camera, if it's not already on
		if (!initialized){
//			std::cerr << "ANDOR885_Camera: Initializing..." << std::endl;
			initialized = !InitializeCamera();
		}

		if (!initialized){
			throw ANDOR885_Exception("Error initializing camera");
		} else {
//			std::cerr << "ANDOR885_Camera: Camera on" << std::endl;
			cameraStat = ANDOR_ON;
		}
	} 
	else if (cStat == ANDOR_OFF) {

		// Turn off camera, if it's not already off.
		if (initialized) {
			error = deviceExit();
		}

		if (error) {
			throw ANDOR885_Exception("Error shutting down camera");
		} else {
		//	std::cerr << "ANDOR885_Camera: Camera off" << std::endl;
			cameraStat = ANDOR_OFF;
		}
	} 
	else {
		throw ANDOR885_Exception("Unrecognized camera status requested");
	}
}

int	ANDOR885_Camera::getAcquisitionMode()
{
	return acquisitionMode;
}
void ANDOR885_Camera::setAcquisitionMode(int aMode) throw(std::exception)
{
	int errorValue;

	// Kinetic series can only be used with external acquisition modes
	if (aMode == ACQMODE_KINETIC_SERIES && triggerMode == TRIGGERMODE_INTERNAL)
	{
		setTriggerMode(TRIGGERMODE_EXTERNAL_EXPOSURE);
	}


	errorValue = SetAcquisitionMode(aMode);
	throwError(errorValue, "Error setting acquisition mode");
	acquisitionMode = aMode;

}
int	ANDOR885_Camera::getReadMode()
{
	return readMode;
}
void ANDOR885_Camera::setReadMode(int rMode) throw(std::exception)
{
	int errorValue;
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
	if(rMode == READMODE_IMAGE) {
		if(readMode != READMODE_IMAGE){

			errorValue=SetReadMode(rMode);
			throwError(errorValue, "Error setting Read Mode");
			readMode = READMODE_IMAGE;

			SetImage(1,1,1,imageWidth,1,imageHeight);
		}
	}
	else {
		throw ANDOR885_Exception("Unrecognized Read Mode requested");
	}
}
float ANDOR885_Camera::getExposureTime()
{
	return exposureTime;
}
void ANDOR885_Camera::setExposureTime(float expTime) throw(std::exception)
{
	int errorValue;

	errorValue = SetExposureTime(expTime);
	throwError(errorValue, "Error setting Exposure Time");

	exposureTime = expTime;

	// It is necessary to get the actual times as the system will calculate the
	// nearest possible time. eg if you set exposure time to be 0, the system
	// will use the closest value (around 10 us in FrameTransfer mode)
	// If in external exposure triggermode, the exposure time will be set to 10 us
	GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
	if (triggerMode == TRIGGERMODE_EXTERNAL_EXPOSURE)
	{
		exposureTime = expTime;
	}
}
float ANDOR885_Camera::getKineticTime()
{
	return kineticTime;
}
int	ANDOR885_Camera::getShutterMode()
{
	return shutterMode;
}
void ANDOR885_Camera::setShutterMode(int mode) throw(std::exception)
{
	int errorValue;

	errorValue = SetShutter(ttl, mode, closeTime, openTime);
	throwError(errorValue, "Error setting Shutter Mode");

	shutterMode = mode;
}
int	ANDOR885_Camera::getCloseTime()
{
	return closeTime;
}
void ANDOR885_Camera::setCloseTime(int time) throw(std::exception)
{
	int errorValue;

	if (time < 1) {
		throw ANDOR885_Exception("Error: Shutter closing time minimum: 1 ms");
	}

	errorValue = SetShutter(ttl, shutterMode, time, openTime);
	throwError(errorValue, "Error setting Shutter Close Time");

	closeTime = time;
}
int	ANDOR885_Camera::getOpenTime()
{
	return openTime;
}
void ANDOR885_Camera::setOpenTime(int time) throw(std::exception)
{
int errorValue;

	if (time < 1) {
		throw ANDOR885_Exception("Error: Shutter opening time minimum: 1 ms");
	}

	errorValue = SetShutter(ttl, shutterMode, closeTime, time);
	throwError(errorValue, "Error setting Shutter Open Time");

	openTime = time;
}
int	ANDOR885_Camera::getTriggerMode()
{
	return triggerMode;
}
void ANDOR885_Camera::setTriggerMode(int mode) throw(std::exception)
{
	int errorValue;

	// Kinetic series can only be used with external acquisition modes
	if (mode == TRIGGERMODE_INTERNAL && acquisitionMode == ACQMODE_KINETIC_SERIES)
	{
		setAcquisitionMode(ACQMODE_RUN_TILL_ABORT);
	}

	errorValue = SetTriggerMode(mode);
	throwError(errorValue, "Error in setting Trigger Mode");

	triggerMode = mode;
}
int	ANDOR885_Camera::getCoolerSetpt()
{
	return coolerSetpt;
}
void ANDOR885_Camera::setCoolerSetpt(int setpt) throw(std::exception)
{
	int errorValue;

	if (setpt > maxTemp || setpt < minTemp) {
		throw ANDOR885_Exception("Chosen temperature out of range.\n Temperature must be between " 
			+ STI::Utils::valueToString(minTemp) + " and " + STI::Utils::valueToString(maxTemp));
	}

	errorValue = SetTemperature(coolerSetpt);
	throwError(errorValue, "Error setting cooler temperature");
		
	coolerSetpt = setpt;
}
int	ANDOR885_Camera::getCoolerStat()
{
	return coolerStat;
}
void ANDOR885_Camera::setCoolerStat(int stat) throw(std::exception)
{
	int errorValue;

	if (stat == ANDOR_ON) {
		if (coolerStat != ANDOR_ON) {
			errorValue = SetTemperature(coolerSetpt);
			throwError(errorValue, "Error setting cooler temperature");
			
			errorValue = CoolerON();
			throwError(errorValue, "Error turning on cooler");
			
			coolerStat = ANDOR_ON;
		}
	} 
	else if (stat == ANDOR_OFF) {
		if (coolerStat != ANDOR_OFF) {
			errorValue = CoolerOFF();
			throwError(errorValue, "Error turning off cooler");
			
			coolerStat = ANDOR_OFF;
		}
	}
	else {
		throw ANDOR885_Exception("Unrecognized Cooler Status requested");
	}
}
int	ANDOR885_Camera::getCameraTemp() throw(std::exception)
{
	int temp = -999;
	int errorValue;

	errorValue = GetTemperature(&temp);
	switch(errorValue){
		case DRV_SUCCESS:
		case DRV_ACQUIRING:
		case DRV_TEMP_STABILIZED:
		case DRV_TEMP_NOT_REACHED:
		case DRV_TEMP_NOT_STABILIZED:
		case DRV_TEMP_OFF:
			cameraTemp = temp;
			break;
		case DRV_TEMP_DRIFT:
			cameraTemp = temp;
			std::cerr << "Cooler temp is " << temp << " deg C" << std::endl;
			std::cerr << "Cooler temperature has drifted. Try resetting setpoint" << std::endl;
			break;
		default:
			throw ANDOR885_Exception("Error getting temperature: ANDOR error code is " 
			+ STI::Utils::valueToString(errorValue));
			break;
	}
	return cameraTemp;
}
int	ANDOR885_Camera::getPreAmpGain()
{
	return preAmpGain;
}
void ANDOR885_Camera::setPreAmpGain(int gainIndex) throw(std::exception)
{
	int errorValue;

	errorValue = SetPreAmpGain(gainIndex);
	throwError(errorValue, "Error setting PreAmp Gain");

	preAmpGain = gainIndex;
}

int	ANDOR885_Camera::getVerticalShiftSpeed()
{
	return verticalShiftSpeed;
}
void ANDOR885_Camera::setVerticalShiftSpeed(int speedIndex)
{
	int errorValue;
	float expTime = exposureTime;

	errorValue = SetVSSpeed(speedIndex);
	throwError(errorValue, "Error setting Vertical Shift Speed");

	verticalShiftSpeed = speedIndex;

	//Recalculate kineticTime and exposureTime, if not in External Exposure trigger mode
	if(triggerMode == TRIGGERMODE_EXTERNAL_EXPOSURE) {
		GetAcquisitionTimings(&expTime,&accumulateTime,&kineticTime);
	} else {
		GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
	}
}
int	ANDOR885_Camera::getVerticalClockVoltage()
{
	return verticalClockVoltage;
}
void ANDOR885_Camera::setVerticalClockVoltage(int voltageIndex) throw(std::exception)
{
	int errorValue;

	errorValue = SetVSAmplitude(voltageIndex);
	throwError(errorValue, "Error setting Vertical Clock Voltage");

	verticalClockVoltage = voltageIndex;
}
int	ANDOR885_Camera::getHorizontalShiftSpeed()
{
	return horizontalShiftSpeed;
}
void ANDOR885_Camera::setHorizontalShiftSpeed(int speedIndex) throw(std::exception)
{
	int errorValue;
	float expTime = exposureTime;

	errorValue = SetHSSpeed(0, speedIndex);
	throwError(errorValue, "Error setting Horizontal Shift Speed");

	horizontalShiftSpeed = speedIndex;

	//Recalculate kineticTime and exposureTime, if not in External Exposure trigger mode
	if(triggerMode == TRIGGERMODE_EXTERNAL_EXPOSURE) {
		GetAcquisitionTimings(&expTime,&accumulateTime,&kineticTime);
	} else {
		GetAcquisitionTimings(&exposureTime,&accumulateTime,&kineticTime);
	}
}