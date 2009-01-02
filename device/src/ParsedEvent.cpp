/*! \file ParsedEvent.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ParsedEvent
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

#include <ParsedEvent.h>
#include <sstream>

ParsedEvent::ParsedEvent(const STI_Server_Device::TDeviceEvent& deviceEvent, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	event_l.time = deviceEvent.time;
	event_l.channel = deviceEvent.channel;
	event_l.value = deviceEvent.value;
}

ParsedEvent::~ParsedEvent()
{
}

ParsedEvent& ParsedEvent::operator= (const ParsedEvent& other)
{
	eventNumber_l = other.eventNum();
	return (*this);
}

std::string ParsedEvent::print() const
{
	std::stringstream evt;

	//<Time=2.1, Channel=4, Type=Number, Value=3.4>
	evt << "<Time=" << time();
	evt << ", Channel=" << channel();
	evt << ", Type=";
	
	switch(type())
	{
	case ValueNumber:
		evt << TValueToStr(ValueNumber) << ", Value=" << numberValue();
		break;
	case ValueString:
		evt << TValueToStr(ValueString) << ", Value=" << stringValue();
		break;
	case ValueDDSTriplet:
		evt << TValueToStr(ValueDDSTriplet) << ", Value={" 
			<< ddsValue().ampl << "," 
			<< ddsValue().freq << "," 
			<< ddsValue().phase << "}";
		break;
	case ValueMeas:
		evt << TValueToStr(ValueMeas) << ", Value=None";
		break;
	default:
		evt << "Invalid" << ", Value=Invalid";
		break;
	}

	return evt.str();
}

std::string ParsedEvent::TValueToStr(STI_Server_Device::TValue tValue)
{
	switch(tValue)
	{
	case ValueNumber:
		return "Number";
	case ValueString:
		return "String";
	case ValueDDSTriplet:
		return "DDS Triplet";
	case ValueMeas:
		return "Measurement";
	default:
		return "Invalid";
	}
}

double ParsedEvent::time() const
{
	return event_l.time;
}
unsigned short ParsedEvent::channel() const
{
	return event_l.channel;
}
STI_Server_Device::TValue ParsedEvent::type() const
{
	return event_l.value._d();
}

double ParsedEvent::numberValue() const
{
	if(type() == ValueNumber)
		return event_l.value.number();
	else
		return 0;
}

std::string ParsedEvent::stringValue() const
{
	if(type() == ValueString)
		return event_l.value.stringVal();
	else
		return "";
}
STI_Server_Device::TDDS ParsedEvent::ddsValue() const
{

	if(type() == ValueDDSTriplet)
		return event_l.value.triplet();
	else
	{
		STI_Server_Device::TDDS dummy;
		dummy.ampl = 0;
		dummy.freq = 0;
		dummy.phase = 0;
		
		return dummy;
	}
}

bool ParsedEvent::operator==(const ParsedEvent &other) const
{
	if(
		time() == other.time() && 
		channel() == other.channel() && 
		type() == other.type()) 
	{
		switch(type())
		{
		case ValueNumber:
			return numberValue() == other.numberValue();
		case ValueString:
			return stringValue() == other.stringValue();
		case ValueDDSTriplet:
			return (
				ddsValue().ampl == other.ddsValue().ampl &&
				ddsValue().freq == other.ddsValue().freq &&
				ddsValue().phase == other.ddsValue().phase);
		case ValueMeas:
			return true;
		default:
			return true;
		}
	}
	return false;
}

bool ParsedEvent::operator!=(const ParsedEvent &other) const
{
	return !( (*this)==other );
}

unsigned ParsedEvent::eventNum() const
{
	return eventNumber_l;
}

