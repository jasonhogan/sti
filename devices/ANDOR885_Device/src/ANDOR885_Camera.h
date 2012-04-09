#ifndef ANDOR885_CAMERA_H
#define ANDOR885_CAMERA_H

#include "device.h"

#include <math.h>
#include <iostream>
#include "atmcd32d.h"				// includes windows.h
#include <stdio.h>
#include <fstream>					// To redirect cerr to a logfile
#include <ImageMagick.h>
#include "ANDOR885_Exception.h"
#include "ImagePreprocessor.h"
#include "VectorArithmetic.h"
#include "utils.h"

#define ANDOR_ON					   1
#define ANDOR_OFF					   0

#define ACQMODE_SINGLE_SCAN			   1
#define ACQMODE_KINETIC_SERIES		   3
#define ACQMODE_RUN_TILL_ABORT		   5

#define TRIGGERMODE_INTERNAL           0
#define TRIGGERMODE_EXTERNAL           1
#define TRIGGERMODE_EXTERNAL_EXPOSURE  7
/* 
#define TRIGGERMODE_EXTERNAL_START     6
#define TRIGGERMODE_EXTERNAL_SOFTWARE 10
*/

#define READMODE_MULTI_TRACK           1
#define READMODE_RANDOM_TRACK          2
#define READMODE_SINGLE_TRACK          3
#define READMODE_IMAGE                 4

//#define EXPOSURE_DEFAULT            0.01f //exposure time; f denotes a float

#define TTL_OPEN_LOW                   0  // external shutter opens on low TTL
#define TTL_OPEN_HIGH                  1  // external shutter opens on high TTL
#define SHUTTERMODE_AUTO               0  // shutter controlled automatically
#define SHUTTERMODE_OPEN               1  // shutter always open
#define SHUTTERMODE_CLOSE              2  // shutter always closed
#define SHUTTER_CLOSE_TIME             0  // time it takes to close shutter in ms. See p. 57 of SDK, bottom
#define SHUTTER_OPEN_TIME              0  // time it takes to open shutter in ms

#define ANDOR_ERROR					   0  // for error handling
#define ANDOR_SUCCESS				   1

#define NOT_AVAILABLE				  -1


class ANDOR885_Camera
{

public:

	ANDOR885_Camera();
	virtual ~ANDOR885_Camera();

	bool debugging;

	bool deviceExit();

	bool initialized;

	void printError(int errorValue, std::string errorMsg, bool *success, int flag);
	void throwError(int errorValue, std::string errorMsg) throw(std::exception);

	//Get & Set Functions
	int			getImageWidth();
	int			getImageHeight();
	std::string	getFilePath();
	void		setFilePath(std::string path);
	char *		getPalPath();					
	int			getCameraStat();							//Is the camera on or off?
	void		setCameraStat(int cStat);
	int			getAcquisitionMode();						//Acquisition Mode; usually Single Scan (1) or Run Till Abort (5)
	void		setAcquisitionMode(int aMode);
	int			getReadMode();								//Readout Mode; usually Image (4)
	void		setReadMode(int rMode);
	float		getExposureTime();							//Exposure time in seconds; usually 0.01
	void		setExposureTime(float expTime) throw(std::exception);
	float		getKineticTime();							//Kinetic cylce time; determines time between frames.
	int			getShutterMode();							//Shutter Mode; usually Open (1)
	void		setShutterMode(int mode);
	int			getCloseTime();								//Time required to close shutter in ms; usually 1
	void		setCloseTime(int time) throw(std::exception);
	int			getOpenTime();								//Time required to open shutter in ms; usually 1
	void		setOpenTime(int time) throw(std::exception);
	int			getTriggerMode();							//Trigger Mode; External exposure (7)
	void		setTriggerMode(int mode) throw(std::exception);
	int			getCoolerSetpt();
	void		setCoolerSetpt(int setpt);
	int			getCoolerStat();
	void		setCoolerStat(int stat);
	int			getCameraTemp();
	int			getPreAmpGain();								//position in camera's preAmpGain vector
	void		setPreAmpGain(int gainIndex) throw(std::exception);
	int			getVerticalShiftSpeed();
	void		setVerticalShiftSpeed(int speedIndex) throw(std::exception);
	int			getVerticalClockVoltage();					// horizontal shift speed
	void		setVerticalClockVoltage(int voltageIndex) throw(std::exception);
	int			getHorizontalShiftSpeed();					// horizontal shift speed	
	void		setHorizontalShiftSpeed(int speedIndex) throw(std::exception);
	int			getEMCCDGain();					// horizontal shift speed	
	void		setEMCCDGain(int gain) throw(std::exception);

