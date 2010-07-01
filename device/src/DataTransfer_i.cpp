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

typedef std::vector<STI::Types::TMeasurement> measurementVec;

DataTransfer_i::DataTransfer_i(STI_Device* device) : sti_Device(device)
{	
}

DataTransfer_i::~DataTransfer_i()
{
}

::CORBA::Boolean DataTransfer_i::ping()
{
	return true;
}

//char* DataTransfer_i::errMsg()
//{
//	CORBA::String_var error( sti_Device->dataTransferErrorMsg().c_str() );
//	return error._retn();
//}

STI::Types::TLabeledData* DataTransfer_i::getData(const char* dataLabel)
{
	using STI::Types::TLabeledData;
	using STI::Types::TLabeledData_var;

	TLabeledData_var labeledData( new TLabeledData() );

	labeledData->label = CORBA::string_dup( dataLabel ) ;
//	labeledData->data.stringVal(

	return labeledData._retn();
}

STI::Types::TMeasurementSeq* DataTransfer_i::getRecentMeasurements(::CORBA::Long startingIndex)
{
	using STI::Types::TMeasurement;
	using STI::Types::TMeasurementSeq;
	using STI::Types::TMeasurementSeq_var;

	DataMeasurementVector& measurements = sti_Device->getMeasurements();

	unsigned i,j;
	unsigned numReadyMeasurements = 0;
	unsigned start = static_cast<unsigned>(startingIndex);

	for(i = start; i < measurements.size(); i++)
	{
		if(!measurements[i].isMeasured())
			break;

		numReadyMeasurements++;
	}
	
	TMeasurementSeq_var measurementSeq( new TMeasurementSeq() );
	measurementSeq->length( numReadyMeasurements );

	for(i = start, j = 0; 
		i < (start + numReadyMeasurements) && 
		j < measurementSeq->length() && 
		i < measurements.size(); i++, j++)
	{
		if(!measurements[i].isMeasured())
			break;

		measurementSeq[j].channel     = measurements[i].channel();
		measurementSeq[j].time        = measurements[i].time();
		measurementSeq[j].data        = measurements[i].data();
		measurementSeq[j].description = CORBA::string_dup(measurements[i].getDescription().c_str());
	}

	return measurementSeq._retn();

}

STI::Types::TMeasurementSeq* DataTransfer_i::measurements()
{
	using STI::Types::TMeasurement;
	using STI::Types::TMeasurementSeq;
	using STI::Types::TMeasurementSeq_var;

	DataMeasurementVector& measurements = sti_Device->getMeasurements();
	TMeasurementSeq_var measurementSeq( new TMeasurementSeq( measurements.size() ) );

	for(unsigned i = 0; i < measurements.size(); i++)
	{
		measurementSeq[i].channel     = measurements[i].channel();
		measurementSeq[i].time        = measurements[i].time();
		measurementSeq[i].data        = measurements[i].data();
		measurementSeq[i].description = CORBA::string_dup(measurements[i].getDescription().c_str());
	}

	return measurementSeq._retn();


	//const DataMeasurementMap& measurements = sti_Device->getMeasurements();
	//TMeasurementSeqSeq_var measurementSeqSeq( new TMeasurementSeqSeq(measurements.size()) );

	//for(iter = measurements.begin(), i = 0; iter != measurements.end(); iter++, i++)
	//{
	//	measurementSeqSeq[i] = TMeasurementSeq( (iter->second).size() );
	//	for(j = 0; j < (iter->second).size(); j++)
	//	{
	//		measurementSeqSeq[i][j].channel = (iter->second)[j].channel();
	//		measurementSeqSeq[i][j].time    = (iter->second)[j].time();
	//		measurementSeqSeq[i][j].data    = (iter->second)[j].data();
	//	}
	//}
	//return measurementSeqSeq._retn();
}

STI::Types::TMeasurementSeq* DataTransfer_i::getStreamingData(
                                                    ::CORBA::UShort channel,
													::CORBA::Double initial_t, 
													::CORBA::Double final_t, 
													::CORBA::Double delta_t)
{
//	sti_Device->streamingBuffers[channel].getData(initial_t, final_t, delta_t);
	STI::Types::TMeasurementSeq* dummy = 0;
	return dummy;
}
