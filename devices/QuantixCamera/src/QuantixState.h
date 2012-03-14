#ifndef QUANTIXSTATE_H
#define QUANTIXSTATE_H

#include "device.h"

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include "utils.h"
#include "master.h"
#include "pvcam.h"
#include "CameraException.h"

#define CAMERA_ON					   1
#define CAMERA_OFF					   0

#define TRIGGERMODE_INTERNAL           TIMED_MODE
#define TRIGGERMODE_EXTERNAL           STROBED_MODE
#define TRIGGERMODE_EXTERNAL_EXPOSURE  BULB_MODE
/* 
#define TRIGGERMODE_EXTERNAL_START     TRIGGER_FIRST_MODE
*/

//#define EXPOSURE_DEFAULT            0.01f //exposure time; f denotes a float

#define SHUTTERMODE_AUTO               OPEN_PRE_TRIGGER  // shutter controlled automatically
#define SHUTTERMODE_IGNORE             OPEN_NO_CHANGE  // shutter always open
#define SHUTTERMODE_CLOSE              OPEN_NEVER  // shutter always closed
#define SHUTTER_CLOSE_TIME             0  // time it takes to close shutter in ms. See p. 57 of SDK, bottom
#define SHUTTER_OPEN_TIME              0  // time it takes to open shutter in ms

#define CAMERA_ERROR				   0  // for error handling
#define CAMERA_SUCCESS				   1

#define PREAMP_BLANK				  -1


class QuantixState
{

public:

	QuantixState(int16 handle) throw(std::exception);
	~QuantixState() {};

	class CameraAttribute {
	public:
		CameraAttribute() {name = "Unnamed!"; isAvailable = false; currentValue = "42"; currentLabel = "No Label!";};
		std::string name;
		std::map<std::string, std::string> choices;
		std::string inverseFind(std::string value);

		virtual std::string getLabel();
		virtual std::string get();
		
	private:
		bool isAvailable;

		std::string currentLabel;
		std::string currentValue;

		virtual void set(std::string newValue, int16 cameraHandle) throw(std::exception) = 0;

		friend class QuantixState;
	};

	class ClearMode : public CameraAttribute {
	private: 
		void set (std::string newValue, int16 cameraHandle);
		friend class QuantixState;
	} clearMode;
	
	class ClockingMode : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception);
	friend class QuantixState;
	} clockingMode;
	
	class Gain : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception);
	friend class QuantixState;
	} gain;

	class ReadoutSpeed : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception);
	friend class QuantixState;
	} readoutSpeed;
	
	class ShutterMode : public CameraAttribute 	{private: void set (std::string newValue, int16 cameraHandle)throw(std::exception);
	friend class QuantixState;
	} shutterMode;

	class ShutterDelay : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception) {};
	friend class QuantixState;
	};
	ShutterDelay shutterOpenDelay, shutterCloseDelay;
	
	class TriggerMode : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception) {currentValue = newValue;};
	friend class QuantixState;
	} triggerMode;

	class ImageSize : public CameraAttribute {
	public: int size;
	private: void set (std::string newValue, int16 cameraHandle)throw(std::exception) {};
	friend class QuantixState;
	}; 
	ImageSize imageHeight,imageWidth;

	class ClearCycles : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception) {};
	friend class QuantixState;
	} clearCycles;

	class BinSize : public CameraAttribute {private: void set (std::string newValue, int16 cameraHandle)throw(std::exception);
	friend class QuantixState;
	} binSize;

	class CoolerSetpoint : public CameraAttribute{
	public: 
		
	private:
		void set (std::string newValue, int16 cameraHandle);

		int16 maxTemp; //Given in degrees C x 100
		int16 minTemp; //Given in degrees C x 100
		friend class QuantixState;
	} coolerSetpoint;

	class Temperature : public CameraAttribute { friend class QuantixState;
	public:
		std::string getLabel() {return currentValue;};
	private:
		void set (std::string newValue, int16 cameraHandle) {};
		std::string get(int16 cameraHandle);
		friend class QuantixState;
	} temperature;


	void set(CameraAttribute &attribute, std::string newValueString);
	std::string get(CameraAttribute &attribute);

	std::vector <CameraAttribute*> guiAttributes;

private:
	int16 cameraHandle;

	void InitializeCamera() throw(std::exception);
};

#endif