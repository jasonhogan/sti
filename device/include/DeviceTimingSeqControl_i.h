/*! \file DeviceTimingSeqControl_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DeviceTimingSeqControl_i
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

#ifndef DEVICETIMINGSEQCONTROL_I_H
#define DEVICETIMINGSEQCONTROL_I_H

#include "device.h"
#include <sstream>

class STI_Device;

class DeviceTimingSeqControl_i : public POA_STI::Server_Device::DeviceTimingSeqControl
{
public:

	DeviceTimingSeqControl_i(STI_Device* device);
	~DeviceTimingSeqControl_i();

	void reset();
	void load();
	::CORBA::Boolean prepareToPlay();
	void play();
	void pause();
	void stop();

	void waitForStatus(STI::Types::DeviceStatus status);

	char* transferErr();
	::CORBA::Boolean transferEvents(
		const STI::Types::TDeviceEventSeq &events,
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
