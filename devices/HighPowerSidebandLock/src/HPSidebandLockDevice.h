/*! \file HPSidebandLockDevice.h
 *  \author Jason Hogan
 *  \brief header file for HPSidebandLockDevice
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


#ifndef HPSIDEBANDLOCKDEVICE_H
#define HPSIDEBANDLOCKDEVICE_H

#include <STI_Device_Adapter.h>
#include <ConfigFile.h>

#include "CalibrationResults.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>

class HPSidebandLockDevice : public STI_Device_Adapter
{
public:

	HPSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string configFilename, const CalibrationResults_ptr& calResults);
	~HPSidebandLockDevice();

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	void defineChannels();
	void definePartnerDevices();

	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);

	bool isInitialized() { return initialized; }

private:
	



	class HPLockCallback;
	friend class HPLockCallback;
//	typedef boost::shared_ptr<HPLockCallback> HPLockCallback_ptr;


	class HPLockCallback : public MeasurementCallback
	{
	public:
		HPLockCallback(HPSidebandLockDevice* thisDevice, unsigned short hpLockChannel) : 
		  _this(thisDevice), _hpLockChannel(hpLockChannel) {}
		void handleResult(const STI::Types::TMeasurement& measurement);
		
	private:
		HPSidebandLockDevice* _this;
		unsigned short _hpLockChannel;
	};


	class HPSidebandLockEvent;
	friend class HPSidebandLockEvent;

	class HPSidebandLockEvent : public SynchronousEventAdapter
	{
	public:
		HPSidebandLockEvent(double time, unsigned short channel, HPSidebandLockDevice* device) : 
		  SynchronousEventAdapter(time, device), _this(device), _channel(channel) {}
		
		/*void playEvent()
		{
			_this->tmp += 0.2;
			_this->dynamicFeedbackValue->setValue(_this->tmp);
		}*/

		void collectMeasurementData();
		unsigned short getChannel() { return _channel; }

	private:
		HPSidebandLockDevice* _this;
		unsigned short _channel;
	};



	DynamicValue_ptr dynamicTemperatureSetpoint;
	DynamicValue_ptr dynamicRFSetpoint;
	
	typedef boost::shared_ptr<ConfigFile> ConfigFile_ptr;
	ConfigFile_ptr configFile;

	CalibrationResults_ptr calibrationResults;
		
	std::vector <double> scopeData;

	double rfSetpointCalibration;
	
	unsigned short lockLoopChannel;
	unsigned short calibrationTraceChannel;
	unsigned short rfAmplitudeActuatorChannel;
	short sensorChannel;
	unsigned short arroyoTemperatureSetChannel;

	bool initialized;

	std::vector <double> lastFeedbackResults;

	void asymmetryLockLoop(double errorSignalSidebandDifference);
	void peakRatioLockLoop(double errorSignalSidebandCarrierRatio);

	void handleMeasuredSpectrumFirstToCarrier(const STI::Types::TDataMixedSeq& rawSidebandData);
	void handleMeasuredSpectrumSecondToFirst(const STI::Types::TDataMixedSeq& rawSidebandData);

	double gainSidebandAsymmetry;
	double gainPeakRatio;

	double temperatureSetpoint;
	double rfSetpoint;
	
	double feedbackDelay_ms;

	double asymmetrySetpointTarget;
	double peakRatioTarget;

	double maxTemperatureStep;		//if the temperature change is larger than this, do no change the temperature

	bool asymmetryLockEnabled;
	bool peakRatioLockEnabled;

	double carrierOffset_ms;

	//Spectrum peak finding guesses
	double calibrationFSR_ms;
	double firstSidebandSpacing_ms;
	double secondSidebandSpacing_ms;
	double calibrationPeakHeight_V;
	double minSpectrumX_ms;
	double peakTargetRange_ms;
	
	double maximumFractionalChangeSplitting;

	MixedData targetSpectrumPeaks;		//{carrier, 1st sidebands} or {1st sidebands, 2nd sidebands}, depending on Peak Ratio Selector attribute
	MixedData feedbackSignals;
	
	mutable boost::shared_mutex spectrumMutex;
	boost::condition_variable_any callbackCondition;

	enum PeakRatioSelectionType {FirstToCarrier, SecondToFirst};

	PeakRatioSelectionType peakRatioSelection;

	static double minPointsPerPeak;

};

#endif
