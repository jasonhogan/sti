/*! \file LoggedMeasurement.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class LoggedMeasurement
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

#ifndef LOGGEDMEASUREMENT_H
#define LOGGEDMEASUREMENT_H

#include <Clock.h>
#include <STI_Device.h>

#include <string>

class LoggedMeasurement
{
public:
	
	LoggedMeasurement(unsigned short channel, unsigned int measureInterval_secs, 
		unsigned int saveInterval_secs, double deviationThreshold, STI_Device* sti_device, MixedValue& valueInput);
	LoggedMeasurement(std::string attributeKey, unsigned int measureInterval_secs, 
		unsigned int saveInterval_secs, double deviationThreshold, STI_Device* sti_device);
	~LoggedMeasurement();

	int getTimeTillNextMeasurement();
	int getTimeTillNextSave();

	void makeMeasurement();
	bool isMeasurementWithinThreshold();
	const MixedData& saveResult();

	enum LoggedMeasurementType {Channel, Attribute};

	LoggedMeasurementType getType() const { return type; }
	std::string getKey() const { return measurementKey; }
	unsigned short getChannel() const { return measurementChannel; }

	void resetTimers();

private:

	void getDeviceData(MixedData& data);

	MixedData savedResult;

	unsigned int measureInterval;
	unsigned int saveInterval;
	double threshold;
	bool thresholdExceeded;

	Clock measurementTimer;
	Clock saveTimer;

	unsigned short measurementChannel;
	std::string measurementKey;


	LoggedMeasurementType type;

	MixedData measurement;	//the average of all measurements between saves;  holds the most recent measurement if that measurement exceeds the deviation threshold
	MixedData sigma;
	int numberAveragedMeasurements;

	STI_Device* device;

	MixedValue valueIn;
};

#endif

