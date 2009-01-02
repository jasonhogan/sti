/*! \file ParsedMeasurement.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ParsedMeasurement
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

#include <ParsedMeasurement.h>
#include <sstream>

ParsedMeasurement::ParsedMeasurement() : eventNumber_l(0)
{
}
ParsedMeasurement& ParsedMeasurement::operator= (const ParsedMeasurement& other)
{
	eventNumber_l = other.eventNum();
	return (*this);
}

ParsedMeasurement::ParsedMeasurement(const ParsedMeasurement& copy) : 
eventNumber_l(copy.eventNum())
{
}

ParsedMeasurement::ParsedMeasurement(
		const STI_Server_Device::TMeasurement &measurement, unsigned eventNumber) :
eventNumber_l(eventNumber)
{
	measurement_l.time = measurement.time;
	measurement_l.channel = measurement.channel;
	measurement_l.data = measurement.data;
}

ParsedMeasurement::~ParsedMeasurement()
{
}

std::string ParsedMeasurement::print() const
{
	std::stringstream meas;

	//<Time=2.1, Channel=4, Type=Number, Value=3.4>
	meas << "<Time=" << time();
	meas << ", Channel=" << channel();
	meas << ", Type=";
	
	switch(dataType())
	{
	case DataNumber:
		meas << TDataToStr(DataNumber) << ", Data=" << numberValue();
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

std::string ParsedMeasurement::TDataToStr(STI_Server_Device::TData tData)
{
	switch(tData)
	{
	case DataNumber:
		return "Number";
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

double ParsedMeasurement::time() const
{
	return measurement_l.time;
}
unsigned short ParsedMeasurement::channel() const
{
	return measurement_l.channel;
}
STI_Server_Device::TData ParsedMeasurement::dataType() const
{
	return measurement_l.data._d();
}

double ParsedMeasurement::numberValue() const
{
	if(dataType() == DataNumber)
		return measurement_l.data.number();
	else
		return 0;
}

std::string ParsedMeasurement::stringValue() const
{
	if(dataType() == DataString)
		return measurement_l.data.stringVal();
	else
		return "";
}

STI_Server_Device::TPicture ParsedMeasurement::pictureValue() const
{
	if(dataType() == DataPicture)
		return measurement_l.data.picture();
	else
	{
		STI_Server_Device::TPicture dummy;
		dummy.rowLength = 0;
		dummy.pixels.length(0);
		
		return dummy;
	}
}

const STI_Server_Device::TDataMixed& ParsedMeasurement::data() const
{
	return measurement_l.data;
}

bool ParsedMeasurement::operator==(const ParsedMeasurement &other) const
{
	bool equal = false;

	if(
		time() == other.time() && 
		channel() == other.channel() && 
		dataType() == other.dataType())
	{
		switch(dataType())
		{
		case DataNumber:
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

bool ParsedMeasurement::operator!=(const ParsedMeasurement &other) const
{
	return !( (*this)==other );
}

unsigned ParsedMeasurement::eventNum() const
{
	return eventNumber_l;
}

void ParsedMeasurement::setTime(double time)
{
	measurement_l.time = time;
}

void ParsedMeasurement::setData(double data)
{
	measurement_l.data.number(data);
}

void ParsedMeasurement::setData(std::string data)
{
	measurement_l.data.stringVal( CORBA::string_dup(data.c_str()) );
}

void ParsedMeasurement::setData(STI_Server_Device::TPicture data)
{
	measurement_l.data.picture( data );
}
