/*! \file PcoEdgeDevice.hpp
 *  \author Brannon Klopfer
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Brannon Klopfer <bbklopfer@stanford.edu>
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

#include <iostream>
#include "STI_Device.h"
#include "PcoEdgeCamera.hpp"

#define ATTR_KEY_EXPOSURE "Exposure"
#define ATTR_KEY_HBIN "hbin"
#define ATTR_KEY_VBIN "vbin"
#define ATTR_KEY_ROI "ROI"
#define ATTR_KEY_TEMP_CCD "Temp: CCD"
#define ATTR_KEY_TEMP_ELEC "Temp: Elec"
#define ATTR_KEY_TEMP_POW "Temp: Pow"
#define ATTR_KEY_TEMP_SET "Temp setpoint"

#define PCOEDGE_DEFAULT_FILENAME "STI_UNNAMED_IMAGE.tif"

// TODO: Implement:
#define ATTR_KEY_TRIGGER "Trigger"

class PcoEdgeDevice : public STI_Device
{
public:
	PcoEdgeDevice::PcoEdgeDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber,
                             string logDirectory);
	~PcoEdgeDevice();

	bool initialized;

private:
	PcoEdgeCamera *camera;
	bool deviceMain(int argc, char** argv) {return false;};  // called in a loop while it returns true
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);
	void stopEventPlayback() { return; }
	void pauseEventPlayback() { return; }
	void resumeEventPlayback() { return; }
	std::string PcoEdgeDevice::attrValue(std::string key);
	void PcoEdgeDevice::setAttrIfDifferent(std::string key, std::string value);
	void PcoEdgeDevice::setAttrFromValue(std::string key);

protected:
	// Device partners
	void definePartnerDevices() { return;};

    // Device Channels
	void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	// Device Command line interface setup
	std::string execute(int argc, char** argv) { return string("FIXME: IMPLEMENT"); };
	void parseDeviceEvents(const RawEventMap& eventsIn,
		SynchronousEventVector& eventsOut) throw(std::exception);
};