/*! \file HighFreqSidebandLockDevice.cpp
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

#include "HighFreqSidebandLockDevice.h"

#include "MathematicaPeakFinderHF.h"

#include <iostream>

HighFreqSidebandLockDevice::HighFreqSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string configFilename, const CalibrationResults_ptr& calResults) : 
STI_Device_Adapter(orb_manager, DeviceName, configFilename), calibrationResults(calResults)
{
	initialized = true;
	dynamicTemperatureSetpoint = DynamicValue_ptr(new DynamicValue());
	dynamicRFSetpoint = DynamicValue_ptr(new DynamicValue());

	configFile = ConfigFile_ptr(new ConfigFile(configFilename));

	sensorChannel = 0;
	if (!(configFile->getParameter("sensorChannel", sensorChannel))) {
		initialized = false;
	}

	sensorChannelHighGain = 1;
	if (!(configFile->getParameter("sensorChannelHighGain", sensorChannelHighGain))) {
		initialized = false;
	}

	gainSidebandAsymmetry = 1;
	
	asymmetrySetpointTarget = 0;
	peakRatioTarget = 2;

	maxTemperatureStep = 1;

	gainPeakRatio = 1;

	rfSetpointCalibration = 0;
	rfSetpoint = -0.53;

	feedbackDelay_ms = 3000;

	calibrationFSR_ms = 15;
	firstSidebandSpacing_ms = 6.6;
	secondSidebandSpacing_ms = 2.2;
	calibrationPeakHeight_V = 4.06;
	minSpectrumX_ms = 4;
	peakTargetRange_ms = 0.7;

	maximumFractionalChangeSplitting = 0.05;

	feedbackSignals.addValue(0);
	lowGainPeaks.addValue(0);
	highGainPeaks.addValue(0);

	asymmetryLockEnabled = false;
	peakRatioLockEnabled = false;

	peakRatioSelection = FirstToCarrier;
	carrierOffset_ms = 0;

	minPointsPerPeak = 0;
	if (!(configFile->getParameter("Min. points per peak", minPointsPerPeak)))
		initialized = false;

}
HighFreqSidebandLockDevice::~HighFreqSidebandLockDevice()
{
}

double HighFreqSidebandLockDevice::minPointsPerPeak = 0;

void HighFreqSidebandLockDevice::defineAttributes()
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
	addAttribute("Maximum temperature step (deg C)", maxTemperatureStep);
	addAttribute("Enable Asymmetry Lock", (asymmetryLockEnabled ? "True" : "False"), "True, False");
	
	//RF parameters
	addAttribute("Calibration Trace RF Setpoint", rfSetpointCalibration);
	addAttribute("RF modulation setpoint", rfSetpoint);
	
	
	//"Sideband/Carrier"
	addAttribute("Peak Ratio Gain", gainPeakRatio);
	addAttribute("Enable Peak Ratio Lock", (peakRatioLockEnabled ? "True" : "False"), "True, False");
	
	addAttribute("Feedback delay (ms)", feedbackDelay_ms);

	//Peak finding algorithm attributes
	addAttribute("Calibration Trace FSR (ms)", calibrationFSR_ms);
	addAttribute("Calibration Trace Peak Height (V)", calibrationPeakHeight_V);
	addAttribute("1st Sideband to Carrier Spacing (ms)", firstSidebandSpacing_ms);
	addAttribute("2nd Sideband to Carrier Spacing (ms)", secondSidebandSpacing_ms);
	addAttribute("Peak Search Target Range (ms)", peakTargetRange_ms);
	
	addAttribute("Minimum Spectrum X Position (ms)", minSpectrumX_ms);
	addAttribute("Maximum Fractional Sideband Splitting Change", maximumFractionalChangeSplitting);
	
	addAttribute("Peak Ratio Selection", "1st sideband/carrier", "1st sideband/carrier, 2nd sidebands/1st sidebands");

	addAttribute("Carrier-Calibration Offset (ms)", carrierOffset_ms);
}

void HighFreqSidebandLockDevice::refreshAttributes()
{
	setAttribute("Sideband Asymmetry Gain", gainSidebandAsymmetry);
	setAttribute("Peak Ratio Gain", gainPeakRatio);

	setAttribute("Crystal Temp. Setpoint (deg C)", temperatureSetpoint);
	
	setAttribute("Calibration Trace RF Setpoint", rfSetpointCalibration);
	setAttribute("RF modulation setpoint", rfSetpoint);

	calibrationFSR_ms = calibrationResults->calibrationFSR() * 1000;
	setAttribute("Calibration Trace FSR (ms)", calibrationFSR_ms);

	calibrationPeakHeight_V = calibrationResults->calibrationHeight();
	setAttribute("Calibration Trace Peak Height (V)", calibrationPeakHeight_V);

	setAttribute("1st Sideband to Carrier Spacing (ms)", firstSidebandSpacing_ms);
	setAttribute("2nd Sideband to Carrier Spacing (ms)", secondSidebandSpacing_ms);

	setAttribute("Minimum Spectrum X Position (ms)", minSpectrumX_ms);
	setAttribute("Peak Search Target Range (ms)", peakTargetRange_ms);

	setAttribute("Maximum temperature step (deg C)", maxTemperatureStep);
	
	setAttribute("Enable Asymmetry Lock", (asymmetryLockEnabled ? "True" : "False"));
	setAttribute("Enable Peak Ratio Lock", (peakRatioLockEnabled ? "True" : "False"));
	
	setAttribute("Maximum Fractional Sideband Splitting Change", maximumFractionalChangeSplitting);

	setAttribute("Feedback delay (ms)", feedbackDelay_ms);

	setAttribute("Carrier-Calibration Offset (ms)", carrierOffset_ms);

}

bool HighFreqSidebandLockDevice::updateAttribute(std::string key, std::string value)
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
	else if( key.compare("Peak Ratio Gain") == 0 ) {
		double newGain;
		//if( STI::Utils::stringToValue(value, newGain) && newGain > 0 ) {
		if( STI::Utils::stringToValue(value, newGain)) {
			gainPeakRatio = newGain;
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
	else if( key.compare("1st Sideband to Carrier Spacing (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			firstSidebandSpacing_ms = newVal;
			success = true;
		}
	}
	else if( key.compare("2nd Sideband to Carrier Spacing (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			secondSidebandSpacing_ms = newVal;
			success = true;
		}
	}
	else if( key.compare("Minimum Spectrum X Position (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			minSpectrumX_ms = newVal;
			success = true;
		}
	}
	else if( key.compare("Peak Search Target Range (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			peakTargetRange_ms = newVal;
			success = true;
		}
	}
	else if( key.compare("Maximum Fractional Sideband Splitting Change") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			maximumFractionalChangeSplitting = newVal;
			success = true;
		}
	}
	else if( key.compare("Maximum temperature step (deg C)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			maxTemperatureStep = newVal;
			success = true;
		}
	}
	else if( key.compare("Feedback delay (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal) && newVal > 0 ) {
			feedbackDelay_ms = newVal;
			success = true;
		}
	}
	else if( key.compare("Enable Asymmetry Lock") == 0 ) {
		asymmetryLockEnabled = (value.compare("True") == 0);
		success = true;
	}
	else if( key.compare("Enable Peak Ratio Lock") == 0 ) {
		peakRatioLockEnabled = (value.compare("True") == 0);
		success = true;
	}
	else if( key.compare("Peak Ratio Selection") == 0 ) {
		success = true;
		if(value.compare("1st sideband/carrier") == 0) {
			peakRatioSelection = FirstToCarrier;
		}
		else if(value.compare("2nd sidebands/1st sidebands") == 0) {
			peakRatioSelection = SecondToFirst;
		}
		else {
			success = false;
		}
	}
	else if( key.compare("Carrier-Calibration Offset (ms)") == 0 ) {
		double newVal;
		if( STI::Utils::stringToValue(value, newVal)) {
			carrierOffset_ms = newVal;
			success = true;
		}
	}

	return success;
}

void HighFreqSidebandLockDevice::defineChannels()
{
	lockLoopChannel = 0;
	calibrationTraceChannel = 1;

	addInputChannel(lockLoopChannel, DataVector, ValueVector, "Lock Loop");
	addInputChannel(calibrationTraceChannel, DataVector, ValueNumber, "Calibration Trace");
}


void HighFreqSidebandLockDevice::definePartnerDevices()
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

void HighFreqSidebandLockDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
	SynchronousEventVector& eventsOut) throw(std::exception)
{
	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastListEvent;

	//double arroyoSetpoint = 0;

	double calibrationTraceTime;
	double sidebandTraceTime;

	double dtFeedback_ns = 1.0e6 * feedbackDelay_ms;
	double rfModulationHoldoff = 200.0e6;

	std::vector<double> scopeSettings;

	MeasurementCallback_ptr sensorCallback;
	MeasurementCallback_ptr highGainSensorCallback;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The HP Sideband Lock cannot currently have multiple events at the same time.");
		}
		
		//Sideband lock event
		if(events->second.at(0).channel() == lockLoopChannel) {
			
			//Setpoints passed in from the timing file in a vector of the form (sideband asymmetry, sideband/carrier ratio)
			asymmetrySetpointTarget = events->second.at(0).value().getVector().at(0).getNumber();
			peakRatioTarget = events->second.at(0).value().getVector().at(1).getNumber();

			sensorCallback = MeasurementCallback_ptr(
				new SpectrumCallback(this, SpectrumCallback::LowGainSpectrum));
			highGainSensorCallback = MeasurementCallback_ptr(
				new SpectrumCallback(this, SpectrumCallback::HighGainSpectrum));

			
			/*scopeSettings.push_back(1);
			scopeSettings.push_back(1);
			scopeSettings.push_back(0);
			scopeSettings.push_back(1000);*/

			//Use empty vector to use default scope settings
			MixedValue scopeSettingsMixed;
			scopeSettingsMixed.setValue(scopeSettings);
			
			//Change the rf setpoint
			dynamicRFSetpoint->setValue(rfSetpoint);

			//Low gain modulated trace (for 1st order sidebands)
			sidebandTraceTime = events->first;
			partnerDevice("RFAmplitude").
				event(sidebandTraceTime - rfModulationHoldoff, rfAmplitudeActuatorChannel, dynamicRFSetpoint, 
					  events->second.at(0), "Turn on RF to measure peaks");
			partnerDevice("Sensor").meas(sidebandTraceTime, sensorChannel, scopeSettingsMixed, events->second.at(0), sensorCallback, "Find sidebands");
		
			//High gain modulated trace (for carrier); occurs at same time on second scope channel
			partnerDevice("Sensor").meas(sidebandTraceTime, sensorChannelHighGain, scopeSettingsMixed, events->second.at(0), highGainSensorCallback, "Find carrier");


			//Change the arroyo temperature
			if(asymmetryLockEnabled) {
				dynamicTemperatureSetpoint->setValue(temperatureSetpoint);
				partnerDevice("Arroyo").event(events->first + dtFeedback_ns, arroyoTemperatureSetChannel, dynamicTemperatureSetpoint, events->second.at(0), "Feedback on crystal temperature");
			}

			//Change the RF amplitude
			if(peakRatioLockEnabled) {
				partnerDevice("RFAmplitude").
					event(events->first + dtFeedback_ns, rfAmplitudeActuatorChannel, dynamicRFSetpoint, events->second.at(0), "Feedback on RF");
			}

			//Add a measurement event to record results of the lock loop
			eventsOut.push_back(new SidebandLockEvent(events->first, lockLoopChannel, this) );
			eventsOut.back().addMeasurement( events->second.at(0) );
		}


		//Calibration event
		if(events->second.at(0).channel() == calibrationTraceChannel) {

			sensorCallback = MeasurementCallback_ptr(new SpectrumCallback(this, SpectrumCallback::CalibrationTrace));
			
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
			eventsOut.push_back(new SidebandLockEvent(calibrationTraceTime, calibrationTraceChannel, this) );
			eventsOut.back().addMeasurement( events->second.at(0) );
		}
	}
}

