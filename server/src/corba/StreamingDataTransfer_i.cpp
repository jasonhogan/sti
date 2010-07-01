/*! \file StreamingDataTransfer_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class StreamingDataTransfer_i
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

#include "StreamingDataTransfer_i.h"
#include "STI_Server.h"

StreamingDataTransfer_i::StreamingDataTransfer_i(STI_Server* server) : sti_Server(server)
{
}

StreamingDataTransfer_i::~StreamingDataTransfer_i()
{
}

STI::Types::TMeasurementSeq* StreamingDataTransfer_i::getStreamingData(
		const char*     deviceID, 
		::CORBA::UShort channel, 
		::CORBA::Double initial_t, 
		::CORBA::Double final_t, 
		::CORBA::Double delta_t)
{

	if(sti_Server->getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
		return sti_Server->getRegisteredDevices().find(deviceID)->second->
			getStreamingData(channel, initial_t, final_t, delta_t);
	}

	STI::Types::TMeasurementSeq_var empty( new STI::Types::TMeasurementSeq(0) );
	return empty._retn();
}

STI::Types::TMeasurementSeq* StreamingDataTransfer_i::getMeasurements(const char* deviceID)
{
	if(sti_Server->getDeviceStatus(deviceID))
	{
		// deviceID found and Device is alive
//		return sti_Server->registeredDevices[deviceID].measurements();
	}

	STI::Types::TMeasurementSeq_var empty( new STI::Types::TMeasurementSeq(0) );
	return empty._retn();
}



