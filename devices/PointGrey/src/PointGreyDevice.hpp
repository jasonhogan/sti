/*! \file PointGreyDevice.hpp
 *  \author Brannon Klopfer
 *  \brief Header-file for the class PointGreyDevice
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
#include <FlyCapture2.h>

#define ATTR_KEY_EXPOSURE "Exposure"
#define ATTR_KEY_HBIN "hbin"
#define ATTR_KEY_VBIN "vbin"
#define ATTR_KEY_ROI "ROI"
#define ATTR_KEY_TEMP_CCD "Temp: CCD"
#define ATTR_KEY_TEMP_ELEC "Temp: Elec"
#define ATTR_KEY_TEMP_POW "Temp: Pow"
#define ATTR_KEY_TEMP_SET "Temp setpoint"


#define ATTR_KEY_PXFMT "Px format"
#define ATTR_VAL_PXFMT_COLOR "Color"
#define ATTR_VAL_PXFMT_BW "B&W"
#define ATTR_KEY_REFRESH "Refresh attrs after each shot"

#define PG_DEFAULT_FILENAME "STI_UNNAMED_IMAGE.tif"

// TODO: Implement:
#define ATTR_KEY_TRIGGER "Trigger"

enum PropField {
	EMPTY,
	AUTO,
	ABS
};

struct AttributeProp {
	PropField field;
	PropField friendField;
	string friendAttribute;
	FlyCapture2::PropertyType type;
};

class PointGreyDevice : public STI_Device
{
public:
	PointGreyDevice::PointGreyDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber,
                             unsigned int serialNo,
                             string logDirectory);
	~PointGreyDevice();

	bool initialized;

private:
	FlyCapture2::Camera *camera;
	FlyCapture2::PGRGuid guid;
	FlyCapture2::PixelFormat pxfmt;
	FlyCapture2::CameraInfo camInfo;
	bool deviceMain(int argc, char** argv);  // called in a loop while it returns true
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);
	void stopEventPlayback() { return; }
	void pauseEventPlayback() { return; }
	void resumeEventPlayback() { return; }
	std::string PointGreyDevice::attrValue(std::string key);
	void PointGreyDevice::setAttrIfDifferent(std::string key, std::string value);
	void PointGreyDevice::setAttrFromValue(std::string key);
	bool PointGreyDevice::wasError(FlyCapture2::Error &error);

	// TODO: Maybe these can be determined dynamically?
	bool cameraCapturing;
	bool cameraConnected;

	void PointGreyDevice::setSoftAttrFromCam(std::string attr, FlyCapture2::PropertyType type, bool aut);
	
	// Mapping of attribute name to the PropertyType and field of a camera property
	std::map<std::string, AttributeProp>* attrProps;
	bool attrsRefreshing; // Hack, see ticket #14
	bool refreshAttrsAfterShot;
protected:
	// Device partners
	void definePartnerDevices() { return;};

    // Device Channels
	void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value) { return false; };

	// Device Command line interface setup
	std::string execute(int argc, char** argv) { return string("FIXME: IMPLEMENT"); };
	void parseDeviceEvents(const RawEventMap& eventsIn,
		SynchronousEventVector& eventsOut) throw(std::exception);
};