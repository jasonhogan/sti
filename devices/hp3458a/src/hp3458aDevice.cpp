/*! \file hp3458aDevice.cpp
 *  \author David M.S. Johnson
 *  \modified by Susannah Dickerson
 *  \brief Source-file for the class hp3458aDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Susannah Dickerson <sdickers@stanford.edu>\n
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

#include "hp3458aDevice.h"

hp3458aDevice::hp3458aDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber, 
							std::string configFileName,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber) : 
GPIB_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory, GCipAddress, GCmoduleNumber)
{ 
	std::string result;
	subprogramName = "";
	initialized = false;
	subprogramSet = false;

	configFile = new TaggedConfigFile(configFileName);

	std::vector <std::string> subprogram;
	if (configFile->getField("subprogram", subprogram) && subprogram.size() > 1)
	{
		if(subprogram.at(0).size() > 4)
		{
			subprogramName = subprogram.at(0).substr(4, std::string::npos);
			initialized = true;
		}
	}
	else
		std::cerr << "Error: could not get subprogram  and/or subprogram name" << std::endl;


	hasTriggerPartner = false;
	trackReadTimeDelay = true;
	readTimeDelay = readTimeDefault = 0.000900; //default of 40 us per point, as acquired by many trials
	gpibCommDelay = 0.000025; // approximate time it takes to communicate with HP, given a single point to read back

	//result = queryDevice("ID?");
	//std::cerr << result << std::endl;

	//success = commandDevice("INBUF ON");
	//if(success) success = commandDevice("TEST");

	//if(success) success = commandDevice("PRESET FAST"); // prepares HP for fast readings, with binary reporting
	//if(success) success = commandDevice("DCV 1"); // reset range to 1 V

//	my commands
	//INBUF ON
	//PRESET FAST
	//TARM HOLD
	//DCV 1
	//TRIG EXT
	//APER 100E-6 //more?
	//TIMER 1E-6
	//NRDGS 2000, TIMER
	//MEM FIFO

	//TARM SYN

	// in initialization event
	// RMEM
	//

}

hp3458aDevice::~hp3458aDevice() 
{
	delete configFile;
};
std::string hp3458aDevice::getDeviceHelp()
{
	std::stringstream stringStream;

	stringStream << "Notes: \n";
	stringStream << "\t-- A meas() event takes a tuple of the form (number of points, time between points (in seconds),";
	stringStream << " (optional) integration time for each point (in seconds)). \n";
	stringStream << "\t-- At the moment, the readings are deliverd in DINT (four byte binary). Changing this will";
	stringStream << " change holdoffs in an unexplored way, so beware. \n";
	stringStream << "\t-- The device will adjust the time permitted between events depending on the speed";
	stringStream << " reported from previous measurements. The attribute \"Track read time delay\" can disable";
	stringStream << " this. If disabled, the delay is set to the default of " << readTimeDefault*1000 << "ms per point.\n";

	return stringStream.str();
}
void hp3458aDevice::defineGpibAttributes()
{	
	//RANGE ONLY APPLIES TO DCV
	//addGpibAttribute("Range", "RANGE", "100E-3, 1, 10, 100", false);
	//addGpibAttribute("Trigger mode", "TRIG", "EXT, AUTO, SYN", false);

	//addGpibAttribute("Integration time", "APER", "", false);
	//addGpibAttribute("Read rate (s)", "TIMER", "", false);
	//addGpibAttribute("Number of readings", "NRDGS", "", false);
	//addGpibAttribute("Readout format","OFORMAT","1, 3", false);
	//addGpibAttribute("A/D format","MFORMAT","1, 3", false);
}

void hp3458aDevice::defineAttributes()
{
	HP3458aAttribute attribute;

	attribute.name = "Range";
	attribute.command = "RANGE";
	attribute.choices["100E-3"] = "100E-3";
	attribute.choices["1.0"] = "1.0";
	attribute.choices["10"] = "10";
	attribute.choices["100"] = "100";
	attribute.currentLabel = "10";

	hpAttributes["Range"] = attribute;

	attribute.choices.clear();
	attribute.name = "Trigger mode";
	attribute.command = "TRIG";
	attribute.choices["1"] = "AUTO";
	attribute.choices["2"] = "EXT";
	attribute.choices["4"] = "HOLD";
	attribute.choices["5"] = "SYN";
	if (hasTriggerPartner)
		attribute.currentLabel = "EXT";
	else
		attribute.currentLabel = "AUTO";

	hpAttributes["Trigger mode"] = attribute;

	attribute.choices.clear();
	attribute.name = "Output format";
	attribute.command = "OFORMAT";
	//attribute.choices["1"] = "ASCII";
	//attribute.choices["2"] = "SINT";
	attribute.choices["3"] = "DINT";
	attribute.currentLabel = "DINT";

	hpAttributes["Output format"] = attribute;

	std::map<std::string, HP3458aAttribute>::iterator it;
	std::string choices;
	for(it = hpAttributes.begin(); it != hpAttributes.end(); it++)
	{
		std::map <std::string,std::string>::iterator mapIt;
		choices = "";
		for(mapIt = it->second.choices.begin(); mapIt != it->second.choices.end(); mapIt++)
		{
			if(mapIt != it->second.choices.begin())
				choices += ", ";
			choices += mapIt->second;
			
		}
		addAttribute(it->second.name, it->second.currentLabel, choices);
	}

	addAttribute("Track read time delay","True","True, False");
	
}

void hp3458aDevice::refreshAttributes()
{
	std::map<std::string, HP3458aAttribute>::iterator it;
	for(it = hpAttributes.begin(); it != hpAttributes.end(); it++)
	{
		setAttribute(it->second.name, it->second.currentLabel);
	}

	setAttribute("Track read time delay", trackReadTimeDelay ? "True" : "False");
}

bool hp3458aDevice::updateAttribute(std::string key, std::string value)
{
	bool success = true;
	std::string newLabel, newValue;
	std::map<std::string, HP3458aAttribute>::iterator it;
	std::map<std::string,std::string>::iterator found;

	commandDevice("END ALWAYS");

	for(it = hpAttributes.begin(); it != hpAttributes.end(); it++)
	{
		if(key.compare(it->second.name) == 0)
		{
			success &= commandDevice(it->second.command + " " + value);
			newValue = queryDevice(it->second.command + "?");
			found = it->second.choices.find(newValue.substr(0,newValue.size()-2)); //remove carriage return and line feed
			if(found != it->second.choices.end())
			{
				newLabel = found->second;
				it->second.currentLabel = newLabel;
				if(newLabel.compare(value) == 0)
					success = true;
				else
					success = false;
			}
			else 
				success = false;

		}
	}

	if(key.compare("Track read time delay") == 0)
	{
		if(value.compare("True") == 0)
		{
			if (trackReadTimeDelay)
				return true;
			else
			{
				readTimeDelay = readTimeDefault; //default of 40 us per point, as spec'd for DINT
				trackReadTimeDelay = true;
			}
		}
		else
		{
			trackReadTimeDelay = false;
			readTimeDelay = readTimeDefault;
		}

	}
	return success;

}


void hp3458aDevice::definePartnerDevices()
{
	std::vector <std::string> triggerDevice;
	if (configFile->getField("triggerDevice", triggerDevice) && triggerDevice.size() == 4)
	{
		
		short module;
		if (stringToValue(triggerDevice.at(1), module) && stringToValue(triggerDevice.at(3), triggerChannel))
		{
			addPartnerDevice("Trigger", triggerDevice.at(2), module, triggerDevice.at(0));
			hasTriggerPartner = true;

			if (triggerDevice.at(0).compare("Slow Analog Out") == 0)
			{
				triggerLow = 0;
				triggerHigh = 5;
			}
			else
			{
				triggerLow = 0;
				triggerHigh = 1;
			}
		}
	}
	
}
void hp3458aDevice::defineChannels()
{
	addInputChannel(0, DataVector, ValueVector);
	//addOutputChannel(1, ValueVector);
	//addOutputChannel(3, ValueNumber);
}
bool hp3458aDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	//
	
	bool success = true;
	//std::string measurements;

	//std::string scaleFactorString;
	//double scaleFactor;

	if(channel == 0)
	{
		return readChannelDefault(0, valueIn, dataOut, 200000010); //minimum start time: 200 ms

		/*if (valueIn.getType() != MixedValue::Vector)
			return false;

		MixedValueVector inputVector = valueIn.getVector();

		std::vector <double> inputValues;
		try
			parseInputVector(inputVector, inputValues);
		catch (EventParsingException &e)
		{
			std::cerr << e.printMessage() << std::endl;
		}*/

		/*if(inputVector.size() != 2 && inputVector.size() != 3)
		{	
			std::cerr << "Error: expecting two or three arguments" << std::endl;
			return false;
		}

		if(inputVector.at(0).getType() != MixedValue::Double ||
			inputVector.at(1).getType() != MixedValue::Double)
		{	
			std::cerr << "Error: expecting numerical values (#, #, optional #)" << std::endl;
			return false;
		}*/
		//std::string numReadings = STI::Utils::valueToString((int) inputValues.at(0));
		//std::string timeBtwReadings = STI::Utils::valueToString(inputValues.at(1));
		//std::string integrationTime = STI::Utils::valueToString(inputValues.at(2));

		/*std::string integrationTime
		if(inputVector.size() == 3)
		{
			if (inputVector.at(2).getType() != MixedValue::Double)
			{
				std::cerr << "Error: expecting numerical values (#, #, optional #)" << std::endl;
				return false;
			}

			integrationTime = STI::Utils::valueToString(inputVector.at(2).getDouble());
		}
		else
		{
			//~20 us is how long the multimeter needs to prepare with DINT. Factor of 2 buffer
			std::string currentFormat = hpAttributes.find("Output Format")->second.currentLabel;
			if(currentFormat.compare("DINT") == 0)
				integrationTime = inputVector.at(1).getDouble() - 0.00004;
			else
			{
				std::cerr << "Warning: Unknown minimum time required between readings for output formats other than DINT" << std::endl;
				std::cerr << "\t Setting to default: 100 us" << std::endl;
				integrationTime = inputVector.at(1).getDouble() - 0.0001;
			}
		}*/

		/*if(!readyToRead)
		{
			if (!write(1, valueIn))
				return false;
		}*/

		/*/*std::vector <std::string> name;
		configFile->getField("subprogramName", name);

		if(!name.empty())
			success &= commandDevice("CALL " + name.at(0));

		success &= commandDevice("DCV " + getGpibAttribute("Range").stringValue);
		success &= commandDevice("TRIG " + getGpibAttribute("Trigger mode").stringValue);
		success &= commandDevice("NRDGS " + numReadings + ", TIMER");
		success &= commandDevice("TIMER " + timeBtwReadings);
		success &= commandDevice("TARM SGL");*/

		/*if (success)
		{
			int readingSize = 4;  //4 for DINT, 2 for SINT
			MixedData measurements;
			std::vector <double> measurementsDouble;
			success &= readFromDevice("RMEM 1," + numReadings, readingSize*((int) inputValues.at(0)), measurements);
			commandDevice("END ALWAYS"); //makes queries snappier 'casue EOI is sent at end of transmission
			scaleFactorString = queryDevice("ISCALE?");
			success &= stringToValue(scaleFactorString, scaleFactor);
			

			if(success && measurements.getType() == MixedData::Vector)
			{
				MixedDataVector measurementVector = measurements.getVector();
				
				for (int i = measurementVector.size()/4; i > 0; i--)
				{
					measurementsDouble.push_back(getDoubleFromChars(measurementVector,(i-1)*readingSize, readingSize)*scaleFactor);
				}
				dataOut.setValue(measurementsDouble);
			}
			*/

		/*	/*measurements = readFromDevice("RMEM 1," + numReadings, (inputVector.at(0).getInt()));
			commandDevice("END ALWAYS"); //makes queries snappier 'casue EOI is sent at end of transmission
			scaleFactorString = queryDevice("ISCALE?");
			success &= stringToValue(scaleFactorString, scaleFactor);

			size_t found = -1;
			size_t foundNext;
			double tempDouble;
			std::vector <double> measurementVector;
			while(found != std::string::npos && success)
			{
				foundNext = measurements.find(",", found + 1);
				success &= stringToValue(measurements.substr(found, foundNext-found), tempDouble);
				if (success)
					measurementVector.push_back(tempDouble * scaleFactor);
				found = foundNext;
			}
*/
/*
			char tempChar;
			double tempMeas, sign;
			std::vector <double> measurementVector;
			string::iterator it;
			int i, j, k, kStart;
			int numSize = 4;
			//Go from back to front, 'cause measurements are read from latest to earliest
			for (i = measurements.size()/numSize - 1; i >= 0 ; i--)
			{
				tempMeas = 0;
				sign = 1;
				for (j = 0; j < numSize; j++)
				{
					tempChar = measurements.at(i*numSize + j);

					//get sign
					if (j == 0)
					{
						tempMeas += -1*((tempChar >> 7) & 1)*pow(2.0,numSize*8-1);
						kStart = 6;
					}
					else
						kStart = 7;

					// get most significant bits
					for (k = kStart; k >= 0; k--)
					{
						tempMeas += ((tempChar >> k) & 1)*pow(2.0, k + 8*(numSize - (j + 1)));	
					}
				}
				measurementVector.push_back(tempMeas*scaleFactor);
			}
			*/
			//dataOut.setValue(measurementVector);	
			// need GPIB option to read to EOI, NOT '#' (why does it do that, anyways?)*/
			
		/*}

		readyToRead = false;
		return success;*/
	}

	std::cerr << "Expecting only channel 0" << std::endl;
	
	return false;
}
bool hp3458aDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
	/*bool success = true;
	if(channel == 1)
	{
		if (value.getType() != MixedValue::Vector)
			return false;

		MixedValueVector inputVector = value.getVector();

		std::vector <double> inputValues;
		try
			parseInputVector(inputVector, inputValues);
		catch (EventParsingException &e)
		{
			std::cerr << e.printMessage() << std::endl;
		}

		/*if(inputVector.size() != 2)
		{	
			std::cerr << "Error: expecting two arguments" << std::endl;
			return false;
		}

		if(inputVector.at(0).getType() != MixedValue::Double ||
			inputVector.at(1).getType() != MixedValue::Double)
		{	
			std::cerr << "Error: expecting numerical values (#, #)" << std::endl;
			return false;
		}
		std::string numReadings = STI::Utils::valueToString(inputVector.at(0).getInt());
		std::string timeBtwReadings = STI::Utils::valueToString(inputVector.at(1).getDouble());
		*/
		/*std::string numReadings = STI::Utils::valueToString((int) inputValues.at(0));
		std::string timeBtwReadings = STI::Utils::valueToString(inputValues.at(1));
		std::string integrationTime = STI::Utils::valueToString(inputValues.at(2));

		success &= prepareDeviceToPlay();
		success &= commandDevice("NRDGS " + numReadings + ", TIMER");
		success &= commandDevice("TIMER " + timeBtwReadings);
		success &= commandDevice("APER " + integrationTime);
		success &= commandDevice("TARM SGL");

		readyToRead = true;
		return success;
	}

	std::cerr << "Expecting either Channel 0 or 1" << std::endl;*/
	return false;
}
void hp3458aDevice::parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception)
{
	double ms = 1000000;
	double s = 1000000000;
	double initEventHoldoff = 200*ms;

	double previousEventDuration;
	//double readingSaveTimeHoldoff;

	/*std::string oformatQueried = queryDevice("OFORMAT?");
	std::string oformat = hpAttributes.find("Output format")->second.choices.find(
		oformatQueried.substr(0,oformatQueried.size()-2))->second;

	if(oformat.compare("DINT") == 0)
	{
		readingSaveTimeHoldoff = 0.00002*s;
	}
	else if(oformat.compare("SINT") == 0)
	{
		readingSaveTimeHoldoff = 0.00001*s;
	}
	else if(oformat.compare("DREAL") == 0)
	{
		readingSaveTimeHoldoff = 0.001*s;
	}
	else
	{
		readingSaveTimeHoldoff = 0.002*s;
	}*/

	if(!prepareDeviceToPlay())
		throw EventParsingException(eventsIn.begin()->second.at(0),
						"Could not prepare hp3458 to play");

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator previousEvents;

	
	double eventTime;


	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{	

		eventTime = events->first;

		if (events->second.at(0).getValueType() == MixedValue::Vector && 
			(events->second.at(0).isMeasurementEvent()))
		{
			const std::vector <MixedValue>& eVector = events->second.at(0).value().getVector();
			std::vector <double> inputValues;

			try
			{
				parseInputVector(eVector, inputValues, events->second.at(0));
			}
			catch(EventParsingException &e)
			{
				throw e;
			}
			//check structure of input
			/*if(sizeOfTuple == 2)
			{
				if(eVector.at(1).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Time between readings must be a number");
				}
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Number of readings must be a number");
				}
				
			}
			else
				throw EventParsingException(events->second.at(0),
						"Expecting a tuple with (int numReadings, double time between readings)");*/

			if(events == eventsIn.begin())
			{
				if (eventTime - initEventHoldoff < 0)
					throw EventParsingException(events->second.at(0), "Need at least " + 
						valueToString(initEventHoldoff/ms) + " ms before first event.");
			}
			else if(eventTime - initEventHoldoff < 
				previousEvents->first + previousEventDuration)
			{
				throw EventParsingException(events->second.at(0), "Need an additional " + 
						valueToString(((previousEvents->first + previousEventDuration)-(eventTime - initEventHoldoff))/ms) +
						" ms before this event.");
			}

			previousEvents = events;
			previousEventDuration = inputValues.at(0)*inputValues.at(1)*s +
				readTimeDelay*inputValues.at(0)*s*1.02 + gpibCommDelay*s; 
			// include reading delay times 5 percent (for many points) and a buffer (for few points)

			//Make a new event
			eventsOut.push_back( new hp3458aInitEvent(eventTime - initEventHoldoff, this, 
				valueToString(inputValues.at(0)), valueToString(inputValues.at(1)), valueToString(inputValues.at(2)) ) );
					//Read the measurement after the data has been acquired.
			eventsOut.push_back( new hp3458aReadEvent(eventTime + previousEventDuration, this, 
				(int) inputValues.at(0), inputValues.at(1), inputValues.at(2) ) );

			//log the last event as a measurement event
			eventsOut.back().addMeasurement( events->second.at(0) );
		}
		else
			throw(events->second.at(0), "The HP can only measure, and expects a vector");


		//if there is a trigger, send that at the desired eventTime
		if (hasTriggerPartner)
			sendTriggerEvents(eventTime, events->second.at(0));
	}
	
}

