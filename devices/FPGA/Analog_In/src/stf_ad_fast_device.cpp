/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_AD_FAST
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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

#include "stf_ad_fast_device.h"
#include "ORBManager.h"

STF_AD_FAST::STF_AD_FAST_Device::
STF_AD_FAST_Device(ORBManager* orb_manager, std::string configFilename) :
FPGA_Device(orb_manager, "Analog In", configFilename)
{
	// ALL TEMPORARY VALUES (copied from Fast Analog Out 2-15-10
	minimumEventSpacing = 1000; //in nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	holdoff = minimumEventSpacing + 1000 + 8000 - 100 - 5000; //we assume the holdoff is equal to the minimum event spacing (to be verified)

	holdMeasurements = true;
	delay_ns = 0;

	analogInMutex = new omni_mutex();
	analogInCondition = new omni_condition(analogInMutex);

	channelCommand = 3;

}

STF_AD_FAST::STF_AD_FAST_Device::~STF_AD_FAST_Device()
{
}

bool STF_AD_FAST::STF_AD_FAST_Device::deviceMain(int argc, char **argv)
{
//	cerr << "print?" << endl;
//	string x;
//	cin >> x;
//	orbManager->printObjectTree(x);
	return false;
}
	
void STF_AD_FAST::STF_AD_FAST_Device::defineAttributes()
{
	 //addAttribute("Hold Before Measuring", (holdMeasurements ? "True" : "False"),"True, False");
	 addAttribute("Measurement delay", delay_ns);
	 addAttribute("Channel 1 command", channelCommand);

	// addAttribute("DAQ Frequency", getDaqFreq());
	// addAttribute("# of MUXed input channels", getNumChannels());
	// addAttribute("Warp Mode", "Off", "On, Off");
//	addAttribute("Value", read_data());
}

void STF_AD_FAST::STF_AD_FAST_Device::refreshAttributes()
{
	//setAttribute("Hold Before Measuring", (holdMeasurements ? "True" : "False") );
	// setAttribute("DAQ Frequency", getDaqFreq());
	// setAttribute("# of MUXed input channels", getNumChannels());
	// setAttribute("Warp Mode", getMode());
//	setAttribute("Value", read_data());

}

bool STF_AD_FAST::
STF_AD_FAST_Device::updateAttribute(std::string key, std::string value)
{
	unsigned tempInt;
//	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;

	if(key.compare("Hold Before Measuring") == 0)
	{
		if(value.compare("True")==0) {
			holdMeasurements = true;
			success = true;
		}
		else if(value.compare("False")==0) {
			holdMeasurements = false;
			success = true;
		}
	}
	if(key.compare("Measurement delay") == 0 && stringToValue(value, tempInt))
	{
		delay_ns = tempInt;
		success = true;
	}
	if(key.compare("Channel 1 command") == 0 && stringToValue(value, tempInt))
	{
		channelCommand = tempInt;
		success = true;
	}

/*	bool success = false;

	if(key.compare("DAQ Frequency") == 0 && successDouble)
		success = setDaqFreq(tempDouble);
	else if(key.compare("Warp Mode") == 0)
	{
		success = true;

		if(value.compare("Off") == 0)
			setMode(0);
		else if(key.compare("On") == 0)
		{
			setMode(1);
		}
		else
			success = false;
	}
*/
	return success;
}

void STF_AD_FAST::STF_AD_FAST_Device::defineChannels()
{
	addInputChannel(0, DataDouble, ValueNumber);
	addInputChannel(1, DataDouble, ValueNumber);
}

//bool STF_AD_FAST::STF_AD_FAST_Device::
//readChannel(DataMeasurement &Measurement)
//{
//	Measurement.setData( read_data() );
	
//	return true;
//}


std::string STF_AD_FAST::STF_AD_FAST_Device::execute(int argc, char **argv)
{

	//command structure:  >analogIn readChannel 1
	//returns the value as a string

	if(argc < 3)
		return "Error: Invalid argument list. Expecting 'channel'.";

	int channel;
	bool channelSuccess = stringToValue(argv[2], channel);

	if(channelSuccess && channel >= 0 && channel <= 1)
	{
		//RawEvent rawEvent(10000, channel, 0);	//time = 1, event number = 0

	//	DataMeasurement measurement(10000, channel, 0);

	//	writeChannel(rawEvent); //runs parseDeviceEvents on rawEvent and executes a short timing sequence

		MixedData data;
		bool success = read(channel, 0, data);
//		makeMeasurement( measurement );


		//DataMeasurementVector& results = getMeasurements();

//		waitForEvent(0)

//int x=0;
//while(x != 3)
//{		
//cerr << "Waiting to send..." << endl;
//cin >> x;
//}


		if(success)
		{
			cerr << "Result to transfer = " << data.getDouble() << endl;
			return valueToString( data.getDouble() );
		}
		else
			return "Error: Failed when attempting to read.";
	}

	return "Error";
}

