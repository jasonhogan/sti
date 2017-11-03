/*! \file DeviceEventPusher.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceEventPusher
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

#include "DeviceEventPusher.h"
#include <iostream>
using namespace std;

DeviceEventPusher::DeviceEventPusher()
{
	active = false;
	freshEvents = false;	
	continuePushLoop = true;
	
	FIFOmutex = new omni_mutex();
	FIFOcondition = new omni_condition(FIFOmutex);

	omni_thread::create(eventPushLoopWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
}

DeviceEventPusher::DeviceEventPusher(STI::Pusher::DeviceEventHandler_ptr deviceEventHandler)
: deviceEventHandlerRef(deviceEventHandler)
{
	active = true;
	freshEvents = false;
	continuePushLoop = true;
	
	FIFOmutex = new omni_mutex();
	FIFOcondition = new omni_condition(FIFOmutex);

	omni_thread::create(eventPushLoopWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
}

DeviceEventPusher::~DeviceEventPusher()
{
	continuePushLoop = false;
	
	FIFOmutex->lock();
	{
		FIFOcondition->broadcast();	//wake up to end the thread
	}
	FIFOmutex->unlock();

}

void DeviceEventPusher::installDeviceEventHandler(STI::Pusher::DeviceEventHandler_ptr deviceEventHandler)
{
	//empty the event FIFO
	FIFOmutex->lock();
	{
		while(!eventFIFO.empty())
		{
			eventFIFO.pop();
		}
	}
	FIFOmutex->unlock();

	active = false;

	try {
		deviceEventHandlerRef = deviceEventHandler;
		deviceEventHandlerRef->ping();	//test the connection
		active = true;
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " in installDeviceEventHandler(). " << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " in installDeviceEventHandler(). " << endl;
	}
	catch(...) {
		cerr << "Caught unknown exception in installDeviceEventHandler()." << endl;
	}

}

void DeviceEventPusher::pushEvent(const DeviceEvent& event)
{
	FIFOmutex->lock();
	{
		eventFIFO.push(event);
		freshEvents = true;
		FIFOcondition->signal();
	}
	FIFOmutex->unlock();
}


void DeviceEventPusher::eventPushLoopWrapper(void* object)
{
	DeviceEventPusher* thisObject = static_cast<DeviceEventPusher*>(object);

	while( thisObject->eventPushLoop() ) {};
}


bool DeviceEventPusher::eventPushLoop()
{
	FIFOmutex->lock();
	{
		freshEvents = false;
	}
	FIFOmutex->unlock();

	while( !eventFIFO_Empty() )
	{
		FIFOmutex->lock();
		{
			if( !eventFIFO.empty() )
			{
				pushDeviceEventToServer( eventFIFO.front() );
				eventFIFO.pop();
			}
		}
		FIFOmutex->unlock();

		omni_thread::yield();
	}

	FIFOmutex->lock();
	{
		if( !freshEvents && continuePushLoop)
			FIFOcondition->wait();
	}
	FIFOmutex->unlock();

	return continuePushLoop;
}

void DeviceEventPusher::pushDeviceEventToServer(const DeviceEvent& event)
{
	if(	!active )
		return;

	try {
		switch(event.getEventType())
		{
		case DeviceEvent::Message:
			deviceEventHandlerRef->pushMessageEvent(event.getTMessageEvent());
			break;
		case DeviceEvent::DeviceRefresh:
			deviceEventHandlerRef->pushDeviceRefreshEvent(event.getTDeviceRefreshEvent());
			break;
		case DeviceEvent::DeviceData:
			deviceEventHandlerRef->pushDeviceDataEvent(event.getTDeviceDataEvent());
			break;
		default:
			break;
		}
	}	
	catch(CORBA::TRANSIENT&) {
		active = false;
	}
	catch(CORBA::SystemException&) {
		active = false;
	}
	catch(CORBA::Exception&) {
		active = false;
	}
}

bool DeviceEventPusher::eventFIFO_Empty()
{
	bool isEmpty;
	FIFOmutex->lock();
	{
		isEmpty = eventFIFO.empty();
	}
	FIFOmutex->unlock();
	return isEmpty;
}

