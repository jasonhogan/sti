/*! \file PDMonitorDevice.cpp
 *  \author Susannah Dickerson
 *  \brief Template for STI_Devices
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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



#include "PDMonitorDevice.h"
#include <fstream>
#include <string>


PDMonitorDevice::PDMonitorDevice(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    configFile,
					   std::string    appConfigFilePath) :
appConfigFile(appConfigFilePath),
STI_Application(orb_manager, DeviceName, configFile)
{
	//Initialization of device

	initialized = true;

	bool success;
	std::map<unsigned short, std::pair<unsigned short, std::string> > channels; //(Device Channel, USB Channel)
	std::map<unsigned short, std::string> channelPartner;
	std::map<unsigned short, std::string > channelLabels;
	std::map<unsigned short, double > channelSetpoints;
	std::map<unsigned short, std::vector<int> > channelLayout;

	success = getChannels(channels);
	success &= getLabels(channelLabels);
	success &= getSetpoints(channelSetpoints);
	success &= getLayout(channelLayout);
	success &= getPartnerDevices();

	if (!success)
		initialized = false;
	else
	{
		PhotoDetector pd;
		std::map<unsigned short, std::pair<unsigned short, std::string> >::iterator it;
		for(it = channels.begin(); it != channels.end(); it++)
		{
			pd.deviceChannel = it->first;
			pd.partnerChannel = it->second.first;
			pd.partnerName = it->second.second;
			if(channelLabels.find(it->first) != channelLabels.end())
				pd.label = channelLabels.find(it->first)->second;
			else
				pd.label = "";
			if(channelSetpoints.find(it->first) != channelSetpoints.end())
				pd.setpoint = channelSetpoints.find(it->first)->second;
			else
				pd.setpoint = 0;
			if(channelLayout.find(it->first) != channelLayout.end())
				pd.layout = channelLayout.find(it->first)->second;
			else {
				pd.layout.clear();
			}

			photoDetectorMap[it->first] = pd;
		}
	}
		

	return;
}

PDMonitorDevice::~PDMonitorDevice()
{

}

bool PDMonitorDevice::getChannels(std::map <unsigned short, std::pair<unsigned short, std::string> > &channels)
{
	unsigned short deviceChannel = 0;
	unsigned short partnerChannel = 0;
	bool success = false;

	std::vector <std::string> field;
	if (!(appConfigFile.getField("Channels", field)))
		return false;

	/*std::map<std::string, std::string> channelStringMap;
	getCommaColonMap(field, channelStringMap);

	channels.clear();

	//import device to usb channel associations
	std::map<std::string, std::string>::iterator it;
	for (it = channelStringMap.begin(); it != channelStringMap.end(); it++)
	{
		success = stringToValue(it->first, deviceChannel);
		success &= stringToValue(it->second, partnerChannel);
		if (success) {
			//USB DAQ Analog input channels must be between 0 and 7, inclusive
			if(partnerChannel >= 0 && partnerChannel < 8)
				channels[deviceChannel] = partnerChannel;
		}
	}*/

	std::vector<std::vector<std::string> > channelStringVector;
	getCommaColonVector(field, channelStringVector);

	channels.clear();
	std::pair<unsigned short, std::string> pairTemp;

	//import device to usb channel associations
	std::vector<std::vector<std::string> >::iterator it;
	for (it = channelStringVector.begin(); it != channelStringVector.end(); it++)
	{
		success = false;
		if (it->size() == 3) {
			success = stringToValue(it->at(0), deviceChannel);
			success &= stringToValue(it->at(1), partnerChannel);
		}
		if (success) {
			//USB DAQ Analog input channels must be between 0 and 7, inclusive
			if(partnerChannel >= 0 && partnerChannel < 8) {
				pairTemp.first = partnerChannel;
				pairTemp.second = it->at(2);
				channels[deviceChannel] = pairTemp; //pair of the partner channel and the partner name
			}
		}
	}

	return success;
}

