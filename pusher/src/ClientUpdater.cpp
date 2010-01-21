/*! \file ClientUpdater.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ClientUpdater
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

#include "ClientUpdater.h"

ClientUpdater::ClientUpdater() :
handlerRef(NULL), orbManager(NULL)
{
	active = false;
}

ClientUpdater::ClientUpdater(STI::Pusher::ServerEventHandler_ptr eventHandlerRef, ORBManager* orb_manager) :
handlerRef( STI::Pusher::ServerEventHandler::_duplicate(eventHandlerRef) ), orbManager(orb_manager)
{
	active = true;
	timeoutPeriod = 10 * 60; //10 minutes
	freshEvents = false;

	timeoutLoopMutex = new omni_mutex();
	timeoutLoopCondition = new omni_condition(timeoutLoopMutex);

	FIFOmutex = new omni_mutex();
	FIFOcondition = new omni_condition(FIFOmutex);

	serverCallback = new ServerCallback_i();
	orbManager->registerServant(serverCallback);

	omni_thread::create(eventPushLoopWrapper, (void*)this, omni_thread::PRIORITY_HIGH);
	omni_thread::create(timeoutLoopWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}

ClientUpdater::~ClientUpdater()
{
	active = false;

	FIFOmutex->lock();
	{
		FIFOcondition->signal();
	}
	FIFOmutex->unlock();

//	orbManager->unregisterServant(serverCallback);
	delete serverCallback;
	delete timeoutLoopCondition;
	delete FIFOcondition;
}

bool ClientUpdater::isEquivalent(STI::Pusher::ServerEventHandler_ptr eventHandlerRef)
{
	return eventHandlerRef->_is_equivalent(handlerRef);
}


bool ClientUpdater::isActive()
{
	if(serverCallback->isDisconnected())
		active = false;

	return active;
}

void ClientUpdater::timeoutLoopWrapper(void* object)
{
	ClientUpdater* thisObject = static_cast<ClientUpdater*>(object);
	while( thisObject->timeoutLoop() ) {};
}

bool ClientUpdater::timeoutLoop()
{
	unsigned long seconds, nanoseconds;
	omni_thread::get_time(&seconds, &nanoseconds, timeoutPeriod, 0);

	//setup a new ping event to send to the client
	STI::Pusher::TPingEvent_var ping( new STI::Pusher::TPingEvent );
	ping->callBack = serverCallback->_this();
	ping->sleep = false;

	//reset the ping detector for this client
	serverCallback->reset();

	//ping the client
	try {
		handlerRef->pushPingEvent(ping);
	}
	catch(CORBA::TRANSIENT&) {
		active = false;
		return false;
	}
	catch(CORBA::SystemException&) {
		active = false;
		return false;
	}

	//give the client time to respond to the ping
	timeoutLoopMutex->lock();
	{
		timeoutLoopCondition->timedwait(seconds, nanoseconds);
	}
	timeoutLoopMutex->unlock();

	//check if the client responded to the ping
	active = serverCallback->pingReceived();

	if(serverCallback->isDisconnected())
		active = false;

	return active;
}

void ClientUpdater::pushEvent(ServerEvent& event)
{
	FIFOmutex->lock();
	{
		eventFIFO.push(event);
		freshEvents = true;
		FIFOcondition->signal();
	}
	FIFOmutex->unlock();
}

void ClientUpdater::eventPushLoopWrapper(void* object)
{
	ClientUpdater* thisObject = static_cast<ClientUpdater*>(object);
	while( thisObject->eventPushLoop() ) {};
}

bool ClientUpdater::eventPushLoop()
{
	FIFOmutex->lock();
	{
		freshEvents = false;
	}
	FIFOmutex->unlock();

	while( !eventFIFO_Empty() )
	{
		pushEventToClient( eventFIFO.front() );
		
		FIFOmutex->lock();
		{
			eventFIFO.pop();
		}
		FIFOmutex->unlock();

		omni_thread::yield();
	}

	FIFOmutex->lock();
	{
		if( !freshEvents )
			FIFOcondition->wait();
	}
	FIFOmutex->unlock();

	return active;
}

void ClientUpdater::pushEventToClient(const ServerEvent& event)
{
	if(	!active )
		return;

	try {
		switch(event.getEventType())
		{
		case ServerEvent::Ping:
			handlerRef->pushPingEvent(event.getTPingEvent());
			break;
		case ServerEvent::Status:
			handlerRef->pushStatusEvent(event.getTStatusEvent());
			break;
		case ServerEvent::Message:
			handlerRef->pushMessageEvent(event.getTMessageEvent());
			break;
		case ServerEvent::Parse:
			handlerRef->pushParseEvent(event.getTParseEvent());
			break;
		case ServerEvent::File:
			handlerRef->pushFileEvent(event.getTFileEvent());
			break;
		case ServerEvent::Controller:
			handlerRef->pushControllerEvent(event.getTControllerEvent());
			break;
		case ServerEvent::DeviceRefresh:
			handlerRef->pushDeviceEvent(event.getTDeviceRefreshEvent());
			break;
		case ServerEvent::DeviceData:
			handlerRef->pushDeviceDataEvent(event.getTDeviceDataEvent());
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
}

bool ClientUpdater::eventFIFO_Empty()
{
	bool isEmpty;
	FIFOmutex->lock();
	{
		isEmpty = eventFIFO.empty();
	}
	FIFOmutex->unlock();
	return isEmpty;
}
