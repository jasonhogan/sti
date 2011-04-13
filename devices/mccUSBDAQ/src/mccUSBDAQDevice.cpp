/*! \file MccUSBDAQDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class MccUSBDAQDevice
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



#include "MccUSBDAQDevice.h"

MccUSBDAQDevice::MccUSBDAQDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							int boardNum_) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber), boardNum(boardNum_)
{
	//constants for the usb daq physical device
    UDStat = 0;

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

	initialized = getChannelInfo();
	
	ADInRange = availableADInRanges.begin()->first;
	DAOutRange = availableDAOutRanges.begin()->first;

}

MccUSBDAQDevice::~MccUSBDAQDevice()
{
}


void MccUSBDAQDevice::defineAttributes() 
{

	if (!availableADInRanges.empty())
		addAttribute("AD In gain", availableADInRanges.find(ADInRange)->second,
			makeRangeString(availableADInRanges));

	if (!availableDAOutRanges.empty())
		addAttribute("DA Out gain", availableDAOutRanges.find(DAOutRange)->second,
			makeRangeString(availableDAOutRanges));

}

void MccUSBDAQDevice::refreshAttributes() 
{
	if (!availableADInRanges.empty())
		setAttribute("AD In gain", availableADInRanges.find(ADInRange)->second);

	if (!availableDAOutRanges.empty())
		setAttribute("DA Out gain", availableDAOutRanges.find(DAOutRange)->second);
}

bool MccUSBDAQDevice::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	//Doesn't actually do anything; this device monitors only

	if(key.compare("AD In gain") == 0)
	{
		success = true;
		std::map <int, std::string>::iterator it;
		for (it = availableADInRanges.begin(); it != availableADInRanges.end(); it++)
		{
			if (it->second.compare(value) == 0)
				break;
		}

		if (it == availableADInRanges.end())
			success = false;
		else
			ADInRange = it->first;
	}
	else if(key.compare("DA Out gain") == 0)
	{
		success = true;
		std::map <int, std::string>::iterator it;
		for (it = availableDAOutRanges.begin(); it != availableDAOutRanges.end(); it++)
		{
			if (it->second.compare(value) == 0)
				break;
		}

		if (it == availableDAOutRanges.end())
			success = false;
		else
			DAOutRange = it->first;
	}
	return success;
}

void MccUSBDAQDevice::defineChannels()
{
	//Analog Input Channels
	for (int i = 0; i < numADChans; i++)
	{
		addInputChannel(i, DataDouble);
		analogInChannels.insert(pair<int,int>(i, i));
	}

	for (int i = 0; i < numDAChans; i++)
	{
		addOutputChannel(i + 10, ValueNumber);
		analogOutChannels.insert(pair<int,int>(i, i + 10));
	}
}


bool MccUSBDAQDevice::writeChannel(unsigned short channel, const MixedValue& value)
{	
	return setOutputVoltage(channel, static_cast<float>(value.getNumber()) );
}

bool MccUSBDAQDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
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

void MccUSBDAQDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	
}
void MccUSBDAQDevice::definePartnerDevices()
{
}

void MccUSBDAQDevice::stopEventPlayback()
{
}

std::string MccUSBDAQDevice::execute(int argc, char **argv)
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
bool MccUSBDAQDevice::deviceMain(int argc, char **argv)
{
	return false;
}
bool MccUSBDAQDevice::setOutputVoltage(int channel, float outputVoltage)
{
	bool success = false;
	// the cbw.h library requires that this takes a float?! really?
	UDStat = cbVOut (boardNum, analogInChannels.find(channel)->second, DAOutRange, outputVoltage, Options);
	if(UDStat == NOERRORS)
		success = true;

	return success;
}
bool MccUSBDAQDevice::readInputChannel(int channel, double& result)
{
	float DataValue;
	UDStat = cbVIn (boardNum, analogInChannels.find(channel)->second, ADInRange,
		&DataValue, Options);


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
bool MccUSBDAQDevice::readMUXedInputChannel(int channel, double measurement)
{
	bool success = false;

	return success;
}

std::string MccUSBDAQDevice::makeRangeString(std::map <int, std::string> rangeMap)
{
	std::map<int, std::string>::iterator it;
	std::string rangeString = "";

	for (it = rangeMap.begin(); it != rangeMap.end(); it++)
	{
		if (it != rangeMap.begin())
			rangeString += ", ";
		rangeString += it->second;
	}

	return rangeString;
}

bool MccUSBDAQDevice::getChannelInfo()
{	

	pair <int, std::string> bip20V (BIP20VOLTS,"+/- 20 V");
	pair <int, std::string> bip10V (BIP10VOLTS,"+/- 10 V");
	pair <int, std::string> bip5V (BIP5VOLTS,"+/- 5 V");
	pair <int, std::string> bip4V (BIP4VOLTS,"+/- 4 V");
	pair <int, std::string> bip2pt5V (BIP2PT5VOLTS,"+/- 2.5 V");
	pair <int, std::string> bip2V (BIP2VOLTS,"+/- 2 V");
	pair <int, std::string> bip1pt25V (BIP1PT25VOLTS,"+/- 1.25 V");
	pair <int, std::string> bip1V (BIP1VOLTS,"+/- 1 V");
	pair <int, std::string> bippt625V (BIPPT625VOLTS,"+/- 0.625 V");
	pair <int, std::string> bippt5V (BIPPT5VOLTS,"+/- 0.5 V");
	pair <int, std::string> bippt25V (BIPPT25VOLTS,"+/- 0.25 V");
	pair <int, std::string> bippt2V (BIPPT2VOLTS,"+/- 0.2 V");
	pair <int, std::string> bippt1V (BIPPT1VOLTS,"+/- 0.1 V");
	pair <int, std::string> bippt05V (BIPPT05VOLTS,"+/- 0.05 V");
	pair <int, std::string> bippt01V (BIPPT01VOLTS,"+/- 0.01 V");
	pair <int, std::string> bippt005V (BIPPT005VOLTS,"+/- 0.005 V");
	pair <int, std::string> bip1pt67V (BIP1PT67VOLTS,"+/- 1.67 V");

	pair <int, std::string> uni10V (UNI10VOLTS,"0 - 10 V");
	pair <int, std::string> uni5V (UNI5VOLTS,"0 - 5 V");
	pair <int, std::string> uni4V (UNI4VOLTS,"0 - 4 V");
	pair <int, std::string> uni2pt5V (UNI2PT5VOLTS,"0 - 2.5 V");
	pair <int, std::string> uni2V (UNI2VOLTS,"0 - 2 V");
	pair <int, std::string> uni1pt67V (UNI1PT67VOLTS,"+/- 1.67 V");
	pair <int, std::string> uni1pt25V (UNI1PT25VOLTS,"+/- 1.25 V");
	pair <int, std::string> uni1V (UNI1VOLTS,"+/- 1 V");
	pair <int, std::string> unipt5V (UNIPT5VOLTS,"+/- 0.5 V");
	pair <int, std::string> unipt25V (UNIPT25VOLTS,"+/- 0.25 V");
	pair <int, std::string> unipt2V (UNIPT2VOLTS,"+/- 0.2 V");
	pair <int, std::string> unipt1V (UNIPT1VOLTS,"+/- 0.1 V");
	pair <int, std::string> unipt05V (UNIPT05VOLTS,"+/- 0.05 V");
	pair <int, std::string> unipt02V (UNIPT02VOLTS,"+/- 0.02 V");
	pair <int, std::string> unipt01V (UNIPT01VOLTS,"+/- 0.01 V");

	std::map <int, std::string> ranges;

	cbGetConfig (BOARDINFO, boardNum, 0, BIBOARDTYPE, &boardType);
	cbGetConfig (BOARDINFO, boardNum, 0, BINUMADCHANS, &numADChans);
	cbGetConfig (BOARDINFO, boardNum, 0, BINUMDACHANS, &numDAChans);
	
	switch(boardType) //ID numbers from Measurement Computing
	{
		//USB 1208LS
		case 122:
			if (numADChans == 8) {
				//single-ended
				availableADInRanges.insert(bip10V);
			}
			else if (numADChans == 4) {
				//Differential
				availableADInRanges.insert(bip20V);
				availableADInRanges.insert(bip10V);
				availableADInRanges.insert(bip5V);
				availableADInRanges.insert(bip4V);
				availableADInRanges.insert(bip2pt5V);
				availableADInRanges.insert(bip2V);
				availableADInRanges.insert(bip1pt25V);
				availableADInRanges.insert(bip1V);

			}
			else {
				return false;
			}

			if (numDAChans == 2) {
				availableDAOutRanges.insert(uni5V);
			}
			else
				return false;

			break;

		//USB 1408FS
		case 161:
			if (numADChans == 8) {
				//single-ended
				availableADInRanges.insert(bip10V);
			}
			else if (numADChans == 4) {
				//Differential
				availableADInRanges.insert(bip20V);
				availableADInRanges.insert(bip10V);
				availableADInRanges.insert(bip5V);
				availableADInRanges.insert(bip4V);
				availableADInRanges.insert(bip2pt5V);
				availableADInRanges.insert(bip2V);
				availableADInRanges.insert(bip1pt25V);
				availableADInRanges.insert(bip1V);

			}
			else {
				return false;
			}

			if (numDAChans == 2) {
				availableDAOutRanges.insert(uni4V);
			}
			else
				return false;

			break;
		default:
			return false;
			break;
	}

	return true;
}

