/*! \file DeviceEventHandler_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceEventHandler_i
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

#ifndef DEVICEEVENTHANDLER_I_H
#define DEVICEEVENTHANDLER_I_H

#include "pusher.h"

#include "ServerEventPusher_i.h"

class DeviceEventHandler_i : public POA_STI::Pusher::DeviceEventHandler
{
public:
	DeviceEventHandler_i(ServerEventPusher_i* pusher);
	~DeviceEventHandler_i();

	void pushMessageEvent(const STI::Pusher::TMessageEvent& event);
	void pushDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event);
	void pushDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event);

private:

	ServerEventPusher_i* eventPusher;

};

#endif