void HighFreqSidebandLockDevice::asymmetryLockLoop(double errorSignalSidebandDifference)
{	
	//Servos the measured errorSignalSidebandDifference to the target asymmetrySetpoint.
	//In order to make the sidebands equal, use target asymmetrySetpointTarget = 0.

	double temperatureStep = gainSidebandAsymmetry * (errorSignalSidebandDifference - asymmetrySetpointTarget);
	
	//Only make the change if the step size is smaller than the max step size.
	//This attempts to avoid changes that are the result of a bad spectrum or an incorrect peak solution.
	//Correct feedback values should be small, because large temperature changes cause the PPLN output
	//power to drop significantly, preventing further spectra from being acquired.
	if(fabs(temperatureStep) < fabs(maxTemperatureStep)) {
		
		if(asymmetryLockEnabled) {
			temperatureSetpoint += temperatureStep;
			dynamicTemperatureSetpoint->setValue(temperatureSetpoint);
		}
	} else {
		cout << "Feedback error: Requested temperature step (" << fabs(temperatureStep) << " C) exceeded the maximum allowed step size." << endl
			<< "The feedback was not applied." << endl;
	}
}

void HighFreqSidebandLockDevice::peakRatioLockLoop(double errorSignalSidebandCarrierRatio)
{	
	//Servos the measured errorSignalSidebandCarrierRatio to the target peakRatioTarget.
	if(peakRatioLockEnabled) {
		rfSetpoint += gainPeakRatio * (errorSignalSidebandCarrierRatio - peakRatioTarget);

		std::cout << "Setting RF to: " << rfSetpoint << std::endl;
		dynamicRFSetpoint->setValue(rfSetpoint);
	}
}

