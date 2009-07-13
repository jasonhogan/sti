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
    DataValue = 0;
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
	addInputChannel(6, DataNumber);
	addInputChannel(7, DataNumber);
}

bool usb1408fsDevice::writeChannel(const RawEvent& Event)
{	
	return setOutputVoltage(Event.channel(), Event.numberValue() );
}

bool usb1408fsDevice::readChannel(ParsedMeasurement& Measurement)
{
	bool success = false;
	double measuredValue = 0;

	bool successReadInput = readInputChannel(Measurement.channel(), measuredValue);
	if(successReadInput)
	{
		Measurement.setData( measuredValue );
		success = true;
	}
	
	return success;
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
	return "";	
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
bool usb1408fsDevice::readInputChannel(int channel, double measurement)
{
	bool success = false;
	UDStat = cbVIn (BoardNum, channel, inputGain, &DataValue, Options);


	if(UDStat == NOERRORS)
	{
		std::cout << "\nThe voltage on Channel" << channel << "is: " << DataValue << std::endl;
		measurement = DataValue;
		success = true;
	}

	return success;
}
bool usb1408fsDevice::readMUXedInputChannel(int channel, double measurement)
{
	bool success = false;

	return success;
}