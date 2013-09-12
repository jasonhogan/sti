/*! \file DeviceEventPusher.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceEventPushe
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



class DeviceEventFactory
{
	virtual DeviceEventTarget_ptr makeMessageEvent(std::string message) = 0;
	virtual DeviceEventTarget_ptr makeParseEvent(...) = 0;
};

class NetworkDeviceEventFactory
{
	DeviceEventTarget_ptr makeMessageEvent(std::string message)
	{
		return DeviceEventTarget_ptr(new NetworkMessageEvent(message));
	}
	class NetworkMessageEvent : public MessageEvent
	{
		NetworkMessageEvent(std::string message);
		std::string setMessage(std::string message) { tEvent.message = message; }
		TMessageEvent tEvent;
	};
};

class DeviceEventPusher
{
	class DeviceEventWrapper : public QueuedEvent
	{
		DeviceEventWrapper(const DeviceEventTarget_ptr& target);
		DeviceEvent_ptr evt;

		void run() {
			target->pushEvent(evt);
		}
	};

	void pushEvent(const DeviceEvent& event)
	{
		fifo.addEvent(event);
	}

	QueuedEventHandler fifo;
};




#ifndef DEVICEEVENTPUSHER_H
#define DEVICEEVENTPUSHER_H

#include "pusher.h"
#include "DeviceEvent.h"

#include <queue>

class STI_Server;

class DeviceEventPusher
{
public:

	DeviceEventPusher();
	DeviceEventPusher(STI::Pusher::DeviceEventHandler_ptr deviceEventHandler);
	~DeviceEventPusher();

	void installDeviceEventHandler(STI::Pusher::DeviceEventHandler_ptr deviceEventHandler);

	template<class T>
	void pushEventToServer(const T& event) { pushEvent( DeviceEvent(event) ); }

	bool eventFIFO_Empty();

private:

	void pushEvent(const DeviceEvent& event);

	static void eventPushLoopWrapper(void* object);
	bool eventPushLoop();

	void pushDeviceEventToServer(const DeviceEvent& event);

	std::queue<DeviceEvent> eventFIFO;

	STI::Pusher::DeviceEventHandler_var deviceEventHandlerRef;

	bool active;
	bool freshEvents;
	bool continuePushLoop;

	
	omni_mutex* FIFOmutex;
	omni_condition* FIFOcondition;



//	void pushMessageEvent(const STI::Pusher::TMessageEvent& event);
//	void pushDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event);
//	void pushDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event);

};

#endif
