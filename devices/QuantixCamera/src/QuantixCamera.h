#ifndef QUANTIX_CAMERA_H
#define QUANTIX_CAMERA_H

#include "device.h"

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <fstream>					// To redirect cerr to a logfile
#include <ImageMagick.h>
#include "ImagePreprocessor.h"
#include "VectorArithmetic.h"
#include "utils.h"
#include "master.h"
#include "pvcam.h"
#include "QuantixState.h"
#include "CameraException.h"

class QuantixCamera
{

public:

	QuantixCamera(int16 handle);
	virtual ~QuantixCamera();

	bool debugging;

	bool cameraExit();

	bool initialized;
	QuantixState *cameraState;

	void printError();
//	void throwError(int errorValue, std::string errorMsg) throw(std::exception);

	//Get & Set Functions
	std::string	getFilePath();
	void		setFilePath(std::string path);


protected:
	

	std::string		 logPath;

	ImagePreprocessor imagePreprocessor;
	
	omni_mutex* pauseCameraMutex;
	omni_condition* pauseCameraCondition;

	omni_mutex* stopEventMutex;
	omni_condition* stopEventCondition;
	bool stopEvent;

	omni_mutex* waitForEndOfAcquisitionMutex;
	omni_condition * waitForEndOfAcquisitionCondition;

	omni_mutex* waitForCleanupEventMutex;
	omni_condition * waitForCleanupEventCondition;
	bool cleanupEvent;

	

	//For saving data
	class EventMetadatum {
	public:

		double exposureTime;
		std::string description;
		std::string filename;
		std::vector <int> cropVector;

		void assign(double e, std::string d = "", std::string f = "");
		void assign(double e, std::string d, std::string f, std::vector <int> cV);
	};

	std::vector <EventMetadatum> *eventMetadata;

	void playCameraAcquisition(std::vector <EventMetadatum> *eM);
	void setupCameraAcquisition(std::vector <EventMetadatum> *eM) throw (std::exception);
	void cleanupCameraAcquisition();
	std::string timeStampFilename(std::string fn);

	std::string timeStamp;
	std::string extension;
	
	omni_mutex* numAcquiredMutex;
	omni_condition* numAcquiredCondition;
	int numAcquired;

	bool takeSaturatedPic;

	bool AbortIfAcquiring();
	bool startAcquisition();

	bool isPlaying;

	double getMinExposureTime() throw(std::exception);
	double getFrameRefreshTime() throw(std::exception);

/*	class CameraException: public std::exception
	{
	public:
		std::string errString;
		CameraException(std::string inString) : errString(inString) {}
		const char* what() const throw() {return errString.c_str();}
	};*/

private:
	int16 cameraHandle;

	static void playCameraWrapper(void* object);
	virtual void playCamera();

	void cropImageData(std::vector <unsigned short> &imageData, std::vector <WORD> & tempImageVector, int imageIndex, std::vector <int> cropVector);
	bool getCameraData(std::vector <unsigned short>& singleImageVector);

	void setMetadata(ImageMagick::MyImage *image);
	void setMetadata(ImageMagick::MyImage *image, EventMetadatum &eventMetadatum);
	void setCommonMetadata(ImageMagick::MyImage *image);

	std::string createFilePath();

	ImageMagick imageWriter;
	double rotationAngle;
	//std::vector <uns16> imageBuffer;
	uns16 *imageBuffer;

	bool notDestructed;
	
	int origShutterMode;						// for playing Events

	// Declare Image Buffers
	std::string 	 filePath;					// must be less than 260 characters
};

#endif