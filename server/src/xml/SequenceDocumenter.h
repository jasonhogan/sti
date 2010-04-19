/*! \file SequenceDocumenter.h
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

#ifndef SEQUENCEDOCUMENTER_H
#define SEQUENCEDOCUMENTER_H

#include <client.h>
#include <Parser_i.h>
#include "XmlManager.h"
#include <STI_Server.h>
#include <DocumentationSettings_i.h>

#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

namespace fs = boost::filesystem;


class SequenceDocumenter
{
public:
	
	SequenceDocumenter(std::string baseDir, Parser_i* parser_i, DocumentationSettings_i* docSettings);
	~SequenceDocumenter();

	void addExperiment(RemoteDeviceMap& devices);

	void writeDirectoryStructureToDisk();
	void copyTimingFiles();
	void createSequenceXML();
	void writeSequenceXML();

	std::string generateExperimentFilename(std::string suffix);
	std::string getExperimentAbsDirectory();
	std::string getSequenceRelativePath();

	void clearSequence();

private:

	void buildDocument();

	XmlManager xmlManager;

	std::string absBaseDir;
	DocumentationSettings_i* documentationSettings;
	std::string sequenceFileAbsPath;
	Parser_i* parser;

	fs::path sequenceFilePath;

};


#endif

