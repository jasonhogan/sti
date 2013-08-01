/*! \file HighPowerIntensityLock.cpp
 *  \author Tim Kovachy
 *  \author Jason Hogan
 *  \brief Source-file for the class HighPowerIntensityLock
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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

#include "HighPowerIntensityLockDevice.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using STI::Utils::valueToString;
using namespace std;

HighPowerIntensityLockDevice::HighPowerIntensityLockDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber),
emissionStatusBitNum(2)
{
	gain = 1;

	dynamicIntensitySetpoint = DynamicValue_ptr(new DynamicValue());
}

HighPowerIntensityLockDevice::~HighPowerIntensityLockDevice()
{
}

void HighPowerIntensityLockDevice::defineChannels()
{
	addInputChannel(0, DataDouble, ValueNumber, "Lock Loop");
}

void HighPowerIntensityLockDevice::definePartnerDevices()
{
	addPartnerDevice("Sensor", "ep-timing1.stanford.edu", 3, "Analog In");
	addPartnerDevice("Actuator", "ep-timing1.stanford.edu", 6, "Fast Analog Out");

	partnerDevice("Sensor").enablePartnerEvents();
	partnerDevice("Actuator").enablePartnerEvents();
}


void HighPowerIntensityLockDevice::defineAttributes()
{

	addAttribute("Gain", gain);
}


void HighPowerIntensityLockDevice::refreshAttributes()
{

	setAttribute("Gain", gain);	
}

bool HighPowerIntensityLockDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	std::string result;
	
	if( key.compare("Gain") == 0 ) {
		double newGain;
		if( STI::Utils::stringToValue(value, newGain) && newGain > 0 ) {
			gain = newGain;
			success = true;
		}
	}

	return success;
}

void HighPowerIntensityLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

//	double lockSetpoint = 0;
	unsigned lockLoopChannel = 0;
	unsigned vcaSetpointChannel = 10;

	unsigned short analogChannel = 0;

	double dtFeedback = 100.0e6;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The HP Sideband Lock cannot currently have multiple events at the same time.");
		}
		
		if(events->second.at(0).channel() == lockLoopChannel) {
//			lockSetpoint = events->second.at(0).value().getNumber();

			dynamicIntensitySetpoint->setValue(nextVCA);

			sensorCallback = MeasurementCallback_ptr(new HPLockCallback(this));

			partnerDevice("Sensor").meas(events->first, analogChannel, ValueNone, events->second.at(0), sensorCallback, "Measure PD voltage");
			
			partnerDevice("Actuator").event(events->first + dtFeedback, 0, dynamicIntensitySetpoint, events->second.at(0), "Feedback on VCA");
//			partnerDevice("Actuator").event(events->first + dtFeedback, 0, nextVCA, events->second.at(0), "Feedback on VCA");

			//Add an measurement event to the HP Intensity Lock Device (this will record the VCA feedback value as a measuremnt)
			eventsOut.push_back( new HPIntensityLockEvent(events->first, this) );
			eventsOut.back().addMeasurement( events->second.at(0) );
		}
	}
}

void HighPowerIntensityLockDevice::intensityLockLoop(double errorSignal)
{	
	nextVCA = lastVCA + gain*errorSignal;
	lastVCA = nextVCA;

	dynamicIntensitySetpoint->setValue(nextVCA);
}

void HighPowerIntensityLockDevice::HPLockCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;
	double gainLocal = _this->gain;
	double setpointLocal = _this->lockSetpoint;
	double error;
	double measure = measurement.data.doubleVal(); 
	error = setpointLocal - measure;
//	_this->nextVCA = _this->lastVCA + gainLocal*error;
//	_this->lastVCA = _this->nextVCA;
	_this->intensityLockLoop(error);
}


void HighPowerIntensityLockDevice::HPIntensityLockEvent::collectMeasurementData()
{
	//save the current value of the VCA setpoint as a measurement for the HP Intensity Lock device
	eventMeasurements.at(0)->setData( _this->nextVCA );
}

