/*! \file gpib_hub_device.h
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


#ifndef GPIBCONTROLLERDEVICE_H
#define GPIBCONTROLLERDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <STI_Device.h>
//#include "ENET_GPIB_device.h"
//#include "GPIB_USB_HS_device.h"
#include "GPIB_device.h"



class gpibControllerDevice : public STI_Device
{
public:
	
	gpibControllerDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber, std::string gpibHost);
	~gpibControllerDevice();

private:

	// Device main()
	bool deviceMain(int argc, char **argv);

	// Device Attributes
	void defineAttributes() {};
	void refreshAttributes() {};
	bool updateAttribute(std::string key, std::string value) {return false;};

	// Device Channels
	void defineChannels(); //{};
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut); //{return false;}
	bool writeChannel(unsigned short channel, const MixedValue& value) {return false;}


	// Device Command line interface setup
	void definePartnerDevices() {}; // requires none
	std::string execute(int argc, char **argv);

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception) {};
	
	// Event Playback control
	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};


private:

	//ENET_GPIB_device* gpibController;
	//GPIB_USB_HS_device* gpibController;
	GPIB_device* gpibController;
	std::string result;
	

};

#endif

