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
	std::string configFilename) : 
STI_Device_Adapter(orb_manager, DeviceName, configFilename)
{
	initialized = true;
	dynamicTemperatureSetpoint = DynamicValue_ptr(new DynamicValue());

	configFile = ConfigFile_ptr(new ConfigFile(configFilename));

	sensorChannel = 0;
	if (!(configFile->getParameter("sensorChannel", sensorChannel)))
		initialized = false;
}
HPSidebandLockDevice::~HPSidebandLockDevice()
{
}

void HPSidebandLockDevice::defineChannels()
{
	addInputChannel(0, DataVector, ValueNumber, "Lock Loop");
}


void HPSidebandLockDevice::definePartnerDevices()
{

	std::string sensorIP = "";
	short sensorModule = 0;
	std::string sensorDeviceName = "";
	
	configFile->getParameter("sensorIP", sensorIP);
	configFile->getParameter("sensorModule", sensorModule);
	configFile->getParameter("sensorDeviceName", sensorDeviceName);

	addPartnerDevice("Sensor", sensorIP, sensorModule, sensorDeviceName);


	std::string actuatorIP = "";
	short actuatorModule = 0;
	std::string actuatorDeviceName = "";
	
	configFile->getParameter("actuatorIP", actuatorIP);
	configFile->getParameter("actuatorModule", actuatorModule);
	configFile->getParameter("actuatorDeviceName", actuatorDeviceName);

	addPartnerDevice("Actuator", actuatorIP, actuatorModule, actuatorDeviceName);

	partnerDevice("Sensor").enablePartnerEvents();
	partnerDevice("Actuator").enablePartnerEvents();
}

void HPSidebandLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	double actuatorSetpoint = 0;
	unsigned lockLoopChannel = 0;

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

			std::vector<double> scopeSettings;
			scopeSettings.push_back(0.01);
			scopeSettings.push_back(1);
			scopeSettings.push_back(0);
			scopeSettings.push_back(1000);

			MixedValue scopeSettingsMixed;
			scopeSettingsMixed.setValue(scopeSettings);

//			partnerDevice("Sensor").meas(events->first, analogChannel, 1, events->second.at(0), "Measure PD voltage");
			partnerDevice("Sensor").meas(events->first, sensorChannel, scopeSettingsMixed, events->second.at(0), sensorCallback, "Measure sidebands");
			

			//tmp = actuatorSetpoint;
			//dynamicTemperatureSetpoint->setValue(actuatorSetpoint);
			//partnerDevice("Actuator").event(events->first + dtFeedback, 0, dynamicTemperatureSetpoint, events->second.at(0), "Feedback on crystal temperature");
//			partnerDevice("Actuator").event(events->first + dtFeedback, 0, lockSetpoint, events->second.at(0), "Feedback on VCA");
		}

		//Add a measurement event to record results of the lock loop
		eventsOut.push_back(new HPSidebandLockEvent(events->first + dtFeedback, this) );
		eventsOut.back().addMeasurement( events->second.at(0) );

	}
}

void HPSidebandLockDevice::HPLockCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;
	
	_this->tmp += 0.2;
	_this->dynamicTemperatureSetpoint->setValue(_this->tmp);

	MixedData myDataVector;
	myDataVector.setValue(measurement.data.vector());
	cout << "Measurement: " << myDataVector.getVector().at(0).getDouble() << endl;
//	_this
	_this->lastFeedbackResults.clear();
	_this->lastFeedbackResults.push_back(1234);
}

void HPSidebandLockDevice::HPSidebandLockEvent::collectMeasurementData()
{
	//save the current value of the VCA setpoint as a measurement for the HP Intensity Lock device
	eventMeasurements.at(0)->setData( _this->lastFeedbackResults );
}