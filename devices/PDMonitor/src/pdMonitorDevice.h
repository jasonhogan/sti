/*! \file pdMonitorDevice.h
 *  \author David M.S. Johnson, modified from digitelSPCDevice by Susannah Dickerson
 *  \brief header file
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
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


#ifndef PDMONITORDEVICE_H
#define PDMONITORDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

//#include "rs232Controller.h"
#include <STI_Device.h>


class pdMonitorDevice : public STI_Device
{
public:
	
	pdMonitorDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber,
		std::string logDirectory,
		std::string calibrationDirectoryIn);
	~pdMonitorDevice();

private:

// Device main()
    bool deviceMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) {return false;}
	bool writeChannel(unsigned short channel, const MixedValue& value) {return false;}

    // Device Command line interface setup
	void definePartnerDevices();
    std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) {};

	// Event Playback control
	void stopEventPlayback() {};	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};


private:

	std::string calibrationDirectory;

	class attributePDMonitor {
	public:

		attributePDMonitor():value(0),slope(0),offset(0) {};
		~attributePDMonitor() {};

		double value;
		std::string tag;
		std::string usbCommand;
		std::string calibrationFilename;
		double slope;
		double offset;
	};

	attributePDMonitor pd1;
	attributePDMonitor pd2;

	std::string result;
	bool enableDataLogging;
	bool enableCalibration;
	std::string pumpName;

	bool getCalibration(attributePDMonitor &pd);

	//define data containers
	
};

#endif

