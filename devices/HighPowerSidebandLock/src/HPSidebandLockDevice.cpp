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

#include "MathematicaPeakFinder.h"

#include <iostream>

HPSidebandLockDevice::HPSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string configFilename, const CalibrationResults_ptr& calResults) : 
STI_Device_Adapter(orb_manager, DeviceName, configFilename), calibrationResults(calResults)
{
	initialized = true;
	dynamicTemperatureSetpoint = DynamicValue_ptr(new DynamicValue());

	configFile = ConfigFile_ptr(new ConfigFile(configFilename));

	sensorChannel = 0;
	if (!(configFile->getParameter("sensorChannel", sensorChannel)))
		initialized = false;

	gainSidebandAsymmetry = 1;
	asymmetrySetpoint = 0;

	gainModulationDepth = 1;

	rfSetpointCalibration = 0;
	rfSetpoint = -0.53;

	calibrationFSR_ms = 15;
	sidebandSpacing_ms = 6.6;
	calibrationPeakHeight_V = 4.06;

	feedbackSignals.addValue(0);
	carrierAndSidebandPeaks.addValue(0);

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

	//Temperature parameters
	addAttribute("Crystal Temp. Setpoint (deg C)", temperatureSetpoint);
	addAttribute("Sideband Asymmetry Gain", gainSidebandAsymmetry);
	
	//RF parameters
	addAttribute("Calibration Trace RF Setpoint", rfSetpointCalibration);
	addAttribute("RF modulation setpoint", rfSetpoint);
	addAttribute("Modulation Depth Gain", gainModulationDepth);

	//
	addAttribute("Calibration Trace FSR (ms)", calibrationFSR_ms);
	addAttribute("Calibration Trace Peak Height (V)", calibrationPeakHeight_V);
	addAttribute("Sideband to Carrier Spacing (ms)", sidebandSpacing_ms);
}

void HPSidebandLockDevice::refreshAttributes()
{
	setAttribute("Sideband Asymmetry Gain", gainSidebandAsymmetry);
	setAttribute("Modulation Depth Gain", gainModulationDepth);

	setAttribute("Crystal Temp. Setpoint (deg C)", temperatureSetpoint);
	
	setAttribute("Calibration Trace RF Setpoint", rfSetpointCalibration);
	setAttribute("RF modulation setpoint", rfSetpoint);

	calibrationFSR_ms = calibrationResults->calibrationFSR() * 1000;
	setAttribute("Calibration Trace FSR (ms)", calibrationFSR_ms);

	calibrationPeakHeight_V = calibrationResults->calibrationHeight();
	setAttribute("Calibration Trace Peak Height (V)", calibrationPeakHeight_V);

	setAttribute("Sideband to Carrier Spacing (ms)", sidebandSpacing_ms);

}

