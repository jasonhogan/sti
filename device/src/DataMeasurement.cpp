/*! \file DataMeasurement.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DataMeasurement
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

#include <DataMeasurement.h>
#include <sstream>
#include <iostream>

//DataMeasurement::DataMeasurement() : eventNumber_l(0)
//{
//	scheduled = false;
//}
//DataMeasurement& DataMeasurement::operator= (const DataMeasurement& other)
//{
//	eventNumber_l = other.eventNum();
//	return (*this);
//}
//
//DataMeasurement::DataMeasurement(const DataMeasurement& copy) : 
//eventNumber_l(copy.eventNum())
//{
//}

DataMeasurement::DataMeasurement(double time, unsigned short channel, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	setTime(time);
	measurement_l.channel = channel;

	measured = false;
	scheduled = false;
}


DataMeasurement::DataMeasurement(
		const STI::Types::TMeasurement &measurement, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	measurement_l.time = measurement.time;
	measurement_l.channel = measurement.channel;
	measurement_l.data = measurement.data;

	measured = false;
	scheduled = false;
}

DataMeasurement::~DataMeasurement()
{
}

std::string DataMeasurement::print() const
{
	std::stringstream meas;

	//<Time=2.1, Channel=4, Type=Number, Value=3.4>
	meas << "<Time=" << time();
	meas << ", Channel=" << channel();
	meas << ", Type=";
	
	switch(dataType())
	{
	case DataDouble:
		meas << TDataToStr(DataDouble) << ", Data=" << numberValue();
		break;
	case DataLong:
		meas << TDataToStr(DataLong) << ", Data=" << numberValue();
		break;
	case DataString:
		meas << TDataToStr(DataString) << ", Data=" << stringValue();
		break;
	case DataPicture:
		meas << TDataToStr(DataPicture) << ", Data={"
			<< pictureValue().rowLength << " x " 
			<< ( pictureValue().rowLength == 0 ? 0 : 
			static_cast<unsigned short>(pictureValue().pixels.length() 
			/ pictureValue().rowLength) ) << "}";
		break;
	case DataNone:
		meas << TDataToStr(DataNone) << ", Data=None";
		break;
	default:
		meas << "Invalid" << ", Value=Invalid";
		break;
	}

	return meas.str();
}

std::string DataMeasurement::TDataToStr(STI::Types::TData tData)
{
	switch(tData)
	{
	case DataDouble:
		return "Double";
	case DataLong:
		return "Integer";
	case DataString:
		return "String";
	case DataPicture:
		return "Picture";
	case DataNone:
		return "None";
	default:
		return "Invalid";
	}
}

double DataMeasurement::time() const
{
	return measurement_l.time;
}
unsigned short DataMeasurement::channel() const
{
	return measurement_l.channel;
}
STI::Types::TData DataMeasurement::dataType() const
{
	return measurement_l.data._d();
}

double DataMeasurement::numberValue() const
{
	if(dataType() == DataDouble)
		return measurement_l.data.doubleVal();
	else
		return 0;
}

std::string DataMeasurement::stringValue() const
{
	if(dataType() == DataString)
		return measurement_l.data.stringVal();
	else
		return "";
}

STI::Types::TPicture DataMeasurement::pictureValue() const
{
	if(dataType() == DataPicture)
		return measurement_l.data.picture();
	else
	{
		STI::Types::TPicture dummy;
		dummy.rowLength = 0;
		dummy.pixels.length(0);
		
		return dummy;
	}
}

const STI::Types::TDataMixed& DataMeasurement::data() const
{
	return measurement_l.data;
}

bool DataMeasurement::operator==(const DataMeasurement &other) const
{
	bool equal = false;

	if(
		time() == other.time() && 
		channel() == other.channel() && 
		dataType() == other.dataType())
	{
		switch(dataType())
		{
		case DataDouble:
			return numberValue() == other.numberValue();
		case DataLong:
			return numberValue() == other.numberValue();
		case DataString:
			return stringValue() == other.stringValue();
		case DataPicture:
			equal = pictureValue().pixels.length() == other.pictureValue().pixels.length();		
			for(unsigned i = 0; (i < pictureValue().pixels.length()	&& equal); i++)
			{
				equal &= (pictureValue().pixels[i] == other.pictureValue().pixels[i]);
			}
			return equal;
		case DataNone:
			return true;
		default:
			return true;
		}
	}
	return false;
}

bool DataMeasurement::operator!=(const DataMeasurement &other) const
{
	return !( (*this)==other );
}

unsigned DataMeasurement::eventNum() const
{
	return eventNumber_l;
}

void DataMeasurement::setTime(double time)
{
	measurement_l.time = time;
}

void DataMeasurement::setData(double data)
{
	std::cerr << "DataMeasurement::setData(" << data << ")" << std::endl;
	measurement_l.data.doubleVal(data);
	measured = true;
}

void DataMeasurement::setData(std::string data)
{
	measurement_l.data.stringVal( CORBA::string_dup(data.c_str()) );
	measured = true;
}

void DataMeasurement::setData(STI::Types::TPicture data)
{
	measurement_l.data.picture( data );
	measured = true;
}

void DataMeasurement::setScheduleStatus(bool enabled)
{
	scheduled = enabled;
}

bool DataMeasurement::isScheduled() const
{
	return scheduled;
}

bool DataMeasurement::isMeasured() const
{
	return measured;
}

