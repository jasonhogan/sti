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

#ifndef STI_TIMINGENGINE_RAWEVENT_H
#define STI_TIMINGENGINE_RAWEVENT_H

//This is a thin wrapper class around a TimingEvent_ptr that is mainly in place
//to provide backwards compatibility to legacy STI code. In particular, this 
//class gets added to the event vector by value, so it allows value semantics
//for references member functions.
//
//The class redirects to a shared_ptr to a TimingEvent. Since TimingEvent is
//polymorphic, we can provide different implementations for local vs network
//events. RawEvents lets us keep the same interface without switching to -> notation.

#include "TimingEngineTypes.h"
#include "TimingEvent.h"
#include <string>

namespace STI
{
namespace TimingEngine
{

class RawEvent : public TimingEvent
{
public:

	RawEvent(const TimingEvent_ptr& timingEvent);

	bool getTimingEvent(TimingEvent_ptr& timingEvent) const;
	TimingEvent_ptr getTimingEvent() const;

	bool operator==(const TimingEvent& rhs) const;
	bool operator!=(const TimingEvent& rhs) const;
	bool operator<(const TimingEvent& rhs) const;
	bool operator>(const TimingEvent& rhs) const;

	const EventTime& time() const;
	const Channel& channel() const;

	const STI::Utils::MixedValue& value() const;

	//Deprecated -- use MixedValue value() instead. Kept for backwards compatibility
//	double                  numberValue() const;
//	std::string             stringValue() const;
//	const MixedValueVector& vectorValue() const;

	bool getDynamicValue(STI::TimingEngine::DynamicValue_ptr& dynamicValue) const;
	
	const std::string& description() const;

	unsigned eventNum() const;
	bool isMeasurementEvent() const;

	const TextPosition& position() const;

private:
	void checkForNullTimingEvent(const std::string& location) const;
	void throwNullTimingEvent(const std::string& location) const;

	TimingEvent_ptr evt;
};

}
}

#endif