bool HPSidebandLockDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	std::string result;
	
	if( key.compare("Sideband Asymmetry Gain") == 0 ) {
		double newGain;
		if( STI::Utils::stringToValue(value, newGain) && newGain > 0 ) {
			gainSidebandAsymmetry = newGain;
			success = true;
		}
	}
	else if( key.compare("Modulation Depth Gain") == 0 ) {
		double newGain;
		if( STI::Utils::stringToValue(value, newGain) && newGain > 0 ) {
			gainModulationDepth = newGain;
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
	else if( key.compare("Calibration Trace RF Setpoint") == 0 ) {
		double newSetpoint;
		if( STI::Utils::stringToValue(value, newSetpoint) ) {
			rfSetpointCalibration = newSetpoint;
			success = true;
		}
	}
	else if( key.compare("RF modulation setpoint") == 0 ) {
		double newSetpoint;
		if( STI::Utils::stringToValue(value, newSetpoint) ) {
			rfSetpoint = newSetpoint;
			success = true;
		}
	}
	else if( key.compare("Calibration Trace FSR (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			calibrationFSR_ms = newVal;
			calibrationResults->overrideFSR(calibrationFSR_ms * 0.001);		//force FSR to value
			success = true;
		}
	}
	else if( key.compare("Calibration Trace Peak Height (V)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			calibrationPeakHeight_V = newVal;
			calibrationResults->overrideCalibrationHeight(calibrationPeakHeight_V);		//force peak height
			success = true;
		}
	}
	else if( key.compare("Sideband to Carrier Spacing (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			sidebandSpacing_ms = newVal;
			success = true;
		}
	}

	return success;
}

void HPSidebandLockDevice::defineChannels()
{
	lockLoopChannel = 0;
	calibrationTraceChannel = 1;

	addInputChannel(lockLoopChannel, DataVector, ValueNumber, "Lock Loop");
	addInputChannel(calibrationTraceChannel, DataVector, ValueNumber, "Calibration Trace");
}


void HPSidebandLockDevice::definePartnerDevices()
{
	//Fabry Perot specturm
	std::string sensorIP = "";
	short sensorModule = 0;
	std::string sensorDeviceName = "";
	
	configFile->getParameter("sensorIP", sensorIP);
	configFile->getParameter("sensorModule", sensorModule);
	configFile->getParameter("sensorDeviceName", sensorDeviceName);

	addPartnerDevice("Sensor", sensorIP, sensorModule, sensorDeviceName);

	//Temperature control (oven setpoint)
	std::string arroyoIP = "";
	short arroyoModule = 0;
	std::string arroyoDeviceName = "";
	
	configFile->getParameter("arroyoIP", arroyoIP);
	configFile->getParameter("arroyoModule", arroyoModule);
	configFile->getParameter("arroyoDeviceName", arroyoDeviceName);
	configFile->getParameter("arroyoTemperatureSetChannel", arroyoTemperatureSetChannel);

	addPartnerDevice("Arroyo", arroyoIP, arroyoModule, arroyoDeviceName);

	//RF amplitude control
	std::string rfAmplitudeIP = "";
	short rfAmplitudeModule = 0;
	std::string rfAmplitudeDeviceName = "";

	configFile->getParameter("rfAmplitudeActuatorIP", rfAmplitudeIP);
	configFile->getParameter("rfAmplitudeActuatorModule", rfAmplitudeModule);
	configFile->getParameter("rfAmplitudeActuatorDeviceName", rfAmplitudeDeviceName);
	configFile->getParameter("rfAmplitudeActuatorChannelNumber", rfAmplitudeActuatorChannel);

	addPartnerDevice("RFAmplitude", rfAmplitudeIP, rfAmplitudeModule, rfAmplitudeDeviceName);

	partnerDevice("Sensor").enablePartnerEvents();
	partnerDevice("Arroyo").enablePartnerEvents();
	partnerDevice("RFAmplitude").enablePartnerEvents();

}

void HPSidebandLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	//double arroyoSetpoint = 0;

	double calibrationTraceTime;
	double sidebandTraceTime;

	double dtFeedback = 200.0e6;
	double rfModulationHoldoff = 200.0e6;

	std::vector<double> scopeSettings;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The HP Sideband Lock cannot currently have multiple events at the same time.");
		}
		
		//Sideband lock event
		if(events->second.at(0).channel() == lockLoopChannel) {
			asymmetrySetpoint = events->second.at(0).value().getNumber();

			sensorCallback = MeasurementCallback_ptr(new HPLockCallback(this, lockLoopChannel));

			
			/*scopeSettings.push_back(1);
			scopeSettings.push_back(1);
			scopeSettings.push_back(0);
			scopeSettings.push_back(1000);*/

			//Use empty vector to use default scope settings
			MixedValue scopeSettingsMixed;
			scopeSettingsMixed.setValue(scopeSettings);
			

			//Sideband trace
			sidebandTraceTime = events->first;
			partnerDevice("RFAmplitude").
				event(sidebandTraceTime - rfModulationHoldoff, rfAmplitudeActuatorChannel, rfSetpoint, 
					  events->second.at(0), "RF setpoint");
			partnerDevice("Sensor").meas(sidebandTraceTime, sensorChannel, scopeSettingsMixed, events->second.at(0), sensorCallback, "Calibration");
			
			//Change the arroyo temperature
			dynamicTemperatureSetpoint->setValue(temperatureSetpoint);
			partnerDevice("Arroyo").event(events->first + dtFeedback, arroyoTemperatureSetChannel, dynamicTemperatureSetpoint, events->second.at(0), "Feedback on crystal temperature");
			
			
			//Add a measurement event to record results of the lock loop
			eventsOut.push_back(new HPSidebandLockEvent(events->first + dtFeedback, lockLoopChannel, this) );
			eventsOut.back().addMeasurement( events->second.at(0) );
		}


		//Calibration event
		if(events->second.at(0).channel() == calibrationTraceChannel) {

			sensorCallback = MeasurementCallback_ptr(new HPLockCallback(this, calibrationTraceChannel));
			
			/*scopeSettings.push_back(1);
			scopeSettings.push_back(1);
			scopeSettings.push_back(0);
			scopeSettings.push_back(1000);*/

			//Use empty vector to use default scope settings
			MixedValue scopeSettingsMixed;
			scopeSettingsMixed.setValue(scopeSettings);


			//Sideband trace
			calibrationTraceTime = events->first;
			partnerDevice("RFAmplitude").
				event(calibrationTraceTime - rfModulationHoldoff, rfAmplitudeActuatorChannel, rfSetpointCalibration, 
					  events->second.at(0), "RF setpoint");
			partnerDevice("Sensor").meas(calibrationTraceTime, sensorChannel, scopeSettingsMixed, events->second.at(0), sensorCallback, "Sidebands");		
			
			//Add a measurement event to record results of the lock loop
			eventsOut.push_back(new HPSidebandLockEvent(calibrationTraceTime + dtFeedback, calibrationTraceChannel, this) );
			eventsOut.back().addMeasurement( events->second.at(0) );
		}
	}
}

