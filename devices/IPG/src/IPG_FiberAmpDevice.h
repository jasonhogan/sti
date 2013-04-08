/*! \file IPG_FiberAmpDevice.h
 *  \author Jason Hogan
 *  \brief header file for IPG_FiberAmpDevice
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


#ifndef IPG_FIBERAMPDEVICE_H
#define IPG_FIBERAMPDEVICE_H

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <STI_Device_Adapter.h>
#include "rs232Controller.h"


class IPG_FiberAmpDevice : public STI_Device_Adapter
{
public:

	IPG_FiberAmpDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort);
	~IPG_FiberAmpDevice();

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	std::string execute(int argc, char* argv[]);

	std::string getDeviceHelp();


private:
	
	void powerFeedbackLoop();
	boost::system_time getWakeTime(boost::system_time sleepTime);

	std::string getValueFromResponse(const std::string& response) const;
	bool getStatusWord(unsigned long& status);
	bool getBit(unsigned long word, unsigned bitNumber);

	rs232Controller* serialController;

	int rs232QuerySleep_ms;

	boost::shared_ptr<boost::thread> feedbackLoopThread;	//shared pointer to loop thread object
	mutable boost::shared_mutex feedbackLoopMutex;
	boost::condition_variable_any feedbackLoopCondition;
	bool feedbackEnabled;
	bool deviceAlive;

	double loopUpdateTime_s;
	double loopSetpoint_W;
	double loopDeadband_W;
	double loopStepSize_percent;

	const unsigned emissionStatusBitNum;
};

#endif
