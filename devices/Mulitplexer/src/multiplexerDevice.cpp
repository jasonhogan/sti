/*! \file MultiplexerDevice.cpp
 *  \author David M.S. Johnson and Jason Hogan
 *  \brief Source-file for the class MultiplexerDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
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



#include "multiplexerDevice.h"
#include <iostream>
#include <string>
using namespace std;

MultiplexerDevice::MultiplexerDevice(ORBManager* orb_manager, 
							std::string    DeviceName, 
							std::string configFilename) : 
STI_Device_Adapter(orb_manager, DeviceName, configFilename)
{

	configFile = ConfigFile_ptr(new ConfigFile(configFilename));

	controlChannel0 = 0;
	controlChannel1 = 1;
	controlChannel2 = 2;
	controlChannel3 = 3;

	if( !(configFile->getParameter("controlChannel0", controlChannel0) &&
		configFile->getParameter("controlChannel1", controlChannel1) &&
		configFile->getParameter("controlChannel2", controlChannel2) &&
		configFile->getParameter("controlChannel3", controlChannel3)) 
		)
		cout << "Could not find one or more control channels in config file." << endl;

	controlLineHigh = 1;
	controlLineLow = 0;
	if( !(configFile->getParameter("controlLineHigh", controlLineHigh) &&
		configFile->getParameter("controlLineLow", controlLineLow))
		)
		cout << "Could not find controlLineHigh and/or controlLineLow in config file." << endl;

	muxedPartnerChannel = 0;
	if( !(configFile->getParameter("muxedPartnerChannel", muxedPartnerChannel))
		)
		cout << "Could not find muxed partner channel in config file." << endl;

}


MultiplexerDevice::~MultiplexerDevice()
{
}


void MultiplexerDevice::defineChannels()
{
	for(unsigned i = 0; i < 16; i++) {
		addInputChannel(i, STI::Types::DataDouble, STI::Types::ValueNumber);
	}
}
void MultiplexerDevice::definePartnerDevices()
{
	//Fabry Perot specturm
	std::string muxedPartnerIP = "";
	short muxedPartnerModule = 0;
	std::string muxedPartnerDeviceName = "";
	
	configFile->getParameter("muxedPartnerIP", muxedPartnerIP);
	configFile->getParameter("muxedPartnerModule", muxedPartnerModule);
	configFile->getParameter("muxedPartnerName", muxedPartnerDeviceName);

	addPartnerDevice("Muxed Partner", muxedPartnerIP, muxedPartnerModule, muxedPartnerDeviceName);
	partnerDevice("Muxed Partner").enablePartnerEvents();

	std::string controllerIP = "";
	short controllerModule = 0;
	std::string controllerDeviceName = "";
	
	configFile->getParameter("controllerIP", controllerIP);
	configFile->getParameter("controllerModule", controllerModule);
	configFile->getParameter("controllerName", controllerDeviceName);

	addPartnerDevice("Control Partner", controllerIP, controllerModule, controllerDeviceName);
	partnerDevice("Control Partner").enablePartnerEvents();

}
void MultiplexerDevice::parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{
	double holdoff = 0;
	double minimumAbsoluteStartTime; //see note below;
	double minimumEventSpacing; //100 us. Conservative. Mux should be able to do 350 ns, but there is a 100 kHz filter.

	configFile->getParameter("minimumEventSpacing", minimumEventSpacing);

	minimumAbsoluteStartTime = minimumEventSpacing;

	double nextInitializationTime = 0;	//the next absolute time that the scope is free to run an initialization event

	RawEventMap::const_iterator events;

	MuxCallback_ptr muxCallback;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	int nAverage = 1;
	double dtAverage = 0;
	configFile->getParameter("numberOfPointsAveraged", nAverage);
	configFile->getParameter("timeBetweenPointsAveraged", dtAverage);

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{

		if(events->second.size() > 1) {
			throw EventConflictException(events->second.at(0), events->second.at(1), 
				"The multiplexer cannot currently have multiple events at the same time.");
		}

		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
			previousTime = minimumAbsoluteStartTime - minimumEventSpacing - (nAverage - 1)*dtAverage;
		
		eventTime = events->first - holdoff;

		if( (events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The multiplexer needs " + valueToString(minimumEventSpacing) + " ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The multiplexer needs " + valueToString(minimumAbsoluteStartTime)+ " ns at the beginning of the timing file.");
		}


		sendControlLineEvents(events->first - minimumEventSpacing, events->second.at(0).channel(), events->second.at(0));

		muxCallback = MuxCallback_ptr(new MuxCallback(this, nAverage));

		int i;
		for(i=0; i<nAverage; i++)
		{
			partnerDevice("Muxed Partner").
				meas(events->first + i*dtAverage, muxedPartnerChannel, 1.1, events->second.at(0), muxCallback, "Measure Mux");
		}

		eventsOut.push_back( new MultiplexerEvent(events->first, this, muxCallback) );
		eventsOut.back().addMeasurement( events->second.at(0) );


	}


}

void MultiplexerDevice::sendControlLineEvents(double eventTime, short channel, const RawEvent& evt)
{
	short shiftedChannel;
	double controlLineValue0, controlLineValue1, controlLineValue2, controlLineValue3;

	controlLineValue0 = getControlLineValue(channel%2);
	
	shiftedChannel = channel >> 1;

	controlLineValue1 = getControlLineValue(shiftedChannel%2);

	shiftedChannel = shiftedChannel >> 1;

	controlLineValue2 = getControlLineValue(shiftedChannel%2);

	shiftedChannel = shiftedChannel >> 1;

	controlLineValue3 = getControlLineValue(shiftedChannel%2);


	//std::cerr << "Control line values: " << controlLineValue3 << controlLineValue2 << controlLineValue1 << controlLineValue0 << std::endl;


	partnerDevice("Control Partner").event(eventTime, 
					controlChannel3, controlLineValue3, evt);
	partnerDevice("Control Partner").event(eventTime + 1100, 
					controlChannel2, controlLineValue2, evt);
	partnerDevice("Control Partner").event(eventTime + 2200, 
					controlChannel1, controlLineValue1, evt);
	partnerDevice("Control Partner").event(eventTime + 3300, 
					controlChannel0, controlLineValue0, evt);


}

double MultiplexerDevice::getControlLineValue(short value)
{
	if (value == 1)
		return controlLineHigh;
	else if (value == 0)
		return controlLineLow;
	else
	{
		std::cerr << "Input to setControlLineValue must be 0 or 1. Using Low." << std::endl;
		return controlLineLow;
	}

}


void MultiplexerDevice::MuxCallback::handleResult(const STI::Types::TMeasurement& measurement)
{

/////////////////////////////////////////////
// Include a broadcast when result has been handled
////////////////////////////////////////////

	boost::lock_guard<boost::mutex> lock(callbackMutex);

	result = 0;

	//Only keep measurements for averaging if valid
	if(measurement.data.doubleVal() > -9.9) {
		runningTotal += measurement.data.doubleVal();
		successfulMeasurements++;
	}

	//Take average on final callback
	numberOfResults++;
	if(_nAverage == numberOfResults) {

		if(successfulMeasurements > 0) {
			//Take the average
			result = runningTotal / successfulMeasurements;
		}
		runningTotal = 0;
		successfulMeasurements = 0;
		numberOfResults = 0;
	}

	resultReady = true;
	muxCondition.notify_all();

}

void MultiplexerDevice::MuxCallback::waitForResult() 
{ 
	boost::unique_lock<boost::mutex> lock(callbackMutex);

	while(!resultReady)
	{
		muxCondition.wait(lock);
	}

}

void MultiplexerDevice::MuxCallback::stopWaiting() 
{ 
	
	boost::lock_guard<boost::mutex> lock(callbackMutex);

	if (!resultReady)
	{
		result = 0;
		resultReady = true;
	}
	
	muxCondition.notify_all();
}

double MultiplexerDevice::MuxCallback::getResult()
{
	boost::lock_guard<boost::mutex> lock(callbackMutex);

	return result;
}

void MultiplexerDevice::MultiplexerEvent::collectMeasurementData()
{
	std::cerr << "Waiting for channel " << eventMeasurements.at(0)->channel() << std::endl;
	_muxCallback->waitForResult();

	std::cerr << "Done waiting for channel " << eventMeasurements.at(0)->channel() << std::endl;

	eventMeasurements.at(0)->setData( _muxCallback->getResult() );
}

void MultiplexerDevice::MultiplexerEvent::stop()
{
	STI_Device::SynchronousEvent::stop();

	_muxCallback->stopWaiting();
	
}