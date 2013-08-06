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
#include <iostream>

class STF_DA_FAST_Device : public FPGA_Device
{
public:

	STF_DA_FAST_Device(ORBManager* orb_manager, std::string configFilename);
	~STF_DA_FAST_Device();

private:

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
	void parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);
	double getMinimumEventStartTime() { return minimumAbsoluteStartTime; }
	
	// Event Playback control
//	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

	std::string getDeviceHelp() { return "Fast Analog Out Help"; }
	
	double minimumEventSpacing;
	double minimumAbsoluteStartTime;
	double holdoff;


	class FastAnalogOutEvent : public FPGA_DynamicEvent
	{
	public:
		FastAnalogOutEvent(double time, bool A_WR, bool A_LOAD, bool B_WR, bool B_LOAD, 
			const std::vector<RawEvent>& sourceEvents, unsigned rawEventIndex, FPGA_Device* device);
		
		void collectMeasurementData() { }
		void updateValue(const std::vector<RawEvent>& sourceEvents);	//for DynamicValue induced changes
	
	private:

		void setValueBits(double rawValue);
		
		unsigned rawEventIndex_l;
	};

};


#endif

