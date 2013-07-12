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
#include <utils.h>

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
time_l(time), channel_l(channel), eventNumber_l(eventNumber)
{
	measured = false;
	scheduled = false;
	description = "";
	useCallback = false;
}


DataMeasurement::DataMeasurement(
		const STI::Types::TMeasurement &measurement, unsigned eventNumber) :
time_l(measurement.time), channel_l(measurement.channel), eventNumber_l(eventNumber)
{
	data_l = measurement.data;

	measured = false;
	scheduled = false;
	description = measurement.description;
	useCallback = false;

}

DataMeasurement::~DataMeasurement()
{
}

std::string DataMeasurement::print() const
{
	std::stringstream meas;

//	<Time=2.1, Channel=4, Type=Double, Value=3.4>
	meas << "<Time=" << STI::Utils::printTimeFormated(time());
	meas << ", Channel=" << channel();
	meas << ", Type=" << data_l.getTypeString();
	meas << ", Data=" << data_l.print();
	meas << ">";	
	
	return meas.str();
}

double DataMeasurement::time() const
{
	return time_l;
}
unsigned short DataMeasurement::channel() const
{
	return channel_l;
}
//STI::Types::TData DataMeasurement::dataType() const
//{
//	return measurement_l.data._d();
//}

const STI::Types::TDataMixed DataMeasurement::data() const
{
	return data_l.getTDataMixed();
}

const MixedData& DataMeasurement::getMixedData() const
{
	return data_l;
}

bool DataMeasurement::operator==(const DataMeasurement &other) const
{
	//return (
	//	time() == other.time() && 
	//	channel() == other.channel() && 
	//	getMixedData() == other.getMixedData() );
	return (
		channel() == other.channel() && 
		getMixedData() == other.getMixedData() );
}

bool DataMeasurement::operator!=(const DataMeasurement &other) const
{
	return !( (*this)==other );
}

//unsigned DataMeasurement::eventNum() const
//{
//	return eventNumber_l;
//}

//void DataMeasurement::setTime(double time)
//{
//	time_l = time;
//}

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

void DataMeasurement::clearData()
{
	measured = false;
}

void DataMeasurement::setDescription(std::string desc)
{
	description = desc;
}

std::string DataMeasurement::getDescription() const
{
	return description;
}


void DataMeasurement::installMeasurementCallback(STI::Server_Device::TMeasurementCallback_ptr callback)
{
	useCallback = true;
	measurementCallback = STI::Server_Device::TMeasurementCallback_var(callback);
}
void DataMeasurement::sendMeasurementCallback(const STI::Types::TMeasurement_var& tMeas)
{
	if(!useCallback || !isMeasured())
		return;

	//using STI::Types::TMeasurement;
	//using STI::Types::TMeasurement_var;

	//TMeasurement_var tMeas( new TMeasurement() );

	//tMeas->channel     = channel();
	//tMeas->time        = time();
	//tMeas->data        = data();
	//tMeas->description = CORBA::string_dup(getDescription().c_str());

	try {
		measurementCallback->returnResult(tMeas);
	}
	catch(CORBA::TRANSIENT& ex) {
		std::cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to send measurement callback." << std::endl;
	}
	catch(CORBA::SystemException& ex) {
		std::cerr << "Caught system exception CORBA::" << ex._name() 
			<< " when trying to send measurement callback." << std::endl;
	}
	catch(CORBA::Exception& ex) {
		std::cerr << "Caught general exception CORBA::" << ex._name() 
			<< " when trying to send measurement callback." << std::endl;
	}
	catch(...) {
		std::cerr << "Caught unknown exception" 
			<< " when trying to send measurement callback." << std::endl;
	}
}

