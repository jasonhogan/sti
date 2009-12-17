/*! \file RawEvent.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class RawEvent
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

#ifndef PARSEDEVENT_H
#define PARSEDEVENT_H

#include <string>

#include "device.h"

#include <MixedValue.h>

class ParsedMeasurement;

class RawEvent
{
public:

	RawEvent(ParsedMeasurement& measurementEvent);
	RawEvent(double time, unsigned short channel, unsigned eventNumber);
	
	template<typename T> RawEvent(double time, unsigned short channel, T value, unsigned eventNumber) :
	time_l(time), channel_l(channel), value_l(value), eventNumber_l(eventNumber)
	{
//		time_l = time;
//		channel_l = channel;
//		value_l = value;
	}
	
	RawEvent(const STI::Types::TDeviceEvent& deviceEvent, unsigned eventNumber);
	RawEvent(const RawEvent &copy);
	~RawEvent();

	RawEvent& operator= (const RawEvent& other);

	std::string print() const;

	double time() const;		//time in nanoseconds
	unsigned short channel() const;
	STI::Types::TValue getSTItype() const;
	MixedValue::MixedValueType getValueType() const;

	const MixedValue& value() const;

	//Deprecated -- use MixedValue value() instead. Kept for backwards compatibility
	double                  numberValue() const;
	std::string             stringValue() const;
	const MixedValueVector& vectorValue() const;

	unsigned eventNum() const;

	ParsedMeasurement* getMeasurement() const;
	void setMeasurement(ParsedMeasurement* measurement);

	bool operator==(const RawEvent &other) const;
	bool operator!=(const RawEvent &other) const;

	void setChannel(unsigned short channel);
	
	template<typename T> void setValue(const T& value)
	{
		value_l.setValue( value );
	}
	void setValue(const char* value);

private:
	
	ParsedMeasurement* measurement_;

//	STI::Types::TDeviceEvent event_l;

	double         time_l;
	unsigned short channel_l;   //== STI::Types::TChannel.channel
	MixedValue value_l;

	unsigned eventNumber_l;

};

#endif