void hp3458aDevice::parseInputVector(const std::vector <MixedValue> &valueIn, std::vector <double> &parsedValues, const RawEvent& evt) throw(std::exception)
{
	int sizeOfTuple = valueIn.size();
	double integrationTime = 0;
	double timeBtwMeasurements = 0;

	parsedValues.clear();

	if(sizeOfTuple == 2 || sizeOfTuple == 3)
	{
		if(valueIn.at(1).getType() != MixedValue::Double)
		{
			throw EventParsingException(evt,
				"Time between readings must be a number");
		}
		if(valueIn.at(0).getType() != MixedValue::Double)
		{
			throw EventParsingException(evt,
				"Number of readings must be a number");
		}

		parsedValues.push_back(valueIn.at(0).getDouble());
		timeBtwMeasurements = valueIn.at(1).getDouble();

		parsedValues.push_back(roundTo100ns(timeBtwMeasurements));
	}
	else
		throw EventParsingException(evt,
				"Expecting a tuple with (int numReadings, double time between readings, (optional) double integration time)");

	//Set integration time
	if(sizeOfTuple == 3)
	{
		if (valueIn.at(2).getType() != MixedValue::Double)
		{
			throw EventParsingException(evt,
				"Integration time must be a number");
		}

		integrationTime = valueIn.at(2).getDouble();
		if (integrationTime > parsedValues.at(1))
			throw EventParsingException(evt,
				"Integration time cannot be larger than time between samples");
	}
	else
	{
		//~20 us is how long the multimeter needs to prepare with DINT. Factor of 2 buffer
		std::string currentFormat = hpAttributes.find("Output format")->second.currentLabel;
		if(currentFormat.compare("DINT") == 0)
			integrationTime = parsedValues.at(1) - 0.00004;
		else
		{
			std::cerr << "Warning: Unknown minimum time required between readings for output formats other than DINT" << std::endl;
			std::cerr << "\t Setting to default: 100 us" << std::endl;
			integrationTime = parsedValues.at(1) - 0.0001;
		}
	}

	if (integrationTime < 0.000000500)
		throw EventParsingException(evt,
				"Integration time cannot be less than 500 ns");

	parsedValues.push_back(roundTo100ns(integrationTime));
	
}

