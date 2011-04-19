/*! \file MccUSBDAQDevice.h
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


#ifndef MCCUSBDAQDEVICE_H
#define MCCUSBDAQDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <STI_Device.h>
#include "cbw.h"
// also requires cbw32.lib to be appropriately linked. May have to change project settings in order for the linker to find this file


class MccUSBDAQDevice : public STI_Device
{
public:
	
	MccUSBDAQDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber,
		int boardNum_,
		int numADChans_);
	~MccUSBDAQDevice();

	bool initialized;

private:

	// Device main()
    bool deviceMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

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
	bool setOutputVoltage(int channel, float output_voltage); //returns true if it worked
	bool readInputChannel(int channel, double& result); //returns true if it worked
	bool readMUXedInputChannel(int channel, double measurement); //returns true if it worked

	std::string result;

	int boardNum;
	int UDStat;
	int channel;
	int numADChans;
	int numDAChans;
	int boardType;
//	float DataValue;
	int Options;   
	float RevLevel;


	int ADInRange;
	int DAOutRange;

	std::map <int, std::string> availableADInRanges;
	std::map <int, std::string> availableDAOutRanges;

	std::map <int, int> analogInChannels;
	std::map <int, int> analogOutChannels;

	void getChannelInfo();
	std::string makeRangeString(std::map <int, std::string> rangeMap);

};

#endif

