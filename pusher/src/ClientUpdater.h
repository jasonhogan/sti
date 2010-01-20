/*! \file ClientUpdater.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ClientUpdater
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

#ifndef CLIENTUPDATER_H
#define CLIENTUPDATER_H

#include "pusher.h"
#include <ORBManager.h>
#include "ServerCallback_i.h"
#include "ServerEvent.h"

#include <vector>
#include <queue>

class ClientUpdater
{
public:

	ClientUpdater();
	ClientUpdater(STI::Pusher::ServerEventHandler_ptr eventHandlerRef, ORBManager* orb_manager);
	~ClientUpdater();

	bool isActive();

	void pushEvent(ServerEvent& event);

private:

	static void timeoutLoopWrapper(void* object);
	bool timeoutLoop();
	static void eventPushLoopWrapper(void* object);
	bool eventPushLoop();

	bool eventFIFO_Empty();
	void pushEventToClient(const ServerEvent& event);

	STI::Pusher::ServerEventHandler_var handlerRef;	//reference to the client's event handler
	ORBManager* orbManager;
	ServerCallback_i* serverCallback;

	std::queue<ServerEvent> eventFIFO;

	bool active;
	unsigned long timeoutPeriod;
	bool freshEvents;

	omni_mutex* FIFOmutex;
	omni_condition* FIFOcondition;

	omni_mutex* timeoutLoopMutex;
	omni_condition* timeoutLoopCondition;

};

#endif