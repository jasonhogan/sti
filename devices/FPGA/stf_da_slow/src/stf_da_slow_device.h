/*! \file stf_da_slow_device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class stf_da_slow_device
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


#ifndef STF_DA_SLOW_DEVICE_H
#define STF_DA_SLOW_DEVICE_H

#include "FPGA_Device.h"

class stf_da_slow_device : public FPGA_Device
{
public:

	stf_da_slow_device(ORBManager* orb_manager, std::string configFilename);
	~stf_da_slow_device();

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
	void definePartnerDevices(); // requires none
	std::string execute(int argc, char **argv);

	// Device-specific event parsing
	void parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);
	double getMinimumEventStartTime() { return minimumAbsoluteStartTime; }
	
	// Event Playback control
//	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

//begin module specific definition

	//slow analog out event holdoff parameters
	double minimumEventSpacing;
	double minimumAbsoluteStartTime;
	double holdoff;


	class SlowAnalogOutEvent : public FPGA_DynamicEvent
	{
	public:
		//SlowAnalogOutEvent(
		//	double time, uInt32 voltageInt, bool update, 
		//	uInt32 channelBits, uInt32 registerBits, const std::vector<RawEvent>& sourceEvents,
		//	bool reset, FPGA_Device* device);
		SlowAnalogOutEvent(
			double time, const std::vector<RawEvent>& sourceEvents, FPGA_Device* device);
		void collectMeasurementData() { };
		
		void updateValue(const std::vector<RawEvent>& sourceEvents);
	};
		

};


#endif
