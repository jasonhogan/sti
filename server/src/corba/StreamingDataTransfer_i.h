/*! \file StreamingDataTransfer_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class StreamingDataTransfer_i
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

// For Clients to get channel and attribute information about available 
// Devices and directly control them

#ifndef STREAMINGDATATRANSFER_I_H
#define STREAMINGDATATRANSFER_I_H

#include "client.h"
#include "device.h"

class STI_Server;

    
class StreamingDataTransfer_i : public POA_STI::Client_Server::StreamingDataTransfer
{
public:

	StreamingDataTransfer_i(STI_Server* server);
	~StreamingDataTransfer_i();

	STI::Types::TMeasurementSeq* getStreamingData(
		const char*     deviceID, 
		::CORBA::UShort channel, 
		::CORBA::Double initial_t, 
		::CORBA::Double final_t, 
		::CORBA::Double delta_t);
	STI::Types::TMeasurementSeq* getMeasurements(const char* deviceID);
//    char* getErrMsg(const char* deviceID);

private:

	STI_Server* sti_Server;

};

#endif

