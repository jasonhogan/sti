/*! \file STI_Device_Template.cpp
 *  \author Susannah Dickerson
 *  \brief Template for STI_Devices
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah Dickerson <sdickers@stanford.edu>\n
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



#include "Novatech409B.h"
#include <iomanip>

Novatech409B::Novatech409B(ORBManager* orb_manager, const ConfigFile& configFile) : 
STI_Device(orb_manager, configFile)
{
	unsigned short comPort;

	if (!(configFile.getParameter("ComPort", comPort))) {
		deviceShutdown();
		cerr << "Could not find ComPort number in inialization file." << endl;
		return;
	}

	amplitudeMaxVal = 1023;
	phaseMaxVal = 16383;

	intializeSerial(comPort);
}

//Novatech409B::Novatech409B(ORBManager*    orb_manager, 
//					   std::string    DeviceName, 
//					   std::string    Address, 
//					   unsigned short ModuleNumber, unsigned short comPort) : 
//STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
//{
//	intializeSerial(comPort);
//}

void Novatech409B::intializeSerial(unsigned short comPort)
{
	//Initialization of device
	//std::string myComPort = "COM" + valueToString(comPort);
	serialController  = new rs232Controller("COM" + valueToString(comPort), 19200,8,"None",1);

	//Check that we can talk to the device
	if (!serialController->initialized)
	{
		initialized = false;
		return;
	}

	//read all the frequency channels

	std::string initialState = serialController->queryDevice("QUE", 200, 250);  //string, sleep time in ms for query, read length of 250 characters

	std::cerr << "COM " << valueToString(comPort) << ";  " << initialState << std::endl;

	if (initialState.size() > 225) //a complete query string is ~225 characters long
		parseQUE(initialState);
	else
	{
		std::cerr << initialState.size() << std::endl;
		initialized = false;
		return;
	}

	if (frequencyChannels.size() == 4)
		initialized = true;
	else
		initialized = false;





	return;
}

Novatech409B::~Novatech409B()
{
	delete serialController;
}


bool Novatech409B::deviceMain(int argc, char **argv)
{

	refreshAttributes();
	
	return false;
}

void Novatech409B::defineAttributes() 
{
	/*
	addAttribute("Attribute Name v1", "Initial Option", "Option 1, Option 2, ...");
	addAttribute("Attribute Name v2", some_initial_number);
	*/
}

void Novatech409B::refreshAttributes() 
{

	/*
	setAttribute("Attribute Name v1", "Option to set");   //figuring out which option to set 
														//often requires a switch on some parameter
	setAttribute("Attribute Name v2", some_other_number); 
	*/

}

bool Novatech409B::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	/*
	if(key.compare("Attribute Name v1") == 0)
	{
		success = true;
		
		if(value.compare("Option 1") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else if(value.compare("Option 2") == 0)
		{
			set appropriate variable;
			std::cerr << "Attribute Name v1 = Option 1" << std::endl;
		}
		else
			success = false;
	}
	else if(key.compare("Attribute Name v2") == 0 && successDouble)
	{
		success = true;

		myVariable = tempDouble;

		set "Attribute Name v2" to myVariable
	}
	*/

	success = true;

	return success;
}

void Novatech409B::defineChannels()
{
	//one output channel per frequency channel
	addOutputChannel(0, ValueVector);
	addOutputChannel(1, ValueVector);
	addOutputChannel(2, ValueVector);
	addOutputChannel(3, ValueVector);
	addInputChannel(10, STI::Types::DataVector); //for reading settings
	addInputChannel(11, STI::Types::DataVector); //for reading settings
	addInputChannel(12, STI::Types::DataVector); //for reading settings
	addInputChannel(13, STI::Types::DataVector); //for reading settings
}

