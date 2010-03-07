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
	minimumEventSpacing = 200; //in nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 10000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out
	holdoff = minimumEventSpacing + 1000 + 8000 - 100 - 5000; //we assume the holdoff is equal to the minimum event spacing (to be verified)

	holdMeasurements = true;
	delay_ns = 10000;

	analogInMutex = new omni_mutex();
	analogInCondition = new omni_condition(analogInMutex);

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
	return true;
}
	
void STF_AD_FAST::STF_AD_FAST_Device::defineAttributes()
{
	 addAttribute("Hold Before Measuring", (holdMeasurements ? "True" : "False"),"True, False");
	 addAttribute("Measurement delay", delay_ns);

	// addAttribute("DAQ Frequency", getDaqFreq());
	// addAttribute("# of MUXed input channels", getNumChannels());
	// addAttribute("Warp Mode", "Off", "On, Off");
//	addAttribute("Value", read_data());
}

void STF_AD_FAST::STF_AD_FAST_Device::refreshAttributes()
{
	setAttribute("Hold Before Measuring", (holdMeasurements ? "True" : "False") );
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
	addInputChannel(0, DataDouble);
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

void STF_AD_FAST::STF_AD_FAST_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	uInt32 value;

	cerr << "STF_AD_FAST_Device::parseDeviceEvents()" << endl;

	RawEventMap::const_iterator iter;
	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		//TODO: construct bit line commands from iter->second events
		value = 0;

		eventsOut.push_back( 
			(new AnalogInEvent(iter->first, this))
			->setBits(3)		//temporary
			);

		eventsOut.back().addMeasurement( iter->second.at(0) );	//temporary! (it should pick the right event)
	}
	
	cerr << "STF_AD_FAST_Device::parseDeviceEvents() " <<  eventsOut.size() << endl;
}

void STF_AD_FAST::STF_AD_FAST_Device::AnalogInEvent::collectMeasurementData()
{
	//eventMeasurements.at(i)->channel ==

	if(static_cast<STF_AD_FAST_Device*>(device_)->holdMeasurements)
	{
		int x;
		std::cout << "Analog In: Holding before collection ";
		std::cin >> x;
	}


	static_cast<STF_AD_FAST_Device*>(device_)->delayBeforeMeasurement();

	uInt32 rawValue;
	double cal_factor = 10;
	double result;

	cerr << "STF_AD_FAST_Device::collectMeasurementData() " << getEventNumber() << endl;

	for(unsigned i = 0; i < eventMeasurements.size(); i++)
	{
		rawValue = readBackValue();
		std::cerr << "AnalogIn -- The raw value is : " << rawValue << endl;

		result = cal_factor * ( static_cast<double>(rawValue) - 32768.0 ) / (32768.0);
		
		eventMeasurements.at(i)->setData( result );

		std::cerr << "AnalogIn -- The result is : " << result << endl;
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