bool PDMonitorDevice::getLayout(std::map<unsigned short, std::vector<int> > &channelLayout)
{
	std::vector <std::string> field;
	if (!(appConfigFile.getField("Layout", field)))
		return false;
	
	std::size_t beginPos;
	std::size_t endPos;
	
	unsigned short channel;
	std::vector<int> channelPos;

	channelLayout.clear();

	for (int i = 0; (unsigned) i < field.size(); i ++)
	{
		beginPos = field.at(i).find_first_not_of(" ");
		while(beginPos != std::string::npos)
		{
			//record position of channel
			endPos = field.at(i).find_first_of(" ", beginPos + 1);
			stringToValue(field.at(i).substr(beginPos,endPos), channel);
			
			channelPos.clear();
			channelPos.push_back(beginPos);
			channelPos.push_back(i);

			channelLayout[channel] = channelPos;

			//find next channel
			beginPos = field.at(i).find_first_not_of(" ", endPos);
		}
	}

	if (channelLayout.empty())
		return false;
	else
		return true;
}

bool PDMonitorDevice::getLabels(std::map<unsigned short, std::string > &channelLabels)
{
	unsigned short deviceChannel = 0;
	std::string channelLabel = "";
	bool success = false;

	std::vector <std::string> field;
	if(!(appConfigFile.getField("Labels", field)))
		return false;

	std::map<std::string, std::string> stringMap;
	getCommaColonMap(field, stringMap);

	channelLabels.clear();

	std::map<std::string, std::string>::iterator it;
	for (it = stringMap.begin(); it != stringMap.end(); it++)
	{
		success = stringToValue(it->first, deviceChannel);
		channelLabel = it->second;
		channelLabels[deviceChannel] = channelLabel;
	}

	return success;
}

bool PDMonitorDevice::getSetpoints(std::map<unsigned short, double > &channelSetpoints)
{
	unsigned short deviceChannel = 0;
	double channelSetpoint = 0.0;
	bool success = false;

	std::vector <std::string> field;
	if(!(appConfigFile.getField("Setpoints", field)))
		return false;;

	std::map<std::string, std::string> stringMap;
	getCommaColonMap(field, stringMap);

	channelSetpoints.clear();

	std::map<std::string, std::string>::iterator it;
	for (it = stringMap.begin(); it != stringMap.end(); it++)
	{
		success = stringToValue(it->first, deviceChannel);
		success &= stringToValue(it->second, channelSetpoint);
		channelSetpoints[deviceChannel] = channelSetpoint;
	}

	return success;
}
bool PDMonitorDevice::getPartnerDevices()
{
	std::vector <std::string> field;

	if (!(appConfigFile.getField("PartnerDevices", field)))
		return false;

	partnerSettings.clear();

	getCommaColonVector(field, partnerSettings);
	
	//error check
	if (partnerSettings.size() == 0)
		return false;
	
	std::vector<std::vector<std::string> >::iterator it;
	int partnerModule;
	for (it = partnerSettings.begin(); it != partnerSettings.end(); it++)
	{
		if(it->size() != 3)
			return false;
		if(!stringToValue(it->at(1), partnerModule))
			return false;
	}

	return true;
}

void PDMonitorDevice::getCommaColonMap(std::vector<std::string> &field, std::map<std::string, std::string> &fieldMap)
{
	std::size_t channelBegin;
	std::size_t channelEnd;
	std::string first;
	std::string second;

	std::vector<std::string>::iterator it;
	for (it = field.begin(); it != field.end(); it++)
	{
		channelBegin = it->find_first_not_of(" ");
		
		if (channelBegin == std::string::npos)
			continue;

		channelEnd = it->find_first_of(",", channelBegin + 1);

		if (channelEnd == std::string::npos) {
			std::cerr << "getCommaColonMap: Comma expected; none found.\n";
			continue;
		}

		first = it->substr(channelBegin, channelEnd - channelBegin);

		channelBegin = it->find_first_not_of(" ", channelEnd + 1);
//		channelBegin = channelEnd + 1;

		if (channelBegin == std::string::npos) {
			std::cerr << "getCommaColonMap: Expect more characters after comma";
			continue;
		}

		channelEnd = it->find_first_of(";", channelBegin + 1);

		second = it->substr(channelBegin, channelEnd-channelBegin);

		fieldMap[first] = second;
	}

	return;
}

