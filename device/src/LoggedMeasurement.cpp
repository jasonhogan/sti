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
				  unsigned int saveInterval_secs, double deviationThreshold, STI_Device* sti_device) :
measureInterval(measureInterval_secs), 
saveInterval(saveInterval_secs), 
threshold(deviationThreshold), 
device(sti_device)
{
	measurementChannel = channel;
	type = Channel;
	thresholdExceeded = false;
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

double LoggedMeasurement::getDeviceData()
{
	double value = 0;

	if(type == Attribute)
	{
		device->refreshDeviceAttributes();
		STI_Device::stringToValue(device->getAttribute(measurementKey), value);
	}
	else if(type == Channel)
	{
	}

	return value;
}

void LoggedMeasurement::makeMeasurement()
{
	measurementTimer.reset();

	double newResult = getDeviceData();
	double delta = newResult - measurement;

	thresholdExceeded = false;

	if(sigma != 0 && ((delta < -1*threshold*sigma) || (delta > threshold*sigma) ) )
	{
		//spurious data point detected
		thresholdExceeded = true;
		measurement = newResult;
		numberAveragedMeasurements = 0;
	}
	else
	{
		//the measurement average resets after each save interval
		measurement = (measurement * numberAveragedMeasurements + newResult) / (numberAveragedMeasurements + 1);
	}

	numberAveragedMeasurements++;

	//standard deviation sigma always includes a contribution from the previous sigma (before numberAveragedMeasurements is reset).
	sigma = sqrt(
		(sigma*sigma * numberAveragedMeasurements + delta*delta) / (numberAveragedMeasurements + 1) 
		);
}

bool LoggedMeasurement::isMeasurementWithinThreshold()
{
	return !thresholdExceeded;
}

double LoggedMeasurement::saveResult()
{
	double result = measurement;
	saveTimer.reset();

	if(!thresholdExceeded)
	{
		numberAveragedMeasurements = 0;
	}

	return result;
}
