/*! \file PointGreyDevice.cpp
 *  \author Brannon Klopfer
 *  \brief Source-file for the class PointGreyDevice
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

#include "PointGreyDevice.hpp"

//#define PRINTF_DEBUG
#ifdef PRINTF_DEBUG
#define DEBUGHERE cerr << __FUNCTION__ << " (" << __LINE__ << ")" << endl
#define IMPLEMENT cerr << "Implement (if needed): " <<  __FUNCTION__ << "() in " << __FILE__ << ":" << __LINE__ << endl
#define FIXME(feature) cerr << "FIXME: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define TODO(something) cerr << "TODO: " << something << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define ADD_FEATURE(feature) cerr << "TODO: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define DEBUG(msg) cerr << __FUNCTION__ << "(): " << msg << endl
#else
#define DEBUGHERE // Do nothing if PRINTF_DEBUG isn't defined
#define IMPLEMENT
#define FIXME(feature)
#define TODO(something)
#define ADD_FEATURE(feature)
#define DEBUG(msg)
#endif

#define bool2yesno(b) (b ? "yes" : "no")
#define bool2tf(b) (b ? "true" : "false")

using namespace std;
using namespace FlyCapture2;

PointGreyDevice::PointGreyDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber, // Ugh, if this were unsigned int we could fit the serial #...
                             unsigned int serialNo,
                             string logDirectory) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	initialized = false;
	cameraConnected = false;
	cameraCapturing = false;
	Error error;
	attrsRefreshing = true; // Hack, see ticket #14
	refreshAttrsAfterShot = true;

	/* Dynamic, clean these up: */
	// Kindly given to us by the Point Grey SDK
	camera = new Camera();
	attrProps = new map<std::string, AttributeProp>();
	/* End dynamic */

	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (wasError(error)) return;
	
	DEBUG("Number of cameras connected: " << numCameras);

	error = busMgr.GetCameraFromSerialNumber(serialNo, &guid);
	if (wasError(error)) {
		cerr << "Uh-oh, could not find camera with S/N "
		     << serialNo << endl;

		return;
	}
	
	// Connect to a camera
	error = camera->Connect(&guid);
	if (wasError(error)) return;

	cameraConnected = true;

	error = camera->GetCameraInfo(&camInfo);
	if (wasError(error)) return;

	// Color, if available
	if (camInfo.isColorCamera)
		pxfmt = PIXEL_FORMAT_RGB;
	else
		pxfmt = PIXEL_FORMAT_MONO8;

	TriggerMode trig;
	trig.onOff = true;

	camera->SetTriggerMode(&trig);

	initialized = true;
}

PointGreyDevice::~PointGreyDevice()
{
	TODO("Verify we're all cleaned up!");

	// Stop capturing images
	if (cameraCapturing)
		wasError(camera->StopCapture());

	// Disconnect the camera
	if (cameraConnected)
		wasError(camera->Disconnect());

	free(camera);
	free(attrProps);
}

bool PointGreyDevice::deviceMain(int argc, char** argv)
{
	// This whole thing is somewhat hacky.
	// See ticket #14

	// This will handle the attrsRefreshing bool
	refreshDeviceAttributes(); // Fill in the values for the client
	
	// Start capturing images (we're triggered, so this isn't wasteful)
	Error error = camera->StartCapture();
	if (!wasError(error))
		cameraCapturing = true;

	return false;
}

bool PointGreyDevice::wasError(Error &error)
{
	if (error == PGRERROR_OK)
		return false;

	error.PrintErrorTrace();

	return true;
}

void PointGreyDevice::defineChannels()
{
	DEBUGHERE;

	addInputChannel(0, DataString, ValueString, "Pic");
	addInputChannel(1, DataDouble, ValueNumber, "Auto Adjust");
	
	return;
}

