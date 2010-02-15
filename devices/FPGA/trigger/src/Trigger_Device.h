/*! \file Trigger_Device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class Trigger_Device
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

#ifndef TRIGGER_DEVICE_H
#define TRIGGER_DEVICE_H

#include <STI_Device.h>
#include <EtraxBus.h>


class Trigger_Device : public STI_Device
{
public:

	Trigger_Device(ORBManager* orb_manager, std::string configFilename, uInt32 EtraxMemoryAddress);
    ~Trigger_Device();

private:

	// Device main()
    bool deviceMain(int argc, char** argv);

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
	void stopEventPlayback();
	void pauseEventPlayback();
	void resumeEventPlayback();

	class TriggerEvent : public BitLineEvent<32>
	{
	public:
		TriggerEvent(double time, uInt32 value, Trigger_Device* device) 
			: BitLineEvent<32>(time, value, device), trigger(device) {}

		void setupEvent() { };
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData() { };
	private:
		Trigger_Device* trigger;
	};

	
	void writeData(uInt32 data);
	void waitForExternalTrigger();
	uInt32 getOffsetArmBits();

	uInt32 etraxMemoryAddress;
	EtraxBus* bus;

	uInt32 sleepTime;
	omni_mutex* busSleepMutex;
	omni_condition* busSleepCondition;

	omni_mutex* serverPauseMutex;
	
	omni_mutex* triggerPauseMutex;
	omni_condition* triggerPauseCondition;

	uInt32 play;
	uInt32 stop;
	uInt32 pause;
	uInt32 waitForExternal;
	uInt32 armBits;

	bool waitingForExternalTrigger;
	bool triggerPaused;

};

#endif
