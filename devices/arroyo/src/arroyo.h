/*! \file arroyo.h
 *  \author Alex sugarbaker
 *  \brief Include-file for the class arroyo
 *  \section license License
 *
 *  Copyright (C) 2013 Alex Sugarbaker <sugarbak@stanford.edu>
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

#ifndef ARROYO_H
#define ARROYO_H

#include <STI_Device.h>
#include "rs232Controller.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <ConfigFile.h>
#include <boost/lexical_cast.hpp>
#include <vector>

class arroyo : public STI_Device
{
public:

    //***************BEGIN STI_Device functions***************//

    arroyo(ORBManager* orb_manager,  std::string    DeviceName, 
             std::string Address,    unsigned short ModuleNumber, unsigned short comPort);
    arroyo(ORBManager* orb_manager,  std::string    DeviceName, 
             std::string Address,    unsigned short ModuleNumber, unsigned short comPortCurrent, unsigned short comPortTemprtr);
    ~arroyo();

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

	std::string arroyo::getDeviceHelp() ;

    //****************END STI_Device functions***************//

	bool initialized;

	void initializeRS232values();
	void getAttributesFromDevice();
	std::string cleanCommand(std::string commandString);

private:

	rs232Controller * serConCurrent;
	rs232Controller * serConTemprtr;

	int rs232QuerySleep_ms;
	int rs232ReadLength;
	int waitAfterTurnLaserOn_s;

	//Attribute variables
	double currentLimit;
	double temperatureSetPoint;
	bool laserOn;
	bool intensityLock;


};

#endif
