/*! \file Trigger_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Trigger_Device
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

#include "Trigger_Device.h"

#include <iostream>

Trigger_Device::Trigger_Device(ORBManager* orb_manager, std::string DeviceName, 
							   std::string IPAddress, unsigned short ModuleNumber, 
							   uInt32 EtraxMemoryAddress) : 
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber),
etraxMemoryAddress(EtraxMemoryAddress)
{
	bus = new EtraxBus(EtraxMemoryAddress);
	
	play  = (1 << 0);
	stop  = (1 << 1);
	pause = (1 << 2);
}

Trigger_Device::~Trigger_Device()
{
	delete bus;
}

bool Trigger_Device::deviceMain(int argc, char** argv)
{
	return false;
}

void Trigger_Device::defineAttributes()
{
}

void Trigger_Device::refreshAttributes()
{
}

bool Trigger_Device::updateAttribute(std::string key, std::string value)
{
	return false;
}

void Trigger_Device::defineChannels()
{
	addOutputChannel(0, ValueString);
}

bool Trigger_Device::readChannel(ParsedMeasurement& Measurement)
{
	return false;
}

bool Trigger_Device::writeChannel(const RawEvent& Event)
{
	return false;
}

void Trigger_Device::definePartnerDevices()
{
}

std::string Trigger_Device::execute(int argc, char** argv)
{
	return "";
}

void Trigger_Device::parseDeviceEvents(const RawEventMap& eventsIn,
					   SynchronousEventVector& eventsOut) throw(std::exception)
{
	//Add an "play" event with time = 0
	// for now, we have added a hack work around to arm the fpga. this asserts stop (0b010) then 0 on command bits,
	//followed by pause (0b100) followed by play (0b001) executed as fast as the computer can go. 
	//FPGA clock does not start until play is asserted
	//other non-fpga devices will be ahead by time delay it takes to get to fpga play event
//	eventsOut.push_back( new TriggerEvent(0, stop, this) );
//	eventsOut.push_back( new TriggerEvent(1, 0, this) );
//	eventsOut.push_back( new TriggerEvent(2, pause, this) );
//	eventsOut.push_back( new TriggerEvent(3, play, this) );

	//eventsIn is typically empty, but there can be user defined events
	RawEventMap::const_iterator events;
	uInt32 value = 0;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1 || events->first == 0)	//there is already an event at 0
		{
			throw EventConflictException(events->second.at(0), 
				events->second.at(1), 
				"The trigger cannot have multiple events at the same time." );
		}

		if(events->second.at(0).stringValue().compare("play") == 0 ||
			events->second.at(0).stringValue().compare("Play") == 0 ||
			events->second.at(0).stringValue().compare("PLAY") == 0)
		{
			value = play;
		}
		if(events->second.at(0).stringValue().compare("stop") == 0 ||
			events->second.at(0).stringValue().compare("Stop") == 0 ||
			events->second.at(0).stringValue().compare("STOP") == 0)
		{
			value = stop;
		}
		if(events->second.at(0).stringValue().compare("pause") == 0 ||
			events->second.at(0).stringValue().compare("Pause") == 0 ||
			events->second.at(0).stringValue().compare("PAUSE") == 0)
		{
			value = pause;
		}

		eventsOut.push_back( 
			new TriggerEvent(events->first, value, this) );
	}
}

void Trigger_Device::stopEventPlayback()
{
	bus->writeData(stop);
}


void Trigger_Device::writeData(uInt32 data)
{
	bus->writeData(data);
//std::cout << "Trigger_Device::writeData(" << data << ")" << std::endl;
//	if(bus == NULL)
//		std::cout << "bus is NULL!" << std::endl;
//	else
//		bus->writeData(data, 0);
}


void Trigger_Device::TriggerEvent::playEvent()
{
	trigger->writeData( getValue() );
}


