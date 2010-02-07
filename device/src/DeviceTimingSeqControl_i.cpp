/*! \file DeviceTimingSeqControl_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DeviceTimingSeqControl_i
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

// Was DeviceControl_i -- 2/5/2010

#include "device.h"
#include "DeviceTimingSeqControl_i.h"
#include <STI_Device.h>


DeviceTimingSeqControl_i::DeviceTimingSeqControl_i(STI_Device* device) : sti_device(device)
{
	events_parsed = false;
	events_loaded = false;
}

DeviceTimingSeqControl_i::~DeviceTimingSeqControl_i()
{
}


//STI::Types::TStatus DeviceControl_i::status()
//{
//	STI::Types::TStatus dummy;
//	dummy.curTime = 0;
//	return dummy;
//
//}

void DeviceTimingSeqControl_i::reset()
{
	sti_device->resetEvents();
}

void DeviceTimingSeqControl_i::load()
{
	sti_device->loadEvents();
}
::CORBA::Boolean DeviceTimingSeqControl_i::prepareToPlay()
{
	return sti_device->prepareToPlay();
}

void DeviceTimingSeqControl_i::play()
{
	sti_device->playEvents();
}


void DeviceTimingSeqControl_i::pause()
{
	sti_device->pause();
}


void DeviceTimingSeqControl_i::stop()
{
	sti_device->stop();
}
//char* DeviceControl_i::controlMsg()
//{
//	CORBA::String_var message( "" );
//	return message._retn();
//}

char* DeviceTimingSeqControl_i::transferErr()
{
	CORBA::String_var error( sti_device->eventTransferErr().c_str() );
	return error._retn();
}

::CORBA::Boolean DeviceTimingSeqControl_i::ping()
{
	return true;
}


::CORBA::Boolean DeviceTimingSeqControl_i::transferEvents(
		const STI::Types::TDeviceEventSeq& events,
		::CORBA::Boolean dryrun)
{
	events_parsed = sti_device->transferEvents(events);
	events_loaded = false;

	return events_parsed;
}

::CORBA::Boolean DeviceTimingSeqControl_i::eventsParsed()
{
	return events_parsed;
}

::CORBA::Boolean DeviceTimingSeqControl_i::eventsLoaded()
{
	return sti_device->eventsLoaded();
}

::CORBA::Boolean DeviceTimingSeqControl_i::eventsPlayed()
{
	return sti_device->eventsPlayed();
}