void hp3458aDevice::hp3458aInitEvent::playEvent()
{
	clock_t clockTime;
	clockTime = clock();

	hp3458aDevice_->commandDevice("NRDGS " + numReadings + ", TIMER");
	hp3458aDevice_->commandDevice("TIMER " + timeBtwReadings);
	hp3458aDevice_->commandDevice("APER " + integrationTime);
	hp3458aDevice_->commandDevice("END ON");
	hp3458aDevice_->commandDevice("TARM SGL");

	clockTime = clock() - clockTime;
	std::cerr << "It took " << ((double) clockTime/CLOCKS_PER_SEC) << " seconds to prepare the measurement" << std::endl;
}

void hp3458aDevice::hp3458aReadEvent::playEvent()
{
	dataOut.clear();

	bool success = true;
	int readingSize = 4;  //4 for DINT, 2 for SINT
	MixedData measurementOctets;
	std::vector <MixedData> measurements;
	MixedDataVector metadata;
	MixedDataVector metadatum;
	std::string tmp;
	std::string numReads = valueToString(numReadings);
	std::string scaleFactorString;
	double scaleFactor;
	
	//Get data from HP
	clock_t clockTime;
	clockTime = clock();
	success &= hp3458aDevice_->readFromDevice("RMEM 1," + numReads, readingSize*numReadings, measurementOctets);
	clockTime = clock() - clockTime;
	std::cerr << "It took " << ((double) clockTime)/CLOCKS_PER_SEC << " seconds to read the data" << std::endl;
	std::cerr << "It took " << ((double) clockTime)/CLOCKS_PER_SEC/numReadings * 1000 << " ms to read each point" << std::endl;
	
	if(hp3458aDevice_->trackReadTimeDelay)
	{
		//abs to make sure it never goes negative
		hp3458aDevice_->rememberReadTime(abs(((double) clockTime)/CLOCKS_PER_SEC/numReadings - 
			hp3458aDevice_->gpibCommDelay), numReadings);

	}
	
	
	success &= hp3458aDevice_->commandDevice("MEM FIFO");
	hp3458aDevice_->commandDevice("END ALWAYS"); //makes queries snappier 'casue EOI is sent at end of transmission
	scaleFactorString = hp3458aDevice_->queryDevice("ISCALE?");
	success &= stringToValue(scaleFactorString, scaleFactor);

	//set metadata
	tmp = "Number of reads";
	metadatum.push_back(tmp);
	metadatum.push_back(numReadings);
	metadata.push_back(metadatum);
	
	metadatum.clear();
	tmp = "Time between reads (s)";
	metadatum.push_back(tmp);
	metadatum.push_back(timeBtwReadings);
	metadata.push_back(metadatum);

	metadatum.clear();
	tmp = "Integration time (s)";
	metadatum.push_back(tmp);
	metadatum.push_back(integrationTime);
	metadata.push_back(metadatum);

	//translate data from octets to doubles
	if(success && measurementOctets.getType() == MixedData::Vector)
	{
		MixedDataVector measurementVector = measurementOctets.getVector();

		clockTime = clock();
		
		for (int i = measurementVector.size()/4; i > 0; i--)
		{
			measurements.push_back(getDoubleFromChars(measurementVector,(i-1)*readingSize, readingSize)*scaleFactor);
		}

		clockTime = clock() - clockTime;
		std::cerr << "It took " << ((double)clockTime)/CLOCKS_PER_SEC << " seconds to translate the data" << std::endl;
		std::cerr << "It took " << ((double)clockTime)/CLOCKS_PER_SEC/numReadings * 1000 << " ms to translate each point" << std::endl;
	
		
		clockTime = clock();
		dataOut.addValue(measurements);
		clockTime = clock() - clockTime;
		std::cerr << "It took " << ((double) clockTime)/CLOCKS_PER_SEC << " seconds to transfer the data" << std::endl;
		std::cerr << "It took " << ((double) clockTime/CLOCKS_PER_SEC/numReadings * 1000) << " ms to transfer each point" << std::endl;
	

		dataOut.addValue(metadata);
		std::cerr << "Got " + valueToString(measurementVector.size()/4) << " readings" << std::endl;
	}

}