bool Novatech409B::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::vector <std::vector<double> > freqAmpPhases;
	std::vector <double> freqAmpPhase;

	std::string state;

	if (channel > 9 && channel < 14)
	{
//		for (unsigned int i = 0; i < frequencyChannels.size(); i++)
//		{
//			freqAmpPhase.clear();

		if(refreshLocallyStoredFrequencyChannels()) {

			freqAmpPhase.push_back(frequencyChannels.at(channel - 10).frequency);
			freqAmpPhase.push_back(frequencyChannels.at(channel - 10).amplitude);
			freqAmpPhase.push_back(frequencyChannels.at(channel - 10).phase);
			freqAmpPhases.push_back(freqAmpPhase);
	
			dataOut.setValue(freqAmpPhase);
		}
		else {
			std::cerr << "Failed to read channel." << endl;
			return false;
		}
	}
	else
	{
		std::cerr << "Expecting channel 10 - 13 for a read command" << std::endl;
		return false;
	}

	return true;
}

bool Novatech409B::refreshLocallyStoredFrequencyChannels()
{
	std::string state = serialController->queryDevice("QUE", 200, 250);  //string, sleep time in ms for query, read length of 250 characters

	if (state.size() > 225) { //a complete query string is ~225 characters long
		parseQUE(state);
		return true;
	}
	else {
		return false;
	}
}

bool Novatech409B::writeChannel(unsigned short channel, const MixedValue& valuet)
{
	std::string queryResult;
	MixedValueVector tempVec;
	enum error {TYPE, CHANNEL, RANGE};
	std::string errorMessage;

	FrequencyChannel newFrequencyTriplet;

//	double frequency;
//	double amplitude;
//	double phase;

	int amplitudeN;
	int phaseN;

	try {
		if (channel >= 0 && channel < 4)
		{
			if (valuet.getType() == MixedValue::Vector)
			{
				tempVec = valuet.getVector();
				if (tempVec.size() != 3)
					throw TYPE;

				newFrequencyTriplet.frequency = tempVec.at(0).getDouble();
				newFrequencyTriplet.amplitude = tempVec.at(1).getDouble();
				newFrequencyTriplet.phase = tempVec.at(2).getDouble();

				amplitudeN = static_cast<int>( (newFrequencyTriplet.amplitude / 100.0) * amplitudeMaxVal );
				phaseN = static_cast<int>( (newFrequencyTriplet.phase / 360.0) * phaseMaxVal );

				if( !checkRanges(newFrequencyTriplet, errorMessage) ) {
					//Bad range found
					throw RANGE;
				}
			}
			else
				throw TYPE;	
		}
		else
			throw CHANNEL;
	}
	catch (error e)
	{
		if (e == CHANNEL)
			std::cerr << this->getDeviceName() << " expects channel 0, 1, 2,or 3 for a write command" << std::endl;
		if (e == TYPE)
			std::cerr << this->getDeviceName() << " requires a triplet of doubles: (frequency, amplitude, phase)" << std::endl;
		if (e == RANGE)
			std::cerr << this->getDeviceName() << " allows ranges of (0-171.1276031 MHz, 0-100 percent, 0-360 degrees)" << std::endl;

		return false;
	}


	std::string frequencyString, amplitudeString, phaseString;

	//Set frequency
	if (frequencyChannels.at(channel).frequency != newFrequencyTriplet.frequency)
	{
		frequencyString = "f" + valueToString(channel) + " " + 
			valueToString(newFrequencyTriplet.frequency, "", ios::dec, 10);
		queryResult = serialController->queryDevice(frequencyString, 50, 30);
		if (queryResult.find("OK") == std::string::npos)
		{
			std::cerr << "Unable to set frequency of " << this->getDeviceName() << " channel " << channel << std::endl;
			return false;
		}
		frequencyChannels.at(channel).frequency = newFrequencyTriplet.frequency;
	}

	//Set amplitude
	if (frequencyChannels.at(channel).amplitude != newFrequencyTriplet.amplitude)
	{
		amplitudeString = "v" + valueToString(channel) + " " + valueToString(amplitudeN);
		queryResult = serialController->queryDevice(amplitudeString);
		if (queryResult.find("OK") == std::string::npos)
		{
			std::cerr << "Unable to set amplitude of " << this->getDeviceName() << " channel " << channel << std::endl;
			return false;
		}
		frequencyChannels.at(channel).amplitude = newFrequencyTriplet.amplitude;
	}

	//Set phase
	if (frequencyChannels.at(channel).phase != newFrequencyTriplet.phase)
	{
		phaseString = "p" + valueToString(channel) + " " + valueToString(phaseN);
		queryResult = serialController->queryDevice(phaseString);
		if (queryResult.find("OK") == std::string::npos)
		{
			std::cerr << "Unable to set phase of " << this->getDeviceName() << " channel " << channel << std::endl;
			return false;
		}
		frequencyChannels.at(channel).phase = newFrequencyTriplet.phase;
	}

	return true;
}

