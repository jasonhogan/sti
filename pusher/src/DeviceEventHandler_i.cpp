/*! \file DeviceEventHandler_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceEventHandler_i
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


#include "DeviceEventHandler_i.h"
#include <STI_Server.h>

DeviceEventHandler_i::DeviceEventHandler_i(ServerEventPusher_i* pusher, STI_Server* server) :
eventPusher(pusher), sti_server(server)
{
	FIFOmutex = new omni_mutex();
	FIFOcondition = new omni_condition(FIFOmutex);

	handlerMutex = new omni_mutex();
	handlerCondition = new omni_condition(handlerMutex);
	
	readyToHandleNextEvent = true;
	continueHandlerLoop = true;
	freshEvents = false;

	omni_thread::create(eventHandlerLoopWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
	omni_thread::create(handleSingleEventLoopWrapper, (void*)this, omni_thread::PRIORITY_HIGH);

}

DeviceEventHandler_i::~DeviceEventHandler_i()
{
	continueHandlerLoop = false;

	FIFOmutex->lock();
	{
		FIFOcondition->broadcast();	//wake up to end the thread
	}
	FIFOmutex->unlock();

	handlerMutex->lock();
	{
		handlerCondition->broadcast();	//wake up to end the thread
	}
	handlerMutex->unlock();
}

void DeviceEventHandler_i::pushMessageEvent(const STI::Pusher::TMessageEvent& event)
{
	handleEvent( DeviceEvent(event) );
}

void DeviceEventHandler_i::pushDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event)
{
	handleEvent( DeviceEvent(event) );
}

void DeviceEventHandler_i::pushDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event)
{
	handleEvent( DeviceEvent(event) );
}



void DeviceEventHandler_i::handleEvent(const DeviceEvent& event)
{
	FIFOmutex->lock();
	{
		eventFIFO.push(event);
		freshEvents = true;
		FIFOcondition->signal();
	}
	FIFOmutex->unlock();
}

bool DeviceEventHandler_i::eventFIFO_Empty()
{
	bool isEmpty;
	FIFOmutex->lock();
	{
		isEmpty = eventFIFO.empty();
	}
	FIFOmutex->unlock();
	return isEmpty;
}


void DeviceEventHandler_i::eventHandlerLoopWrapper(void* object)
{
	DeviceEventHandler_i* thisObject = static_cast<DeviceEventHandler_i*>(object);

	while( thisObject->eventHandlerLoop() ) {};
}


bool DeviceEventHandler_i::eventHandlerLoop()
{
	FIFOmutex->lock();
	{
		freshEvents = false;
	}
	FIFOmutex->unlock();

	while( !eventFIFO_Empty() && readyToHandleNextEvent )
	{
		if(readyToHandleNextEvent)
		{
			//handleDeviceEvent() has returned in handleSingleEventLoop()

			FIFOmutex->lock();
			{
				if( !eventFIFO.empty() )
				{
					eventToHandle = eventFIFO.front();
					eventFIFO.pop();
				}
			}
			FIFOmutex->unlock();

			handlerMutex->lock();
			{
				handlerCondition->signal();			//wake up handleSingleEventLoop()
				readyToHandleNextEvent = false;		//will not be true again until handleSingleEventLoop() is waiting
			}
			handlerMutex->unlock();
		}

		omni_thread::yield();
	}

	FIFOmutex->lock();
	{
		if( (!freshEvents || !readyToHandleNextEvent) && continueHandlerLoop )
			FIFOcondition->wait();
	}
	FIFOmutex->unlock();

	return continueHandlerLoop;
}
void DeviceEventHandler_i::handleSingleEventLoopWrapper(void* object)
{
	DeviceEventHandler_i* thisObject = static_cast<DeviceEventHandler_i*>(object);

	thisObject->handleSingleEventLoop();
}

void DeviceEventHandler_i::handleSingleEventLoop()
{
	while(continueHandlerLoop)
	{
		handlerMutex->lock();
		{
			readyToHandleNextEvent = true;

			//Wake up the event handler loop
			FIFOmutex->lock();
			{
				FIFOcondition->signal();
			}
			FIFOmutex->unlock();

			//go to sleep until another event needs handling
			handlerCondition->wait();
		}
		handlerMutex->unlock();
		
		handleDeviceEvent(eventToHandle);
	}
}

void DeviceEventHandler_i::handleDeviceEvent(const DeviceEvent& event)
{
	switch(event.getEventType())
	{
	case DeviceEvent::Message:
		handleMessageEvent(event.getTMessageEvent());
		break;
	case DeviceEvent::DeviceRefresh:
		handleDeviceRefreshEvent(event.getTDeviceRefreshEvent());
		break;
	case DeviceEvent::DeviceData:
		handleDeviceDataEvent(event.getTDeviceDataEvent());
		break;
	default:
		break;
	}
}


void DeviceEventHandler_i::handleMessageEvent(const STI::Pusher::TMessageEvent& event)
{
	eventPusher->pushMessageEvent(event);
}

void DeviceEventHandler_i::handleDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event)
{
	sti_server->handleDeviceRefreshEvent(event);
	eventPusher->pushDeviceRefreshEvent(event);
}

void DeviceEventHandler_i::handleDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event)
{
	eventPusher->pushDeviceDataEvent(event);
}


