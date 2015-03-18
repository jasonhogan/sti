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
	DEBUG("OPENING CAMERA");
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
	addInputChannel(0, DataVector, ValueVector, "Pic");
}

bool PcoSensicamDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	cerr << "No write channels available!" << endl;
	return false;
}

bool PcoSensicamDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	DEBUG("Taking picture (read)...");

	DEBUG("ValueIn: " << valueIn.getString());



	string filename = valueIn.getString();
	if (dataOut != NULL) dataOut.addValue(filename);
	camera->takePicture(filename);
	/*DWORD status = 0;
	status = camera->GetImage(pcobuf);
	std:cout << "Status: " << status << ", " << pcobuf->Width() << "x" << pcobuf->Height() << endl;
	*/
	/*
	if (filename.empty()) {
		filename = PcoSensicam_DEFAULT_FILENAME;
	}

	if (dataOut != NULL) dataOut.addValue(filename);

	int rval = camera->takePicture(filename);

	if (rval != PcoSensicam_ERROR_NONE) {
		cerr << "Bad news bears. Camera says:" << endl;
		camera->printErrors();
	}

	return (rval == PcoSensicam_ERROR_NONE); */
	
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
	string defaultExposure = "10"; // 10 ms
	addAttribute(ATTR_KEY_EXPOSURE, defaultExposure);

	// TODO: Grab available binning options from camera.
	addAttribute(ATTR_KEY_HBIN, 1, "1,2,4");
	addAttribute(ATTR_KEY_VBIN, 1, "1,2,4");

	// FIXME: Using strings as vectors, is this the proper STI way of doing things?
	addAttribute(ATTR_KEY_ROI, "0,0,0,0");

	// Temperatures.
	addAttribute(ATTR_KEY_TEMP_CCD, "");
	addAttribute(ATTR_KEY_TEMP_ELEC, "");
	addAttribute(ATTR_KEY_TEMP_POW, "");
	// The pco.edge 5.5 seems to have a fixed setpoint of 5C,
	// so no need to show the setpoint.

	// TODO: Add trigger control.

	refreshAttributes();
}

void PcoSensicamDevice::refreshAttributes()
{
	DEBUG("Refreshing attributes!");

	setAttrFromValue(ATTR_KEY_EXPOSURE);
	setAttrFromValue(ATTR_KEY_HBIN);
	setAttrFromValue(ATTR_KEY_VBIN);
	setAttrFromValue(ATTR_KEY_ROI);

	// Deal with temps:
	short ccdt, elect, powt;
	stringstream ss;

	/*
	camera->getTemperature(ccdt, elect, powt);
	ss << ccdt;
	setAttribute(ATTR_KEY_TEMP_CCD, ss.str());
	ss.str("");
	ss.clear();*/
	ss << elect;
	setAttribute(ATTR_KEY_TEMP_ELEC, ss.str());
	ss.str("");
	ss.clear();
	ss << powt;
	setAttribute(ATTR_KEY_TEMP_POW, ss.str());
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
	} /*else if (key == ATTR_KEY_ROI) {
		PcoEdge_ROI roi = camera->getRoi();
		ss << roi.x1 << ','
		   << roi.x2 << ','
		   << roi.y1 << ','
		   << roi.y2;
	}
	*/
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
	} /*else if (key == ATTR_KEY_ROI) {
		// NB: For some reason, boost::split() won't compile. Maybe boost+VC++'s fault?
		// split(roistr, value, boost::is_any_of(", "), boost::token_compress_on);
		// TODO: Better error checking!
		short roi[4];
		using namespace boost;
		char_separator<char> sep(", ");
		tokenizer< char_separator<char> > tokens(value, sep);
		int i = 0;
		BOOST_FOREACH (const string& t, tokens) {
			istringstream j(t);
			j >> roi[i++];
		}

		// If in doubt, set to full resolution
		if (i != 4)
			roi[0] = roi[1] = roi[2] = roi[3] = 0;

		// ROI is constrained in the vertical direction to be symmetric. Specify the top pixel.
		camera->SetROI(roi[0], roi[1], roi[2], roi[3]);
	}
	*/

	camera->commitAttributes(); // Force camera to accept changes.

	return true; // eh?
}
