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

DeviceEventHandler_i::DeviceEventHandler_i(ServerEventPusher_i* pusher) : eventPusher(pusher)
{
}

DeviceEventHandler_i::~DeviceEventHandler_i()
{
}

void DeviceEventHandler_i::pushMessageEvent(const STI::Pusher::TMessageEvent& event)
{
	eventPusher->pushMessageEvent(event);
}

void DeviceEventHandler_i::pushDeviceRefreshEvent(const STI::Pusher::TDeviceRefreshEvent& event)
{
	eventPusher->pushDeviceRefreshEvent(event);
}

void DeviceEventHandler_i::pushDeviceDataEvent(const STI::Pusher::TDeviceDataEvent& event)
{
	eventPusher->pushDeviceDataEvent(event);
}