bool Novatech409B::checkRanges(FrequencyChannel& triplet, std::string& error)
{
	bool success = true;
	if (triplet.frequency < 0 || triplet.frequency > 171.1276031) {
		success = false;
		error = "Frequency out of range.";
	}
	if (triplet.amplitude < 0 || triplet.amplitude > 100) {
		success = false;
		error = "Amplitude out of range.";
	}
	if (triplet.phase < 0 || triplet.phase > 360) {
		success = false;
		error = "Phase out of range.";
	}

	return success;
}


void Novatech409B::definePartnerDevices()
{
}

std::string Novatech409B::execute(int argc, char **argv)
{
	std::vector<std::string> args;
	STI::Utils::convertArgs(argc, argv, args);

	std::stringstream command;

	for(unsigned i = 1; i < args.size(); i++)
	{
		command << args.at(i);
		
		if(i == (args.size() - 1))
			command << "\n";	//terminator at end of command
		else
			command << " ";		//spaces between command words
	}

	std::string result = serialController->queryDevice(command.str(), 200);

	return result;
}

void Novatech409B::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{

	//Make sure the current state of all channels is up to date.
	refreshLocallyStoredFrequencyChannels();

	cerr << endl << endl << "Table: " << endl;

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator lastTableEvents;

	double holdoff = 0;
	double minimumEventSpacing = 0;
	double minimumAbsoluteStartTime = 0;
	double tableStartHoldoff_ns = 1000 * 1000;	//1 ms

	bool goodFormat = true;
	bool holdEventFound = false;
	bool channelZeroOrOne = false;

	FrequencyChannel currentTriplet;

	FrequencyChannel currentTriplet0 = frequencyChannels.at(0);
	FrequencyChannel currentTriplet1 = frequencyChannels.at(1);

	unsigned tableAddress = 0;
	double tableDwellTime_ns = 0;	//in ns
	double lastTableDwellTime_ns = 0;	//in ns

	std::string errorMessage;

	double eventTime;
	double previousTime = minimumAbsoluteStartTime;
	
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		eventTime = events->first - holdoff;
		
		if( (events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The Novatech needs " + 
						valueToString(minimumEventSpacing) + 
						" ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The Novatech needs " + 
						valueToString(minimumAbsoluteStartTime) + 
						" ns at the beginning of the timing file.");
		}

		//Verify format. Parameter convention:  (freq, amplitude, phase, [Hold])
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			goodFormat = events->second.at(i).value().getVector().size() == 3 || 
				events->second.at(i).value().getVector().size() == 4;

			for(unsigned k = 0; k < 3; k++)
			{
				goodFormat &= events->second.at(i).value().getVector().at(k).getType() == MixedValue::Double;
			}

			if(!goodFormat) {
				throw EventParsingException(events->second.at(i),
					"Bad format. Novatech commands must be of the form (frequency, amplitude, phase, [Hold]).");
			}

			currentTriplet.frequency = events->second.at(i).value().getVector().at(0).getDouble();
			currentTriplet.amplitude = events->second.at(i).value().getVector().at(1).getDouble();
			currentTriplet.phase     = events->second.at(i).value().getVector().at(2).getDouble();

			//Check parameter range limits
			if( !checkRanges(currentTriplet, errorMessage) ) {
				throw EventParsingException(events->second.at(i),
					"Out of range. Allowed ranges are (0-171.1276031) MHz; (0-100) percent; (0-360) degrees.");
			}
		}

		//Check for trigger hold command
		if(	isHoldEvent(events) ) {
			holdEventFound = true;	//always True after 1st hold event is found (switches to Table Mode).
		}

		//As long as their is no "Hold" event found, assume the events are soft timing and use PsuedoSynchronousEvents
		if(!holdEventFound) {
			eventsOut.push_back( 
				new STI_Device::PsuedoSynchronousEvent(events->first, events->second, this) );

			//Update channel 0 and 1 triplets to most recent event. This allows for a dummy event at the start
			//of table mode. The dummy event will be whatever value was last played on the channel.
			for(unsigned j = 0; j < events->second.size(); j++) {
				if(events->second.at(j).channel() == 0) {
					getTriplet(events->second.at(j).value(), currentTriplet0);
				}
				if(events->second.at(j).channel() == 1) {
					getTriplet(events->second.at(j).value(), currentTriplet1);
				}
			}
			
			//Register any measurements
			for(unsigned j = 0; j < events->second.size(); j++)
			{
				if( events->second.at(j).isMeasurementEvent() ) {
					eventsOut.back().addMeasurement( events->second.at(j) );
				}
			}
		}
		else {
			//A Hold event was found. Run the rest of the events in Table Mode.

			///////  Table Mode   ///////

			//Only channels 0 and 1 can be set using table mode. Check for violations.
			channelZeroOrOne = true;
			for(unsigned j = 0; j < events->second.size(); j++)
			{
				channelZeroOrOne = (events->second.at(j).channel() == 0 || events->second.at(j).channel() == 1);
				
				if( !channelZeroOrOne) {
					throw EventParsingException(events->second.at(j),
						std::string("Only Channels 0 and 1 can be changed after a 'Hold' command is given\n") +
							"('Table Mode' only supports setting channels 0 and 1).");
				}
			}

			//Now we can assume that the 'events' vector only contains events for channels 0 and/or 1.
			
			tableDwellTime_ns = eventTime - previousTime;
			
			if(tableAddress > 0) {
				// New table point; add the previous point

				if( tableDwellTime_ns > 25400000 ) {
					throw EventParsingException(lastTableEvents->second.at(0),
						std::string("Dwell time overflow.\n Maximum dwell time between events in a triggered table\n is 25.4 ms.\n")
						+ "The computed dwell time was " + STI::Utils::valueToString(tableDwellTime_ns / 1000000) + " ms.");
				}

				//Get triplets for this event. 
				//Possibly only one channel will be explicitly set. Make a dummy event for the other
				//because table commands must be made for both channels.
				for(unsigned j = 0; j < lastTableEvents->second.size(); j++) {
					if(lastTableEvents->second.at(j).channel() == 0) {
						getTriplet(lastTableEvents->second.at(j).value(), currentTriplet0);
					}
					if(lastTableEvents->second.at(j).channel() == 1) {
						getTriplet(lastTableEvents->second.at(j).value(), currentTriplet1);
					}
				}

//				bool temp = isHoldEvent(lastTableEvents);
				addTablePoint(currentTriplet0, currentTriplet1, tableAddress, tableDwellTime_ns, 
					 isHoldEvent(lastTableEvents));
				//When tableAddress==1 we ignore the explicit Hold event, since the dummy event at the
				//0th table entry already has the hold.  tableAddress != 1 &&
			}
			else {
				//first table point
				preTableCommands();

				//currentTriplet0;
				//First table point is a "dummy" point that maintains the current state. It asserts ff for the hold
				//so that the next point in the table will be reached after the trigger.
//				addTablePoint(currentTriplet0, currentTriplet1, 0, tableDwellTime_ns, true);

				eventsOut.push_back( new NovatechTableEvent(events->first - tableStartHoldoff_ns, this) );
			}

			lastTableDwellTime_ns = tableDwellTime_ns;
			lastTableEvents = events;
			tableAddress += 1;
		}
		previousTime = eventTime;
	}

	if(tableAddress > 0) {
		//Add final table point with dwell time of 0 us

		for(unsigned j = 0; j < lastTableEvents->second.size(); j++) {
			if(lastTableEvents->second.at(j).channel() == 0) {
				getTriplet(lastTableEvents->second.at(j).value(), currentTriplet0);
			}
			if(lastTableEvents->second.at(j).channel() == 1) {
				getTriplet(lastTableEvents->second.at(j).value(), currentTriplet1);
			}
		}
		
		addTablePoint(currentTriplet0, currentTriplet1, tableAddress, 0, isHoldEvent(lastTableEvents));

		addTablePoint(currentTriplet0, currentTriplet1, tableAddress, 0, false);	//end table with dwell time 00.

		postTableCommands();
	}

	
}
void Novatech409B::preTableCommands()
{
	std::string cmd = "M a";
	std::string result = serialController->queryDevice(cmd, 100, 100);
}