void hp3458aDevice::hp3458aReadEvent::collectMeasurementData()
{
	clock_t clockTime;
	std::cerr << "Collecting measurements" << std::endl;
	clockTime = clock();
	eventMeasurements.at(0)->setData( dataOut );
	clockTime = clock() - clockTime;
	std::cerr << "It took " << ((double)clockTime)/CLOCKS_PER_SEC << " seconds to collect the data" << std::endl;
	std::cerr << "It took " << ((double)clockTime)/CLOCKS_PER_SEC/numReadings * 1000 << " ms to collect each point" << std::endl;
	std::cerr << std::endl;
}

void hp3458aDevice::sendTriggerEvents(double eventTime, const RawEvent& evt)
{
	//The HP triggers on the falling edge, and needs at least 250 ns for the trigger
	partnerDevice("Trigger").event(eventTime, 
					triggerChannel, triggerLow, evt);
	partnerDevice("Trigger").event(eventTime + 10000, 
					triggerChannel, triggerHigh, evt);
}

std::string hp3458aDevice::execute(int argc, char** argv)
{
	std::string tempArg;
	std::string msg;
	bool readData = false;

	if(argc < 1)
		return "Not enough arguments";

	for(int i = 1; i < argc; i++)
	{
		if(i > 1)
			msg += " ";
		tempArg = argv[i];
		msg += tempArg;
	}

	if(msg.compare(msg.size()-1,1,"?")==0)
		return queryDevice(msg);
	else if(commandDevice(msg))
		return "OK";
/*	else
	{
		bool success;
		std::string measurements;
		MixedValueVector inputVector;

		MixedValue valueIn;

		if(argc < 4)
			return "Error 1";

		if(true)
		{
			//inputVector = valueIn.getVector();

			/*if(inputVector.size() != 2)
			{	
				std::cerr << "Error: expecting two arguments" << std::endl;
				return false;
			}

			if(inputVector.at(0).getType() != MixedValue::Int ||
				inputVector.at(1).getType() != MixedValue::Double)
			{	
				std::cerr << "Error: expecting an integer and a double: (int, double)" << std::endl;
				return false;
			}
			std::string numReadings = STI::Utils::valueToString(inputVector.at(0).getInt());
			std::string timeBtwReadings = STI::Utils::valueToString(inputVector.at(1).getDouble());
			*/

/*			std::string numReadings = argv[2];
			std::string timeBtwReadings = argv[3];

			success &= commandDevice("TARM HOLD");
			success &= commandDevice("NRDGS " + numReadings + ", TIMER");
			success &= commandDevice("TIMER " + timeBtwReadings);
			success &= commandDevice("MEM FIFO");

			success &= commandDevice("TARM SGL");
			measurements = commandAndReadToTermination("RMEM 1, " + numReadings + ", 1");

			dataOut.setValue(measurements);
			return success;
		}
	}*/
	//command structure:  >analogIn readChannel 1
	//returns the value as a string
/*
	if(argc < 3)
		return "Error: Invalid argument list. Expecting 'channel'.";

	int channel;
	bool channelSuccess = stringToValue(argv[2], channel);

	if(channelSuccess && channel >=0 && channel <= 1)
	{
		MixedData data;
		bool success = readChannel(channel, 0, data);

		if(success)
		{
			cerr << "Result to transfer = " << data.getDouble() << endl;
			return valueToString( data.getDouble() );
		}
		else
			return "Error: Failed when attempting to read.";
	}
*/
	return "Error";
}

