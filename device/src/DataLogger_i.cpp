/*! \file DataLogger_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DataLogger_i
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

#include "DataLogger_i.h"
#include <iostream>
#include <fstream>
using namespace std;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;


DataLogger_i::DataLogger_i(std::string logDirectory, STI_Device* device) : logDir(logDirectory), sti_device(device)
{
	logLoopMutex = new omni_mutex();
	logLoopCondition = new omni_condition(logLoopMutex);
}

DataLogger_i::~DataLogger_i()
{
}

void DataLogger_i::startLogging()
{
	omni_thread::create(logLoopWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}

void DataLogger_i::addLoggedMeasurement(unsigned short channel, unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold)
{
}

void DataLogger_i::addLoggedMeasurement(std::string attributeKey, unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold)
{
}


void DataLogger_i::logLoopWrapper(void* object)
{
	DataLogger_i* thisObject = static_cast<DataLogger_i*>(object);
	thisObject->logLoop();
}

void DataLogger_i::logLoop()
{
	unsigned long secs, nsecs;
	unsigned long sleepTimeSeconds = 10;

	while(true)
	{
		omni_thread::get_time(&secs, &nsecs, sleepTimeSeconds, 0);
		logLoopMutex->lock();
		{
			logLoopCondition->timedwait(secs, nsecs);
		}
		logLoopMutex->unlock();
	}

}

void DataLogger_i::getSavedLogFiles(std::string dir)
{
	logFiles.clear();

	fs::path full_path = fs::system_complete( fs::path( dir, fs::native ) );

	std::string fileName;
	if ( fs::is_directory( full_path ) )
	{
		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter;	++dir_itr )
		{
			try
			{
				if ( is_regular( dir_itr->status() ) )
				{
					fileName = dir_itr->path().native_file_string();

					if( belongsToDevice(fileName) )
						logFiles.push_back( new NetworkFileSource( fileName ) );
				}
			}
			catch ( const std::exception & ex )
			{
				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
			}
		}
	}
}

bool DataLogger_i::belongsToDevice(std::string fileName)
{
	return true;
}

STI::Types::TNetworkFileSeq* DataLogger_i::getLogFiles()
{
	getSavedLogFiles( logDir );

	using STI::Types::TNetworkFileSeq;
	using STI::Types::TNetworkFileSeq_var;

	TNetworkFileSeq_var fileSeq( new TNetworkFileSeq );
	fileSeq->length( logFiles.size() );

	for(unsigned i = 0; i < logFiles.size(); i++)
	{
		fileSeq[i] = logFiles.at(i).getNetworkFileReference();
	}

	return fileSeq._retn();
}

void DataLogger_i::clearLocalLogFiles()
{
	getSavedLogFiles( logDir );

	//Delete all local copies of log files
	for(unsigned i = 0; i < logFiles.size(); i++)
	{
		std::remove( logFiles.at(i).getFileName().c_str() );
	}

	logFiles.clear();
}
