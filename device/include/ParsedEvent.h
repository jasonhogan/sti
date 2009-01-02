/*! \file ParsedEvent.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ParsedEvent
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
using STI_Server_Device::ValueNumber;
using STI_Server_Device::ValueString;
using STI_Server_Device::ValueDDSTriplet;
using STI_Server_Device::ValueMeas;


class ParsedEvent
{
public:

	ParsedEvent(const STI_Server_Device::TDeviceEvent& deviceEvent, unsigned eventNumber);
	~ParsedEvent();

	ParsedEvent& operator= (const ParsedEvent& other);

	std::string print() const;

	double time() const;
	unsigned short channel() const;
	STI_Server_Device::TValue type() const;

	double                  numberValue() const;
	std::string             stringValue() const;
	STI_Server_Device::TDDS ddsValue() const;

	unsigned eventNum() const;

	bool operator==(const ParsedEvent &other) const;
	bool operator!=(const ParsedEvent &other) const;

	static std::string TValueToStr(STI_Server_Device::TValue tValue);

private:

	STI_Server_Device::TDeviceEvent event_l;

	unsigned eventNumber_l;

};

#endif