void HighFreqSidebandLockDevice::SpectrumCallback::handleResult(const STI::Types::TMeasurement& measurement)
{
	using namespace std;

	//
	//	Magical mathematica code
	//

	boost::unique_lock< boost::shared_mutex > writeLock(_this->spectrumMutex);

	if(isType(CalibrationTrace)) {
//	if(_hpLockChannel == _this->calibrationTraceChannel) {
		//Callback has calibration data
		MathematicaPeakFinderHF peakFinder(minPointsPerPeak);
		
		peakFinder.findCalibrationPeaks(
			measurement.data.vector(), 
			_this->calibrationFSR_ms * 0.001, 
			_this->minSpectrumX_ms * 0.001, 
			_this->calibrationResults);

		_this->callbackCondition.notify_all();

	}
	if(isType(LowGainSpectrum)) {
		//Callback has sideband spectrum data
		
		_this->handleMeasuredSpectrumLowGain(measurement.data.vector());
	}

	if(isType(HighGainSpectrum)) {
		//Callback has sideband spectrum data
		
		_this->handleMeasuredSpectrumHighGain(measurement.data.vector());
	}

	if(_this->spectrumTraceManager.hasBothTraces()) {

		if( _this->computeFeedbackSignals() ) {
			//The feedback signals are within allowed ranges
			_this->applyFeedback();
		}

		_this->callbackCondition.notify_all();
		_this->spectrumTraceManager.reset();
	}
}

