/*! \file ServerEventPusher_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ServerEventPusher_i
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

#include "ServerEventPusher_i.h"

ServerEventPusher_i::ServerEventPusher_i(ORBManager* orb_manager) : orbManager(orb_manager)
{
	updatersMutex = new omni_mutex();
}

ServerEventPusher_i::~ServerEventPusher_i()
{
}

void ServerEventPusher_i::addNewClient(STI::Pusher::ServerEventHandler_ptr handler, 
									   const STI::Pusher::TStatusEvent& initialState)
{
	//First remove all inactive updaters.
	//Check if a reference to this handler already exists; 
	//if it does, delete the old and use the new.

	updatersMutex->lock();
	{
		ClientUpdaterVector::iterator current = clientUpdaters.begin();
		ClientUpdaterVector::iterator next = current;

		while(current != clientUpdaters.end())
		{
			next++;

			if( !current->isActive() || current->isEquivalent(handler) )
			{
				current = clientUpdaters.erase(current);
				next = current;
			}
			else
			{
				current = next;
			}
		}
		
		clientUpdaters.push_back( new ClientUpdater(handler, ServerEvent(initialState), orbManager) );
	}
	updatersMutex->unlock();
}

void ServerEventPusher_i::pushEvent(const ServerEvent& event)
{
	updatersMutex->lock();
	{
		ClientUpdaterVector::iterator current = clientUpdaters.begin();
		ClientUpdaterVector::iterator next = current;

		while(current != clientUpdaters.end())
		{
			next++;

			if(current->isActive())
			{
				current->pushEvent(event);
				current = next;
			}
			else
			{
				current = clientUpdaters.erase(current);
				next = current;
			}

		}
	}
	updatersMutex->unlock();
}

void ServerEventPusher_i::pushPingEvent(const STI::Pusher::TPingEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushStatusEvent(const STI::Pusher::TStatusEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushSequenceEvent(const STI::Pusher::TSequenceEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushMessageEvent(const STI::Pusher::TMessageEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushParseEvent(const STI::Pusher::TParseEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushFileEvent(const STI::Pusher::TFileEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushControllerEvent(const STI::Pusher::TControllerEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event)
{
	pushEvent(ServerEvent(event));
}
void ServerEventPusher_i::pushDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event)
{
	pushEvent(ServerEvent(event));
}

