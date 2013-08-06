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

	gain = 1;
	asymmetrySetpoint = 0;
}
HPSidebandLockDevice::~HPSidebandLockDevice()
{
}

void HPSidebandLockDevice::defineAttributes()
{

	//Contact arroyo to determine initial temperature setpoint
	//Note that defineAttributes does NOT get called until after all the partners are registered.
	//Channel 2 for Arroyos is the read on the temperature, channel 0 allows a general query to get the temperature setpoint
	MixedValue valueIn;
	valueIn.setValue("TEC:SET:T?");
	MixedData dataOut;
	bool success = partnerDevice("Arroyo").read(0, valueIn, dataOut);
	double tempSetpoint;
	if (success && STI::Utils::stringToValue(dataOut.getString(), tempSetpoint))
		temperatureSetpoint = tempSetpoint;
	else
		cout << "Could not contact Arroyo to determine current temperature setpoint" << endl;

	addAttribute("Gain", gain);
	addAttribute("Crystal Temp. Setpoint (deg C)", temperatureSetpoint);
}

void HPSidebandLockDevice::refreshAttributes()
{
	addAttribute("Gain", gain);
	addAttribute("Crystal Temp. Setpoint (deg C)", temperatureSetpoint);
}

bool HPSidebandLockDevice::updateAttribute(std::string key, std::string value)
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
	else if( key.compare("Crystal Temp. Setpoint (deg C)") == 0 ) {
		double newSetpoint;
		if( STI::Utils::stringToValue(value, newSetpoint) ) {
			temperatureSetpoint = newSetpoint;
			success = true;
		}
	}

	return success;
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


	std::string arroyoIP = "";
	short arroyoModule = 0;
	std::string arroyoDeviceName = "";
	
	configFile->getParameter("arroyoIP", arroyoIP);
	configFile->getParameter("arroyoModule", arroyoModule);
	configFile->getParameter("arroyoDeviceName", arroyoDeviceName);

	addPartnerDevice("Arroyo", arroyoIP, arroyoModule, arroyoDeviceName);

	partnerDevice("Sensor").enablePartnerEvents();
	partnerDevice("Arroyo").enablePartnerEvents();

}

void HPSidebandLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	//double arroyoSetpoint = 0;
	unsigned lockLoopChannel = 0;

	double dtFeedback = 20.0e6;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The HP Sideband Lock cannot currently have multiple events at the same time.");
		}
		
		if(events->second.at(0).channel() == lockLoopChannel) {
			asymmetrySetpoint = events->second.at(0).value().getNumber();

			sensorCallback = MeasurementCallback_ptr(new HPLockCallback(this));

			
			std::vector<double> scopeSettings;
			/*scopeSettings.push_back(1);
			scopeSettings.push_back(1);
			scopeSettings.push_back(0);
			scopeSettings.push_back(1000);*/

			//Use empty vector to use default scope settings
			MixedValue scopeSettingsMixed;
			scopeSettingsMixed.setValue(scopeSettings);
			

//			partnerDevice("Sensor").meas(events->first, analogChannel, 1, events->second.at(0), "Measure PD voltage");
			partnerDevice("Sensor").meas(events->first, sensorChannel, scopeSettingsMixed, events->second.at(0), sensorCallback, "Measure sidebands");
			
			//tmp = arroyoSetpoint;
			//dynamicTemperatureSetpoint->setValue(arroyoSetpoint);
			//partnerDevice("Arroyo").event(events->first + dtFeedback, 0, dynamicTemperatureSetpoint, events->second.at(0), "Feedback on crystal temperature");
//			partnerDevice("Arroyo").event(events->first + dtFeedback, 0, lockSetpoint, events->second.at(0), "Feedback on VCA");
		}

		//Add a measurement event to record results of the lock loop
		eventsOut.push_back(new HPSidebandLockEvent(events->first + dtFeedback, this) );
		eventsOut.back().addMeasurement( events->second.at(0) );

	}
}

void HPSidebandLockDevice::asymmetryLockLoop(double errorSignal)
{	
//	nextVCA = lastVCA + gain * errorSignal;
//	lastVCA = nextVCA;

	temperatureSetpoint += gain * errorSignal;

	dynamicTemperatureSetpoint->setValue(temperatureSetpoint);

	refreshDeviceAttributes();	//update the attribute text file and the client
}
void HPSidebandLockDevice::HPLockCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;
	
	MixedData myDataVector;
	myDataVector.setValue(measurement.data.vector());
	
	//
	//	Magical mathematica code
	//

	_this->lastFeedbackResults.clear();
	_this->lastFeedbackResults.push_back(1234);
}

void HPSidebandLockDevice::HPSidebandLockEvent::collectMeasurementData()
{
	//save the current value of the VCA setpoint as a measurement for the HP Intensity Lock device
	eventMeasurements.at(0)->setData( _this->lastFeedbackResults );
}