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
	
//	fs::path sequencePath(absBaseDir);
	sequencePath = absBaseDir;
	sequencePath /= docSettings->getSequenceFilesRelDir();
//	sequencePath /=
	fs::create_directories(sequencePath);

//	docSettings->getSequenceFilesRelDir();
	sequenceFileAbsPath = sequencePath.native_directory_string();


		
	std::string dtdDir = docSettings->getDTDFileAbsDir();
	std::string dtdRelDir = STI::Utils::getRelativePath(dtdDir, sequencePath.native_file_string());
	//fs::path dtdPath(dtdRelDir + STI::Utils::getNativePathSeparator() + "experiment.dtd");
	fs::path dtdPath = dtdRelDir;
	dtdPath /= "series.dtd";

	xmlManager.createDocument("series", STI::Utils::convertPathToURL(dtdPath.native_file_string()), "series");

	generateTimeStamp();

	filename = STI::Utils::generateUniqueTimeBasedFileName(timeStamp, ".xml", sequencePath);

	sequenceFilePath = sequencePath / filename;

	isFirstExperiment = true;
	timingSubdirectory = "";

	buildDocument();
}

SequenceDocumenter::~SequenceDocumenter()
{
}

void SequenceDocumenter::generateTimeStamp()
{
	time_t rawtime;
	time(&rawtime);
	
	timeStamp = localtime(&rawtime);
}

void SequenceDocumenter::addExperiment(RemoteDeviceMap& devices, const std::map<std::string, std::string>& overwrittenVars)
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

	addExperimentToXML( documenter.getAbsExpFilepath(), overwrittenVars );

	documenter.writeToDisk();
	
	if(isFirstExperiment)
	{
		documenter.copyTimingFiles();
	}
	isFirstExperiment = false;

}
void SequenceDocumenter::addExperimentToXML(fs::path experimentXMLAbsPath, const std::map<std::string, std::string>& overwrittenVars)
{
	DOMNodeWrapper* newExperiment = experimentsRoot->appendChildElement("experiment");

	std::map<std::string, std::string>::const_iterator var = overwrittenVars.begin();

	for(std::map<std::string, std::string>::const_iterator var = overwrittenVars.begin();
		var != overwrittenVars.end(); var++)
	{
		newExperiment->appendChildElement("var")
			->setAttribute("name", var->first )
			->setAttribute("value", var->second );
	}

	newExperiment->appendChildElement("file")->appendTextNode(
		STI::Utils::convertPathToURL(
		STI::Utils::getRelativePath(experimentXMLAbsPath, sequencePath))
		);
}

void SequenceDocumenter::writeDirectoryStructureToDisk()
{
}


void SequenceDocumenter::createSequenceXML()
{
}

void SequenceDocumenter::writeToDisk()
{
	xmlManager.PrintDocumentToFile(sequenceFilePath.native_directory_string());
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
	root->appendChildElement("title")->appendTextNode(
		STI::Utils::getFilenameNoExtension(filename)
		);
	root->appendChildElement("date")->appendTextNode( asctime(timeStamp) );
	
	//Sequence description (optional)
	std::string description = documentationSettings->getSequenceDescription();
	if( description.length() > 0 )
		root->appendChildElement("description")->appendTextNode(description);

	//Loop script (optional)
	std::string script = parser->getLoopScript();
	if( script.length() > 0 )
		root->appendChildElement("script")->appendTextNode(script);

	experimentsRoot = root->appendChildElement("experiments");

//	DOMNodeWrapper* experimentsRoot = root->appendChildElement("experiments");
//	experimentsRoot->appendChildElement("experiment")->appendChildElement("file")->appendTextNode("trial1.xml");
//	experimentsRoot->appendChildElement("experiment")->appendChildElement("file")->appendTextNode("trial2.xml");

}