/*! \file ADF4360_Device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Analog_Devices_VCO::ADF4360_Device
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

#ifndef ANALOG_DEVICES_ADF4360_DEVICE
#define ANALOG_DEVICES_ADF4360_DEVICE

#include "ADF4360.h"
#include "STI_Device.h"

namespace Analog_Devices_VCO {

class ADF4360_Device : public ADF4360, public STI_Device
{
public:

	ADF4360_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string IPAddress, 
		unsigned short ModuleNumber,
		unsigned int VCO_Address,
		unsigned int EtraxMemoryAddress,
		unsigned short ADF4360_model);
	~ADF4360_Device();

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
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	// Device Command line interface setup
	std::string execute(int argc, char **argv);
	void definePartnerDevices();

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback() {}
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};


private:

	//Digital out board event holdoff parameters
	double minimumEventSpacing;
	double minimumAbsoluteStartTime;
	double holdoff;
	double localHoldoff;
	unsigned digitalStartChannel;
	double digitalMinimumEventSpacing;

	std::bitset<24> event_controlLatch;
	std::bitset<24> event_nCounterLatch;
	std::bitset<24> event_rCounterLatch;

	VCOLatches eventLatches;

	std::string printUsage(std::string executableName);
	std::string parseArgs(int argc, char **argv);

	void setFvcoEvent(const RawEvent& evt) throw(std::exception);
	void setOutputPowerEvent(const RawEvent& evt) throw(std::exception);

	double buildAndSendBuffer(double eventTime, const RawEvent& evt, std::bitset<24>& latch);

		
	class ADF4360Event : public SynchronousEvent
	{
	public:
		ADF4360Event(double time, ADF4360_Device* device, VCOLatches& latches) 
			: SynchronousEvent(time, device), latches_(latches), device_adf(device) {};
		void setupEvent() {};
		void loadEvent() {};
		void playEvent();
		void collectMeasurementData() {};
		
		VCOLatches& getLatches() { return latches_; };

	private:
		VCOLatches latches_;
		ADF4360_Device* device_adf;

	};

};

}

#endif
