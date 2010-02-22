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
{
	//todaysBasePath = "\\\\atomsrv1\\EP\\Data\\STI-Test\\2010\\2\\10\\";
	//timingFileRelativeDir = "timing\\";
	//experimentsRelativeDir = "experiments\\";
	//dataRelativeDir = "data\\";

	todaysBasePath = absBaseDir;
	std::string dtdDir = docSettings->getDTDFileAbsDir();

	std::string dtdRelDir = STI::Utils::getRelativePath(dtdDir, absBaseDir);

	fs::path dtdPath(dtdRelDir + STI::Utils::getNativePathSeparator() + "experiment.dtd");
	xmlManager.createDocument("experiment", dtdPath.native_file_string(), "experiment");

	//fs::create_directories(fs::path(todaysBasePath));

	timingFileRelativeDir = docSettings->getTimingFilesRelDir();
	experimentsRelativeDir = docSettings->getExperimentFilesRelDir();
	dataRelativeDir = docSettings->getDataFilesRelDir();
	
	sequenceRelativeDir = STI::Utils::getRelativePath(sequenceFileAbsPath, absBaseDir);

	fs::create_directories(fs::path(todaysBasePath + timingFileRelativeDir));
	fs::create_directories(fs::path(todaysBasePath + experimentsRelativeDir));
	fs::create_directories(fs::path(todaysBasePath + dataRelativeDir));

	experimentFileName = generateXMLFileName();
	buildDocument(description, isSequenceMember);
}




ExperimentDocumenter::~ExperimentDocumenter()
{
}

void ExperimentDocumenter::buildDocument(std::string description, bool isSequenceMember)
{
	
	DOMNodeWrapper* root = xmlManager.getRootNode();
	root->appendChildElement("title")
		->appendTextNode( getFilenameNoExtension(experimentFileName) );
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

void ExperimentDocumenter::addTimingFiles(const std::vector<std::string>& files)
{
	fs::path dir(timingFileRelativeDir + STI::Utils::getNativePathSeparator());
	
	fs::path timingFile;
	for(unsigned i = 0; i < files.size(); i++)
	{
		timingFile = dir / files.at(i);
		timingRoot->appendChildElement("file")->appendTextNode( timingFile.native_file_string() );
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
void ExperimentDocumenter::addDeviceData(const RemoteDevice& device)
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
	DOMNodeWrapper* nextMeasurement;

	const DataMeasurementVector& measurements = device.getMeasurements();
	for(unsigned i = 0; i < measurements.size(); i++)
	{
		nextMeasurement = measurementsNode->appendChildElement("measurement")
			->setAttribute("time", STI::Utils::valueToString( measurements.at(i).time() ))
			->setAttribute("channel", STI::Utils::valueToString( measurements.at(i).channel() ))
			->setAttribute("description", measurements.at(i).getDescription());

		//add value of measurement
		addMixedDataToMeasurementNode(nextMeasurement, measurements.at(i).getMixedData());
	}

}

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


std::string ExperimentDocumenter::getFilenameNoExtension(std::string filename)
{
	//assumes that the only period "." in the filename is at the start of the extension

	std::string::size_type period = filename.find_last_of(".");

	if(period != std::string::npos && period > 0)
		return filename.substr(0, period - 1);
	else
		return filename;
}

std::string ExperimentDocumenter::getDateAndTime()
{
	time_t rawtime;
	tm* timeinfo;
	
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	
	return asctime(timeinfo);
}


void ExperimentDocumenter::writeToDisk()
{

	xmlManager.PrintDocumentToFile(todaysBasePath + experimentsRelativeDir + experimentFileName);

	//std::string xmlDocument = xmlManager.getDocumentAsString();
	//std::cout << "ExperimentDocumenter: " << std::endl;
	//std::cout << xmlDocument << std::endl;
}


std::string ExperimentDocumenter::generateXMLFileName()
{
	time_t rawtime;
	time(&rawtime);
	tm* timeStruct =localtime(&rawtime);

	std::stringstream fileName;

	//get native path separator
	std::string nativePathSep = STI::Utils::getNativePathSeparator();

	//add native path
//	fs::path nativePath(logDir, fs::native);
//	fileName << nativePath.native_directory_string();

	//make sure to add an extra separator if needed
	if( fileName.str().find_last_of( nativePathSep ) != fileName.str().length() - 1 )
		fileName << nativePathSep;	

	//add file name and device-specific suffix
	fileName << (timeStruct->tm_mon + 1) << "_" << (timeStruct->tm_mday) << "_" << (1900 + timeStruct->tm_year) 
		<< "-" 
		<< timeStruct->tm_hour << "_" << timeStruct->tm_min << "_" << timeStruct->tm_sec 
		<< ".xml";

	//asctime( localtime(&rawtime) )

	return fileName.str();
}
