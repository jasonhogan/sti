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

#ifndef STI_TIMINGENGINE_DATAMEASUREMENT_H
#define STI_TIMINGENGINE_DATAMEASUREMENT_H


#include <MixedValue.h>

#include <string>


namespace STI
{
namespace TimingEngine
{

class TimingMeasurement
{
public:

	TimingMeasurement(unsigned short channel, unsigned eventNumber);

	virtual double time() const;
	virtual void setTime(double time);

	virtual unsigned short channel() const;
	virtual const STI::Utils::MixedValue& value() const;
	virtual STI::Utils::MixedValue& value();
	
	virtual void setDescription(std::string desc);
	virtual std::string getDescription() const;
	
	unsigned eventNum() const { return eventNumber_l; }

protected:
	
	double time_l;
	unsigned short channel_l;
	STI::Utils::MixedValue data_l;
	unsigned eventNumber_l;
	std::string description;
};

//class SynchronousMeasurement : public TimingMeasurement
//{
//};

class ScheduledMeasurement : public TimingMeasurement
{
public:
	ScheduledMeasurement(unsigned short channel, unsigned eventNumber);
	void setScheduleStatus(bool enabled);
	bool isScheduled() const;
};

class DataMeasurement
{
public:

	DataMeasurement(double time, unsigned short channel, unsigned eventNumber);
//	DataMeasurement(const STI::Types::TMeasurement& measurement, unsigned eventNumber);

	~DataMeasurement();

	//DataMeasurement(const DataMeasurement& copy);
	//DataMeasurement& operator= (const DataMeasurement& other);

	std::string print() const;

	double time() const;
//	void setTime(double time);

	unsigned short channel() const;
//	STI::Types::TData dataType() const;

	const STI::Utils::MixedValue& getValue() const;
//	const STI::Types::TDataMixed data() const;

	//A measurement is scheduled by adding it to a SynchronousEvent
	void setScheduleStatus(bool enabled);
	bool isScheduled() const;
	bool isMeasured() const;

	template<class T> void setData(T data) 
	{
		measured = true;
		data_l.setValue(data);
	}

	void clearData();

	unsigned eventNum() const { return eventNumber_l; }

	bool operator==(const DataMeasurement &other) const;
	bool operator!=(const DataMeasurement &other) const;

	void setDescription(std::string desc);
	std::string getDescription() const;

private:

	double time_l;
	unsigned short channel_l;
	STI::Utils::MixedValue data_l;
	std::string description;

	unsigned eventNumber_l;
	bool scheduled;
	bool measured;

};

}
}

#endif
