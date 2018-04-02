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
#include <DOMNodeWrapper.h>

#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <map>

namespace fs = boost::filesystem;

omni_mutex* DataLogger_i::xercesMutex = new omni_mutex();

DataLogger_i::DataLogger_i(std::string logDirectory, STI_Device* device) : logDir(logDirectory), sti_device(device)
{
	activeXMLdoc = NULL;
	writeToDiskTime = 6*60*60;	//every 6 hours
	logFileDTDfilename = "devicelogfile.dtd";
	logging = false;


	logLoopMutex = new omni_mutex();
	logLoopCondition = new omni_condition(logLoopMutex);
	
	std::stringstream fileNameSuffixStrm;
	fileNameSuffixStrm.str("");
	fileNameSuffixStrm << std::string(sti_device->getTDevice().deviceName);
	fileNameSuffixStrm<< "_" << sti_device->getTDevice().moduleNum << ".xml";

	fileNameSuffix = fileNameSuffixStrm.str();
}

DataLogger_i::~DataLogger_i()
{
	if(logging)
		stopLogging();
}

void DataLogger_i::setLogDirectory(std::string logDirectory)
{
	logDir = logDirectory;
}

/*void DataLogger_i::startLogging()
{
	if(logging)
		return;

	if(loggedMeasurements.size() == 0)
		return;

	logging = true;
	
	createNewXMLdoc();

	omni_thread::create(logLoopWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}*/

void DataLogger_i::stopLogging()
{
	if(!isLogging())
		return;

	setLogging(false);
	logDataCondition.notify_one();
	/*if(activeXMLdoc != NULL)
	{
		saveXML();
	}*/

}

std::string DataLogger_i::generateXMLFileName()
{
	time_t rawtime;
	time(&rawtime);
	tm* timeStruct =localtime(&rawtime);

	std::stringstream fileName;

	//get native path separator
	fs::path pathSeparator("/");
	std::string nativePathSep = pathSeparator.string();

	//add native path
	fs::path nativePath(logDir);
	fileName << nativePath.string();

	//make sure to add an extra separator if needed
	if( fileName.str().find_last_of( nativePathSep ) != fileName.str().length() - 1 )
		fileName << pathSeparator.string();	

	//add file name and device-specific suffix
	//SMD edit 09/13/10; added information to place file into a year and month subdirectories.
	fileName << (1900 + timeStruct->tm_year) << nativePathSep << (timeStruct->tm_mon + 1) << nativePathSep 
	    << (timeStruct->tm_mon + 1) << "_" << (timeStruct->tm_mday) << "_" << (1900 + timeStruct->tm_year) 
		<< "-" 
		<< timeStruct->tm_hour << "_" << timeStruct->tm_min << "_" << timeStruct->tm_sec 
		<< "-" 
		<< fileNameSuffix;

	//asctime( localtime(&rawtime) )

	return fileName.str();
}

void DataLogger_i::addLoggedMeasurement(unsigned short channel, unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold, MixedValue& valueIn)
{
	loggedMeasurements.push_back( new LoggedMeasurement(channel, measureInterval, saveInterval, deviationThreshold, sti_device, valueIn) );
}

void DataLogger_i::addLoggedMeasurement(std::string attributeKey, unsigned int measureInterval, unsigned int saveInterval, double deviationThreshold)
{
	loggedMeasurements.push_back( new LoggedMeasurement(attributeKey, measureInterval, saveInterval, deviationThreshold, sti_device) );
}

void DataLogger_i::addDataToActiveLog(Int64 time, unsigned short channel, const MixedData &value)
{
	if(activeXMLdoc == NULL && dataNode != NULL)
		return;

	dataNode->appendChildElement("channeldata")
		->setAttribute("time", STI_Device::valueToString(time) )
		->setAttribute("channel", STI_Device::valueToString(channel) )
		->appendMixedDataNode(value);
}

void DataLogger_i::addDataToActiveLog(Int64 time, std::string key, double value)
{
	if(activeXMLdoc == NULL && dataNode != NULL)
		return;

	dataNode->appendChildElement("attributedata")
		->setAttribute("time", STI_Device::valueToString(time) )
		->setAttribute("key", key )
		->setAttribute("value", STI_Device::valueToString(value) );
}

void DataLogger_i::createNewXMLdoc()
{
	xercesMutex->lock();
	{
		activeXMLdoc = new XmlManager();
		activeXMLdoc->createDocument("devicelogfile", logFileDTDfilename, "devicelogfile");
	}
	xercesMutex->unlock();

	DOMNodeWrapper* root = activeXMLdoc->getRootNode();
	root->appendChildElement("device")
		->setAttribute("devicename", std::string(sti_device->getTDevice().deviceName) )
		->setAttribute("ipaddress", std::string(sti_device->getTDevice().address) )
		->setAttribute("module", STI_Device::valueToString(sti_device->getTDevice().moduleNum) );

	timeIntervalNode = root->appendChildElement("timeinterval");
	timeIntervalNode->appendChildElement("start")->appendTextNode( STI_Device::valueToString(getLocalTime()) );

	
	dataNode = root->appendChildElement("data");
}