void PDMonitorDevice::getCommaColonVector(std::vector<std::string> &field, std::vector<std::vector<std::string> > &fieldVector)
{
	std::size_t entryBegin;
	std::size_t entryEnd;
	std::vector<std::string> parsedLine;

	std::vector<std::string>::iterator it;
	for (it = field.begin(); it != field.end(); it++)
	{
		parsedLine.clear();
		entryBegin = it->find_first_not_of(" ");
		
		if (entryBegin == std::string::npos)
			continue;

		entryEnd = it->find_first_of(",", entryBegin + 1);
		while (entryEnd != std::string::npos)
		{
			parsedLine.push_back(it->substr(entryBegin, entryEnd - entryBegin));

			entryBegin = it->find_first_not_of(" ", entryEnd + 1);

			if (entryBegin == std::string::npos) {
				std::cerr << "getCommaColonVector: Expect more characters after comma";
				continue;
			}
			entryEnd = it->find_first_of(",", entryBegin + 1);
		}

		entryEnd = it->find_first_of(";", entryBegin + 1);

		parsedLine.push_back(it->substr(entryBegin, entryEnd-entryBegin));

		fieldVector.push_back(parsedLine);
	}

	return;
}

/*void PDMonitorDevice::getChannels(TaggedConfigFile &appConfigFile)
{
	std::vector <std::string> field;
	appConfigFile.getField("Channels", field);
	
	std::size_t channelBegin;
	std::size_t channelEnd;
	unsigned short deviceChannel;
	unsigned short partnerChannel;
	bool success = true;

	channels.clear();

	std::vector<std::string>::iterator it;
	for (it = field.begin(); it != field.end(); it++)
	{
		channelBegin = it->find_first_not_of(" ");
		channelEnd = it->find_first_of(",", channelBegin + 1);

		success = stringToValue(it->substr(channelBegin, channelEnd), deviceChannel);

//		channelBegin = it->find_first_not_of(" ", channelEnd + 1);
		channelBegin = channelEnd + 1;
		channelEnd = it->find_first_of(";", channelBegin + 1);

		success &= stringToValue(it->substr(channelBegin, channelEnd), partnerChannel);
		channels[deviceChannel]=partnerChannel;
	}

	return;
}*/


MixedData PDMonitorDevice::handleFunctionCall(std::string functionName, std::vector<MixedValue> args)
{
	bool success;

	if (functionName.compare("readChannel")==0) {
		if (args.size() != 2)
			return MixedData();

		unsigned short channel = (short) args.at(0).getInt();
		MixedValue valueIn = MixedValue(args.at(1));
		MixedData dataOut;
		success = readAppChannel(channel, valueIn, dataOut);
		if (success)
			return dataOut;
	}
	if (functionName.compare("getChannelLayout") == 0) {
		if (args.size() != 1)
			return MixedData();

		unsigned short channel = (short) args.at(0).getInt();
		MixedData position;
		if(!(photoDetectorMap.find(channel)->second.layout.empty()))
			position = photoDetectorMap.find(channel)->second.layout;
		else
			position = MixedData();

		return position;
	}
	if(functionName.compare("getChannelLabel") == 0)
	{
		if (args.size() != 1)
			return MixedData();

		unsigned short channel = (short) args.at(0).getInt();
		MixedData label = photoDetectorMap.find(channel)->second.label;
		return label;
	}
	if(functionName.compare("getChannelSetpoint") == 0)
	{
		if (args.size() != 1)
			return MixedData();

		unsigned short channel = (short) args.at(0).getInt();
		MixedData setpoint = photoDetectorMap.find(channel)->second.setpoint;
		return setpoint;
	}
	if (functionName.compare("writeSetpoints") == 0)
	{
		std::vector<MixedValue> argVector;
		if (args.size() != 1)
			return MixedData(false);
		if (args.at(0).getType() != MixedValue::Vector)
			return MixedData(false);

		argVector = args.at(0).getVector();

		if (argVector.size() != photoDetectorMap.size())
			return MixedData(false);

		std::vector<MixedValue>::iterator it;
		std::vector<MixedValue> tmp;
		
		//error checking; expecteing a vector of (ushort channel, double setpont value)
		for(it = argVector.begin(); it != argVector.end(); it++)
		{
			if (it->getType() != MixedValue::Vector)
				return MixedData(false);
			tmp = it->getVector();
			if (tmp.size() != 2)
				return MixedData(false);
			if (tmp.at(0).getType() != MixedValue::Double || tmp.at(1).getType() != MixedValue::Double)
				return MixedData(false);

			if (photoDetectorMap.find((short) tmp.at(0).getDouble()) == photoDetectorMap.end())
				return MixedData(false);

			photoDetectorMap.find((short) tmp.at(0).getDouble())->second.setpoint = tmp.at(1).getDouble();
		}

		return MixedData(writeSetpoints());
	}


	return MixedData();
}

bool PDMonitorDevice::appMain(int argc, char **argv)
{

	std::cerr << "OK" << std::endl;

	return false;
}

