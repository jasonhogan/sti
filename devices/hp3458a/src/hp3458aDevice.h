/*! \file hp3458Device.h
 *  \author David M.S. Johnson
 *  \modified by Susannah Dickerson
 *  \brief header file for hp83711bDevice class
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>\n
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


#ifndef hp3458aDEVICE_H
#define hp3458aDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "GPIB_Device.h"
#include <map>
#include <string>
#include "TaggedConfigFile.h"


class hp3458aDevice : public GPIB_Device
{
public:
	
	hp3458aDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber, std::string configFileName,
							std::string logDirectory = "//atomsrv1/EP/Data/deviceLogFiles",
							std::string GCipAddress = "li-gpib.stanford.edu",
							unsigned short GCmoduleNumber = 0);
	~hp3458aDevice();


	bool initialized;

private:

	std::string getDeviceHelp();
	// Device main()
	bool deviceMain(int argc, char** argv) {return false;};    //called in a loop while it returns true

    // Device Attributes
	void defineGpibAttributes();
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);


	class HP3458aAttribute 
	{
	public:
		HP3458aAttribute() {name = "Unnamed!"; currentLabel = "No Label!";};
		std::string name;
		std::map<std::string, std::string> choices;

		std::string inverseFind(std::string value);

		std::string currentLabel;
		std::string command;
	};

	std::map <std::string, HP3458aAttribute> hpAttributes;

	// Device partners
	void definePartnerDevices();

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);
	void hp3458aDevice::parseInputVector(const std::vector <MixedValue> &valueIn, 
		std::vector <double> &parsedValues, const RawEvent& evt) throw(std::exception);

	class hp3458aInitEvent : public SynchronousEvent
	{
	public:

		hp3458aInitEvent(double time, hp3458aDevice* device, std::string numReads, 
			std::string timeBtwReads, std::string intTime) 
			: SynchronousEvent(time, device), hp3458aDevice_(device), 
			numReadings(numReads), timeBtwReadings(timeBtwReads), integrationTime(intTime)  {}
		~hp3458aInitEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData() {}

		void waitBeforeCollectData() {}

		hp3458aDevice* hp3458aDevice_;

		std::string numReadings;
		std::string timeBtwReadings;
		std::string integrationTime;

	private:
	};

	class hp3458aReadEvent : public SynchronousEvent
	{
	public:

		hp3458aReadEvent(double time, hp3458aDevice* device, int numReads, 
			double timeBtwReads, double intTime) 
			: SynchronousEvent(time, device), hp3458aDevice_(device), 
			numReadings(numReads), timeBtwReadings(timeBtwReads), integrationTime(intTime)  {}
		~hp3458aReadEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData();

		hp3458aDevice* hp3458aDevice_;

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

