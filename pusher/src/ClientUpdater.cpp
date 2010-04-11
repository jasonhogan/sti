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

//ClientUpdater::ClientUpdater() :
//handlerRef(NULL), orbManager(NULL)
//{
//	active = false;
//}

ClientUpdater::ClientUpdater(STI::Pusher::ServerEventHandler_ptr eventHandlerRef, 
							 const ServerEvent& initialState, ORBManager* orb_manager) :
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

	pushLoopRunning = false;
	timeoutLoopRunning = false;

	pushEvent(initialState);

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

	timeoutLoopMutex->lock();
	{
		timeoutLoopCondition->signal();
	}
	timeoutLoopMutex->unlock();

	//The timeoutLoopCondition takes a long time to wake up from its timedwait
	//since it is in a LOW_PRIORITY thread (the timeout thread).
	//The FIFOmutex should be free since the event thread is HIGH_PRIORITY.
	//Use the FIFOcondition to sleep the destructor while waiting for the 
	//timeoutLoopCondition to wake up.
	
	unsigned long seconds, nanoseconds;

	while(pushLoopRunning || timeoutLoopRunning) 
	{
		omni_thread::get_time(&seconds, &nanoseconds, 1, 0);	//1 second
		
		FIFOmutex->lock();
		{
			FIFOcondition->timedwait(seconds, nanoseconds);
		}
		FIFOmutex->unlock();

		omni_thread::yield();
	}

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

	thisObject->timeoutLoopRunning = true;
	while( thisObject->timeoutLoop() ) {};
	thisObject->timeoutLoopRunning = false;
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

	if( !active )
		return false;

	//give the client time to respond to the ping
	timeoutLoopMutex->lock();
	{
		timeoutLoopCondition->timedwait(seconds, nanoseconds);
	}
	timeoutLoopMutex->unlock();

	if( !active )
		return false;

	//check if the client responded to the ping
	active = serverCallback->pingReceived();

	if(serverCallback->isDisconnected())
		active = false;

	return active;
}

void ClientUpdater::pushEvent(const ServerEvent& event)
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

	thisObject->pushLoopRunning = true;
	while( thisObject->eventPushLoop() ) {};
	thisObject->pushLoopRunning = false;
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
		FIFOmutex->lock();
		{
			pushEventToClient( eventFIFO.front() );
			eventFIFO.pop();
		}
		FIFOmutex->unlock();

		omni_thread::yield();
	}

	FIFOmutex->lock();
	{
		if( !freshEvents && active)
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
			handlerRef->pushDeviceRefreshEvent(event.getTDeviceRefreshEvent());
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
	catch(CORBA::Exception&) {
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