void Novatech409B::postTableCommands()
{
//	std::string cmd = "M t";
//	queryResult = serialController->queryDevice(cmd, 50, 30);
}

bool Novatech409B::isHoldEvent(const RawEventMap::const_iterator& events)
{
	unsigned holdStringIndex = 3;
	bool foundHold = false;
	bool temp;
	//Check for trigger hold command
	for(unsigned i = 0; i < events->second.size() && !foundHold; i++)
	{
		if(events->second.at(i).value().getVector().size() > 3) {
			temp = events->second.at(i).value().getVector().at(holdStringIndex).getType() == MixedValue::String ;
			temp = events->second.at(i).value().getVector().at(holdStringIndex).getString().compare("Hold") == 0;

			foundHold = (events->second.at(i).value().getVector().at(holdStringIndex).getType() == MixedValue::String 
			&& events->second.at(i).value().getVector().at(holdStringIndex).getString().compare("Hold") == 0);
		}
	}

	return foundHold;
}

void Novatech409B::getTriplet(const MixedValue& eventValue, FrequencyChannel& triplet)
{
	triplet.frequency = eventValue.getVector().at(0).getDouble();
	triplet.amplitude = eventValue.getVector().at(1).getDouble();
	triplet.phase     = eventValue.getVector().at(2).getDouble();
}