void PDMonitorDevice::defineAttributes()
{
	enableCalibration = false;
	enableDataLogging = false;
	addAttribute("Calibration?", "Off", "On, Off, Refresh");
	addAttribute("Data Logging?", "Off", "On, Off");
	stopDataLogging();
}

void PDMonitorDevice::refreshAttributes() 
{
	setAttribute("Calibration?", (enableCalibration ? "On" : "Off"));
	setAttribute("Data Logging?", (enableDataLogging ? "On" : "Off")); //response to the IDN? query
}

bool PDMonitorDevice::updateAttribute(string key, string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;


	if(key.compare("Calibration?") == 0)
	{
		success = true;
		// fill calibration vector if the calibration is not already set
		// or if the user has requested the vector to be refreshed
		if ((value.compare("On") == 0 && !enableCalibration) || value.compare("Refresh") == 0)
		{
			success = enableCalibration;
		} else if (value.compare("Off") == 0 && enableCalibration) {
			enableCalibration = false;
/*			pd1.slope = 1;
			pd1.offset = 0;
			pd2.slope = 1;
			pd2.offset = 0;*/
			success = true;
		}
	}
	else if(key.compare("Data Logging?") == 0)
	{
		if(value.compare("On") == 0)
		{
			enableDataLogging = true;
			startDataLogging();
		}
		else
		{
			enableDataLogging = false;
			stopDataLogging();
		}

		success = true;
	}


	return success;
}

void PDMonitorDevice::defineAppChannels()
{

	//Input Channels: photodetectors
	std::map<unsigned short, PhotoDetector>::iterator it;
	for(it = photoDetectorMap.begin(); it != photoDetectorMap.end(); it++)
	{
		addInputChannel(it->first, DataString, ValueNumber);
		addLoggedMeasurement(it->first, 60, 5*60, 3);
	}
}

bool PDMonitorDevice::readAppChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	bool success = true;

	std::string daqVoltageString; 
	std::string channelQuery;
	double daqVoltage;
	PhotoDetector pd;

	pd = photoDetectorMap.find(channel)->second;

	channelQuery = valueToString(pd.partnerChannel) + " 1";
	daqVoltageString = partnerDevice(pd.partnerName).execute(channelQuery);
	
	success = stringToValue(daqVoltageString, daqVoltage);
	if(success)
		dataOut.setValue(daqVoltage);
	else
		dataOut.setValue(0);

	return success;
}

bool PDMonitorDevice::writeAppChannel(unsigned short channel, const MixedValue& valuet)
{
	bool success = true;
	std::string msg = "";
	//double valueDouble;

	/*
	if (channel != 4)
	{
		std::cerr << "Expect a channel equal to 3 not " << valueToString(channel) << std::endl;
		return false;
	} else if (valuet.getType() != MixedValue::Double)
	{
		std::cerr << "Expect a double as input to writeChannel" << std::endl;
		return false;
	}
 
	valueDouble = valuet.getDouble();

	testDeviceValue = valueDouble;*/

	std::cerr << "PDMonitor does not allow channel writes" << std::endl;

	return success;
}


void PDMonitorDevice::definePartnerDevices()
{
	int partnerModule;

	std::vector <std::vector <std::string> >::iterator it;
	for (it = partnerSettings.begin(); it != partnerSettings.end(); it++)
	{
		stringToValue(it->at(1), partnerModule); //should be already checked

		addPartnerDevice(it->at(0), it->at(2), partnerModule, it->at(0));
		//addPartnerDevice("little table usb daq", "eplittletable.stanford.edu", 29, "usb daq #29");
	}
}

std::string PDMonitorDevice::Execute(int argc, char **argv)
{
	return "";
}

void PDMonitorDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	parseDeviceEventsDefault(eventsIn, eventsOut);
}

void PDMonitorDevice::stopEventPlayback()
{
}

bool PDMonitorDevice::getCalibration(PhotoDetector &pd)
{
	return true;
}
bool PDMonitorDevice::writeSetpoints()
{
	std::vector <std::string> field;
	std::string line = "";

	std::map<unsigned short, PhotoDetector>::iterator it;
	for (it = photoDetectorMap.begin(); it != photoDetectorMap.end(); it++)
	{
		line = valueToString(it->first) + ", " + valueToString(it->second.setpoint) + ";";
		field.push_back(line);
	}
	if (field.empty())
		return false;

	return appConfigFile.writeField("Setpoints", field);
}