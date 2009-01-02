/*! \file DataTransfer_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DataTransfer_i
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
#include "DataTransfer_i.h"
#include "STI_Device.h"

#include <vector>

typedef std::vector<STI_Server_Device::TMeasurement> measurementVec;

DataTransfer_i::DataTransfer_i(STI_Device* device) : sti_Device(device)
{	
}

DataTransfer_i::~DataTransfer_i()
{
}


char* DataTransfer_i::errMsg()
{
	CORBA::String_var error( sti_Device->dataTransferErrorMsg().c_str() );
	return error._retn();
}


STI_Server_Device::TMeasurementSeqSeq* DataTransfer_i::measurements()
{
	using STI_Server_Device::TMeasurement;
	using STI_Server_Device::TMeasurementSeq;
	using STI_Server_Device::TMeasurementSeqSeq;
	using STI_Server_Device::TMeasurementSeqSeq_var;


	ParsedMeasurementMap::const_iterator iter;
	unsigned i,j;

	const ParsedMeasurementMap& measurements = sti_Device->getMeasurements();
	TMeasurementSeqSeq_var measurementSeqSeq( new TMeasurementSeqSeq(measurements.size()) );

	for(iter = measurements.begin(), i = 0; iter != measurements.end(); iter++, i++)
	{
		measurementSeqSeq[i] = TMeasurementSeq( (iter->second).size() );
		for(j = 0; j < (iter->second).size(); j++)
		{
			measurementSeqSeq[i][j].channel = (iter->second)[j].channel();
			measurementSeqSeq[i][j].time    = (iter->second)[j].time();
			measurementSeqSeq[i][j].data    = (iter->second)[j].data();
		}
	}
	return measurementSeqSeq._retn();
}

STI_Server_Device::TMeasurementSeq* DataTransfer_i::getStreamingData(
                                                    ::CORBA::UShort channel,
													::CORBA::Double initial_t, 
													::CORBA::Double final_t, 
													::CORBA::Double delta_t)
{
//	sti_Device->streamingBuffers[channel].getData(initial_t, final_t, delta_t);
	STI_Server_Device::TMeasurementSeq* dummy = 0;
	return dummy;
}