void Novatech409B::addTablePoint(FrequencyChannel& triplet0, FrequencyChannel& triplet1, unsigned address, double dwell_ns, bool isHoldEvent)
{
	std::string cmd;
	std::string queryResult;

	//Write commands over serial
	makeTableCommand(triplet0, 0, address, dwell_ns, isHoldEvent, cmd);
	queryResult = serialController->queryDevice(cmd, 100, 30);

	cerr << cmd << endl;
	
	makeTableCommand(triplet1, 1, address, dwell_ns, isHoldEvent, cmd);
	queryResult = serialController->queryDevice(cmd, 100, 30);
	cerr << cmd << endl;

}

void Novatech409B::makeTableCommand(FrequencyChannel& triplet, int channel, unsigned address, double dwell_ns, bool isHoldEvent, std::string& command)
{
	std::stringstream cmd;
	int freqN;
	int ampN;
	int phaseN;
	int dwellN;

	cmd << "t" << STI::Utils::valueToString(channel) << " ";

	//Address
	cmd << std::setfill('0') << std::setw(4) 
		<< STI::Utils::valueToString(address, "0000", std::ios::hex, 4) << " ";

	//Frequency
	freqN = static_cast<int>( triplet.frequency * 10 * 1000000 );	//converts from MHz to number of 0.1 Hz increments.
	cmd << std::setfill('0') << std::setw(8) 
		<< STI::Utils::valueToString(freqN, "00000000", std::ios::hex, 8) << ",";

	//Phase
	phaseN = static_cast<int>( (triplet.phase / 360) * 16383 );	//converts from degrees to range [0, 16383].
	cmd << std::setfill('0') << std::setw(4) 
		<< STI::Utils::valueToString(phaseN, "0000", std::ios::hex, 4) << ",";

	//Amplitude
	ampN = static_cast<int>( (triplet.amplitude / 100) * 1023 );	//converts from percent to range [0, 1023].
	cmd << std::setfill('0') << std::setw(4) 
		<< STI::Utils::valueToString(ampN, "0000", std::ios::hex, 4) << ",";
	
	//Dwell time
	if(isHoldEvent) {
		cmd << "ff" << "\n";
	}
	else {
		dwellN = static_cast<int>( (dwell_ns / 1000) / 100 );	//converts from ns to number of 100 us.
		cmd << std::setfill('0') << std::setw(2) 
			<< STI::Utils::valueToString(dwellN, "00", std::ios::hex, 2) << "\n";
	}

	command = cmd.str();
}

