/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_AD_FAST
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

#ifndef STF_AD_FAST_DEVICE_H
#define STF_AD_FAST_DEVICE_H

#include "stf_ad_fast.h"
#include "FPGA_Device.h"

namespace STF_AD_FAST {

class STF_AD_FAST_Device : public ad_fast, public FPGA_Device
{
public:

	STF_AD_FAST_Device(ORBManager* orb_manager, std::string configFilename, unsigned int EtraxMemoryAddress);
	~STF_AD_FAST_Device();

	//STI_Device functions

private:
	// Device main()
	bool deviceMain(int argc, char **argv);

	// Device Attributes
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();

	// Device Command line interface setup
	std::string execute(int argc, char **argv);
	void definePartnerDevices() {}; // requires none

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception);
	
	// Event Playback control
	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

private:

	class AnalogInEvent : public FPGA_Event
	{
	public:
		AnalogInEvent(double time, FPGA_Device* device) 
			: FPGA_Event(time, device) {}
		void collectMeasurementData();
	};
};

}

#endif
