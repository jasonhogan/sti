/*! \file CompositeEvent.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class CompositeEvent
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#include "CompositeEvent.h"

CompositeEvent::CompositeEvent(const STI::Types::TEvent& tEvent) : 
tEvent_ptr(&tEvent)
{
}

CompositeEvent::CompositeEvent(STI::Types::TDeviceEvent& tDeviceEvent, const STI::Types::TEvent& tEvent) : 
tDeviceEvent_(tDeviceEvent), tEvent_ptr(&tEvent)
{
}

CompositeEvent::~CompositeEvent()
{
}

CompositeEvent& CompositeEvent::operator= (const CompositeEvent& other)
{
	tDeviceEvent_ = other.tDeviceEvent_;
	tEvent_ptr = other.tEvent_ptr;
	return (*this);
}


STI::Types::TDeviceEvent& CompositeEvent::getTDeviceEvent()
{
	return tDeviceEvent_;
}


const STI::Types::TEvent& CompositeEvent::getTEvent()
{
	return (*tEvent_ptr);
}


