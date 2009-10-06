/*! \file STF_DA_FAST_Device.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class STF_DA_FAST_Device
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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

#ifndef STF_DA_FAST_DEVICE_H
#define STF_DA_FAST_DEVICE_H

#include "FPGA_Device.h"

class STF_DA_FAST_Device : public FPGA_Device
{
public:

	STF_DA_FAST_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string IPAddress,
		unsigned short ModuleNumber);
	~STF_DA_FAST_Device();

private:
	//STI_Device functions

	// Device setup
	bool deviceMain(int argc, char **argv);

	// Device Attributes
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();

	// Device Command line interface setup
	void definePartnerDevices(); // requires none
	std::string execute(int argc, char **argv);

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception);
	
	// Event Playback control
	virtual void stopEventPlayback() {};

	class FastAnalogOutEvent : public FPGA_Event
	{
	public:
		FastAnalogOutEvent(double time, uInt32 value, FPGA_Device* device) 
			: FPGA_Event(time, device) {setBits(value);}
		void collectMeasurementData() { };
	};


};


#endif