void STF_AD_FAST::STF_AD_FAST_Device::parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{
	uInt16 intSamples;
	double numSamples;
	double oldNumSamples = 1;
	uInt32 channel;
	std::string errorMessage;

	double lastEventTime = 10*minimumEventSpacing*0;

	//cerr << "STF_AD_FAST_Device::parseDeviceEvents()" << endl;
//	int i = 1;

	RawEventMap::const_iterator events;
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		channel = events->second.at(0).channel();
		//TODO: construct bit line commands from iter->second events
		numSamples = 1;
		// check what type of event it is
		if(events->second.at(0).getValueType() == MixedValue::Double) //one value given, number of samples to average
		{
			numSamples = events->second.at(0).value().getDouble();  //retrieve the number of samples to average
		}

		if(numSamples <= 32768.0 && numSamples >= 1.0)
			intSamples = static_cast<uInt16>(floor(numSamples-1)); // we write one less, because the FPGA will always take 1 data point, and this value is the number of additional samples to accumulate
		else
			throw EventParsingException(events->second.at(0),
				"The Analog-In only supports values between 1 and 32768. This integer is the number of A/D samples to average @ ~1MHz for this data point.");

		if(lastEventTime + (oldNumSamples)*minimumEventSpacing > events->first)
		{
			errorMessage = "The analog-in requires at least " + valueToString((oldNumSamples)*minimumEventSpacing) + " ns between these events to allow for averaging " + valueToString(oldNumSamples) + " samples first @ ~1MHz.";
			throw EventParsingException(events->second.at(0), errorMessage);
		}
		
		//with averaging
//		eventsOut.push_back( 
//			(new AnalogInEvent(events->first, this))
//			->setBits(intSamples, 16, 31)		//set the upper 16 bits to the number of samples to average
//			->setBits(channel, 0, 15)
//			);
		//original code

		if(events->second.at(0).channel() == 0) {
			eventsOut.push_back( 
				(new AnalogInEvent(events->first, this))
				->setBits(3)
				);
		}
		if(events->second.at(0).channel() == 1) {
			eventsOut.push_back( 
				(new AnalogInEvent(events->first, this))
				->setBits(channelCommand)
				);
		}

//		std::cerr << "About to add measurement " << i << std::endl;

		eventsOut.back().addMeasurement( events->second.at(0) );	//temporary! (it should pick the right event)

		//set old event values to the current values before looping
		oldNumSamples = numSamples;
		lastEventTime = events->first;
//		std::cerr << "Parsed event number: " << i << std::endl;
	}

	events--;
//	throw EventParsingException(events->second.at(0), "Finished parsing");
	
//	cerr << "STF_AD_FAST_Device::parseDeviceEvents() " <<  eventsOut.size() << endl;
}

void STF_AD_FAST::STF_AD_FAST_Device::AnalogInEvent::collectMeasurementData()
{
	//eventMeasurements.at(i)->channel ==

	/*
	if(static_cast<STF_AD_FAST_Device*>(device_)->holdMeasurements)
	{
		int x;
		std::cout << "Analog In: Holding before collection ";
		std::cin >> x;
	}
	*/


	static_cast<STF_AD_FAST_Device*>(device_)->delayBeforeMeasurement();

	uInt32 rawValue;
	uInt32 numSamples = 1;
	double cal_factor = 10;
	double result;

	//cerr << "STF_AD_FAST_Device::collectMeasurementData() " << getEventNumber() << endl;

	for(unsigned i = 0; i < eventMeasurements.size(); i++)
	{
		rawValue = readBackValue();
		numSamples = getBits(16, 31)+1;
		//std::cerr << "AnalogIn -- The raw value is : " << rawValue << endl;

		result = cal_factor * ( (static_cast<double>(rawValue)/numSamples) - 32768.0 ) / (32768.0);
		
		eventMeasurements.at(i)->setData( result );

		//std::cerr << "AnalogIn -- The result is : " << result << endl;
	}


}
void STF_AD_FAST::STF_AD_FAST_Device::delayBeforeMeasurement()
{
	unsigned long wait_s, wait_ns;
	if(delay_ns > 0)
	{
		analogInMutex->lock();
		{
			omni_thread::get_time(&wait_s, &wait_ns, 0, delay_ns);
			analogInCondition->timedwait(wait_s, wait_ns);
		}
		analogInMutex->unlock();
	}
}

