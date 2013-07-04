/*! \file IPG_FiberAmpDevice.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class IPG_FiberAmpDevice
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

#include "highPowerIntensityLock.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using STI::Utils::valueToString;
using namespace std;

highPowerIntensityLockDevice::highPowerIntensityLockDevice(ORBManager* orb_manager, std::string DeviceName, 
	std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort) : 
STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber),
emissionStatusBitNum(2)
{
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 57600, 8, "None", 1);

	rs232QuerySleep_ms = 200;

	//Power feedback loop parameters
	loopUpdateTime_s = 10;
	loopStepSize_percent = 0.1;

	loopSetpoint_W = 30;
	loopDeadband_W = 0.1;

	gain=1;

	feedbackEnabled = false;
	deviceAlive = true;

	feedbackLoopThread = boost::shared_ptr<boost::thread>(
			new boost::thread(&highPowerIntensityLockDevice::intensityLockLoop, this));
}
highPowerIntensityLockDevice::~highPowerIntensityLockDevice()
{
	{
		boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
		deviceAlive = false;
	}
    feedbackLoopCondition.notify_one();
}

void highPowerIntensityLockDevice::defineAttributes()
{

	boost::shared_lock< boost::shared_mutex > readLock(feedbackLoopMutex);

	addAttribute("Gain", gain);

//	addAttribute("Power Feedback Loop", "Disabled", "Enabled, Disabled");
//	addAttribute("Feedback Setpoint (W)", loopSetpoint_W);
//	addAttribute("Feedback Time Constant (s)", loopUpdateTime_s);
//	addAttribute("Feedback Deadband (W)", loopDeadband_W);
//	addAttribute("Feedback Step Size (%)", loopStepSize_percent);
}


void highPowerIntensityLockDevice::refreshAttributes()
{

	setAttribute("Gain", gain);

//	setAttribute("Feedback Setpoint (W)", loopSetpoint_W);
//	setAttribute("Feedback Time Constant (s)", loopUpdateTime_s);
//	setAttribute("Feedback Deadband (W)", loopDeadband_W);
//	setAttribute("Feedback Step Size (%)", loopStepSize_percent);
}

bool highPowerIntensityLockDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	std::string result;
	
	if( key.compare("Power Feedback Loop") == 0 ) {
		if( value.compare("Enabled") == 0 ) {
			{
				boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
				feedbackEnabled = true;
			}
			feedbackLoopCondition.notify_one();
			success = true;
		}
		if( value.compare("Disabled") == 0 ) {
			{
				boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
				feedbackEnabled = false;
			}
			success = true;
		}
	}

	if( key.compare("Feedback Setpoint (W)") == 0 ) {
		double newSetpoint;
		if( STI::Utils::stringToValue(value, newSetpoint) && newSetpoint > 0 ) {
			boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
			loopSetpoint_W = newSetpoint;
			success = true;
		}
	}
	if( key.compare("Feedback Time Constant (s)") == 0 ) {
		double newTimeConstant;
		if( STI::Utils::stringToValue(value, newTimeConstant) && newTimeConstant > 0 ) {
			boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
			loopUpdateTime_s = newTimeConstant;
			success = true;
		}
		if(success) {
			feedbackLoopCondition.notify_one();		//wakeup sleeping loop so it can check its new update time
		}
	}
	if( key.compare("Feedback Deadband (W)") == 0 ) {
		double newDeadband;
		if( STI::Utils::stringToValue(value, newDeadband) && newDeadband > 0 ) {
			boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
			loopDeadband_W = newDeadband;
			success = true;
		}
	}
	if( key.compare("Feedback Step Size (%)") == 0 ) {
		double newStepSize;
		if( STI::Utils::stringToValue(value, newStepSize) && newStepSize > 0 ) {
			boost::unique_lock< boost::shared_mutex > writeLock(feedbackLoopMutex);
			loopStepSize_percent = newStepSize;
			success = true;
		}
	}

	return success;
}

void highPowerIntensityLockDevice::intensityLockLoop()
{
	double lastVal;
	double setpoint;
	double lastVCA;
	double nextVCA;
	double error = setpoint-lastVal;
	nextVCA = lastVCA + gain*error;
	lastVCA = nextVCA;
}

boost::system_time highPowerIntensityLockDevice::getWakeTime(boost::system_time sleepTime)
{
	return sleepTime + boost::posix_time::milliseconds(static_cast<long>(loopUpdateTime_s * 1000));
}

std::string highPowerIntensityLockDevice::execute(int argc, char* argv[])
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		
		if(i == (args.size() - 1))
			command;	//terminator at end of command
		else
			command << " ";		//spaces between command words
	}

	std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);

	return result;
}

void highPowerIntensityLockDevice::defineChannels()
{

	addInputChannel(0, DataDouble, "Setpoint");

//	addOutputChannel(0, ValueNumber, "Set Diode Current");
	
//	addInputChannel(1, DataDouble, "Read Current Setpoint");
//	addInputChannel(2, DataDouble, "Read Output Power");

//	addInputChannel(3, DataDouble, "Read Amplifier Temperature");
//	addInputChannel(4, DataDouble, "Read Diode Current");

//	addInputChannel(5, DataString, "Device Status");
	
//	addInputChannel(6, DataBoolean, "Read Emission Status");
}



bool highPowerIntensityLockDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	bool success = false;
	std::stringstream command;
	std::string result;
	
	double numValue;
	
	if(channel == 1) {	//Read Current Setpoint
		result = serialController->queryDevice("RCS", rs232QuerySleep_ms);
		success = STI::Utils::stringToValue(getValueFromResponse(result), numValue);

		if(success) {
			dataOut.setValue(numValue);		//in percent
		}
	}
	
	if(channel == 2) {	//Read Output Power
		result = serialController->queryDevice("ROP", rs232QuerySleep_ms);

		std::string power = getValueFromResponse(result);
		unsigned found;

		//For low power, IPG returs 'Low' string. Map this to 0.
		found = power.find("Low");
		if (found != std::string::npos) {
			power = "0";
		}
		found = power.find("Off");
		if (found != std::string::npos) {
			power = "0";
		}

		success = STI::Utils::stringToValue(power, numValue);

		if(success) {
			dataOut.setValue(numValue);		//in Watts
		}
	}
	if(channel == 3) {	//Read Amplifier Temperature
		result = serialController->queryDevice("RCT", rs232QuerySleep_ms);
		success = STI::Utils::stringToValue(getValueFromResponse(result), numValue);

		if(success) {
			dataOut.setValue(numValue);		//in degrees C
		}
	}
	if(channel == 4) {	//Read Diode Current
		result = serialController->queryDevice("RDC", rs232QuerySleep_ms);
		success = STI::Utils::stringToValue(getValueFromResponse(result), numValue);

		if(success) {
			dataOut.setValue(numValue);		//in Amps
		}
	}
	
	if(channel == 5) {	//Device Status
		
		unsigned long status = 0;

		if(getStatusWord(status)) {
			dataOut.setValue( valueToString(status, "", ios::hex) );	//32 bit status word
			success = true;
		}
	}
	
	if(channel == 6) {	//Emission Status
		unsigned long status = 0;

		if(getStatusWord(status)) {
			dataOut.setValue(getBit(status, emissionStatusBitNum));	//0 = Emission Off, 1 = Emission On
			success = true;
		}
	}

	return success;
}

bool highPowerIntensityLockDevice::getBit(unsigned long word, unsigned bitNumber)
{
	return static_cast<bool> ( (word >> bitNumber) & 0x1 );
}


bool highPowerIntensityLockDevice::getStatusWord(unsigned long& status)
{
	std::string result = serialController->queryDevice("STA", rs232QuerySleep_ms);

	return STI::Utils::stringToValue(getValueFromResponse(result), status);
}

bool highPowerIntensityLockDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	bool success = false;
	std::stringstream command;

	if(channel == 0) {	//Set Diode Current
		double setpoint = value.getDouble();

		if(setpoint >= 0 && setpoint <= 100) {
			command << "SDC " << setpoint;
			std::string result = serialController->queryDevice(command.str(), rs232QuerySleep_ms);
			
			double numValue = 0;
			success = STI::Utils::stringToValue(getValueFromResponse(result), numValue);
		}

	}

	return success;
}

std::string highPowerIntensityLockDevice::getDeviceHelp() 
{ 
	std::stringstream help;
	help <<	
		"Channels: \n\n" <<
		"Channel 0: Set diode current (percent of max).\n" <<
		"\n" <<
		"Channel 1: Read the setpoint for the pump current (percent of max).\n" <<
		"\n" <<
		"Channel 2: Read the output power (Watts).\n" <<
		"\n" <<
		"Channel 3: Read internal temperature of the amplifier (degrees C).\n" <<
		"\n" <<
		"Channel 4:  Read actual pump diode current (Amps).\n" <<
		"\n" <<
		"Channel 5:  Read status word. Decimal value of 32 bit encoded status.\n" <<
		"\n" <<
		"Channel 6:  Read emission status (boolean). True = Emission On, False = Emission Off.\n" <<
		"\n";
	return help.str();

}


std::string highPowerIntensityLockDevice::getValueFromResponse(const std::string& response) const
{
	unsigned found = response.find_last_of(":");
	if (found != std::string::npos) {
		return response.substr(found + 1);
	}
	else {
		return response;
	}
}