/*std::string hp3458aDevice::readFromDevice(std::string command, int readSize)
{
	std::string fullCommand;
	std::string result;
	// the final 3 tells the partner to read 'til termination.
	fullCommand = valueToString(primaryAddress) + " " + valueToString(secondaryAddress) + " " + command + " " + valueToString(readSize) +  " 4";
	std::cerr << "fullCommand: " << fullCommand << std::endl;

	result = partnerDevice("gpibController").execute(fullCommand.c_str());

	return result;
}*/

bool hp3458aDevice::prepareDeviceToPlay()
{
	bool success = true;

	// Run subprogram, which puts the HP in the right state
	if(!subprogramSet)
	{
		std::vector<std::string> subprogram;
		configFile->getField("subprogram",subprogram);

		std::vector<std::string>::iterator it;
		for(it = subprogram.begin(); it != subprogram.end() && success; it++)
		{
			success &= commandDevice(*it);
		}

		if(!success)
			return false;

		subprogramSet = true;
	}

	if(!commandDevice("CALL " + subprogramName))
			return false;

	//can't be in an external trigger mode if there is not partner
	if(hpAttributes.find("Trigger mode")->second.currentLabel.compare("EXT") == 0 &&
		!hasTriggerPartner)
		return false;

	success &= commandDevice("RANGE " + hpAttributes.find("Range")->second.currentLabel);
	success &= commandDevice("TRIG " + hpAttributes.find("Trigger mode")->second.currentLabel);
	success &= commandDevice("OFORMAT " + hpAttributes.find("Output format")->second.currentLabel);

	return success;
}

