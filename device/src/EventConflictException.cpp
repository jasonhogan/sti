/*! \file EventConflictException.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class EventConflictException
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

#include "EventConflictException.h"

#include <string>

using STI::TimingEngine::EventConflictException;
using STI::TimingEngine::TimingEvent_ptr;

EventConflictException::EventConflictException(
	const TimingEvent_ptr& Event, const std::string& message) :
STI_Exception(message),
Event1(Event),
Event2(Event)
{
}

EventConflictException::EventConflictException(
	const TimingEvent_ptr& event1, const TimingEvent_ptr& event2, const std::string& message) :
STI_Exception(message),
Event1(event1),
Event2(event2)
{
}

EventConflictException::~EventConflictException() throw()
{
}

double EventConflictException::lastTime() const
{
	if(Event1->time() > Event2->time())
		return Event1->time();
	else
		return Event2->time();
}

const TimingEvent_ptr& EventConflictException::getEvent1() const
{
	return Event1;
}

const TimingEvent_ptr& EventConflictException::getEvent2() const
{
	return Event2;
}