bool HighFreqSidebandLockDevice::computeFeedbackSignals()
{
	MathematicaPeakFinderHF peakFinder(minPointsPerPeak);

	if(!peakFinder.calculateFeedbackSignalsHighLow(lowGainPeaks, highGainPeaks, feedbackSignals))	//(sidebands, carrier, results)
		return false;	//error

	//check feedback parameter ranges
}

void HighFreqSidebandLockDevice::applyFeedback()
{
	//Feedback on sideband asymmetry
	asymmetryLockLoop(feedbackSignals.getVector().at(0).getDouble());

	//Feedback on sideband/carrier ratio
	peakRatioLockLoop(feedbackSignals.getVector().at(1).getDouble());

	//Do this once after calling both loops
	refreshDeviceAttributes();	//update the attribute text file and the client

}


void HighFreqSidebandLockDevice::handleMeasuredSpectrumLowGain(const STI::Types::TDataMixedSeq& rawSidebandData)
{
	//Looks for the 1st order sidebands

	MathematicaPeakFinderHF peakFinder(minPointsPerPeak);
	
	if(!peakFinder.findFirstSidebandPeaks(
		rawSidebandData, 
		calibrationResults, 
		firstSidebandSpacing_ms * 0.001, 
		minSpectrumX_ms * 0.001, 
		peakTargetRange_ms * 0.001, 
		carrierOffset_ms * 0.001,
		lowGainPeaks ))
	{
		return;	//error
	}

	spectrumTraceManager.hasLowGainTrace = true;
}

