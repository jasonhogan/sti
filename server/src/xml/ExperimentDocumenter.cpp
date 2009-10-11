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

#include <time.h>

#include <iostream>

using std::string;

ExperimentDocumenter::ExperimentDocumenter(const STI::Types::TExpRunInfo& info)
{
	xmlManager.createDocument("experiment", "experiment.dtd", "experiment");

	buildDocument(info);
}


ExperimentDocumenter::~ExperimentDocumenter()
{
}

void ExperimentDocumenter::buildDocument(const STI::Types::TExpRunInfo& info)
{
	
	DOMNodeWrapper* root = xmlManager.getRootNode();
	root->appendChildElement("title")
		->appendTextNode( getFilenameNoExtension(std::string(info.filename)) );
	root->appendChildElement("date")
		->appendTextNode( getDateAndTime() );

	if(info.isSequenceMember)
	{
		root->appendChildElement("series")->appendChildElement("file")
			->appendTextNode( std::string(info.sequenceRelativePath) );
	}

	string description(info.description);
	if(description.length() > 0)
	{
		root->appendChildElement("description")
			->appendTextNode( std::string(info.description) );
	}

	DOMNodeWrapper* timingRoot = root->appendChildElement("timing");
	timingRoot->appendChildElement("file")->appendTextNode("timing.py");
	timingRoot->appendChildElement("file")->appendTextNode("channels.py");




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
	std::string xmlDocument = xmlManager.getDocumentAsString();

	std::cout << "ExperimentDocumenter: " << std::endl;
	std::cout << xmlDocument << std::endl;
}
