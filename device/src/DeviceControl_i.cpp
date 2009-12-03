/*! \file DeviceControl_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceControl_i
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#include "device.h"
#include "DeviceControl_i.h"
#include <STI_Device.h>


DeviceControl_i::DeviceControl_i(STI_Device* device) : sti_device(device)
{
	events_parsed = false;
	events_loaded = false;
}

DeviceControl_i::~DeviceControl_i()
{
}

STI::Types::TStatus DeviceControl_i::status()
{
	STI::Types::TStatus dummy;
	dummy.curTime = 0;
	return dummy;

}

void DeviceControl_i::reset()
{
	sti_device->resetEvents();
}

void DeviceControl_i::load()
{
	sti_device->loadEvents();
}
::CORBA::Boolean DeviceControl_i::prepareToPlay()
{
	return sti_device->prepareToPlay();
}

void DeviceControl_i::play()
{
	sti_device->playEvents();
}


void DeviceControl_i::pause()
{
	sti_device->pause();
}


void DeviceControl_i::stop()
{
	sti_device->stop();
}
char* DeviceControl_i::controlMsg()
{
	CORBA::String_var message( "" );
	return message._retn();
}

char* DeviceControl_i::transferErr()
{
	CORBA::String_var error( sti_device->eventTransferErr().c_str() );
	return error._retn();
}

::CORBA::Boolean DeviceControl_i::ping()
{
	return true;
}


::CORBA::Boolean DeviceControl_i::transferEvents(
		const STI::Types::TDeviceEventSeq& events,
		::CORBA::Boolean dryrun)
{
	events_parsed = sti_device->transferEvents(events);
	events_loaded = false;

	return events_parsed;
}

::CORBA::Boolean DeviceControl_i::eventsParsed()
{
	return events_parsed;
}

::CORBA::Boolean DeviceControl_i::eventsLoaded()
{
	return sti_device->eventsLoaded();
}

::CORBA::Boolean DeviceControl_i::eventsPlayed()
{
	return sti_device->eventsPlayed();
}

