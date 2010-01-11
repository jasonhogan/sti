/*! \file CompositeEvent.h
*  \author Jason Michael Hogan
*  \brief Include-file for the class CompositeEvent
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

#ifndef COMPOSITE_EVENT_H
#define COMPOSITE_EVENT_H

#include "orbTypes.h"

class CompositeEvent
{
public:
	CompositeEvent(const STI::Types::TEvent& tEvent);
	CompositeEvent(STI::Types::TDeviceEvent& tDeviceEvent, const STI::Types::TEvent& tEvent);
	~CompositeEvent();
	CompositeEvent& operator= (const CompositeEvent& other);

	STI::Types::TDeviceEvent& getTDeviceEvent();
	const STI::Types::TEvent& getTEvent();

private:

	STI::Types::TDeviceEvent tDeviceEvent_;
	const STI::Types::TEvent* tEvent_ptr;
};

#endif