void HPSidebandLockDevice::asymmetryLockLoop(double errorSignalSidebandDifference)
{	
//	nextVCA = lastVCA + gain * errorSignal;
//	lastVCA = nextVCA;

	temperatureSetpoint += gainSidebandAsymmetry * errorSignalSidebandDifference;

	dynamicTemperatureSetpoint->setValue(temperatureSetpoint);

	refreshDeviceAttributes();	//update the attribute text file and the client

}

void HPSidebandLockDevice::HPLockCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;
	
//	MixedData myDataVector;
//	myDataVector.setValue(measurement.data.vector());


	//
	//	Magical mathematica code
	//

	MathematicaPeakFinder peakFinder;

	if(_hpLockChannel == _this->calibrationTraceChannel) {
		//Callback has calibration data
		peakFinder.findCalibrationPeaks(measurement.data.vector(), _this->calibrationFSR_ms * 0.001, _this->calibrationResults);
	}
		
	if(_hpLockChannel == _this->lockLoopChannel) {
		//Callback has sideband spectrum data
		if(!peakFinder.findCarrierAndSidebandPeaks(measurement.data.vector(), 
			_this->calibrationResults, _this->sidebandSpacing_ms * 0.001, _this->carrierAndSidebandPeaks))
		{
			return;	//error
		}
		if(!peakFinder.calculateFeedbackSignals(_this->carrierAndSidebandPeaks, _this->feedbackSignals)) {
			return;	//error
		}

		
		////Update sideband splitting
		//_this->sidebandSpacing_ms = 1000 *
		//	(((_this->carrierAndSidebandPeaks.getVector().at(1).getVector().at(0).getDouble() +
		//	_this->carrierAndSidebandPeaks.getVector().at(2).getVector().at(0).getDouble()) / 2.0) - 
		//	_this->carrierAndSidebandPeaks.getVector().at(0).getVector().at(0).getDouble());
		//Update sideband splitting
		_this->sidebandSpacing_ms = 1000 *
			(_this->carrierAndSidebandPeaks.getVector().at(1).getVector().at(0).getDouble() - 
			_this->carrierAndSidebandPeaks.getVector().at(0).getVector().at(0).getDouble());


		_this->asymmetryLockLoop(_this->feedbackSignals.getVector().at(0).getDouble());

//		_this->lastFeedbackResults.clear();
//		_this->lastFeedbackResults.push_back(feedbackSignals);
//		_this->lastFeedbackResults.push_back(feedbackSignals);
	}

}

void HPSidebandLockDevice::HPSidebandLockEvent::collectMeasurementData()
{

	MixedData results;
//	results.addValue(0);
//	eventMeasurements.at(0)->setData( results );
//	return;

	if(getChannel() == _this->calibrationTraceChannel) {
		
		MixedData calPeaks;
		_this->calibrationResults->getPeakValues(calPeaks);
		
		MixedData calResults;
		calResults.addValue(std::string("Calibration Peaks"));
		calResults.addValue(calPeaks);
		
		results.addValue(calResults);

		eventMeasurements.at(0)->setData( results );
	}
	else if (getChannel() == _this->lockLoopChannel) {

		MixedData peakResults;
		peakResults.addValue(std::string("Spectral Peaks"));
		peakResults.addValue(_this->carrierAndSidebandPeaks);
		
		MixedData feedbackResults;
		feedbackResults.addValue(std::string("Feedback Signals (sideband difference, sideband to carrier ratio)"));
		feedbackResults.addValue(_this->feedbackSignals);

		results.addValue(feedbackResults);
		results.addValue(peakResults);
		
		eventMeasurements.at(0)->setData( results );
	}
}
