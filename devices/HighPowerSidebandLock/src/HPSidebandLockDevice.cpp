/*! \file HPSidebandLockDevice.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class HPSidebandLockDevice
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

#include "HPSidebandLockDevice.h"

#include <iostream>

HPSidebandLockDevice::HPSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	dynamicFeedbackValue = DynamicValue_ptr(new DynamicValue());
}
HPSidebandLockDevice::~HPSidebandLockDevice()
{
}

void HPSidebandLockDevice::defineChannels()
{
	addInputChannel(0, DataDouble, ValueNumber, "Lock Loop");
}

void HPSidebandLockDevice::definePartnerDevices()
{
	addPartnerDevice("Sensor", "ep-timing1.stanford.edu", 3, "Analog In");
	addPartnerDevice("Actuator", "ep-timing1.stanford.edu", 4, "Slow Analog Out");

	partnerDevice("Sensor").enablePartnerEvents();
	partnerDevice("Actuator").enablePartnerEvents();
}

void HPSidebandLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	double lockSetpoint = 0;
	unsigned lockLoopChannel = 0;

	unsigned short analogChannel = 0;

	double dtFeedback = 20.0e6;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The HP Sideband Lock cannot currently have multiple events at the same time.");
		}
		
		if(events->second.at(0).channel() == lockLoopChannel) {
			lockSetpoint = events->second.at(0).value().getNumber();

			sensorCallback = MeasurementCallback_ptr(new HPLockCallback(this));

//			partnerDevice("Sensor").meas(events->first, analogChannel, 1, events->second.at(0), "Measure PD voltage");
			partnerDevice("Sensor").meas(events->first, analogChannel, 1, events->second.at(0), sensorCallback, "Measure PD voltage");
			

			tmp = lockSetpoint;
			dynamicFeedbackValue->setValue(lockSetpoint);
			partnerDevice("Actuator").event(events->first + dtFeedback, 0, dynamicFeedbackValue, events->second.at(0), "Feedback on VCA");
//			partnerDevice("Actuator").event(events->first + dtFeedback, 0, lockSetpoint, events->second.at(0), "Feedback on VCA");
		}

//		eventsOut.push_back(
//			new HPSidebandLockEvent(events->first + dtFeedback, this) );

	}
}

void HPSidebandLockDevice::HPLockCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;
	
	_this->tmp += 0.2;
	_this->dynamicFeedbackValue->setValue(_this->tmp);

	cout << "Measurement: " << measurement.data.doubleVal() << endl;
//	_this
}
