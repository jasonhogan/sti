/*! \file DeviceControl_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceControl_i
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

#ifndef DEVICECONTROL_I_H
#define DEVICECONTROL_I_H

#include "device.h"
#include <sstream>

class STI_Device;

class DeviceControl_i : public POA_STI_Server_Device::DeviceControl
{
public:

	DeviceControl_i(STI_Device* device);
	~DeviceControl_i();

	STI_Server_Device::TStatus status();
	void reset();
	void load();
	void play();
	void pause();
	void stop();
	char* controlMsg();
	char* transferErr();
	::CORBA::Boolean transferEvents(
		const STI_Server_Device::TDeviceEventSeq &events,
		::CORBA::Boolean dryrun);
	::CORBA::Boolean eventsParsed();
    ::CORBA::Boolean eventsLoaded();
	::CORBA::Boolean eventsPlayed();

	::CORBA::Boolean ping();

private:

	STI_Device* sti_device;

	bool events_parsed;
	bool events_loaded;
};

#endif
