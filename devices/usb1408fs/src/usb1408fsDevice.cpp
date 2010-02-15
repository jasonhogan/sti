/*! \file usb1408fsDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class usb1408fsDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  This uses code from Measurement Computing to control the USB 1408FS DAQ.
 *  The copywrite status of that code is unknown. 
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



#include "usb1408fsDevice.h"

usb1408fsDevice::usb1408fsDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	//constants for the usb 1408fs physical device
	BoardNum = 0;
    UDStat = 0;
    channel = 6;
    inputGain = BIP10VOLTS;
	outputGain = UNI4VOLTS;
//    DataValue = 0;
	Options = DEFAULTOPTION;
    // cbw software revision number
    RevLevel = (float)CURRENTREVNUM;

	// Declare UL Revision Level 
	UDStat = cbDeclareRevision(&RevLevel);

	/* Initiate error handling
       Parameters:
           PRINTALL :all warnings and errors encountered will be printed
           DONTSTOP :program will continue even if error occurs.
                     Note that STOPALL and STOPFATAL are only effective in 
                     Windows applications, not Console applications. 
   */
    cbErrHandling (PRINTALL, DONTSTOP);
}

usb1408fsDevice::~usb1408fsDevice()
{
}


void usb1408fsDevice::defineAttributes() 
{
	
}

void usb1408fsDevice::refreshAttributes() 
{

}

bool usb1408fsDevice::updateAttribute(string key, string value)
{
	return false;
}

void usb1408fsDevice::defineChannels()
{
	addOutputChannel(0, ValueNumber);
	addOutputChannel(1, ValueNumber);
	addInputChannel(6, DataDouble);
	addInputChannel(7, DataDouble);
}


bool usb1408fsDevice::writeChannel(unsigned short channel, const MixedValue& value)
{	
	return setOutputVoltage(channel, static_cast<float>(value.getNumber()) );
}

bool usb1408fsDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	double result;

	if( readInputChannel(channel, result) )
	{
		dataOut.setValue( result );
		return true;
	}
	else
	{
		return false;
	}
}

void usb1408fsDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void usb1408fsDevice::definePartnerDevices()
{
}

void usb1408fsDevice::stopEventPlayback()
{
}

std::string usb1408fsDevice::execute(int argc, char **argv)
{
	int channel;
	bool channelSuccess;
	int query = 0; //true (1) or false (0) if the command is expecting a response
	bool querySuccess;
	bool measureSuccess;
	double measuredValue = 0;
	bool commandSuccess;
	float commandValue;
	bool outputSuccess;
	string result;

	//command comes as "channel query(t/f)? value"
	if(argc == 3)
	{
		channelSuccess = stringToValue(argv[1], channel);
		querySuccess = stringToValue(argv[2], query);
		//std::cerr << " i got 3 values" << std::endl;
		//std::cerr << " channel is: " << channel << std::endl;
		//std::cerr << "query (tf)? " << query << std::endl;
	}
	else if(argc == 4)
	{
		channelSuccess = stringToValue(argv[1], channel);
		querySuccess = stringToValue(argv[2], query);
		commandSuccess = stringToValue(argv[3], commandValue);
	}
	else
		return "0"; //command needs to contain 2 pieces of information

	if(query == 1 && querySuccess)
	{
		// measure channel
		measureSuccess = readInputChannel(channel, measuredValue);
		std::cerr << "measured value? " << measuredValue << std::endl;
		result = valueToString(measuredValue);
		return result;
	}
	else if(!query && querySuccess)
	{
		//command an output voltage
		outputSuccess = setOutputVoltage(channel, commandValue );
		if(outputSuccess)
			return "1";
		else
			return "0";
	}
	else
		return "0";	
}
bool usb1408fsDevice::deviceMain(int argc, char **argv)
{
	return false;
}
bool usb1408fsDevice::setOutputVoltage(int channel, float outputVoltage)
{
	bool success = false;
	// the cbw.h library requires that this takes a float?! really?
	UDStat = cbVOut (BoardNum, channel, outputGain, outputVoltage, Options);
	if(UDStat == NOERRORS)
		success = true;

	return success;
}
bool usb1408fsDevice::readInputChannel(int channel, double& result)
{
	float DataValue;
	UDStat = cbVIn (BoardNum, channel, inputGain, &DataValue, Options);


	if(UDStat == NOERRORS)
	{
		std::cout << "\nThe voltage on Channel" << channel << "is: " << DataValue << std::endl;
		result = DataValue;
		return true;
	}
	else
	{
		std::cerr << "There were errors on reading the data from the USB1408FS." << std::endl;
		return false;
	}

}
bool usb1408fsDevice::readMUXedInputChannel(int channel, double measurement)
{
	bool success = false;

	return success;
}