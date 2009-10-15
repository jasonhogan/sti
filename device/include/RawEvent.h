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

//TValue
using STI::Types::ValueNumber;
using STI::Types::ValueString;
using STI::Types::ValueDDSTriplet;
using STI::Types::ValueMeas;

using STI::Types::TDDSValue;

class ParsedMeasurement;

class RawEvent
{
public:

	RawEvent(ParsedMeasurement& measurementEvent);
	RawEvent(double time, unsigned short channel, unsigned eventNumber);
	RawEvent(double time, unsigned short channel, double value, unsigned eventNumber);
	RawEvent(double time, unsigned short channel, std::string value, unsigned eventNumber);
	RawEvent(double time, unsigned short channel, STI::Types::TDDS value, unsigned eventNumber);
	RawEvent(const STI::Types::TDeviceEvent& deviceEvent, unsigned eventNumber);
	RawEvent(const RawEvent &copy);
	~RawEvent();

	RawEvent& operator= (const RawEvent& other);

	std::string print() const;
	std::string printDDSValue(const TDDSValue& value) const;

	double time() const;		//time in nanoseconds
	unsigned short channel() const;
	STI::Types::TValue type() const;

	double                  numberValue() const;
	std::string             stringValue() const;
	STI::Types::TDDS ddsValue() const;

	unsigned eventNum() const;

	ParsedMeasurement* getMeasurement() const;
	void setMeasurement(ParsedMeasurement* measurement);

	bool operator==(const RawEvent &other) const;
	bool operator!=(const RawEvent &other) const;
	bool TDDSValueEqual(const TDDSValue& right, const TDDSValue& left) const;

	void setChannel(unsigned short channel);
	void setValue(double value);
	void setValue(std::string value);
	void setValue(STI::Types::TDDS value);

	static std::string TValueToStr(STI::Types::TValue tValue);

private:
	
	ParsedMeasurement* measurement_;

	STI::Types::TDeviceEvent event_l;

	unsigned eventNumber_l;

};

#endif
