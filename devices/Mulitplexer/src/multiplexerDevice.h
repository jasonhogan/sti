/*! \file IVIScopeDevice.h
 *  \author Jason Hogan
 *  \brief header file for the class IVIScopeDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef MULTIPLEXERDEVICE_H
#define MULTIPLEXERDEVICE_H

#include <STI_Device_Adapter.h>
#include <ConfigFile.h>
#include <MixedData.h>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

#include <vector>
#include <string>

class MultiplexerDevice : public STI_Device_Adapter
{
public:
	
	MultiplexerDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string configFilename);
	~MultiplexerDevice();

private:

    // Device Attributes
//    void defineAttributes();
//    void refreshAttributes();
//    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
//	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
//	bool writeChannel(unsigned short channel, const MixedValue& value);

	void definePartnerDevices();

    // Device Command line interface setup
//	std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);

	std::string getDeviceHelp() { return "Multiplexer help"; }

	

private:

	short controlChannel0, controlChannel1, controlChannel2, controlChannel3;
	double controlLineHigh, controlLineLow;

	typedef boost::shared_ptr<ConfigFile> ConfigFile_ptr;
	ConfigFile_ptr configFile;

	void sendControlLineEvents(double eventTime, short channel, const RawEvent& evt);
	double getControlLineValue(short value);

	short muxedPartnerChannel;


	class MuxCallback;
	friend class MuxCallback;
	class MuxCallback : public MeasurementCallback
	{
	public:
		MuxCallback(MultiplexerDevice* thisDevice, int nAverage) 
			: _this(thisDevice), _nAverage(nAverage), runningTotal(0), successfulMeasurements(0), numberOfResults(0), result(0), resultReady(false) {}

		void handleResult(const STI::Types::TMeasurement& measurement);

		void waitForResult();
		void stopWaiting();
		double getResult();
		
	private:
		MultiplexerDevice* _this;
		int _nAverage;
		int successfulMeasurements;
		int numberOfResults;
		double runningTotal;

		double result;
		bool resultReady;
		mutable boost::mutex callbackMutex;
		boost::condition_variable muxCondition;
	};

	typedef boost::shared_ptr<MuxCallback> MuxCallback_ptr;

	class MultiplexerEvent;
	friend class MultiplexerEvent;
	class MultiplexerEvent : public SynchronousEventAdapter
	{
	public:
		MultiplexerEvent(double time, MultiplexerDevice* device, const MuxCallback_ptr& muxCallback) 
			: SynchronousEventAdapter(time, device), _this(device), _muxCallback(muxCallback) {} 

		void collectMeasurementData();
		void stop();

	private:
		MultiplexerDevice* _this;
		MuxCallback_ptr _muxCallback;
	};

	


};

#endif

