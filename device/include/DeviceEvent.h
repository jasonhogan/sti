/*! \file DeviceEvent.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceEvent
 *  \section license License
 *
 *  Copyright (C) 2011 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef DEVICEEVENT_H
#define DEVICEEVENT_H

#include "pusher.h"

class DeviceEvent
{
public:

	enum DeviceEventType;

	DeviceEvent() : eventType(NULLEvent) {};
	DeviceEvent(const STI::Pusher::TMessageEvent& event) : eventType(Message), messageEvent(event) {};
	DeviceEvent(const STI::Pusher::TDeviceRefreshEvent& event) : eventType(DeviceRefresh), deviceRefreshEvent(event) {};
	DeviceEvent(const STI::Pusher::TDeviceDataEvent& event) : eventType(DeviceData), deviceDataEvent(event) {};

	~DeviceEvent() {};

	const DeviceEventType getEventType() const {return eventType;}

	const STI::Pusher::TMessageEvent& getTMessageEvent() const {return messageEvent;}
	const STI::Pusher::TDeviceRefreshEvent& getTDeviceRefreshEvent() const {return deviceRefreshEvent;}
	const STI::Pusher::TDeviceDataEvent& getTDeviceDataEvent() const {return deviceDataEvent;}

	enum DeviceEventType {NULLEvent, Message, DeviceRefresh, DeviceData};

private:

	DeviceEventType eventType;

	STI::Pusher::TMessageEvent       messageEvent;
	STI::Pusher::TDeviceRefreshEvent deviceRefreshEvent;
	STI::Pusher::TDeviceDataEvent    deviceDataEvent;
};

#endif

