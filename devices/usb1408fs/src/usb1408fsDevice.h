/*! \file usb1408fsDevice.h
 *  \author David M.S. Johnson
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


#ifndef USB1408FSDEVICE_H
#define USB1408FSDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <STI_Device.h>
#include "cbw.h"
// also requires cbw32.lib to be appropriately linked. May have to change project settings in order for the linker to find this file


class usb1408fsDevice : public STI_Device
{
public:
	
	usb1408fsDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber);
	~usb1408fsDevice();

private:

	// Device main()
    bool deviceMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
    bool readChannel(ParsedMeasurement& Measurement);
    bool writeChannel(const RawEvent& Event);

    // Device Command line interface setup
    void definePartnerDevices();
    std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

private:

	//functions for generating commands
	bool usb1408fsDevice::setOutputVoltage(int channel, float output_voltage); //returns true if it worked
	double usb1408fsDevice::readInputChannel(int channel); //returns true if it worked
	bool usb1408fsDevice::readMUXedInputChannel(int channel, double measurement); //returns true if it worked

	std::string result;

	int BoardNum;
	int UDStat;
	int channel;
	int inputGain;
	int outputGain;
	float DataValue;
	int Options;   
	float RevLevel;
	

};

#endif

