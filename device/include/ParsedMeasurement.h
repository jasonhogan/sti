/*! \file ParsedMeasurement.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ParsedMeasurement
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

#ifndef PARSEDMEASUREMENT_H
#define PARSEDMEASUREMENT_H

#include <string>

#include "device.h"

//TData
using STI::Types::DataDouble;
using STI::Types::DataLong;
using STI::Types::DataString;
using STI::Types::DataPicture;
using STI::Types::DataNone;


class ParsedMeasurement
{
public:

	//ParsedMeasurement();	//for using STL
	ParsedMeasurement(double time, unsigned short channel, unsigned eventNumber);
	ParsedMeasurement(const STI::Types::TMeasurement& measurement, unsigned eventNumber);

	~ParsedMeasurement();

	//ParsedMeasurement(const ParsedMeasurement& copy);
	//ParsedMeasurement& operator= (const ParsedMeasurement& other);

	std::string print() const;

	double time() const;
	void setTime(double time);

	unsigned short channel() const;
	STI::Types::TData dataType() const;

	double                      numberValue() const;
	std::string                 stringValue() const;
	STI::Types::TPicture pictureValue() const;
	const STI::Types::TDataMixed& data() const;

	//A measurement is scheduled by adding it to a SynchronousEvent
	void setScheduleStatus(bool enabled);
	bool isScheduled() const;
	bool isMeasured() const;

	void setData(double data);
	void setData(std::string data);
	void setData(STI::Types::TPicture data);

	unsigned eventNum() const;

	bool operator==(const ParsedMeasurement &other) const;
	bool operator!=(const ParsedMeasurement &other) const;

	static std::string TDataToStr(STI::Types::TData tData);

private:

	STI::Types::TMeasurement measurement_l;

	unsigned eventNumber_l;
	bool scheduled;
	bool measured;

};

#endif
