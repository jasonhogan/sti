/*! \file SequenceDocumenter.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class SequenceDocumenter
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

#include "SequenceDocumenter.h"
#include <ExperimentDocumenter.h>
#include <utils.h>



SequenceDocumenter::SequenceDocumenter(std::string baseDir, Parser_i* parser_i, DocumentationSettings_i* docSettings)
: parser(parser_i)
{
	absBaseDir = baseDir;
	documentationSettings = docSettings;
	
	fs::path sequencePath(absBaseDir);
	sequencePath /= docSettings->getSequenceFilesRelDir();
//	sequencePath /=
	fs::create_directories(sequencePath);

	docSettings->getSequenceFilesRelDir();
	sequenceFileAbsPath = sequencePath.native_directory_string();

	sequenceFilePath = sequencePath / "test_seq.xml";

	isFirstExperiment = true;
	timingSubdirectory = "";
}

SequenceDocumenter::~SequenceDocumenter()
{
}

void SequenceDocumenter::addExperiment(RemoteDeviceMap& devices)
{
	ExperimentDocumenter documenter(absBaseDir, documentationSettings, 
		parser->getParsedDescription(), true, sequenceFilePath.native_file_string());

	if(isFirstExperiment)
	{
		timingSubdirectory = documenter.generateTimingSubdirectoryName();
	}

	documenter.addTimingFiles( parser->getTimingFiles(), timingSubdirectory);
	documenter.addVariables( parser->getParsedVars() );

	RemoteDeviceMap::iterator it;
	for(it = devices.begin(); it != devices.end(); it++)
	{
		documenter.addDeviceData( *it->second );
	}

	documenter.writeToDisk();
	
	if(isFirstExperiment)
	{
		documenter.copyTimingFiles();
	}
	isFirstExperiment = false;
}

void SequenceDocumenter::writeDirectoryStructureToDisk()
{
}


void SequenceDocumenter::createSequenceXML()
{
}

void SequenceDocumenter::writeSequenceXML()
{
}


std::string SequenceDocumenter::generateExperimentFilename(std::string suffix)
{
	return "";
}

std::string SequenceDocumenter::getExperimentAbsDirectory()
{
	return "";
}

std::string SequenceDocumenter::getSequenceRelativePath()
{
	return "";
}


void SequenceDocumenter::clearSequence()
{
}


void SequenceDocumenter::buildDocument()
{
	DOMNodeWrapper* root = xmlManager.getRootNode();
	root->appendChildElement("title")->appendTextNode("My Series Title");
	root->appendChildElement("date")->appendTextNode("7/10/09");
	
	DOMNodeWrapper* timingRoot = root->appendChildElement("timing");
	timingRoot->appendChildElement("file")->appendTextNode("timing.py");
	timingRoot->appendChildElement("file")->appendTextNode("channels.py");

	DOMNodeWrapper* experimentsRoot = root->appendChildElement("experiments");
	experimentsRoot->appendChildElement("experiment")->appendChildElement("file")->appendTextNode("trial1.xml");
	experimentsRoot->appendChildElement("experiment")->appendChildElement("file")->appendTextNode("trial2.xml");

}