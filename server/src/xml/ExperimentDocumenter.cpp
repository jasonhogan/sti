/*! \file ExperimentDocumenter.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ExperimentDocumenter
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#include "ExperimentDocumenter.h"
#include <DOMNodeWrapper.h>
#include <RemoteDevice.h>
#include <MixedData.h>
#include <utils.h>
#include <DocumentationSettings_i.h>

#include <time.h>

#include <iostream>

using std::string;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

namespace fs = boost::filesystem;

ExperimentDocumenter::ExperimentDocumenter(std::string absBaseDir, DocumentationSettings_i* docSettings, 
										   std::string description, bool isSequenceMember, 
										   std::string sequenceFileAbsPath)
: basePath(absBaseDir), experimentsPath(absBaseDir), dataPath(absBaseDir), timingPath(absBaseDir)
{
	//todaysBasePath = "\\\\atomsrv1\\EP\\Data\\STI-Test\\2010\\2\\10\\";
	//timingFileRelativeDir = "timing\\";
	//experimentsRelativeDir = "experiments\\";
	//dataRelativeDir = "data\\";

	generateTimeStamp();	//time and date of this experiment

	todaysBasePath = absBaseDir;
	std::string dtdDir = docSettings->getDTDFileAbsDir();
	
	timingFileRelativeDir = docSettings->getTimingFilesRelDir();
	experimentsRelativeDir = docSettings->getExperimentFilesRelDir();
	dataRelativeDir = docSettings->getDataFilesRelDir();

	timingPath /= timingFileRelativeDir;
	dataPath /= dataRelativeDir;
	experimentsPath /= experimentsRelativeDir;


	std::string dtdRelDir = STI::Utils::getRelativePath(dtdDir, experimentsPath.native_file_string());
	//fs::path dtdPath(dtdRelDir + STI::Utils::getNativePathSeparator() + "experiment.dtd");
	fs::path dtdPath = dtdRelDir;
	dtdPath /= "experiment.dtd";
	xmlManager.createDocument("experiment", dtdPath.native_file_string(), "experiment");

	//fs::create_directories(fs::path(todaysBasePath));

	sequenceRelativeDir = STI::Utils::getRelativePath(sequenceFileAbsPath, experimentsPath);

	fs::create_directories(timingPath);
	fs::create_directories(experimentsPath);
	fs::create_directories(dataPath);

	if(!isSequenceMember)
	{
		timingSubdirectoryName = generateTimingSubdirectoryName();
	}

	experimentFileName = generateXMLFileName();
	buildDocument(description, isSequenceMember);
}




ExperimentDocumenter::~ExperimentDocumenter()
{
	timingFiles.clear();
}

void ExperimentDocumenter::buildDocument(std::string description, bool isSequenceMember)
{
	
	DOMNodeWrapper* root = xmlManager.getRootNode();
	root->appendChildElement("title")
		->appendTextNode( STI::Utils::getFilenameNoExtension(experimentFileName) );
	root->appendChildElement("date")
		->appendTextNode( getDateAndTime() );

	if(isSequenceMember)
	{
		root->appendChildElement("series")->appendChildElement("file")
			->appendTextNode( sequenceRelativeDir );
	}

	if(description.length() > 0)
	{
		root->appendChildElement("description")
			->appendTextNode( description );
	}

	timingRoot = root->appendChildElement("timing");
	devicesRoot = root->appendChildElement("devices");
}



fs::path ExperimentDocumenter::getAbsExpFilepath()
{
	fs::path experimentPath(todaysBasePath);

	experimentPath /= experimentsRelativeDir;
	experimentPath /= experimentFileName;

	return experimentPath;
}

void ExperimentDocumenter::addTimingFiles(const std::vector<std::string>& files, std::string timingSubDirectory)
{
	//This is used by the SequenceDocumenter to enforce a timing subdirectory that
	//is the same for all experiments in a sequence.
	timingSubdirectoryName = timingSubDirectory;
	addTimingFiles(files);
}

void ExperimentDocumenter::addTimingFiles(const std::vector<std::string>& files)
{
	
	timingFiles.clear();

	for(unsigned i = 0; i < files.size(); i++)
	{
		timingFiles.push_back(files.at(i));
	}

	generateTimingFileRelativeDirectoryStructure();

	fs::path timingBasePath(timingPath);
	timingBasePath /= timingSubdirectoryName;

	fs::path timingFileRelPath;

	//Add files to xml document (using directories relative to the experiment xml file base directory)
	for(unsigned i = 0; i < timingFiles.size(); i++)
	{
		timingFileRelPath = timingRelativeDirs.at(i);
		timingFileRelPath /= STI::Utils::getFilenameNoDirectory( timingFiles.at(i) );

		timingRoot->appendChildElement("file")->appendTextNode(
			STI::Utils::convertPathToURL(
				STI::Utils::getRelativePath(timingBasePath / timingFileRelPath, experimentsPath)
				)
			);
	}

}

void ExperimentDocumenter::addVariables(const std::vector<libPython::ParsedVar>& vars)
{
	for(unsigned i = 0; i < vars.size(); i++)
	{
		// Only save variables that have a non-NULL position.
		// These correspond to variables that are defined in the timing file using setVar().
		if(vars[i].position != NULL)
		{
			timingRoot->appendChildElement("var")
				->setAttribute("name", vars.at(i).name )
				->setAttribute("value", vars.at(i).value.str() );
		}
	}
}
void ExperimentDocumenter::addDeviceData(RemoteDevice& device)
{
	DOMNodeWrapper* deviceNode = devicesRoot->appendChildElement("device")
		->setAttribute("devicename", STI::Utils::valueToString(device.getDevice().deviceName) )
		->setAttribute("ipaddress", STI::Utils::valueToString(device.getDevice().address))
		->setAttribute("module", STI::Utils::valueToString(device.getDevice().moduleNum));


	//attributes
	DOMNodeWrapper* attributesNode = deviceNode->appendChildElement("attributes");

	const AttributeMap& attributes = device.getAttributes();
	AttributeMap::const_iterator it;

	for(it = attributes.begin(); it != attributes.end(); it++)
	{
		attributesNode->appendChildElement("attribute")
			->setAttribute("key", it->first)
			->setAttribute("value", it->second.value());
	}
	
	//partners
	const std::vector<std::string>& partners = device.getRegisteredPartners();
	
	if(partners.size() > 0)
	{
		DOMNodeWrapper* partnersNode = deviceNode->appendChildElement("partners");
	
		for(unsigned i = 0; i < partners.size(); i++)
		{
			partnersNode->appendChildElement("partner")->appendTextNode(partners.at(i));
		}
	}

	//measurements
	DOMNodeWrapper* measurementsNode = deviceNode->appendChildElement("measurements");
//	DOMNodeWrapper* nextMeasurement;

	const DataMeasurementVector& measurements = device.getMeasurements();
	for(unsigned i = 0; i < measurements.size(); i++)
	{
		measurementsNode->appendChildElement("measurement")
			->setAttribute("time", STI::Utils::valueToString( measurements.at(i).time() ))
			->setAttribute("channel", STI::Utils::valueToString( measurements.at(i).channel() ))
			->setAttribute("description", measurements.at(i).getDescription())
			->appendMixedDataNode(measurements.at(i).getMixedData());
/*
		nextMeasurement = measurementsNode->appendChildElement("measurement")
			->setAttribute("time", STI::Utils::valueToString( measurements.at(i).time() ))
			->setAttribute("channel", STI::Utils::valueToString( measurements.at(i).channel() ))
			->setAttribute("description", measurements.at(i).getDescription())

		//add value of measurement
		addMixedDataToMeasurementNode(nextMeasurement, measurements.at(i).getMixedData());
*/
	}

}