bool PointGreyDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	Error error;
	Image rawImage;
	
	// Take picture
	if (channel == 0) {
		Image convertedImage;

		error = camera->FireSoftwareTrigger();
		if (wasError(error)) return false;

		error = camera->RetrieveBuffer(&rawImage);
		if (wasError(error)) return false;

		error = rawImage.Convert(pxfmt, &convertedImage);
		if (wasError(error)) return false;

		// Deal with filename
		string filename = valueIn.getString();

		if (filename.empty())
			filename = PG_DEFAULT_FILENAME;

		// Save the image. If a file format is not passed in, then the file
		// extension is parsed to attempt to determine the file format.
		error = convertedImage.Save(filename.c_str());
		if (wasError(error)) return false;

		if (dataOut != NULL) dataOut.addValue(filename);

	} else if (channel == 1) {
		// Attempt to auto-adjust the image by just taking a bunch of shots...
		int nshots = 10;

		// If a number was provided, take that many shots
		if (valueIn.getType() == MixedValue::Int ||
				valueIn.getType() == MixedValue::Double)
			nshots = valueIn.getInt();

		for (int i = 0; i < nshots; i++) {
			error = camera->FireSoftwareTrigger();
			if (wasError(error)) return false;

			error = camera->RetrieveBuffer(&rawImage);
			if (wasError(error)) return false;
		}

		if (dataOut != NULL) dataOut.addValue(nshots);
	}

	if (refreshAttrsAfterShot)
			refreshDeviceAttributes();

	return true;
}

void PointGreyDevice::defineAttributes()
{
	Error er;
	
	// These properties are taken from FlyCapture2Defs.h
	map<string, PropertyType> props;
	props.insert(pair<string, PropertyType>("Brightness", PropertyType::BRIGHTNESS));
	props.insert(pair<string, PropertyType>("Auto Exposure", PropertyType::AUTO_EXPOSURE));
	props.insert(pair<string, PropertyType>("Sharpness", PropertyType::SHARPNESS));
	props.insert(pair<string, PropertyType>("White balance", PropertyType::WHITE_BALANCE));
	props.insert(pair<string, PropertyType>("Hue", PropertyType::HUE));
	props.insert(pair<string, PropertyType>("Saturation", PropertyType::SATURATION));
	props.insert(pair<string, PropertyType>("Gamma", PropertyType::GAMMA));
	props.insert(pair<string, PropertyType>("Iris", PropertyType::IRIS));
	props.insert(pair<string, PropertyType>("Focus", PropertyType::FOCUS));
	props.insert(pair<string, PropertyType>("Zoom", PropertyType::ZOOM));
	props.insert(pair<string, PropertyType>("Pan", PropertyType::PAN));
	props.insert(pair<string, PropertyType>("Tilt", PropertyType::TILT));
	props.insert(pair<string, PropertyType>("Shutter", PropertyType::SHUTTER));
	props.insert(pair<string, PropertyType>("Gain", PropertyType::GAIN));
	// Not sure what trigger mode (auto, manual) actually does...
	props.insert(pair<string, PropertyType>("Trigger mode", PropertyType::TRIGGER_MODE));
	props.insert(pair<string, PropertyType>("Trigger delay", PropertyType::TRIGGER_DELAY));
	props.insert(pair<string, PropertyType>("Frame rate", PropertyType::FRAME_RATE));
	props.insert(pair<string, PropertyType>("Temperature", PropertyType::TEMPERATURE));

	map<string, PropertyType>::iterator it;
	for(it = props.begin(); it != props.end(); it++) {
		Property prop;
		PropertyInfo propi;
		prop.type = it->second;
		propi.type = it->second;
		er = camera->GetPropertyInfo(&propi);
		DEBUG("Property: " << it->first << ", present: "
			<< propi.present << ", Manual supported: " << propi.manualSupported << endl
			<< "on off: " << propi.onOffSupported
			<< ", abs supported:" << propi.absValSupported << endl
			<< "Read supported?" << bool2yesno(propi.readOutSupported));
		
		// For properties set by value (e.g., shutter in ms)
		string autoName, absName;
		bool hasAuto = propi.autoSupported && propi.manualSupported;
		bool hasAbs = propi.manualSupported && propi.absValSupported;
		
		if (hasAbs) {
			absName = it->first;

			if (strlen(propi.pUnitAbbr)) {
				absName += " (";
				absName += propi.pUnitAbbr;
				absName += ")";
			} else absName += " value"; // No units associated with this setting (e.g., gamma)

			addAttribute(absName, "");
		}

		// For automatic properties (e.g., auto exposure)
		if (hasAuto) {
			autoName = it->first;

			addAttribute(autoName,
			             (prop.autoManualMode ? "auto" : "man"),
			             "auto,man");
			
			AttributeProp ap;
			ap.field = AUTO;
			ap.type = it->second;
			// TODO: Maybe the dependency is more complicated? e.g., need list of friendAttributes...?
			if (hasAbs) {
				ap.friendAttribute = absName; // This will need to be updated.
				ap.friendField = ABS;
			}
			attrProps->insert(pair<string, AttributeProp>(autoName, ap));
		}

		if (hasAbs) {
			AttributeProp ap;
			ap.field = ABS;
			ap.type = it->second;
			if (hasAuto) {
				ap.friendAttribute = autoName; // This will need to be updated.
				ap.friendField = AUTO;
			}

			attrProps->insert(pair<string, AttributeProp>(absName, ap));
		}
	}

	string selectedPxFmt;
	if (pxfmt == PIXEL_FORMAT_RGB)
		selectedPxFmt = ATTR_VAL_PXFMT_COLOR;
	else
		selectedPxFmt = ATTR_VAL_PXFMT_BW;

	addAttribute(ATTR_KEY_PXFMT, selectedPxFmt,
	                             ATTR_VAL_PXFMT_BW ","
	                             ATTR_VAL_PXFMT_COLOR);

	// For auto settings, this will update the set value after each shot
	addAttribute(ATTR_KEY_REFRESH, bool2yesno(refreshAttrsAfterShot), "yes,no");
}

