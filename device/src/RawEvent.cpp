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
//#include "NetworkDynamicValue.h"
#include <sstream>
#include <utils.h>
#include "DynamicValue.h"


RawEvent::RawEvent(DataMeasurement& measurementEvent)
{
	time_l = measurementEvent.time();
	channel_l = measurementEvent.channel();
	measurement_ = new DataMeasurement(measurementEvent);
	isMeasurement = true;
	hasDynamicValue = false;

	fileLocation = "";
	lineLocation = 0;
}

RawEvent::RawEvent(double time, unsigned short channel, unsigned eventNumber, bool isMeasurementEvent) :
eventNumber_l(eventNumber), isMeasurement(isMeasurementEvent), hasDynamicValue(false)
{
	time_l = time;
	channel_l = channel;

	fileLocation = "";
	lineLocation = 0;

	if(isMeasurement)
		measurement_ = new DataMeasurement(time, channel, eventNumber);
	else
		measurement_ = NULL;
}


RawEvent::RawEvent(const STI::Types::TDeviceEvent& deviceEvent, unsigned eventNumber) :
eventNumber_l(eventNumber), hasDynamicValue(false)
{
	time_l = deviceEvent.time;
	channel_l = deviceEvent.channel;
	value_l.setValue(deviceEvent.value);
	isMeasurement = deviceEvent.isMeasurementEvent;
	fileLocation = deviceEvent.pos.file;
	lineLocation = deviceEvent.pos.line;

	hasDynamicValue = deviceEvent.hasDynamicValue;
	
	if(isMeasurement) {
		measurement_ = new DataMeasurement(time_l, channel_l, eventNumber_l);
		if(deviceEvent.useCallback) {
			measurement_->installMeasurementCallback(
				STI::Server_Device::TMeasurementCallback::_duplicate(deviceEvent.callbackRef));
		}
	}
	else {
		measurement_ = NULL;
	}

	if(hasDynamicValue) {
		dynamicValue_l = DynamicValue_ptr(new DynamicValue(value_l));
//		dynamicValue_l->setValue(value_l);	//initialization

//		remoteDynamicValueLinkRef = STI::Server_Device::DynamicValueLink::_duplicate(deviceEvent.dynamicValueRef);

		try {
			//DynamicValueLink servant for the device that play's the event; this servant will
			//listen to changes made to the DynamicValue that originate elsewhere.
			dynamicValueLink = DynamicValueLink_i_ptr(
				new DynamicValueLink_i(dynamicValue_l,
				STI::Server_Device::DynamicValueLink::_duplicate(deviceEvent.dynamicValueRef)));

			//Give the remote instance of the DynamicValueLink a reference to the local instance.
			//This lets the remote instance trigger refresh events on the local DynamicValue.
//			remoteDynamicValueLinkRef->addLink(dynamicValueLink->_this());
		} catch(...) {
			hasDynamicValue = false;
		}
	}
}

RawEvent::RawEvent(const RawEvent &copy)
{
	time_l = copy.time_l;
	channel_l = copy.channel_l;
	value_l = copy.value_l;
	eventNumber_l = copy.eventNumber_l;
	isMeasurement = copy.isMeasurement;
	measurement_ = copy.measurement_; //just get the pointer

	hasDynamicValue = copy.hasDynamicValue;
	dynamicValue_l = copy.dynamicValue_l;

	fileLocation = copy.fileLocation;
	lineLocation = copy.lineLocation;

	dynamicValue_l = copy.dynamicValue_l;
	dynamicValueLink = copy.dynamicValueLink;
//	remoteDynamicValueLinkRef = copy.remoteDynamicValueLinkRef;
}

RawEvent::~RawEvent()
{
	//if(hasDynamicValue && remoteDynamicValueLinkRef != 0) {
	//	try {
	//		remoteDynamicValueLinkRef->unLink();
	//	} catch(...) {
	//	}
	//}
	//if(measurement_ != NULL)
	//{
	//	delete measurement_;
	//	measurement_ = NULL;
	//}
}

RawEvent& RawEvent::operator= (const RawEvent& other)
{
	time_l = other.time_l;
	channel_l = other.channel_l;
	value_l = other.value_l;
	eventNumber_l = other.eventNum();
	isMeasurement = other.isMeasurement;
	measurement_ = other.measurement_;	//just get the pointer

	fileLocation = other.fileLocation;
	lineLocation = other.lineLocation;

	hasDynamicValue = other.hasDynamicValue;
	dynamicValue_l = other.dynamicValue_l;
	dynamicValueLink = other.dynamicValueLink;

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
	evt << "<Time=" << STI::Utils::printTimeFormated(time());
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
	case MixedValue::Empty:
		evt << "Empty";
		break;
	default:
		evt << "Unknown";
		break;
	}

	evt << ", Value=" << value().print() << ">";
	
	return evt.str();
}

std::string RawEvent::file() const
{
	return fileLocation;
}

long RawEvent::line() const
{
	return lineLocation;
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
	case MixedValue::String:
		return STI::Types::ValueString;
	case MixedValue::Vector:
		return STI::Types::ValueVector;
	case MixedValue::Empty:
		return STI::Types::ValueNone;
	default:
		return STI::Types::ValueNumber;	//this should never happen (?)
	}
}
MixedValue::MixedValueType RawEvent::getValueType() const
{
	return value_l.getType();
}
const MixedValue& RawEvent::value() const
{
	if(hasDynamicValue) {
//		MixedValue* val = 0;
		return dynamicValue_l->getValue();
		//if(val != 0) {
		//	return *val;
		//}
	}
	return value_l;
}

double RawEvent::numberValue() const
{
	if(getValueType() == MixedValue::Double)
		return value().getDouble();
	else if(getValueType() == MixedValue::Int)
		return static_cast<double>( value().getInt() );
	else
	{
		double result = 0;
		return 0.0 / result;	//NaN
	}
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
	if(time() == other.time() 
		&& channel() == other.channel() 
		&& line() == other.line()
		&& file().compare(other.file()) == 0 ) 
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

bool RawEvent::getDynamicValue(DynamicValue_ptr& dynamicValue) const
{
	dynamicValue = dynamicValue_l;
	return hasDynamicValue && (dynamicValue != 0);
}
//bool RawEvent::hasLinkedValue() const
//{
//	return hasLinkedVal;
//}

