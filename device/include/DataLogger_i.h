/*! \file DataLogger_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DataLogger_i
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

#ifndef DATALOGGER_I_H
#define DATALOGGER_I_H

#include "device.h"

#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <math.h>

#include "NetworkFileSource.h"

class STI_Device;

typedef boost::ptr_vector<NetworkFileSource> NetworkFileSourceVector;

class DataLogger_i : public POA_STI::Server_Device::DataLogger
{
public:

	DataLogger_i(std::string logDirectory, STI_Device* device);
	~DataLogger_i();
	
	STI::Types::TNetworkFileSeq* getLogFiles();
	void clearLocalLogFiles();

	void startLogging();
	void addLoggedMeasurement(unsigned short channel,   unsigned int measureInterval=60, unsigned int saveInterval=60, double deviationThreshold=2.0);
	void addLoggedMeasurement(std::string attributeKey, unsigned int measureInterval=60, unsigned int saveInterval=60, double deviationThreshold=2.0);

private:

	void getSavedLogFiles(std::string dir);
	bool belongsToDevice(std::string fileName);

	void logLoop();
	static void logLoopWrapper(void* object);

	std::string logDir;
	NetworkFileSourceVector logFiles;

	omni_mutex* logLoopMutex;
	omni_condition* logLoopCondition;

	STI_Device* sti_device;

	class LoggedMeasurement
	{
	public:
		LoggedMeasurement();
		
		unsigned int getNextMeasurementTime();
		unsigned int getNextSaveTime();

		void makeMeasurement()
		{
			double newResult = 0;

			thresholdExceeded = false;

			if(sigma != 0 && ((newResult < measurement - threshold*sigma) || (newResult > measurement + threshold*sigma) ) )
			{
				thresholdExceeded = true;
				measurement = newResult;
			}
			else
			{
				measurement = (measurement * numberAveragedMeasurements + newResult) / (numberAveragedMeasurements + 1);
				sigma = sqrt( 
					(sigma*sigma * numberAveragedMeasurements + (measurement-newResult)*(measurement-newResult)) / (numberAveragedMeasurements + 1) 
					);
				numberAveragedMeasurements++;
			}
		}
		bool isMeasurementWithinThreshold();
		double saveResult()
		{
			double result = measurement;

			numberAveragedMeasurements = 1;

			return result;
		}

	private:

		unsigned int measureInterval;
		unsigned int saveInterval;
		double threshold;
		bool thresholdExceeded;

		double measurement;	//the average of all measurements between saves;  holds the most recent measurement if that measurement exceeds the deviation threshold
		double sigma;
		unsigned numberAveragedMeasurements;

	};

};

#endif