/*void DataLogger_i::logLoopWrapper(void* object)
{
	DataLogger_i* thisObject = static_cast<DataLogger_i*>(object);
	thisObject->logLoop();
}

void DataLogger_i::logLoop()
{
	time_t local_time;
	time(&local_time);
	localtime(&local_time);

	unsigned long secs, nsecs;
	unsigned long sleepTimeSeconds = 24*60*60;
	int measurementSleepTime, saveSleepTime, xmlSleepTime, deltaMeasure, deltaSave;

	Clock writeToDiskTimer;
	writeToDiskTimer.reset();

	//Reset measurements for better behavior when stopping and starting logging
	for(unsigned i = 0; i < loggedMeasurements.size(); i++)
	{
		loggedMeasurements.at(i).resetTimers();
	}

	while(logging)
	{
		for(unsigned i = 0; i < loggedMeasurements.size(); i++)
		{
			measurementSleepTime = loggedMeasurements.at(i).getTimeTillNextMeasurement();
			if(measurementSleepTime <= 0)
			{
				loggedMeasurements.at(i).makeMeasurement();
				measurementSleepTime = loggedMeasurements.at(i).getTimeTillNextMeasurement();
			}
			
			deltaMeasure = measurementSleepTime - static_cast<int>(sleepTimeSeconds);

			if(deltaMeasure < 0 && measurementSleepTime > 0)
			{
				sleepTimeSeconds = measurementSleepTime;
			}
			saveSleepTime = loggedMeasurements.at(i).getTimeTillNextSave();

			if(saveSleepTime <= 0)
			{
				time(&local_time);
				localtime(&local_time);

				switch(loggedMeasurements.at(i).getType())
				{
				case LoggedMeasurement::Attribute:
					addDataToActiveLog(local_time, loggedMeasurements.at(i).getKey(), loggedMeasurements.at(i).saveResult().getDouble());
					break;
				case LoggedMeasurement::Channel:
					addDataToActiveLog(local_time, loggedMeasurements.at(i).getChannel(), loggedMeasurements.at(i).saveResult());
					break;
				}
				saveSleepTime = loggedMeasurements.at(i).getTimeTillNextSave();
			}
			
			deltaSave = saveSleepTime - static_cast<int>(sleepTimeSeconds);

			if( deltaSave < 0 && saveSleepTime > 0)
			{
				sleepTimeSeconds = saveSleepTime;
			}
		}

		//periodically save xml to disk
		logLoopMutex->lock();
		{
			if( static_cast<int>(Clock::get_s(writeToDiskTimer.getCurrentTime())) >= static_cast<int>(writeToDiskTime) && activeXMLdoc != NULL)
			{
				timeIntervalNode->appendChildElement("end")->appendTextNode(STI_Device::valueToString(getLocalTime()));
				activeXMLdoc->PrintDocumentToFile(generateXMLFileName());
				//std::cout<<activeXMLdoc->getDocumentAsString() << std::endl;
				delete activeXMLdoc;
				activeXMLdoc = NULL;
			
				createNewXMLdoc();
				
				writeToDiskTimer.reset();
			}
		}
		logLoopMutex->unlock();

		xmlSleepTime = (static_cast<int>(writeToDiskTime) - 
			static_cast<int>(Clock::get_s(writeToDiskTimer.getCurrentTime())));

		if( xmlSleepTime < static_cast<int>(sleepTimeSeconds) &&  xmlSleepTime > 0)
			sleepTimeSeconds = xmlSleepTime;

		//go to sleep
		logLoopMutex->lock();
		{
			omni_thread::get_time(&secs, &nsecs, sleepTimeSeconds, 0);
			if(logging)
				logLoopCondition->timedwait(secs, nsecs);
		}
		logLoopMutex->unlock();
	}

}

*/
Int64 DataLogger_i::getLocalTime()
{
	Int64 local_time;
	time_t rawtime;

	time(&rawtime);
	localtime(&rawtime);
	local_time = rawtime;

	return local_time;
}

void DataLogger_i::getSavedLogFiles(std::string dir)
{
	logFiles.clear();

	fs::path full_path = fs::system_complete(fs::path(dir));

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
					fileName = dir_itr->path().string();

					if( belongsToDevice(fileName) )
						logFiles.push_back( new NetworkFileSource( fileName ) );
				}
			}
			catch ( const std::exception & ex )
			{
				std::cout << dir_itr->path().string() << " " << ex.what() << std::endl;
			}
		}
	}
}

