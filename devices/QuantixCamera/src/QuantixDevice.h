/*! \file QuatnixDevice.cpp
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

#ifndef ANDOR885_DEVICE_H
#define ANDOR885_DEVICE_H

#include <STI_Device.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <iostream>					// To redirect cerr to a logfile
#include <fstream>					// To redirect cerr to a logfile
#include <ImageMagick.h>
#include "master.h"
#include "pvcam.h"
#include "QuantixCamera.h"

#define INIT_EVENT					  -1
#define END_EVENT					  -2

class QuantixDevice : public QuantixCamera, public STI_Device
{

public:

	QuantixDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber, int16 handle);
	~QuantixDevice();


	bool initialized;
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
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value) {return false;}

	// Device Command line interface setup
	std::string execute(int argc, char **argv);
	void definePartnerDevices(); // requires none

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap &eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);
 
	// Event Playback control
	void stopEventPlayback();
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

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

	class QuantixEvent : public SynchronousEvent
	{
	public:

		QuantixEvent(double time, QuantixDevice* device) 
			: SynchronousEvent(time, device), quantixDevice_(device), filenameBase("")  {}
		~QuantixEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData();

		void waitBeforeCollectData();

		EventMetadatum eventMetadatum;
		std::string filenameBase;

		int exposureIndex;

		std::vector <EventMetadatum> eventMetadata;

		QuantixDevice* quantixDevice_;

	private:
	};

	
	std::string testCropVector(const MixedValueVector & cropVectorIn, std::vector <int>& cropVectorOut);

	unsigned short digitalChannel;
	unsigned short slowAnalogChannel;
	void sendDigitalLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime);
	void sendSlowAnalogLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime);

	double minimumAbsoluteStartTime;
	
	enum CameraTriggerDevice {DigitalBoard, SlowAnalogBoard};
	CameraTriggerDevice cameraTriggerDevice;

	std::vector <QuantixState::CameraAttribute*> cameraAttributes;

	int16 cameraHandle;
	void printError();
	void printParamAvailability();

	std::string makeAttributeString(std::map<std::string, std::string> &choices);
};


#endif
