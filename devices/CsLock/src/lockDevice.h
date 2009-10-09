/*! \file lockDevice.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class lockDevice
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef LOCKDEVICE_H
#define LOCKDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "STI_Device.h"
#include "CsLock.h"
#include <string>

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>


class lockDevice : public STI_Device
{
public:
	
	lockDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber);
	~lockDevice();

private:

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

private:
	
	enum ArgType {pGain, iGain, dGain, iiGain, offset, oEnable, iEnable, iiEnable, 
				filename, serialAddress, help, invalid};

	struct commandArg {
		int circuit;
		int argType;
		double value;
	};

	stringstream terminalStream;
	std::string currentSettingFileBase;
	string currentSettingFile;

	CsLock* lockBoard;
	
	void lockDevice::showTextMenu();
	void printUsage(void);

	commandArg parseArg(std::string arg);
	std::string parseCommandLineArgs(int argc, char* argv[]);
	std::string parseCommandLineArgs(std::vector<std::string> strArgv);
	
	std::vector<std::string> getStringArgsFromFile(const std::string &fileName);
	void printCurrentSettingsToFile(const std::string &fileName);
	void getCurrentSettings(std::string settingsFile);
	std::string constructSettingFileName(int serAddress);

};

#endif