	double		getRotationAngle();
	void		setRotationAngle(double angle);

protected:

	std::string		 logPath;

	ImagePreprocessor imagePreprocessor;

	class AndorAttribute {
	public:
		std::string name;
		std::string initial;
		std::map<int, std::string> choices;

		int inverseFind(std::string value);
		std::string makeAttributeString();
	};

	// interfaces with device
	
	AndorAttribute acquisitionMode_t;			//Acquisition Mode
	AndorAttribute readMode_t;					//Read Mode
	AndorAttribute shutterMode_t;				//Shutter Mode
	AndorAttribute triggerMode_t;				//Trigger Mode
	AndorAttribute verticalShiftSpeed_t;		//VerticalShiftSpeed
	AndorAttribute preAmpGain_t;
	AndorAttribute verticalClockVoltage_t;
	AndorAttribute horizontalShiftSpeed_t;
	
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

	void setupEventAcquisition(std::vector <EventMetadatum> *eM);
	void cleanupEventAcquisition();
	std::string timeStampFilename(std::string fn);

	std::string timeStamp;
	std::string extension;
	
	omni_mutex* numAcquiredMutex;
	omni_condition* numAcquiredCondition;
	int numAcquired;

	bool takeThrowawayImage;

	bool AbortIfAcquiring();
	bool startAcquisition();

	bool isPlaying;

private:

	bool InitializeCamera();
	static void playCameraWrapper(void* object);
	virtual void playCamera();

	void cropImageData(std::vector <unsigned short> &imageData, std::vector <WORD> & tempImageVector, int imageIndex, std::vector <int> cropVector);
	bool getCameraData(int *numAcquired_p, int numExposures, std::vector <WORD>& tempImageVector);

#ifndef _DEBUG
	void setMetadata(ImageMagick::MyImage &image);
	void setMetadata(ImageMagick::MyImage &image, EventMetadatum &eventMetadatum);
	void setCommonMetadata(ImageMagick::MyImage &image);
#endif

	std::string createFilePath();

	ImageMagick imageWriter;
	double rotationAngle;

	bool notDestructed;
	
	int origShutterMode;						// for playing Events

	// Declare Image Buffers
	std::string 	 filePath;					// must be less than 260 characters
	char			 *palPath;
	

	//Inherent camera parameters
	AndorCapabilities caps;                     // AndorCapabilities structure
	char              model[32];                // headmodel
	int 		      imageWidth;       		// dims of (gblXPixels)
	int				  imageHeight;       		//      CCD chip (gblYPixels)
//	int				  VSnumber;					// Location of fastest vertical speed in speed index table
//	int				  HSnumber;					// Location of fastest horizontal speed in speed index table
	int               ADnumber;                 // AD Index
	int				  minTemp;
	int				  maxTemp;
	int				  bitDepth;

	std::vector <int> availableTriggerModes;

	//Camera parameters we can change with attributes
	int cameraStat;								//Is the camera on or off?
//	int acquisitionStat;						//Is the camera acquiring data or not?
	int	acquisitionMode;						//Acquisition Mode; usually Single Scan (1) or Run Till Abort (5)
	int readMode;								//Readout Mode; usually Image (4)
	float exposureTime;							//Exposure time in seconds; usually 0.01
	float accumulateTime;						//Accumulation cycle time; not usually used.
	float kineticTime;							//Kinetic cylce time; determines time between frames.
	int	shutterMode;							//Shutter Mode; usually Open (1) 
	int	triggerMode;							//Trigger Mode; External exposure (7)
//	int numExposures;							//Number of exposures to take in a Kinetic cycle
	int coolerSetpt;
	int coolerStat;
	int cameraTemp;
//	int saveMode;
	int preAmpGain;								//position in camera's preAmpGain vector
//	int preAmpGainPos;							//position in program's preAmpGain vector
//	int numPerFile;
	int verticalShiftSpeed;
	int verticalClockVoltage;					// horizontal shift speed	
	int horizontalShiftSpeed;					// horizontal shift speed	
	int EMCCDGain;

	//Camera parameters we don't change
	int	ttl;									//Determines if shutter opens or closes on a TTL high
	int	closeTime;								//Time required to close shutter in ms; usually 1
	int	openTime;								//Time required to open shutter in ms; usually 1
	int frameTransfer;							//Frame Transfer Mode; usually off.
};




#endif