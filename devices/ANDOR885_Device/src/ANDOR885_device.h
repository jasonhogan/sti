/*! \file ANDOR885_device.cpp
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

#ifndef ANDOR885_DEVICE_H
#define ANDOR885_DEVICE_H

#include <STI_Device.h>
#include <math.h>
#include <iostream>
#include "atmcd32d.h"				// includes windows.h
#include <stdio.h>
#include <time.h>
#include <iostream>					// To redirect cerr to a logfile
#include <fstream>					// To redirect cerr to a logfile
#include <ImageMagick.h>			

#define ON							   1
#define OFF							   0

#define ACQMODE_SINGLE_SCAN			   1
#define ACQMODE_KINETIC_SERIES		   3
#define ACQMODE_RUN_TILL_ABORT		   5

#define TRIGGERMODE_INTERNAL           0
#define TRIGGERMODE_EXTERNAL           1
/* 
#define TRIGGERMODE_EXTERNAL_START     6
#define TRIGGERMODE_EXTERNAL_EXPOSURE  7
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
#define SHUTTER_CLOSE_TIME             1  // time it takes to close shutter in ms
#define SHUTTER_OPEN_TIME              1  // time it takes to open shutter in ms

#define ANDOR_ERROR					   0  // for error handling
#define ANDOR_SUCCESS				   1

#define PREAMP_BLANK				  -1




class ANDOR885_Device : public STI_Device
{

public:

	ANDOR885_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber);
	~ANDOR885_Device();

	bool deviceExit();

	bool isInitialized();

//STI_Device functions

private:

	// Device setup
	bool deviceMain(int argc, char **argv);

	// Device Attributes
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();
	bool readChannel(DataMeasurement &Measurement);
	bool writeChannel(const RawEvent& Event);

	// Device Command line interface setup
	std::string execute(int argc, char **argv);
	void definePartnerDevices(); // requires none

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap &eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);
 
	// Event Playback control
	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

	class Andor885Event : public SynchronousEvent
	{
	public:

		Andor885Event(double time, ANDOR885_Device* device) 
			: SynchronousEvent(time, device), ANDORdevice_(device)  {}
		~Andor885Event() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData() { }
/*
		std::vector <double> exposureTimes;
		std::vector <std::string> descriptions;
		std::vector <std::string> filenames;
		*/
		double eventExposureTime;
		int eventNumExposures;
		int eventNumExpPerFile;

		ANDOR885_Device* ANDORdevice_;

	private:
	};

private:
	
	static omni_mutex* acquisitionStatMutex;

	bool initialized;							//Is camera initialized? (not user-changable)

	bool InitializeCamera();
	int AllocateBuffers(at_32 *pImageArray, int bufferSize);
	void FreeBuffers(at_32 *pImageArray);
	bool AbortIfAcquiring();
	bool SaveSingleScan();
	void printError(int errorValue, std::string errorMsg, bool *success, int flag);

	static void saveContinuousDataWrapper(void* object);
	virtual void saveContinuousData();

	std::string makeTimeString();
	//Makes single string from a vector of strings for addAttributes.
	std::string makeString(vector <std::string>& choices);
//	void saveImageArray();
	void addMetadata(ImageMagick::Metadata &metadata);
	void saveImageVector();

	class toggleAttribute {
	public:
		std::string name;
		std::string initial;
		std::vector <std::string> choices;
		std::vector <int>		 choiceFlags;
	};

	std::string findToggleAttribute(toggleAttribute &attr, int flag);

	// Declare Image Buffers
	std::string 	 filePath;					// must be less than 260 characters
	char			 *palPath;

	//Inherent camera parameters
	AndorCapabilities caps;                     // AndorCapabilities structure
	char              model[32];                // headmodel
	int 			  gblXPixels;       		// dims of
	int				  gblYPixels;       		//      CCD chip
	int				  VSnumber;					// Location of fastest vertical speed in speed index table
	int				  HSnumber;					// Location of fastest horizontal speed in speed index table
	int               ADnumber;                 // AD Index
	int				  minTemp;
	int				  maxTemp;

	std::vector <std::vector <WORD>> pImageVector;  //needed for debug mode ONLY
	ImageMagick images;

	//Camera parameters we can change
	int cameraStat;								//Is the camera on or off?
	int acquisitionStat;						//Is the camera acquiring data or not?
	int	acquisitionMode;						//Acquisition Mode; usually Single Scan (1) or Run Till Abort (5)
	toggleAttribute acquisitionMode_t;
	int readMode;								//Readout Mode; usually Image (4)
	toggleAttribute readMode_t;					//Read mode
	float exposureTime;							//Exposure time in seconds; usually 0.01
	float accumulateTime;						//Accumulation cycle time; not usually used.
	float kineticTime;							//Kinetic cylce time; not usually used.
	int	ttl;									//Determines if shutter opens or closes on a TTL high
	int	shutterMode;							//Shutter Mode; usually Open (1) 
	toggleAttribute shutterMode_t;				//Shutter Mode
	int	closeTime;								//Time required to close shutter in ms; usually 1
	int	openTime;								//Time required to open shutter in ms; usually 1
	int	triggerMode;							//Trigger Mode; usually Internal (0) or External (1)
	int frameTransfer;							//Frame Transfer Mode; usually on.
//	int spoolMode;								//Spool data
	int numExposures;							//Number of exposures to take in a Kinetic cycle
	int coolerSetpt;
	int coolerStat;
	int cameraTemp;
	int saveMode;
	toggleAttribute preAmpGain_t;
	int preAmpGain;								//position in camera's preAmpGain vector
	int preAmpGainPos;							//position in program's preAmpGain vector
	int numPerFile;
};


#endif