void Novatech409B::stopEventPlayback()
{
}

void Novatech409B::parseQUE(std::string queOutput)
{
	std::string chInfo;

	size_t locBegin = 0;
	size_t locEnd = 0;

	FrequencyChannel tempFC;
	int tempVal;

	for (int i = 0; i < 4; i++)
	{
		locBegin = queOutput.find_first_of('\n', locEnd);
		if (locBegin == std::string::npos)
			return;

		locEnd = queOutput.find_first_of('\n',locBegin+1);
		if (locEnd == std::string::npos)
			return;

		//return if the substring isn't be long enough to contain the required information
		if (locEnd - locBegin < 20)
			return;

		chInfo.assign(queOutput.begin()+ locBegin + 1, queOutput.begin()+locEnd-1);
		//std::cerr << chInfo << std::endl;

		//Get the frequency (which comes in units of 0.1 Hz (so 1 Hz is written as 10 in decimal)
		stringToValue(chInfo.substr(0,8),tempVal,ios::hex);
		tempFC.frequency = static_cast<double>(tempVal)/10/1000000;  //write frequency in MHz

		//Get the phase
		stringToValue(chInfo.substr(9,4),tempVal,ios::hex);
		tempFC.phase = (static_cast<double>(tempVal) / static_cast<double>(phaseMaxVal)) * 360.0;

		//Get the amplitude
		stringToValue(chInfo.substr(14,4),tempVal,ios::hex);
		tempFC.amplitude = ( static_cast<double>(tempVal) / static_cast<double>(amplitudeMaxVal) ) * 100;

		frequencyChannels.push_back(tempFC);
	}

	return;
}


std::string Novatech409B::getDeviceHelp() 
{ 
	std::stringstream help;
	help <<	
		"Channels: \n\n" <<
		"  0 -- 3:  Outputs. For commanding the output of Novatech channels 0 through 3." << "\n" <<
		"           The command format is (Frequency, Amplitude, Phase, [Hold]) where" << "\n" <<
		"               ** 'Frequency' is in MHz within the range (0, 171.1276031). \n" <<
		"               ** 'Amplitude' is in percent within the range (0, 100). \n" <<
		"               ** 'Phase' is in degrees within the range (0, 360). \n" <<
		"               ** 'Hold' is an optional parameter that will delay the execution \n" <<
		"                   of the event until the Novatech receives an external trigger.\n" <<
		"                   The allowed valus is the string 'Hold'.\n" <<
		"10 -- 13:  Inputs. For measuring the state of Novatech channels 0 through 3." <<
		"\n" <<
		"\n";

	return help.str();

}