bool PointGreyDevice::updateAttribute(std::string key, std::string value)
{
	DEBUG("Updating " << key << " to "
		<< value << " (hardware update: "
		<< bool2yesno(!attrsRefreshing) << ")");

	if (attrsRefreshing) return true;  // Hack, see ticket #14

	/* Deal with a Property of the camera */
	if (attrProps->count(key)) {
		Error er, setPropEr;
		// If camera
		bool cameraWasCapturing = cameraCapturing;
		if (cameraCapturing) {
			DEBUG("Stopping capture...");
			er = camera->StopCapture();
			if (wasError(er)) return false;
			cameraCapturing = false;
		}

		AttributeProp ap = attrProps->at(key);
		Property p;
		p.type = ap.type;
		camera->GetProperty(&p);

		// Is this a value, or on/off, or...
		if (ap.field == ABS) {
			istringstream iss(value);
			p.autoManualMode = false;
			p.absControl = true;
			iss >> p.absValue;

		} else if (ap.field == AUTO) {
			// TODO: Sort out what all needs to be done here.
			// There's a chance that p.absControl needs attention...
			if (value == "auto") {
				p.autoManualMode = true;
			} else if (value == "man") {
				p.autoManualMode = false;
			}
		}

		setPropEr = camera->SetProperty(&p);

		// Return camera to previous state
		if (cameraWasCapturing) {
			DEBUG("Restarting capture");
			er = camera->StartCapture();
			if (wasError(er)) return false;
			cameraCapturing = true;
		}

		if (wasError(setPropEr)) {
			cerr << "WARNING: Failed to set property..." << endl;

			return false;
		}

		return true;
	}

	/* Deal with non-hardware attributes (i.e., not a Property of the camera) */
	if (key == ATTR_KEY_PXFMT) {
		if (value == ATTR_VAL_PXFMT_BW)
			pxfmt = PIXEL_FORMAT_MONO8;
		else if (value == ATTR_VAL_PXFMT_COLOR)
			pxfmt = PIXEL_FORMAT_RGB;
		else
			return false;

		return true;
	}

	if (key == ATTR_KEY_REFRESH) {
		if (value != "yes" && value != "no")
			return false;

		refreshAttrsAfterShot = (value == "yes");

		return true;
	}

	return false;
}

// Update our notion of the attribute value from the camera's hardware
void PointGreyDevice::setSoftAttrFromCam(string attr, PropertyType type, bool aut)
{
	Property p;
	p.type = type;
	camera->GetProperty(&p);
	if (aut) {
		DEBUG("soft set: setting " << attr << " to " << (p.autoManualMode ? "auto" : "man") );
		setAttribute(attr, (p.autoManualMode ? "auto" : "man"));
	} else {
		DEBUG("abs value: setting " << attr << " to " << p.absValue);
		setAttribute(attr, p.absValue);
	}
}

void PointGreyDevice::refreshAttributes()
{
	// See ticket #14
	attrsRefreshing = true;

	map<string, AttributeProp>::iterator it;
	for(it = attrProps->begin(); it != attrProps->end(); it++) {
		string key = it->first;
		AttributeProp ap = it->second;
		setSoftAttrFromCam(key, ap.type, (ap.field == AUTO ? true : false));
	}

	attrsRefreshing = false;
}

void PointGreyDevice::parseDeviceEvents(const RawEventMap& eventsIn,
                                      SynchronousEventVector& eventsOut)
                                     throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}