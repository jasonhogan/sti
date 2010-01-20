/*! \file ServerEvent.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ServerEvent
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef SERVEREVENT_H
#define SERVEREVENT_H

#include "pusher.h"

class ServerEvent
{
public:

	enum ServerEventType;

	ServerEvent() : eventType(NULLEvent) {};

	ServerEvent(const STI::Pusher::TPingEvent& event) : eventType(Ping), pingEvent(event) {};
	ServerEvent(const STI::Pusher::TStatusEvent& event) : eventType(Status), statusEvent(event) {};
	ServerEvent(const STI::Pusher::TMessageEvent& event) : eventType(Message), messageEvent(event) {};
	ServerEvent(const STI::Pusher::TParseEvent& event) : eventType(Parse), parseEvent(event) {};
	ServerEvent(const STI::Pusher::TFileEvent& event) : eventType(File), fileEvent(event) {};
	ServerEvent(const STI::Pusher::TControllerEvent& event) : eventType(Controller), controllerEvent(event) {};
	ServerEvent(const STI::Pusher::TDeviceRefreshEvent& event) : eventType(DeviceRefresh), deviceRefreshEvent(event) {};
	ServerEvent(const STI::Pusher::TDeviceDataEvent& event) : eventType(DeviceData), deviceDataEvent(event) {};

	~ServerEvent() {};

	const ServerEventType getEventType() const {return eventType;}

	const STI::Pusher::TPingEvent& getTPingEvent() const {return pingEvent;}
	const STI::Pusher::TStatusEvent& getTStatusEvent() const {return statusEvent;}
	const STI::Pusher::TMessageEvent& getTMessageEvent() const {return messageEvent;}
	const STI::Pusher::TParseEvent& getTParseEvent() const {return parseEvent;}
	const STI::Pusher::TFileEvent& getTFileEvent() const {return fileEvent;}
	const STI::Pusher::TControllerEvent& getTControllerEvent() const {return controllerEvent;}
	const STI::Pusher::TDeviceRefreshEvent& getTDeviceRefreshEvent() const {return deviceRefreshEvent;}
	const STI::Pusher::TDeviceDataEvent& getTDeviceDataEvent() const {return deviceDataEvent;}

	enum ServerEventType {NULLEvent, Ping, Status, Message, Parse, File, Controller, DeviceRefresh, DeviceData};

private:

	ServerEventType eventType;

	STI::Pusher::TPingEvent pingEvent;
	STI::Pusher::TStatusEvent statusEvent;
	STI::Pusher::TMessageEvent messageEvent;
	STI::Pusher::TParseEvent parseEvent;
	STI::Pusher::TFileEvent fileEvent;
	STI::Pusher::TControllerEvent controllerEvent;
	STI::Pusher::TDeviceRefreshEvent deviceRefreshEvent;
	STI::Pusher::TDeviceDataEvent deviceDataEvent;
};

#endif
