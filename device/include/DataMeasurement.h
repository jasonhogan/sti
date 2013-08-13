/*! \file DataMeasurement.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DataMeasurement
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

//was "ParsedMeasurement.h" 2/9/2010

#ifndef DATAMEASUREMENT_H
#define DATAMEASUREMENT_H

#include <string>

#include <MixedData.h>
#include "device.h"

//TData
using STI::Types::DataDouble;
using STI::Types::DataLong;
using STI::Types::DataString;
using STI::Types::DataPicture;
using STI::Types::DataNone;


class DataMeasurement
{
public:

	DataMeasurement(double time, unsigned short channel, unsigned eventNumber);
	DataMeasurement(const STI::Types::TMeasurement& measurement, unsigned eventNumber);

	~DataMeasurement();

	//DataMeasurement(const DataMeasurement& copy);
	//DataMeasurement& operator= (const DataMeasurement& other);

	std::string print() const;

	double time() const;
//	void setTime(double time);

	unsigned short channel() const;
//	STI::Types::TData dataType() const;

	const MixedData& getMixedData() const;
	const STI::Types::TDataMixed data() const;

	//A measurement is scheduled by adding it to a SynchronousEvent
	void setScheduleStatus(bool enabled);
	bool isScheduled() const;
	bool isMeasured() const;

	void installMeasurementCallback(STI::Types::TMeasurementCallback_ptr callback);
	void sendMeasurementCallback(const STI::Types::TMeasurement_var& tMeas);

	template<class T> void setData(T data) 
	{
		measured = true;
		data_l.setValue(data);
		if(useCallback) {
			using STI::Types::TMeasurement;
			using STI::Types::TMeasurement_var;

			TMeasurement_var tMeas( new TMeasurement() );
			tMeas->time = time();
			tMeas->channel = channel();
			tMeas->data = this->data();
			tMeas->description = CORBA::string_dup(getDescription().c_str());

//			measurementCallback->returnResult(tMeas._retn());
//			measurementCallback->returnResult(tMeas);
			sendMeasurementCallback(tMeas);
		}
	}

	void clearData();

	unsigned eventNum() const { return eventNumber_l; }

	bool operator==(const DataMeasurement &other) const;
	bool operator!=(const DataMeasurement &other) const;

	void setDescription(std::string desc);
	std::string getDescription() const;

private:

	bool useCallback;
	STI::Types::TMeasurementCallback_var measurementCallback;

	double time_l;
	unsigned short channel_l;
	MixedData data_l;

	unsigned eventNumber_l;
	bool scheduled;
	bool measured;
	std::string description;

};

#endif
