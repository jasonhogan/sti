/*! \file GenericDevice.h 
 *  \author David M.S. Johnson
 *  \author Susannah Dickerson
 *  \author Brannon Klopfer
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>,
 *  Brannon Klopfer <bbklopfer@stanford.edu>
 *
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

#ifndef GenericDevice_H
#define GenericDevice_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "GPIB_Device.h"
#include "..\..\rs232Controller\src\rs232Controller.h"
#include <map>
#include <string>
#include "TaggedConfigFile.h"
#include "ConfigFile.h"

#include "GenericDeviceConfig.hpp"
#include <boost/algorithm/string.hpp> // for trim()
#include <boost/lexical_cast.hpp> // for casting strings to numerical

// TODO: Clean up. Get rid of hp3458a-specific stuff.
class GenericDevice : public STI_Device
{
public:
	GenericDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string configFileName,
							GenericDeviceConfig* deviceConfig,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber = 0); // FIXME WHAT IS THIS? BOARD INDEX?
	~GenericDevice();

	bool initialized;

private:
	GenericDeviceConfig* deviceConfig;
	std::string getDeviceHelp();
	// Device main()
	bool deviceMain(int argc, char** argv) {return false;};  // called in a loop while it returns true

    // Device Attributes
	void defineGpibAttributes();
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Keep track of what sort of device we are
	int deviceType;
	rs232Controller* serialController;
	int rs232QuerySleep_ms;

	// TODO: Should implement these? Or too specific?
	void stopEventPlayback() { return; }
	void pauseEventPlayback() { return; }
	void resumeEventPlayback() { return; }

protected:
	std::string GenericDevice::queryDevice(std::string query);
	bool GenericDevice::commandDevice(std::string command);

	std::string gpibControllerIPAddress;
	unsigned short gpibControllerModule;
	int primaryAddress;
	int secondaryAddress;

	class GenericAttribute 
	{
	public:
		GenericAttribute() {name = "Unnamed!"; currentLabel = "No Label!";};
		std::string name;
		std::map<std::string, std::string> choices;

		std::string inverseFind(std::string value);

		std::string currentLabel;
		std::string command;
		// TODO: optional different commands for setting, getting attributes.
	};

	std::map <std::string, GenericAttribute> genAttributes;

	// Device partners
	void definePartnerDevices();

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);
	void GenericDevice::parseInputVector(const std::vector <MixedValue> &valueIn, 
		std::vector <double> &parsedValues, const RawEvent& evt) throw(std::exception);

	class GenericInitEvent : public SynchronousEvent
	{
	public:

		GenericInitEvent(double time, GenericDevice* device, std::string numReads, 
			std::string timeBtwReads, std::string intTime) 
			: SynchronousEvent(time, device), GenericDevice_(device), 
			numReadings(numReads), timeBtwReadings(timeBtwReads), integrationTime(intTime)  {}
		~GenericInitEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData() {}

		void waitBeforeCollectData() {}

		GenericDevice* GenericDevice_;

		std::string numReadings;
		std::string timeBtwReadings;
		std::string integrationTime;

	private:
	};

	class GenericReadEvent : public SynchronousEvent
	{
	public:

		GenericReadEvent(double time, GenericDevice* device, int numReads, 
			double timeBtwReads, double intTime) 
			: SynchronousEvent(time, device), GenericDevice_(device), 
			numReadings(numReads), timeBtwReadings(timeBtwReads), integrationTime(intTime)  {}
		~GenericReadEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData();

		GenericDevice* GenericDevice_;

		int numReadings;
		double timeBtwReadings;
		double integrationTime;

	private:
		double getDoubleFromChars(std::vector<MixedData> &charVector, int pos, int numChars);
		MixedData dataOut;
	};

	void sendTriggerEvents(double eventTime, const RawEvent& evt);

	// Device Command line interface setup
	std::string execute(int argc, char** argv);

	std::string gpibID;

	//std::string readFromDevice(std::string command, int readSize);
	bool readFromDevice(std::string command, int readSize, MixedData & readings);

	bool prepareDeviceToPlay();
	double roundTo100ns(double n);

	TaggedConfigFile * configFile;

	bool subprogramSet;
	std::string subprogramName;

	bool hasTriggerPartner;
	unsigned short triggerChannel;
	int triggerLow, triggerHigh;

	bool trackReadTimeDelay;
	double readTimeDelay, readTimeDefault, gpibCommDelay;
	void rememberReadTime(double newReadTime, int numPoints);
};

#endif

