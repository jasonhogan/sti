/*! \file STI_Device_Adapter.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_Device
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef STI_DEVICE_ADAPTER_H
#define STI_DEVICE_ADAPTER_H

#include "STI_Device.h"

class STI_Device_Adapter : public STI_Device
{
public:

	STI_Device_Adapter(ORBManager* orb_manager, std::string DeviceName, std::string configFilename) 
		: STI_Device(orb_manager, DeviceName, configFilename) {};
	
	STI_Device_Adapter(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".")
		: STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber, logDirectory) {};

	virtual ~STI_Device_Adapter() {};

protected:

	// Device main()
	virtual bool deviceMain(int argc, char* argv[]) { return false; };

	// Device Attributes
	virtual void defineAttributes() {};
	virtual void refreshAttributes() {};
	virtual bool updateAttribute(std::string key, std::string value) { return false; };

	// Device Channels
	virtual void defineChannels() {};
	virtual bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) { return false; };
	virtual bool writeChannel(unsigned short channel, const MixedValue& value)  { return false; };

	// Device Command line interface setup
	virtual void definePartnerDevices() {};
	virtual std::string execute(int argc, char* argv[]) { return ""; };

	// Device-specific event parsing
	virtual void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) { parseDeviceEventsDefault(eventsIn, eventsOut); };

	// Event Playback control
	virtual void stopEventPlayback() {};
	virtual void pauseEventPlayback() {};
	virtual void resumeEventPlayback() {};
};



#endif
