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
#include "ANDOR885_Camera.h"

#define INIT_EVENT					  -1
#define END_EVENT					  -2

unsigned int __stdcall SetHSSpeedWrapped(int index);

class ANDOR885_Device : public ANDOR885_Camera, public STI_Device
{

public:

	ANDOR885_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber);
	~ANDOR885_Device();



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


	class Andor885Event : public SynchronousEvent
	{
	public:

		Andor885Event(double time, ANDOR885_Device* device) 
			: SynchronousEvent(time, device), ANDORdevice_(device), filenameBase("")  {}
		~Andor885Event() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData();

		void waitBeforeCollectData();

		EventMetadatum eventMetadatum;
		std::string filenameBase;

		int exposureIndex;

		std::vector <EventMetadatum> eventMetadata;

		ANDOR885_Device* ANDORdevice_;

	private:
	};

	unsigned short digitalChannel;
	void sendDigitalLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime);

	double minimumAbsoluteStartTime;
};


#endif
