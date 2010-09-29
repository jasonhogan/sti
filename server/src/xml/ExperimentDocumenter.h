/*! \file ExperimentDocumenter.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class SequenceDocumenter
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

#ifndef EXPERIMENTDOCUMENTER_H
#define EXPERIMENTDOCUMENTER_H

#include "XmlManager.h"
#include <client.h>
#include <parsedvar.h>

#include <vector>
#include <string>


#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

namespace fs = boost::filesystem;



class RemoteDevice;
class MixedData;
class DocumentationSettings_i;

class ExperimentDocumenter
{
public:
	
	ExperimentDocumenter(std::string absBaseDir, DocumentationSettings_i* docSettings, 
		std::string description, bool isSequenceMember=false,
		std::string sequenceFileAbsPath="");
	~ExperimentDocumenter();

	void addTimingFiles(const std::vector<std::string>& files);
	void addDeviceData(RemoteDevice& device);
	void addVariables(const std::vector<libPython::ParsedVar>& vars);
	void writeToDisk();

private:
	
	std::string generateXMLFileName();
//	void addMixedDataToMeasurementNode(DOMNodeWrapper* measurementNode, const MixedData& data);
	std::string getFilenameNoExtension(std::string filename);
	std::string getDateAndTime();

	void buildDocument(std::string description, bool isSequenceMember=false);

	XmlManager xmlManager;

	DOMNodeWrapper* timingRoot;
	DOMNodeWrapper* devicesRoot;

	std::string timingFileRelativeDir;
	std::string dataRelativeDir;
	std::string experimentsRelativeDir;
	std::string sequenceRelativeDir;


	std::string experimentFileName;
	std::string todaysBasePath;

	std::string sequenceRelativePath;

	fs::path basePath;
	fs::path experimentsPath;
	fs::path dataPath;
	fs::path timingPath;

};


#endif

