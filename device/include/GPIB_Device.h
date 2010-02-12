/*! \file GPIB_Device.h
 *  \author David Johnson
 *  \brief Include-file for the class GPIB_Device
 *  \section license License
 *
 *  Copyright (C) 2010 David Johnson <david.m.johnson@stanford.edu>\n
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

#ifndef GPIB_DEVICE_H
#define GPIB_DEVICE_H

#include <STI_Device.h>

#include "gpibAttribute.h"

typedef std::map<std::string, gpibAttribute> gpibAttributeMap;

class GPIB_Device : public STI_Device
{
public:
	GPIB_Device(ORBManager* orb_manager, std::string DeviceName, std::string IPAddress, 
						 unsigned short ModuleNumber, std::string logDirectory = "//atomsrv1/EP/Data/deviceLogFiles", std::string GCipAddress = "eplittletable.stanford.edu", 
						 unsigned short GCmoduleNumber = 0);

	virtual ~GPIB_Device() {};


private:

	// Device main()
	bool deviceMain(int argc, char* argv[]) {return false;};	//called in a loop while it returns true

	// Device Attributes
	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;
	virtual void defineGpibAttributes() = 0;

	// Device Channels
	virtual void defineChannels() = 0;

	// Device Command line interface setup
	void definePartnerDevices();
	virtual std::string execute(int argc, char** argv) = 0;

	// Device-specific event parsing
	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) {};

	// Event Playback control
	void stopEventPlayback() {};	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};	//for devices that require non-generic pause commands
	void resumeEventPlayback() {};

	virtual bool writeChannel(const RawEvent& Event) = 0;
	virtual bool readChannel(DataMeasurement& Measurement) = 0;

protected:
	//GPIB specific functions
	bool commandDevice(std::string command);
	std::string queryDevice(std::string query);
	double updateGPIBAttribute(std::string gpibCommand, double gpibDouble, bool initialized);

	void addGpibAttribute(std::string key, std::string gpibCommand, std::string allowedValues = "", bool readOnly = false)
	{
		gpibAttribute newGpibAttribute(gpibCommand, allowedValues, readOnly);
		gpibAttributes[key] = newGpibAttribute;
	};

	const gpibAttribute& getGpibAttribute(std::string key) const {return gpibAttributes.find(key)->second;};

	std::string gpibControllerIPAddress;
	unsigned short gpibControllerModule;
	int primaryAddress;
	int secondaryAddress;

private:
	gpibAttributeMap gpibAttributes;


private:
	class GpibAttributeUpdater : public AttributeUpdater
	{
	public:
		GpibAttributeUpdater(GPIB_Device* thisDevice) : AttributeUpdater(thisDevice), gpibDevice(thisDevice) {};
		void defineAttributes();
		bool updateAttributes(std::string key, std::string value); 
		void refreshAttributes();

	private:
		GPIB_Device* gpibDevice;

	};




};

#endif