bool hp3458aDevice::readFromDevice(std::string command, int readSize, MixedData & readings)
{
	bool success;
	// the final 3 tells the partner to read 'til termination.
	MixedValue inVector;
	inVector.addValue(primaryAddress);
	inVector.addValue(secondaryAddress);
	inVector.addValue(command);
	inVector.addValue(readSize);

	success = partnerDevice("gpibController").read(0, inVector, readings);

	return success;
}

double hp3458aDevice::hp3458aReadEvent::getDoubleFromChars(std::vector<MixedData> &charVector, int pos, int numChars)
{
	char tempChar;
	double tempDouble, sign;
	std::vector <double> measurementVector;
	string::iterator it;
	int i, j, jStart;
	//Go from back to front, 'cause measurements are read from latest to earliest

	if (charVector.size() < (unsigned) (pos + numChars))
		return 0;

	tempDouble = 0;
	sign = 1;
	for (i = 0; i < numChars; i++)
	{
		if (charVector.at(pos + i).getType() != MixedData::Octet)
			return 0;

		tempChar = charVector.at(pos + i).getOctet();

		//get sign
		if (i == 0)
		{
			tempDouble += -1*((tempChar >> 7) & 1)*pow(2.0,numChars*8-1);
			jStart = 6;
		}
		else
			jStart = 7;

		// get most significant bits
		for (j = jStart; j >= 0; j--)
		{
			tempDouble += ((tempChar >> j) & 1)*pow(2.0, j + 8*(numChars - (i + 1)));	
		}
	}

	return tempDouble;
}
std::string hp3458aDevice::HP3458aAttribute::inverseFind(std::string value)
{
	std::map<std::string, std::string>::iterator iter;

	//Find string associated with flag
	for (iter = choices.begin(); iter != choices.end(); iter++ ){
		if (value.compare(iter->second) == 0){
			return iter->first;
		}
	}

	std::cerr << "Error in " << name << " selection: " << value << " not allowed." << std::endl;


	return "";
}

double hp3458aDevice::roundTo100ns(double n)
{
	double nBig, nRound;
	//rounds to nearest 100 ns, given a number in seconds (aper is min)
	nBig = n*10000000;

	nRound = nBig < 0.0 ? ceil(nBig - 0.5) : floor(nBig + 0.5);

	return nRound/10000000;
}
void hp3458aDevice::rememberReadTime(double newReadTime, int numPoints)
{
	//weight towards large data sets
	readTimeDelay = (readTimeDelay + numPoints*newReadTime/1000)/(1 + numPoints/1000);
	std::cerr << "New read time is " << readTimeDelay << " s" << std::endl;
}