void HighFreqSidebandLockDevice::handleMeasuredSpectrumHighGain(const STI::Types::TDataMixedSeq& rawSidebandData)
{
	//Looks for the carriers

	MathematicaPeakFinderHF peakFinder(minPointsPerPeak);

	if(!peakFinder.findCarrierPeaks(
		rawSidebandData, 
		calibrationResults, 
		minSpectrumX_ms * 0.001, 
		peakTargetRange_ms * 0.001, 
		carrierOffset_ms * 0.001,
		highGainPeaks ))
	{
		return;	//error
	}

	spectrumTraceManager.hasHighGainTrace = true;
}

void HighFreqSidebandLockDevice::SidebandLockEvent::collectMeasurementData()
{

	MixedData results;
//	results.addValue(0);
//	eventMeasurements.at(0)->setData( results );
//	return;

	boost::shared_lock< boost::shared_mutex > readLock(_this->spectrumMutex);

	long timeout = 5; //seconds
	boost::system_time wakeTime = 
		boost::get_system_time()
		+ boost::posix_time::seconds( static_cast<long>(timeout) );

	//Attempt to wait until the data is ready, or until timeout
	_this->callbackCondition.timed_wait(readLock, wakeTime);

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
		peakResults.addValue(_this->lowGainPeaks);
		peakResults.addValue(_this->highGainPeaks);
		
		MixedData feedbackResults;
		feedbackResults.addValue(std::string("Feedback Signals (sideband difference, sideband to carrier ratio)"));
		feedbackResults.addValue(_this->feedbackSignals);

		results.addValue(feedbackResults);
		results.addValue(peakResults);
		
		eventMeasurements.at(0)->setData( results );
	}
}




