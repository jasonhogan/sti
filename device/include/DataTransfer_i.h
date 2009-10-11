/*! \file DataTransfer_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DataTransfer_i
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

#ifndef DATATRANSFER_I_H
#define DATATRANSFER_I_H

#include "device.h"

class STI_Device;

class DataTransfer_i : public POA_STI::Server_Device::DataTransfer
{
public:

	DataTransfer_i(STI_Device* device);
	~DataTransfer_i();

    STI::Types::TMeasurementSeq* getStreamingData(
        ::CORBA::UShort channel,
		::CORBA::Double initial_t, 
		::CORBA::Double final_t, 
		::CORBA::Double delta_t);
	char* errMsg();
	STI::Types::TMeasurementSeq* measurements();
	STI::Types::TLabeledData* getData(const char* dataLabel);

private:

	STI_Device* sti_Device;

};

#endif
