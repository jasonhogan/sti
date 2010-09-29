/*! \file STI_Device_Template.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class MOTMagn_Device
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

#ifndef MOTMAGN_DEVICE_H
#define MOTMAGN_DEVICE_H

#include <STI_Device.h>
#include "rs232Controller.h"
#include "ConfigFile.h"

class MOTMagn_Device : public STI_Device
{
public:

    //***************BEGIN STI_Device functions***************//

    MOTMagn_Device(ORBManager* orb_manager,  std::string    DeviceName, 
             std::string Address,    unsigned short ModuleNumber,
			 unsigned short comPort, std::string logDirectory, std::string configFilename);
    ~MOTMagn_Device();
    
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


	bool initialized;

    //****************END STI_Device functions***************//


    // Additional MOTMagn_Device class content...
private:

	rs232Controller * myRS485Controller;

	ConfigFile * config;

	class Magnetometer {
	public:

		Magnetometer() {};
		~Magnetometer() {};

		std::string ID;
		std::vector <double> calibration;
		void setMagnetometer(std::string IDv, std::vector <double> calv) { ID = IDv; calibration = calv;};
	};

	std::vector <Magnetometer> magnetometers;

	bool enableDataLogging;

	bool measureField(Magnetometer &magnetometer, std::vector <double> & measurement);

};

#endif
