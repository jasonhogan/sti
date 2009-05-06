/*! \file RawEvent.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class RawEvent
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

#include <RawEvent.h>
#include <sstream>


RawEvent::RawEvent(double time, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	event_l.time = time;
}

RawEvent::RawEvent(double time, unsigned short channel, double value, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
//	event_l.value._d( ValueNumber );
	event_l.time = time;
	event_l.channel = channel;
	event_l.value.number(value);
}

RawEvent::RawEvent(double time, unsigned short channel, std::string value, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
//	event_l.value._d( ValueString );
	event_l.time = time;
	event_l.channel = channel;
	event_l.value.stringVal(value.c_str());
}

RawEvent::RawEvent(double time, unsigned short channel, STI_Server_Device::TDDS value, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
//	event_l.value._d( ValueDDSTriplet );
	event_l.time = time;
	event_l.channel = channel;
	event_l.value.triplet(value);
}

RawEvent::RawEvent(const STI_Server_Device::TDeviceEvent& deviceEvent, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	event_l.time = deviceEvent.time;
	event_l.channel = deviceEvent.channel;
	event_l.value = deviceEvent.value;

	measurement_ = 0;
}

RawEvent::RawEvent(const RawEvent &copy)
{
	event_l.time = copy.event_l.time;
	event_l.channel = copy.event_l.channel;
	event_l.value = copy.event_l.value;
	eventNumber_l = copy.eventNum();
	measurement_ = copy.measurement_;
}

RawEvent::~RawEvent()
{
}

RawEvent& RawEvent::operator= (const RawEvent& other)
{
	event_l.time = other.event_l.time;
	event_l.channel = other.event_l.channel;
	event_l.value = other.event_l.value;
	eventNumber_l = other.eventNum();
	return (*this);
}

void RawEvent::setChannel(unsigned short channel)
{
	event_l.channel = channel;
}

void RawEvent::setValue(double value)
{
//	event_l.value._d( ValueNumber );
	event_l.value.number(value);
}

void RawEvent::setValue(std::string value)
{
//	event_l.value._d( ValueString );
	event_l.value.stringVal(value.c_str());
}

void RawEvent::setValue(STI_Server_Device::TDDS value)
{
//	event_l.value._d( ValueDDSTriplet );
	event_l.value.triplet(value);
}


std::string RawEvent::print() const
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
	evt << ">";
	return evt.str();
}

std::string RawEvent::TValueToStr(STI_Server_Device::TValue tValue)
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

double RawEvent::time() const
{
	return event_l.time;
}
unsigned short RawEvent::channel() const
{
	return event_l.channel;
}
STI_Server_Device::TValue RawEvent::type() const
{
	return event_l.value._d();
}

double RawEvent::numberValue() const
{
	if(type() == ValueNumber)
		return event_l.value.number();
	else
		return 0;
}

std::string RawEvent::stringValue() const
{
	if(type() == ValueString)
		return event_l.value.stringVal();
	else
		return "";
}
STI_Server_Device::TDDS RawEvent::ddsValue() const
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

bool RawEvent::operator==(const RawEvent &other) const
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

bool RawEvent::operator!=(const RawEvent &other) const
{
	return !( (*this)==other );
}

unsigned RawEvent::eventNum() const
{
	return eventNumber_l;
}

ParsedMeasurement* RawEvent::getMeasurement() const
{
	return measurement_;
}

void RawEvent::setMeasurement(ParsedMeasurement* measurement)
{
	measurement_ = measurement;
}
