/*! \file ADF4360_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Analog_Devices_VCO::ADF4360_Device
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#include "ADF4360_Device.h"
#include <sstream>
using std::endl;


Analog_Devices_VCO::ADF4360_Device::ADF4360_Device(
		ORBManager*		orb_manager, 
		std::string		DeviceName, 
		std::string		IPAddress,
		unsigned short	ModuleNumber,
		unsigned int VCO_Address,
		unsigned int EtraxMemoryAddress,
		unsigned short ADF4360_model) :
ADF4360(VCO_Address, EtraxMemoryAddress, ADF4360_model),
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber),
event_controlLatch(0),
event_nCounterLatch(0),
event_rCounterLatch(0),
eventLatches(event_controlLatch, event_nCounterLatch, event_rCounterLatch)
{
	eventLatches.setLatches( getVCOLatches() );

	//ADF4360 event holdoff parameters
	digitalMinimumEventSpacing = 1000;	//500 ns
	minimumEventSpacing = 160000;	//155 * 1us = 160 us (approx)
	minimumAbsoluteStartTime = minimumEventSpacing + 10000;	//10 us pad to wait for the digital board (conservative)
	holdoff = minimumEventSpacing - 6000 + 40;	//empirically determined
	localHoldoff = 0;	//additional holdoff for local events (not digital board partner events)
	digitalStartChannel = 8;
}

Analog_Devices_VCO::ADF4360_Device::~ADF4360_Device()
{
}

bool Analog_Devices_VCO::ADF4360_Device::deviceMain(int argc, char **argv)
{

	return false;
}

void Analog_Devices_VCO::ADF4360_Device::defineAttributes()
{
	addAttribute("Fvco", getFvco());
	addAttribute("Power", "-6 dBm", "-6 dBm, -8 dBm, -11 dBm, -13 dBm, Off");
//	addAttribute("Digital event spacing", digitalMinimumEventSpacing);
}

void Analog_Devices_VCO::ADF4360_Device::refreshAttributes()
{
	setAttribute("Fvco", getFvco());
	setAttribute("Power", getPowerStatus());
//	setAttribute("Digital event spacing", digitalMinimumEventSpacing);

//	sendLatches();
}

bool Analog_Devices_VCO::
ADF4360_Device::updateAttribute(std::string key, std::string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;

	if(key.compare("Fvco") == 0 && successDouble)
		success = setFvco(tempDouble);
	//else if(key.compare("Digital event spacing") == 0 && successDouble)
	//{
	//	success = true;
	//	digitalMinimumEventSpacing = tempDouble;
	//}
	else if(key.compare("Power") == 0)
	{
		success = true;

		if(value.compare("Off") == 0)
			SynchronousPowerDown();
		else if(value.compare("-13 dBm") == 0)
		{
			success &= setOutputPower(0);
		}
		else if(value.compare("-11 dBm") == 0)
		{
			success &= setOutputPower(1);
		}
		else if(value.compare("-8 dBm") == 0)
		{
			success &= setOutputPower(2);
		}
		else if(value.compare("-6 dBm") == 0)
		{
			success &= setOutputPower(3);
		}
		else
			success = false;
	}

	return success;
}

void Analog_Devices_VCO::ADF4360_Device::defineChannels()
{
	addOutputChannel(0, ValueNumber);	//frequency
	addOutputChannel(1, ValueString);	//Power
}



void Analog_Devices_VCO::ADF4360_Device::definePartnerDevices()
{
	addPartnerDevice("Digital Board", "ep-timing1.stanford.edu", 2, "Digital Out");
	partnerDevice("Digital Board").enablePartnerEvents();
}

bool Analog_Devices_VCO::ADF4360_Device::readChannel(ParsedMeasurement &Measurement)
{
	if(Measurement.channel() == 0)	//frequency
	{
		Measurement.setData( getFvco() );
		return true;
	}
	if(Measurement.channel() == 1)	//Power
	{
		Measurement.setData( getPowerStatus() );
		return true;
	}
	return false;
}

bool Analog_Devices_VCO::ADF4360_Device::writeChannel(const RawEvent &Event)
{
	if(Event.channel() == 0 && Event.getValueType() == MixedValue::Double)		//frequency
		return setAttribute("Fvco", Event.numberValue() );
	if(Event.channel() == 1 && Event.getValueType() == MixedValue::String)		//Power
		return setAttribute("Power", Event.stringValue() );

	return false;
}

std::string Analog_Devices_VCO::ADF4360_Device::execute(int argc, char **argv)
{
	return parseArgs(argc, argv);
}



std::string Analog_Devices_VCO::ADF4360_Device::parseArgs(int argc, char **argv)
{
	std::vector<std::string> args;
	convertArgs(argc, argv, args);



	return "";
}

std::string Analog_Devices_VCO::ADF4360_Device::printUsage(std::string executableName)
{
	std::stringstream terminalStream;

    terminalStream 	
        << "Usage: " << endl 
        << executableName << " --help | -h            <Print detailed usage>" << endl
		<< executableName << " [--Fvco<address#>=frequency] [--Power<address#>=power]" << endl
		<< "frequency: The frequency of the VCO in MHz" << endl
		<< "power: The power of the VCO; allowed values are '-6 dBm', '-8 dBm', '-11 dBm', '-13 dBm, Off'" << endl
		<< "address#: The the VCO address." << endl;

	return terminalStream.str();
}



void Analog_Devices_VCO::ADF4360_Device::parseDeviceEvents(
	const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception)
{
//	parseDeviceEventsDefault(eventsIn, eventsOut);
	
	RawEventMap::const_iterator events;

	double eventTime;
	double previousTime;
	double latchHoldoff;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
		{
			previousTime = minimumAbsoluteStartTime - minimumEventSpacing;
		}
		
		eventTime = events->first - holdoff;

		if( (events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The ADF4360 needs " + valueToString(minimumEventSpacing) + " ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The ADF4360 needs " + valueToString(minimumAbsoluteStartTime)+ " ns at the beginning of the timing file.");
		}

		//Setup eventLatches with the new frequency and power settings
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).channel() == 0)	//frequency change
			{
				setFvcoEvent(events->second.at(i));
			}
			if(events->second.at(i).channel() == 1)	//power change
			{
				setOutputPowerEvent(events->second.at(i));
			}
		}

		//keep the program's latches synchronized with what the digital board is writting.
		eventsOut.push_back( 
				new ADF4360Event(eventTime - localHoldoff, this, eventLatches) );

		latchHoldoff = buildAndSendBuffer(
			eventTime, events->second.at(0), eventLatches.rCounterLatch);
		latchHoldoff = buildAndSendBuffer(
			eventTime + latchHoldoff, events->second.at(0), eventLatches.controlLatch);
		buildAndSendBuffer(
			eventTime + 2 * latchHoldoff, events->second.at(0), eventLatches.nCounterLatch);

	}
}

double Analog_Devices_VCO::ADF4360_Device::buildAndSendBuffer(double eventTime, const RawEvent& evt, std::bitset<24>& latch)
{
	BuildSerialBufferLean(latch);

	std::vector<SerialData>& serBuf = getSerialBuffer();

	double latchHoldoff = digitalMinimumEventSpacing;

	std::bitset<8> word(0);
	std::bitset<8> lastWord(0);

	unsigned i,j;
	for(i = 0; i < serBuf.size(); i++)	//send serial stream of words
	{
		for(j = 0; j < 8; j++)	//send 8 bits per word
		{
			word.set(j, serBuf.at(i).getPin(j, getVCOAddress()) );
			if( word.at(j) != lastWord.at(j) || i == 0)	//Send only unique words (plus the first word).
			{
				partnerDevice("Digital Board").event(eventTime + i * digitalMinimumEventSpacing, 
					digitalStartChannel + j, word.test(j), evt);
			}
			lastWord = word;
		}
		latchHoldoff += digitalMinimumEventSpacing;
	}
	return latchHoldoff;

}

void Analog_Devices_VCO::ADF4360_Device::setFvcoEvent(const RawEvent& evt) throw(std::exception)
{
	double oldFreq = getFvco();
	double newFreq = evt.numberValue();

	if(!setFvco(newFreq))
	{
		throw EventParsingException(evt,
						"The ADF4360 failed to set its frequency to " + valueToString(newFreq) + ".");
	}

	//copy the VCO latches into eventLatches before resetting them
	eventLatches.setLatches( getVCOLatches() );

	setFvco(oldFreq);
}

void Analog_Devices_VCO::ADF4360_Device::setOutputPowerEvent(const RawEvent& evt) throw(std::exception)
{
	unsigned short oldPower = getOutputPower();
	bool oldPowerStatus = getPowerStatus();
	std::string newPower = evt.stringValue();
	bool success = false;

	if(newPower.compare("Off") == 0)
	{
		SynchronousPowerDownPrepare();
		success = true;
	}
	else if(newPower.compare("-13 dBm") == 0)
	{
		PowerUpPrepare();
		success = setOutputPower(0);
	}
	else if(newPower.compare("-11 dBm") == 0)
	{
		PowerUpPrepare();
		success = setOutputPower(1);
	}
	else if(newPower.compare("-8 dBm") == 0)
	{
		PowerUpPrepare();
		success = setOutputPower(2);
	}
	else if(newPower.compare("-6 dBm") == 0)
	{
		PowerUpPrepare();
		success = setOutputPower(3);
	}
	else
	{
		success = false;
	}

	if(!success)
	{
		throw EventParsingException(evt,
						"The ADF4360 failed to set its power to " + newPower + ".");
	}

	//copy the VCO latches into eventLatches before resetting them
	eventLatches.setLatches( getVCOLatches() );

	//reset
	if(!oldPowerStatus)	//it was off
	{
		SynchronousPowerDownPrepare();
	}
	else
	{
		PowerUpPrepare();
	}

	setOutputPower(oldPower);
}



void Analog_Devices_VCO::ADF4360_Device::ADF4360Event::playEvent()
{
	device_adf->getVCOLatches().setLatches( latches_ );
}


