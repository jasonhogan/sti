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

	bool deviceExit();

	bool initialized;
	QuantixState *cameraState;

	void printError(int errorValue, std::string errorMsg, bool *success, int flag);
	void throwError(int errorValue, std::string errorMsg) throw(std::exception);

	//Get & Set Functions
/*	int			getImageWidth();
	int			getImageHeight();*/
	std::string	getFilePath();
	void		setFilePath(std::string path);
/*	char *		getPalPath();					
	int			getCameraStat();							//Is the camera on or off?
	void		setCameraStat(int cStat);
	int			getAcquisitionMode();						//Acquisition Mode; usually Single Scan (1) or Run Till Abort (5)
	void		setAcquisitionMode(int aMode);
	int			getReadMode();								//Readout Mode; usually Image (4)
	void		setReadMode(int rMode);
	float		getExposureTime();							//Exposure time in seconds; usually 0.01
	void		setExposureTime(float expTime) throw(std::exception);
	float		getKineticTime();							//Kinetic cylce time; determines time between frames.
	int			getCloseTime();								//Time required to close shutter in ms; usually 1
	void		setCloseTime(int time) throw(std::exception);
	int			getOpenTime();								//Time required to open shutter in ms; usually 1
	void		setOpenTime(int time) throw(std::exception);
	int			getTriggerMode();							//Trigger Mode; External exposure (7)
	void		setTriggerMode(int mode) throw(std::exception); */
/*	int			getClearMode();
	void		setClearMode(int newMode) throw(std::exception);	
	int			getClockingMode();
	void		setClockingMode(int newMode) throw(std::exception);	
	int			getCoolerSetpt();
	void		setCoolerSetpt(int setpt) throw(std::exception);
	int			getCameraTemp();
	int			getGain();								//position in camera's preAmpGain vector
	void		setGain(int newGain) throw(std::exception);
	int			getReadoutSpeed();								//position in camera's preAmpGain vector
	void		setReadoutSpeed(int newSpeed) throw(std::exception);
	int			getShutterMode();							//Shutter Mode; usually Open (1)
	void		setShutterMode(int newMode);
	int			getTriggerMode();							//Trigger Mode; External exposure (7)
	void		setTriggerMode(int newMode);*/
	/*
	int			getVerticalShiftSpeed();
	void		setVerticalShiftSpeed(int speedIndex) throw(std::exception);
	int			getVerticalClockVoltage();					// horizontal shift speed
	void		setVerticalClockVoltage(int voltageIndex) throw(std::exception);
	int			getHorizontalShiftSpeed();					// horizontal shift speed	
	void		setHorizontalShiftSpeed(int speedIndex) throw(std::exception);

	double		getRotationAngle();
	void		setRotationAngle(double angle);*/

protected:
	

	std::string		 logPath;

	ImagePreprocessor imagePreprocessor;

/*	class CameraAttribute {
	public:
		CameraAttribute(QuantixCamera* camera_, std::string name_) : camera(camera_), name(name_)  {}
		~CameraAttribute() {}

		std::string name;
		std::string currentString;
		int currentValue;
		std::map<int, std::string> choices;

		int inverseFind(std::string value);

		virtual void set(int newValue) throw(std::exception) {currentValue = newValue;};
		virtual int get() {return currentValue;};
		virtual std::string getString() {return choices.find(currentValue)->second;};

		QuantixCamera *camera;
	};

	class ClearMode : public CameraAttribute {public: 
		ClearMode(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};
		void set (int newValue);};
	ClearMode *clearMode_t;
	
	class ClockingMode : public CameraAttribute 	{public: 
		ClockingMode(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};
		void set (int newValue);};
	ClockingMode *clockingMode_t;
	
	class Gain : public CameraAttribute 	{public: 
		Gain(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};
		void set (int newValue);};
	Gain* gain_t;

	class ReadoutSpeed : public CameraAttribute {public: 
		ReadoutSpeed(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};
		void set (int newValue);};
	ReadoutSpeed *readoutSpeed_t;
	
	class ShutterMode : public CameraAttribute 	{public: 
		ShutterMode(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};
		void set (int newValue);};
	ShutterMode *shutterMode_t;
	
	class TriggerMode : public CameraAttribute {public: 
		TriggerMode(QuantixCamera* camera_, std::string name_) : CameraAttribute(camera_, name_) {};};
	TriggerMode *triggerMode_t;*/

	// interfaces with device
	
/*	CameraAttribute acquisitionMode_t;			//Acquisition Mode
	CameraAttribute clockingMode_t;				//Read Mode
	CameraAttribute shutterMode_t;				//Shutter Mode
	CameraAttribute triggerMode_t;				//Trigger Mode
	CameraAttribute readoutSpeed_t;		//VerticalShiftSpeed
	CameraAttribute gain_t;
	CameraAttribute clearMode_t;
	*/
	
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
	void setupCameraAcquisition(std::vector <EventMetadatum> *eM);
//	void cleanupEventAcquisition();
	std::string timeStampFilename(std::string fn);

	std::string timeStamp;
	std::string extension;
	
	omni_mutex* numAcquiredMutex;
	omni_condition* numAcquiredCondition;
	int numAcquired;

	bool takeSaturatedPic;

//	bool AbortIfAcquiring();
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

//	void InitializeCamera();
//	static void playCameraWrapper(void* object);
//	virtual void playCamera();

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
};

#endif