/*
void ExperimentDocumenter::addMixedDataToMeasurementNode(DOMNodeWrapper* measurementNode, const MixedData& data)
{
	switch(data.getType())
	{
	case MixedData::Boolean:
		measurementNode->appendChildElement("bool")->appendTextNode( STI::Utils::valueToString(data.getBoolean()));
		break;
	case MixedData::Octet:
		measurementNode->appendChildElement("octet")->appendTextNode( STI::Utils::valueToString(data.getOctet()));
		break;
	case MixedData::Double:
		measurementNode->appendChildElement("double")->appendTextNode( STI::Utils::valueToString(data.getDouble()));
		break;
	case MixedData::Int:
		measurementNode->appendChildElement("int")->appendTextNode( STI::Utils::valueToString(data.getInt()));
		break;
	case MixedData::String:
		measurementNode->appendChildElement("string")->appendTextNode( STI::Utils::valueToString(data.getString()));
		break;
	case MixedData::File:
		measurementNode->appendChildElement("file");
		//->appendTextNode( STI::Utils::valueToString(data));
		break;
	case MixedData::Vector:
		{
			DOMNodeWrapper* vecNode = measurementNode->appendChildElement("vector");
			for(unsigned i = 0; i < data.getVector().size(); i++)
			{
				addMixedDataToMeasurementNode(vecNode, data.getVector().at(i));
			}
		}
		break;
	default:
		break;
	}
}
*/



