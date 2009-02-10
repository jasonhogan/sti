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


STF_AD_FAST::STF_AD_FAST_Device::STF_AD_FAST_Device(
		ORBManager*		orb_manager, 
		std::string		DeviceName, 
		std::string		IPAddress,
		unsigned short	ModuleNumber,
		unsigned int EtraxMemoryAddress) :
ad_fast(EtraxMemoryAddress),
FPGA_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
}

STF_AD_FAST::STF_AD_FAST_Device::~STF_AD_FAST_Device()
{
}

bool STF_AD_FAST::STF_AD_FAST_Device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void STF_AD_FAST::STF_AD_FAST_Device::defineAttributes()
{
	// addAttribute("DAQ Frequency", getDaqFreq());
	// addAttribute("# of MUXed input channels", getNumChannels());
	// addAttribute("Warp Mode", "Off", "On, Off");
	addAttribute("Value", read_data());
}

void STF_AD_FAST::STF_AD_FAST_Device::refreshAttributes()
{
	// setAttribute("DAQ Frequency", getDaqFreq());
	// setAttribute("# of MUXed input channels", getNumChannels());
	// setAttribute("Warp Mode", getMode());
	setAttribute("Value", read_data());

}

bool STF_AD_FAST::
STF_AD_FAST_Device::updateAttribute(std::string key, std::string value)
{
//	double tempDouble;
//	bool successDouble = stringToValue(value, tempDouble);

/*	bool success = false;

	if(key.compare("DAQ Frequency") == 0 && successDouble)		success = setDaqFreq(tempDouble);
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
	bool success = true; // remove this line when implementing the above functions
	return success;
}

void STF_AD_FAST::STF_AD_FAST_Device::defineChannels()
{
	addInputChannel(0, DataNumber);
}

bool STF_AD_FAST::STF_AD_FAST_Device::
readChannel(ParsedMeasurement &Measurement)
{
	Measurement.setData( read_data() );
	
	return true;
}

bool STF_AD_FAST::STF_AD_FAST_Device::
writeChannel(const RawEvent &Event)
{
	return false;
}

std::string STF_AD_FAST::STF_AD_FAST_Device::execute(int argc, char **argv)
{
	return "";
}

void STF_AD_FAST::STF_AD_FAST_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		boost::ptr_vector<SynchronousEvent>  &eventsOut) throw(std::exception)
{
	uInt32 value;

	RawEventMap::const_iterator iter;
	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{
		//TODO: construct bit line commands from iter->second events
		value = 0;

		eventsOut.push_back( 
			(new AnalogInEvent(iter->first, this))
			->setBits(1)		//temporary
			);

		eventsOut.back().addMeasurement( iter->second.at(1) );	//temporary! (it should pick the right event)
	}

}

void STF_AD_FAST::STF_AD_FAST_Device::AnalogInEvent::collectMeasurementData()
{
}
