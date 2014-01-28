/*! \file HighPowerIntensityLock.h
 *  \author Jason Hogan
 *  \brief header file for HighPowerIntensityLock
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


#ifndef HIGH_POWER_INTENSITY_LOCK_H
#define HIGH_POWER_INTENSITY_LOCK_H

#include <STI_Device_Adapter.h>
#include <ConfigFile.h>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

class HighPowerIntensityLockDevice : public STI_Device_Adapter
{
public:

	HighPowerIntensityLockDevice(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);
	~HighPowerIntensityLockDevice();

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	void definePartnerDevices();

	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) { return readChannelDefault(channel, valueIn, dataOut); };
	bool writeChannel(unsigned short channel, const MixedValue& value)  { return writeChannelDefault(channel, value); };


	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);

	void defineChannels();


private:

	typedef boost::shared_ptr<ConfigFile> ConfigFile_ptr;
	ConfigFile_ptr configFile;

	//Channels for partner devices
	unsigned short analogInChannel;
	unsigned short fastChannel;

	
	void intensityLockLoop(double errorSignal);

	std::string getValueFromResponse(const std::string& response) const;
	bool getStatusWord(unsigned long& status);
	bool getBit(unsigned long word, unsigned bitNumber);


	int rs232QuerySleep_ms;

	double gain;
	double vcaSetpoint;
	double photodiodeSetpoint;
	double photodiodeVoltage;
//	double lastVCA;
//	double nextVCA;

	const unsigned emissionStatusBitNum;

	DynamicValue_ptr dynamicIntensitySetpoint;


	class HPLockCallback;
	friend class HPLockCallback;
//	typedef boost::shared_ptr<HPLockCallback> HPLockCallback_ptr;


	class HPLockCallback : public MeasurementCallback
	{
	public:
		HPLockCallback(HighPowerIntensityLockDevice* thisDevice, int nAverage) 
			: _this(thisDevice), _nAverage(nAverage), runningTotal(0), successfulMeasurements(0), numberOfResults(0) {}

		void handleResult(const STI::Types::TMeasurement& measurement);
		
	private:
		HighPowerIntensityLockDevice* _this;
		int _nAverage;
		int successfulMeasurements;
		int numberOfResults;
		double runningTotal;
		mutable boost::mutex callbackMutex;
	};


	MeasurementCallback_ptr sensorCallback;



	class HPIntensityLockEvent;
	friend class HPIntensityLockEvent;
	class HPIntensityLockEvent : public SynchronousEventAdapter
	{
	public:
		HPIntensityLockEvent(double time, HighPowerIntensityLockDevice* device) 
			: SynchronousEventAdapter(time, device), _this(device) {} 

		void collectMeasurementData();

	private:
		HighPowerIntensityLockDevice* _this;
	};

};

#endif

