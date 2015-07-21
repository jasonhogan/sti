/*! \file PcoSensicamDevice.cpp
 *  \author Brannon Klopfer
 *  \brief Source-file for the class PcoSensicamDevice
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

//#include <intsafe.h> // DWORD

//#include <windows.h> // shizzle...DWORD, HANDLE


//#include <WinDef.h>
//#include <intsafe.h> // DWORD
#include <string>
#include "stdafx.h"
//CString duder;
//#include <WinNT.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>

#include "PcoSensicamDevice.hpp"


#include <ORBManager.h>

//#define PRINTF_DEBUG
#ifdef PRINTF_DEBUG
#define DEBUGHERE cerr << __FUNCTION__ << " (" << __LINE__ << ")" << endl
#define IMPLEMENT cerr << "Implement (if needed): " <<  __FUNCTION__ << "() in " << __FILE__ << ":" << __LINE__ << endl
#define FIXME(feature) cerr << "FIXME: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define ADD_FEATURE(feature) cerr << "TODO: " << feature << __FILE__ << ":" << __LINE__ << " [" << __FUNCTION__ << "]" << endl
#define DEBUG(msg) cerr << __FUNCTION__ << "(): " << msg << endl
#else
#define DEBUGHERE // Do nothing if PRINTF_DEBUG isn't defined
#define IMPLEMENT
#define FIXME(feature)
#define ADD_FEATURE(feature)
#define DEBUG(msg)
#endif

using namespace std;
//using namespace boost;

PcoSensicamDevice::PcoSensicamDevice(ORBManager* orb_manager,
                             string DeviceName,
                             string Address,
                             unsigned short ModuleNumber,
                             string logDirectory) :
STI_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory)
{
	initialized = false;
	DWORD err = 0;
	camera = new CameraAbs();
	initialized = true;
}
PcoSensicamDevice::~PcoSensicamDevice()
{
	delete(camera);
}


void PcoSensicamDevice::defineChannels()
{
	// Take single picture
	addInputChannel(CHAN_PIC, DataVector, ValueVector, "Pic");
	addInputChannel(CHAN_PHOTONCOUNT, DataVector, ValueVector, "PhotonCount");
}

bool PcoSensicamDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	cerr << "No write channels available!" << endl;
	return false;
}

bool PcoSensicamDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	string filename = valueIn.getString();
	if (dataOut != NULL)
		dataOut.addValue(filename);
	if (channel == CHAN_PIC) {
		camera->takePicture(filename);
	} else if (channel == CHAN_PHOTONCOUNT) {
		camera->photonCount(filename, npics);
	}

	return true;
}
void PcoSensicamDevice::parseDeviceEvents(const RawEventMap& eventsIn,
                                      SynchronousEventVector& eventsOut)
                                     throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void PcoSensicamDevice::defineAttributes()
{
	string defaultExposure = "100"; // 10 ms
	addAttribute(ATTR_KEY_EXPOSURE, defaultExposure);

	// TODO: Grab available binning options from camera.
	addAttribute(ATTR_KEY_HBIN, 1, "1,2,4,8");
	addAttribute(ATTR_KEY_VBIN, 1, "1,2,4,8,16");

	// FIXME: Using strings as vectors, is this the proper STI way of doing things?
	addAttribute(ATTR_KEY_ROI, "0,0,0,0");

	addAttribute(ATTR_KEY_GAIN, 0, "0,1,3");

	addAttribute(ATTR_KEY_THRESH, 80);
	addAttribute(ATTR_KEY_PHOTONRAD, 1);
	addAttribute(ATTR_KEY_NPICS, 1);

	// TODO: Add trigger control.

	refreshAttributes();
}

void PcoSensicamDevice::refreshAttributes()
{
	/* These are attributes stored on camera. */
	setAttrFromValue(ATTR_KEY_EXPOSURE);
	setAttrFromValue(ATTR_KEY_HBIN);
	setAttrFromValue(ATTR_KEY_VBIN);
	setAttrFromValue(ATTR_KEY_ROI);
}

// Call setAttribute() with the actual value. I think this is what I need to do to update the client...?
string PcoSensicamDevice::attrValue(string key)
{
	stringstream ss;
	if (key == ATTR_KEY_HBIN) {
		ss << camera->getHbin();
	} else if (key == ATTR_KEY_VBIN) {
		ss << camera->getVbin();
	} else if (key == ATTR_KEY_EXPOSURE) {
		ss << camera->getExposureMs();
	} else if (key == ATTR_KEY_GAIN) {
		ss << camera->getGain();	
	}
	return ss.str();
}

void PcoSensicamDevice::setAttrIfDifferent(string key, string value)
{
	if (value != attrValue(key))
		setAttribute(key, value);
}
// This will just call setAttribute() with the current (hopefully hw!) value
void PcoSensicamDevice::setAttrFromValue(string key)
{
	setAttribute(key, attrValue(key));
}

bool PcoSensicamDevice::updateAttribute(std::string key, std::string value)
{
	// TODO: Error checking. This is somewhat sloppy.
	if (key == ATTR_KEY_EXPOSURE) {
		istringstream i(value);
		double exposure;
		if (!(i >> exposure)) {
			cerr << "Invalid exposure setting!" << endl;

			return false;
		}

		camera->setExposureMs(exposure);
	} else if (key == ATTR_KEY_TRIGGER) {
		IMPLEMENT;
	} else if (key == ATTR_KEY_HBIN) {
		istringstream i(value);
		short bin;
		i >> bin;
		camera->setHbin(bin);

		return camera->getHbin();
	} else if (key == ATTR_KEY_VBIN) {
		istringstream i(value);
		short bin;
		i >> bin;
		camera->setVbin(bin);

		return camera->getVbin();
	} else if (key == ATTR_KEY_GAIN) {
		istringstream i(value);
		short gain;
		i >> gain;
		camera->setGain(gain);
	} else if (key == ATTR_KEY_PHOTONRAD) {
		istringstream i(value);
		int rad;
		i >> rad;
		camera->singlePhotonRad = rad;
	}  else if (key == ATTR_KEY_THRESH) {
		istringstream i(value);
		int thresh;
		i >> thresh;
		camera->singlePhotonThresh = thresh;
	} else if (key == ATTR_KEY_NPICS) {
		istringstream i(value);
		i >> npics;
	}

	camera->commitAttributes(); // Force camera to accept changes.

	return true; // eh?
}