//void HighFreqSidebandLockDevice::handleMeasuredSpectrumFirstToCarrier(const STI::Types::TDataMixedSeq& rawSidebandData)
//{
//	MathematicaPeakFinderHF peakFinder(minPointsPerPeak);
//	
//	if(!peakFinder.findCarrierAndSidebandPeaks(
//		rawSidebandData, 
//		calibrationResults, 
//		firstSidebandSpacing_ms * 0.001, 
//		minSpectrumX_ms * 0.001, 
//		peakTargetRange_ms * 0.001,
//		targetSpectrumPeaks))
//	{
//		return;	//error
//	}
//	if(!peakFinder.calculateFeedbackSignals(targetSpectrumPeaks, feedbackSignals)) {
//		return;	//error
//	}
//
//	double newSidebandSplitting_ms = 1000 *
//		(targetSpectrumPeaks.getVector().at(1).getVector().at(0).getDouble() - 
//		targetSpectrumPeaks.getVector().at(0).getVector().at(0).getDouble());
//
//	double fractionalChangeSplitting = fabs( 1 - (newSidebandSplitting_ms / firstSidebandSpacing_ms) );
//
//	//This is a check to varify that the correct scope trace was analyzed. In case the scope trace is
//	//incorrect, the new measured sideband splitting is likely very different. We reject the feedback 
//	//attempt in this case.
//	
//	if(fractionalChangeSplitting < maximumFractionalChangeSplitting) {
//		//New sideband splitting is within acceptable range. Save it and apply feedback.
//		firstSidebandSpacing_ms = newSidebandSplitting_ms;		//Update sideband splitting
//
//		//Feedback on sideband asymmetry
//		asymmetryLockLoop(feedbackSignals.getVector().at(0).getDouble());
//
//		//Feedback on sideband/carrier ratio
//		peakRatioLockLoop(feedbackSignals.getVector().at(1).getDouble());
//
//		//Do this once after calling both loops
//		refreshDeviceAttributes();	//update the attribute text file and the client
//	} else {
//		cout << "Feedback error: Sideband splitting change too large:" << endl
//			<< "   Old splitting: " << firstSidebandSpacing_ms << " ms, new splitting: " 
//			<< newSidebandSplitting_ms << " ms, factional change: " << fractionalChangeSplitting << endl;
//	}
//
//}
//
//
//void HighFreqSidebandLockDevice::handleMeasuredSpectrumSecondToFirst(const STI::Types::TDataMixedSeq& rawSidebandData)
//{
//	MathematicaPeakFinderHF peakFinder(minPointsPerPeak);
//	
//	if(!peakFinder.findFirstAndSecondOrderSidebandPeaks(
//		rawSidebandData, 
//		calibrationResults, 
//		firstSidebandSpacing_ms * 0.001, 
//		secondSidebandSpacing_ms * 0.001, 
//		minSpectrumX_ms * 0.001, 
//		peakTargetRange_ms * 0.001,
//		targetSpectrumPeaks, carrierOffset_ms*0.001))
//	{
//		return;	//error
//	}
//	if(!peakFinder.calculateFeedbackSignalsFromFirstAndSecondSideband(targetSpectrumPeaks, feedbackSignals)) {
//		return;	//error
//	}
//
//	MixedData calPeaks;
//	calibrationResults->getPeakValues(calPeaks);
//	//targetSpectrumPeaks format:	{{L1,R1},{L2,R2}}
//
//	double new1stSidebandSplitting_ms = fabs(
//		1000 *
//		(targetSpectrumPeaks.getVector().at(1).getVector().at(0).getDouble() - 
//		calPeaks.getVector().at(1).getVector().at(0).getDouble())
//		);
//	
//
//	double new1stSidebandSplitting_msA = fabs(new1stSidebandSplitting_ms - carrierOffset_ms/2);
//	double new1stSidebandSplitting_msB = fabs(new1stSidebandSplitting_ms + carrierOffset_ms/2);
//
//	double new2ndSidebandSplitting_ms = fabs(
//		1000 *
//		(targetSpectrumPeaks.getVector().at(3).getVector().at(0).getDouble() - 
//		calPeaks.getVector().at(1).getVector().at(0).getDouble())
//		);
//
//	double new2ndSidebandSplitting_msA = fabs(new2ndSidebandSplitting_ms - carrierOffset_ms/2);
//	double new2ndSidebandSplitting_msB = fabs(new2ndSidebandSplitting_ms + carrierOffset_ms/2);
//
//	double fractionalChangeSplitting1stA = fabs( 1 - (new1stSidebandSplitting_msA / firstSidebandSpacing_ms) );
//	double fractionalChangeSplitting1stB = fabs( 1 - (new1stSidebandSplitting_msB / firstSidebandSpacing_ms) );
//	double fractionalChangeSplitting2ndA = fabs( 1 - (new2ndSidebandSplitting_msA / secondSidebandSpacing_ms) );
//	double fractionalChangeSplitting2ndB = fabs( 1 - (new2ndSidebandSplitting_msB / secondSidebandSpacing_ms) );
//
//	//This is a check to varify that the correct scope trace was analyzed. In case the scope trace is
//	//incorrect, the new measured sideband splitting is likely very different. We reject the feedback 
//	//attempt in this case.
//		
//
//	double fractionalChangeSplitting1st, fractionalChangeSplitting2nd;
//	double new1stSidebandSplittingCarrierOffset_ms, new2ndSidebandSplittingCarrierOffset_ms;
//
//	if ((fractionalChangeSplitting1stA + fractionalChangeSplitting2ndA) < (fractionalChangeSplitting1stB + fractionalChangeSplitting2ndB))
//	{
//		fractionalChangeSplitting1st = fractionalChangeSplitting1stA;
//		fractionalChangeSplitting2nd = fractionalChangeSplitting2ndA;
//
//		new1stSidebandSplittingCarrierOffset_ms = new1stSidebandSplitting_msA;
//		new2ndSidebandSplittingCarrierOffset_ms = new2ndSidebandSplitting_msA;
//	}
//	else
//	{
//		fractionalChangeSplitting1st = fractionalChangeSplitting1stB;
//		fractionalChangeSplitting2nd = fractionalChangeSplitting2ndB;
//
//		new1stSidebandSplittingCarrierOffset_ms = new1stSidebandSplitting_msB;
//		new2ndSidebandSplittingCarrierOffset_ms = new2ndSidebandSplitting_msB;
//	}
//
//	if((fractionalChangeSplitting1st < maximumFractionalChangeSplitting) && 
//		(fractionalChangeSplitting2nd < maximumFractionalChangeSplitting)) {
//		//New sideband splitting is within acceptable range. Save it and apply feedback.
//		firstSidebandSpacing_ms = new1stSidebandSplittingCarrierOffset_ms;		//Update sideband splitting
//		secondSidebandSpacing_ms = new2ndSidebandSplittingCarrierOffset_ms;		//Update sideband splitting
//
//
//		double calPeakMean = (calPeaks.getVector().at(0).getVector().at(0).getDouble() + calPeaks.getVector().at(1).getVector().at(0).getDouble())/2;
//		double firstSidebandMean = (targetSpectrumPeaks.getVector().at(0).getVector().at(0).getDouble() + targetSpectrumPeaks.getVector().at(1).getVector().at(0).getDouble())/2;
//
//		cout << "Old carrier-calibration offset: " << carrierOffset_ms << endl;
//		carrierOffset_ms = 1000*(firstSidebandMean - calPeakMean);
//		cout << "New carrier-calibration offset: " << carrierOffset_ms << endl;
//
//		//Feedback on sideband asymmetry
//		asymmetryLockLoop(feedbackSignals.getVector().at(0).getDouble());
//
//		//Feedback on sideband/carrier ratio
//		peakRatioLockLoop(feedbackSignals.getVector().at(1).getDouble());
//
//		//Do this once after calling both loops
//		refreshDeviceAttributes();	//update the attribute text file and the client
//	} else {
//		cout << "Feedback error: Attempted change to one of the sideband splittings is too large:" << endl
//			<< "   1st order: Old splitting: " << firstSidebandSpacing_ms << " ms, new splitting: " 
//			<< new1stSidebandSplittingCarrierOffset_ms << " ms, fractional change: " << fractionalChangeSplitting1st << endl
//			<< "   2nd order: Old splitting: " << secondSidebandSpacing_ms << " ms, new splitting: " 
//			<< new2ndSidebandSplittingCarrierOffset_ms << " ms, fractional change: " << fractionalChangeSplitting2nd << endl;
//	}
//
//}
//
//