bool DataLogger_i::belongsToDevice(std::string fileName)
{
	std::size_t pos = fileName.find(fileNameSuffix);
	return (pos != std::string::npos);
}

STI::Types::TLogFileSeq* DataLogger_i::getLogFiles()
{
	getSavedLogFiles( logDir );

	using STI::Types::TLogFileSeq;
	using STI::Types::TLogFileSeq_var;

	TLogFileSeq_var fileSeq( new TLogFileSeq );
	fileSeq->length( logFiles.size() );

	for(unsigned i = 0; i < logFiles.size(); i++)
	{
		fileSeq[i].logFile = logFiles.at(i).getNetworkFileReference();
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

void DataLogger_i::startLogging()
{
	if(isLogging())
		return;

	if(loggedMeasurements.size() == 0)
		return;

	setLogging(true);
	
	createNewXMLdoc();

	//spawn threads
	boost::thread logEventThread(&DataLogger_i::logEventLoop, this);

}


void DataLogger_i::saveXML()
{
	
	//Save data to xml; create a new xml doc if still logging
	timeIntervalNode->appendChildElement("end")->appendTextNode(STI_Device::valueToString(getLocalTime()));

	std::string filename = generateXMLFileName();

	std::string filePath = STI::Utils::getDirectory(filename);
	fs::create_directories(fs::path(filePath));
	std::cerr << filePath << endl;

	std::cerr << filename << endl;

	activeXMLdoc->PrintDocumentToFile(filename);
	delete activeXMLdoc;
	activeXMLdoc = NULL;

	if (logging)
		createNewXMLdoc();

}



void DataLogger_i::loadNextMeasurement(multimap<boost::system_time, LoggedMeasurement*> &eventMap, LoggedMeasurement* loggedMeasurement)
{
	boost::system_time eventTime;

	eventTime = boost::get_system_time() + 
		boost::posix_time::seconds(loggedMeasurement->getTimeTillNextMeasurement());

	eventMap.insert(pair<boost::system_time, LoggedMeasurement*>(eventTime, loggedMeasurement));
}

void DataLogger_i::logEventLoop()
{
	//boost::unique_lock<boost::mutex> lock(loggedDataMutex);
	
	boost::system_time timeout;
	boost::system_time timeOfNextSaveXML = 
		boost::get_system_time() + boost::posix_time::seconds(writeToDiskTime);
	LoggedMeasurement* measurementsToLog;

	multimap<boost::system_time, LoggedMeasurement*> eventMap;

	for(unsigned int i = 0; i < loggedMeasurements.size(); i++)
	{
		loadNextMeasurement(eventMap, &(loggedMeasurements.at(i)));
	}

	
	while(isLogging())
	{

		timeout = eventMap.begin()->first;

		//wait for appropriate time; avoid spurious wakes
		while (isLogging() && boost::get_system_time() < timeout)
		{
			boost::unique_lock<boost::mutex> lock(loggedDataMutex);
			logDataCondition.timed_wait(lock, timeout);
		}

		//Measure and save data; reload event vector
		if (isLogging())
		{
			measurementsToLog = eventMap.begin()->second;
			eventMap.erase(eventMap.begin());

			measurementsToLog->makeMeasurement();
			loadNextMeasurement(eventMap, measurementsToLog);

			if (measurementsToLog->resultIsReady)
				saveMeasurement(measurementsToLog);
		}

		if(boost::get_system_time() > timeOfNextSaveXML || !isLogging())
		{
			saveXML();
			timeOfNextSaveXML = 
				boost::get_system_time() + boost::posix_time::seconds(writeToDiskTime);
		}

	}
}

bool DataLogger_i::isLogging()
{
	boost::lock_guard<boost::mutex> lock(loggedDataMutex);
	return logging;
}

void DataLogger_i::setLogging(bool value)
{
	boost::lock_guard<boost::mutex> lock(loggedDataMutex);
	logging = value;
}

void DataLogger_i::saveMeasurement(LoggedMeasurement* loggedMeasurement)
{
	time_t local_time;
	time(&local_time);
	localtime(&local_time);

	switch(loggedMeasurement->getType())
	{
	case LoggedMeasurement::Attribute:
		addDataToActiveLog(local_time, loggedMeasurement->getKey(), loggedMeasurement->saveResult().getDouble());
		break;
	case LoggedMeasurement::Channel:
		addDataToActiveLog(local_time, loggedMeasurement->getChannel(), loggedMeasurement->saveResult());
		break;
	}
}

