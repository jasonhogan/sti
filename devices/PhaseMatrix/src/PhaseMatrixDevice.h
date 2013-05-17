/*! \file PhaseMatrixDevice.h
 *  \author Jason Hogan
 *  \brief header file for PhaseMatrixDevice
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef PHASEMATRIX_DEVICE_H
#define PHASEMATRIX_DEVICE_H


#include <STI_Device_Adapter.h>
#include "rs232Controller.h"


class PhaseMatrixDevice : public STI_Device_Adapter
{
public:

	PhaseMatrixDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort);

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);
	
	void parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);

	std::string execute(int argc, char* argv[]);

	std::string getDeviceHelp();

	void startList();

private:

	enum ExternalTriggerMode {SoftwareTrig = 0, ListTrig = 1, ListPointTrig = 2};

	ExternalTriggerMode triggerMode;

	bool measureFrequency(double& frequency);
	bool measurePower(double& power);
	void clearList();

	void addListPoint(unsigned long point, unsigned long dwell_us, const RawEvent& listEvent);

	bool checkFrequencyFormat(std::string frequency, std::string& formatErrorMessage = trash);
	bool checkPowerFormat(std::string power, std::string& formatErrorMessage = trash);

	unsigned long roundToNearest(double input, double increment);


	rs232Controller* serialController;

	int rs232QuerySleep_ms;

	static std::string trash;

	double currentFreqHz;
	double currentPower;

	double listStartTimeHoldoff;
	unsigned long frequencyChangeHoldoff_us;
	
	class PhaseMatrixListEvent : public SynchronousEvent
	{
	public:
		PhaseMatrixListEvent(double time, PhaseMatrixDevice* device) 
			: SynchronousEvent(time, device), device_(device) {}
	private:
		void setupEvent() { }
		void loadEvent() { }
		void playEvent();
		void collectMeasurementData() {}

		PhaseMatrixDevice* device_;
	};


};

#endif