std::string ExperimentDocumenter::getDateAndTime()
{
	return asctime(timeStruct);
}


void ExperimentDocumenter::writeToDisk()
{
	//fs::path experimentPath(todaysBasePath);

	//experimentPath /= experimentsRelativeDir;
	//experimentPath /= experimentFileName;

	xmlManager.PrintDocumentToFile( getAbsExpFilepath().native_file_string() );
}

void ExperimentDocumenter::copyTimingFiles()
{
	//Copy all timing files
	fs::path timingBasePath(timingPath);
	timingBasePath /= timingSubdirectoryName;

	fs::create_directories(timingBasePath);

	//create timing file subdirectory structure and copy timing files
	for(unsigned i = 0; i < timingFiles.size(); i++)
	{
		fs::create_directories( timingBasePath / timingRelativeDirs.at(i) );

		fs::copy_file( fs::path(timingFiles.at(i)), 
			timingBasePath / timingRelativeDirs.at(i) / STI::Utils::getFilenameNoDirectory(timingFiles.at(i)) );
	}
}

void ExperimentDocumenter::generateTimingFileRelativeDirectoryStructure()
{
	//Gets the directory names relative to the local timing base path.
	//This directory structure will be preserved when the timing files are copied.

	timingRelativeDirs.clear();

	fs::path timingBasePath(timingPath);
	timingBasePath /= timingSubdirectoryName;

	std::string timingLocalBaseDir = "";
	std::string timingFileDir;

	if(timingFiles.size() > 0)
	{
		timingLocalBaseDir = STI::Utils::getDirectory(timingFiles.at(0));
	}

	for(unsigned i = 0; i < timingFiles.size(); i++)
	{
		timingFileDir = STI::Utils::getDirectory(
			STI::Utils::getRelativePath(timingFiles.at(i), timingLocalBaseDir));

		timingRelativeDirs.push_back( timingFileDir );
	}
}


void ExperimentDocumenter::generateTimeStamp()
{
	time_t rawtime;
	time(&rawtime);
	
	timeStruct = localtime(&rawtime);
}

std::string ExperimentDocumenter::generateXMLFileName()
{
	std::stringstream fileName;

	//get native path separator
	std::string nativePathSep = STI::Utils::getNativePathSeparator();

	//add native path
//	fs::path nativePath(logDir, fs::native);
//	fileName << nativePath.native_directory_string();

	//make sure to add an extra separator if needed
	if( fileName.str().find_last_of( nativePathSep ) != fileName.str().length() - 1 )
		fileName << nativePathSep;	

	fileName << (timeStruct->tm_mon + 1) << "_" << (timeStruct->tm_mday) << "_" << (1900 + timeStruct->tm_year) 
		<< "-" 
		<< timeStruct->tm_hour << "_" << timeStruct->tm_min << "_" << timeStruct->tm_sec;
	
	return STI::Utils::getUniqueFilename(fileName.str(), ".xml", fs::path(todaysBasePath + experimentsRelativeDir));


	//asctime( localtime(&rawtime) )

//	return fileName.str();
}


std::string ExperimentDocumenter::generateTimingSubdirectoryName()
{
	std::stringstream dirName;

	dirName << timeStruct->tm_hour << "_" << timeStruct->tm_min << "_" << timeStruct->tm_sec;
	
	return STI::Utils::getUniqueFilename(dirName.str(), "", fs::path(todaysBasePath + timingFileRelativeDir));


	//asctime( localtime(&rawtime) )

//	return fileName.str();
}