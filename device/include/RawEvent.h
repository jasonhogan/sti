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

#ifndef RAWEVENT_H
#define RAWEVENT_H

#include <string>

#include "device.h"

#include <MixedValue.h>

class DataMeasurement;

class RawEvent
{
public:

	RawEvent(DataMeasurement& measurementEvent);
	RawEvent(double time, unsigned short channel, unsigned eventNumber, bool isMeasurementEvent=false);
	
	template<typename T> RawEvent(double time, unsigned short channel, const T& value, unsigned eventNumber, bool isMeasurementEvent=false) :
	time_l(time), channel_l(channel), value_l(value), eventNumber_l(eventNumber), isMeasurement(isMeasurementEvent)
	{
		if(isMeasurement)
			measurement_ = new DataMeasurement(time, channel, eventNumber);
		else
			measurement_ = NULL;
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

	std::string file() const;
	long line() const;

	//Deprecated -- use MixedValue value() instead. Kept for backwards compatibility
	double                  numberValue() const;
	std::string             stringValue() const;
	const MixedValueVector& vectorValue() const;

	unsigned eventNum() const;
	bool isMeasurementEvent() const { return isMeasurement; }

	DataMeasurement* getMeasurement() const;
	void setMeasurement(DataMeasurement* measurement);

	bool operator==(const RawEvent &other) const;
	bool operator!=(const RawEvent &other) const;

	void setChannel(unsigned short channel);
	
	template<typename T> void setValue(const T& value)
	{
		value_l.setValue( value );
	}
	void setValue(const char* value);

private:
	

//	STI::Types::TDeviceEvent event_l;

	double         time_l;
	unsigned short channel_l;   //== STI::Types::TChannel.channel
	MixedValue value_l;

	unsigned eventNumber_l;

	bool isMeasurement;
	DataMeasurement* measurement_;

	std::string fileLocation;
	long lineLocation;
};

#endif
