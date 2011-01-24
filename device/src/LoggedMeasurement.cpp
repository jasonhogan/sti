/*! \file LoggedMeasurement.cpp
*  \author Jason Michael Hogan
*  \brief Source-file for the class LoggedMeasurement
*  \section license License
*
*  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#include "LoggedMeasurement.h"

#include <math.h>

LoggedMeasurement::LoggedMeasurement(unsigned short channel, unsigned int measureInterval_secs, 
				  unsigned int saveInterval_secs, double deviationThreshold, STI_Device* sti_device, MixedValue& valueInput) :
measureInterval(measureInterval_secs), 
saveInterval(saveInterval_secs), 
threshold(deviationThreshold), 
device(sti_device),
valueIn(valueInput)
{
	measurementChannel = channel;
	type = Channel;
	thresholdExceeded = false;
	sigma.setValue(MixedData(0)); //sets value to be Empty
	measurement.setValue(MixedData(0));
	numberAveragedMeasurements = 0;

}

LoggedMeasurement::LoggedMeasurement(std::string attributeKey, unsigned int measureInterval_secs, 
				  unsigned int saveInterval_secs, double deviationThreshold, STI_Device* sti_device) :
measureInterval(measureInterval_secs), 
saveInterval(saveInterval_secs), 
threshold(deviationThreshold), 
device(sti_device)
{
	measurementKey = attributeKey;
	type = Attribute;
	thresholdExceeded = false;

	sigma.setValue(MixedData(0)); //sets value to be Empty
	measurement.setValue(MixedData(0));
	numberAveragedMeasurements = 0;
}

LoggedMeasurement::~LoggedMeasurement()
{
}

int LoggedMeasurement::getTimeTillNextMeasurement()
{
	int result = static_cast<int>(measureInterval);

	result -= Clock::get_s(saveTimer.getCurrentTime());

	return result;
}

int LoggedMeasurement::getTimeTillNextSave()
{
	if(thresholdExceeded)
		return 0;

	int result = static_cast<int>(saveInterval);

	result -= Clock::get_s(saveTimer.getCurrentTime());

	return result;
}

void LoggedMeasurement::getDeviceData(MixedData& data)
{
	double value = 0;

	if(type == Attribute)
	{
		device->refreshDeviceAttributes();
		STI_Device::stringToValue(device->getAttribute(measurementKey), value);
		data.setValue(value);
	}
	else if(type == Channel)
	{
		device->read(this->getChannel(), valueIn, data);
		// Debugging only; broken for vectors
//		value = data.getNumber();
//		std::cerr << "Logged: " << value << std::endl;
	}
}

void LoggedMeasurement::makeMeasurement()
{
	measurementTimer.reset();

	MixedData newResult;
	MixedData delta;
	MixedData sigmaSqrd;
	
	getDeviceData(newResult);
	if(measurement == 0)
		delta.setValue(newResult);
	else
		delta.setValue(newResult - measurement);

	thresholdExceeded = false;

	//Does the -1 have to be on the rhs?
	if(sigma != 0 && ((delta < sigma*threshold*(-1)) || (delta > sigma*threshold) ) )
	{
		//spurious data point detected
		thresholdExceeded = true;
		measurement.setValue(newResult);
		numberAveragedMeasurements = 0;
	}
	else
	{
		//the measurement average resets after each save interval
		measurement.setValue((measurement * numberAveragedMeasurements + newResult) / (numberAveragedMeasurements + 1));
	}

	numberAveragedMeasurements++;

	//standard deviation sigma always includes a contribution from the previous sigma (before numberAveragedMeasurements is reset).
	sigmaSqrd.setValue(
		(sigma*sigma * numberAveragedMeasurements + delta*delta) / (numberAveragedMeasurements + 1) 
		);
	sigma.setValue(sigmaSqrd.sqroot());
}

bool LoggedMeasurement::isMeasurementWithinThreshold()
{
	return !thresholdExceeded;
}

const MixedData& LoggedMeasurement::saveResult()
{
	savedResult.setValue(measurement);
	saveTimer.reset();

	if(!thresholdExceeded)
	{
		numberAveragedMeasurements = 0;
	}

	return savedResult;
}