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
#include "EngineException.h"

using STI::TimingEngine::RawEvent;
using STI::TimingEngine::TimingEvent_ptr;
using STI::TimingEngine::TimingEvent;
using STI::TimingEngine::TextPosition;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::EngineException;

RawEvent::RawEvent(const TimingEvent_ptr& timingEvent) : evt(timingEvent)
{
}

void RawEvent::throwNullTimingEvent(const std::string& location) const
{
	throw EngineException(
		"Tried to dereference a null TimingEvent_ptr when accessing a RawEvent\n at location: " + 
		location + 
		". (RawEvent wrapper contains null TimingEvent_ptr!)");
}

void RawEvent::checkForNullTimingEvent(const std::string& location) const
{
	if(evt == 0) {
		throwNullTimingEvent(location);
	}
}

TimingEvent_ptr RawEvent::getTimingEvent() const
{
	checkForNullTimingEvent("getTimingEvent()");

	return evt;
}

bool RawEvent::getTimingEvent(TimingEvent_ptr& timingEvent) const
{
	timingEvent = evt;
	return !(timingEvent == 0);
}

bool RawEvent::operator==(const TimingEvent& rhs) const
{
	checkForNullTimingEvent("operator==");
	return evt->operator==(rhs);
}
bool RawEvent::operator!=(const TimingEvent& rhs) const
{
	checkForNullTimingEvent("operator!=");
	return evt->operator!=(rhs);
}
bool RawEvent::operator<(const TimingEvent& rhs) const
{
	checkForNullTimingEvent("operator<");
	return evt->operator<(rhs);
}
bool RawEvent::operator>(const TimingEvent& rhs) const
{
	checkForNullTimingEvent("operator>");
	return evt->operator>(rhs);
}

//bool RawEvent::operator==(const RawEvent& rhs) const { return evt->operator==(*(rhs.evt)); }
//bool RawEvent::operator!=(const RawEvent& rhs) const { return evt->operator!=(*(rhs.evt)); }
//bool RawEvent::operator<(const RawEvent& rhs) const { return evt->operator<(*(rhs.evt)); }
//bool RawEvent::operator>(const RawEvent& rhs) const { return evt->operator>(*(rhs.evt)); }

const STI::TimingEngine::EventTime& RawEvent::time() const
{
	checkForNullTimingEvent("time()");
	return evt->time();
}

const STI::TimingEngine::Channel& RawEvent::channel() const
{
	checkForNullTimingEvent("channel()");
	return evt->channel();
}

const STI::Utils::MixedValue& RawEvent::value() const
{
	checkForNullTimingEvent("value()");
	return evt->value();
}


bool RawEvent::getDynamicValue(STI::TimingEngine::DynamicValue_ptr& dynamicValue) const
{
	checkForNullTimingEvent("getDynamicValue()");
	return evt->getDynamicValue(dynamicValue);
}

const std::string& RawEvent::description() const
{
	checkForNullTimingEvent("description()");
	return evt->description();
}

unsigned RawEvent::eventNum() const
{
	checkForNullTimingEvent("eventNum()");
	return evt->eventNum();
}

bool RawEvent::isMeasurementEvent() const
{
	checkForNullTimingEvent("isMeasurementEvent()");
	return evt->isMeasurementEvent();
}

const TextPosition& RawEvent::position() const
{
	checkForNullTimingEvent("position()");
	return evt->position();
}

