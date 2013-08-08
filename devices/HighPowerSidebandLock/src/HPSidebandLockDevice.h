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

class HPSidebandLockDevice : public STI_Device_Adapter
{
public:

	HPSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string configFilename);
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
		HPLockCallback(HPSidebandLockDevice* thisDevice) : _this(thisDevice) {}
		void handleResult(const STI::Types::TMeasurement& measurement);
		
	private:
		HPSidebandLockDevice* _this;
	};


	class HPSidebandLockEvent;
	friend class HPSidebandLockEvent;

	class HPSidebandLockEvent : public SynchronousEventAdapter
	{
	public:
		HPSidebandLockEvent(double time, HPSidebandLockDevice* device, bool calibrationEvent=false) : 
		  SynchronousEventAdapter(time, device), _this(device), _isCalibration(calibrationEvent) {}
		
		/*void playEvent()
		{
			_this->tmp += 0.2;
			_this->dynamicFeedbackValue->setValue(_this->tmp);
		}*/

		void collectMeasurementData();

	private:
		HPSidebandLockDevice* _this;
		bool _isCalibration;
	};



	MeasurementCallback_ptr sensorCallback;
	DynamicValue_ptr dynamicTemperatureSetpoint;
	
	typedef boost::shared_ptr<ConfigFile> ConfigFile_ptr;
	ConfigFile_ptr configFile;
		
	std::vector <double> scopeData;

	double rfSetpointCalibration;
	
	unsigned short lockLoopChannel;
	unsigned short calibrationTraceChannel;
	unsigned short rfAmplitudeActuatorChannel;
	short sensorChannel;

	bool initialized;

	std::vector <double> lastFeedbackResults;

	void asymmetryLockLoop(double errorSignal);

	double gain;
	double temperatureSetpoint;
	double asymmetrySetpoint;

	double rfSetpoint;
};

#endif
