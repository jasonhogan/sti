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
#include <DataMeasurement.h>
#include <sstream>


RawEvent::RawEvent(DataMeasurement& measurementEvent)
{
	time_l = measurementEvent.time();
	channel_l = measurementEvent.channel();
	measurement_ = &measurementEvent;
}

RawEvent::RawEvent(double time, unsigned short channel, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	time_l = time;
	channel_l = channel;
}


RawEvent::RawEvent(const STI::Types::TDeviceEvent& deviceEvent, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	time_l = deviceEvent.time;
	channel_l = deviceEvent.channel;
	value_l.setValue(deviceEvent.value);

	measurement_ = 0;
}

RawEvent::RawEvent(const RawEvent &copy)
{
	time_l = copy.time_l;
	channel_l = copy.channel_l;
	value_l = copy.value_l;
	eventNumber_l = copy.eventNumber_l;
	measurement_ = copy.measurement_;
}

RawEvent::~RawEvent()
{
}

RawEvent& RawEvent::operator= (const RawEvent& other)
{
	time_l = other.time_l;
	channel_l = other.channel_l;
	value_l = other.value_l;
	eventNumber_l = other.eventNum();
	return (*this);
}

void RawEvent::setValue(const char* value)
{
	value_l.setValue( std::string(value) );
}

void RawEvent::setChannel(unsigned short channel)
{
	channel_l = channel;
}

std::string RawEvent::print() const
{
	std::stringstream evt;

	//<Time=2.1, Channel=4, Type=Number, Value=3.4>
	evt << "<Time=" << time();
	evt << ", Channel=" << channel();
	evt << ", Type=";

	switch( getValueType() )
	{
	case MixedValue::Double:
	case MixedValue::Int:
		evt << "Number";
		break;
	case MixedValue::String:
		evt << "String";
		break;
	case MixedValue::Vector:
		evt << "Vector";
		break;
	default:
		evt << "Unknown";
		break;
	}

	evt << ", Value=" << value().print() << ">";
	
	return evt.str();
}


double RawEvent::time() const
{
	return time_l;
}
unsigned short RawEvent::channel() const
{
	return channel_l;
}
STI::Types::TValue RawEvent::getSTItype() const
{
	switch(value_l.getType())
	{
	case MixedValue::Boolean:
	case MixedValue::Double:
	case MixedValue::Int:
		return STI::Types::ValueNumber;
		break;
	case MixedValue::String:
		return STI::Types::ValueString;
		break;
	case MixedValue::Vector:
		return STI::Types::ValueVector;
		break;
	default:
		return STI::Types::ValueMeas;	//this should never happen (?)
		break;
	}
}
MixedValue::MixedValueType RawEvent::getValueType() const
{
	return value_l.getType();
}
const MixedValue& RawEvent::value() const
{
	return value_l;
}

double RawEvent::numberValue() const
{
	if(getValueType() == MixedValue::Double)
		return value().getDouble();
	else if(getValueType() == MixedValue::Int)
		return static_cast<double>( value().getInt() );
	else
		return 0;
}

std::string RawEvent::stringValue() const
{
	if(getValueType() == MixedValue::String)
		return value().getString();
	else
		return "";
}

const MixedValueVector& RawEvent::vectorValue() const
{
	return value().getVector();
}


bool RawEvent::operator==(const RawEvent &other) const
{
	if(time() == other.time() && 
		channel() == other.channel() ) 
	{
		return ( value() == other.value() );
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

DataMeasurement* RawEvent::getMeasurement() const
{
	return measurement_;
}

void RawEvent::setMeasurement(DataMeasurement* measurement)
{
	measurement_ = measurement;
}
