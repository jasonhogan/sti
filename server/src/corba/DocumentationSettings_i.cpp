/*! \file DocumentationSettings_i.cpp
 *  \author Jonathan David Harvey
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DocumentationSettings_i
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

#include "DocumentationSettings_i.h"
#include <ConfigFile.h>
#include <utils.h>
#include <time.h>

DocumentationSettings_i::DocumentationSettings_i(std::string configFilename)
{
	ConfigFile config(configFilename);
	
	bool parseSuccess = config.getParameter("Documentation Base", documentationBaseAbsDir);
	parseSuccess &= config.getParameter("DTD Files", dtdFileAbsDir);
	parseSuccess &= config.getParameter("Timing Files", timingFilesRelDir);
	parseSuccess &= config.getParameter("Data Files", dataFilesRelDir);
	parseSuccess &= config.getParameter("Experiment Files", experimentFilesRelDir);
	parseSuccess &= config.getParameter("Sequence Files", sequenceFilesRelDir);
}

DocumentationSettings_i::~DocumentationSettings_i()
{
}

char* DocumentationSettings_i::DocumentationBaseAbsDir()
{
	CORBA::String_var dir( documentationBaseAbsDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::DocumentationBaseAbsDir(const char* _v)
{
	documentationBaseAbsDir.assign(_v);
}

char* DocumentationSettings_i::DTDFileAbsDir()
{
	CORBA::String_var dir( dtdFileAbsDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::DTDFileAbsDir(const char* _v)
{
	dtdFileAbsDir.assign(_v);
}

char* DocumentationSettings_i::TimingFilesRelDir()
{
	CORBA::String_var dir( timingFilesRelDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::TimingFilesRelDir(const char* _v)
{
	timingFilesRelDir.assign(_v);
}

char* DocumentationSettings_i::DataFilesRelDir()
{
	CORBA::String_var dir( dataFilesRelDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::DataFilesRelDir(const char* _v)
{
	dataFilesRelDir.assign(_v);
}

char* DocumentationSettings_i::ExperimentFilesRelDir()
{
	CORBA::String_var dir( experimentFilesRelDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::ExperimentFilesRelDir(const char* _v)
{
	experimentFilesRelDir.assign(_v);
}

char* DocumentationSettings_i::SequenceFilesRelDir()
{
	CORBA::String_var dir( sequenceFilesRelDir.c_str() );
	return dir._retn();
}

void DocumentationSettings_i::SequenceFilesRelDir(const char* _v)
{
	sequenceFilesRelDir.assign(_v);
}


std::string DocumentationSettings_i::getDocumentationBaseAbsDir() const
{
	return documentationBaseAbsDir;
}

std::string DocumentationSettings_i::getTodaysBaseAbsDir()
{
	std::string slash = STI::Utils::getNativePathSeparator();
	std::stringstream baseDir;

	time_t rawtime;
	time(&rawtime);
	tm* timeStruct = localtime(&rawtime);
 
	baseDir << documentationBaseAbsDir << slash
		<< (1900 + timeStruct->tm_year) << slash
		<< (timeStruct->tm_mon + 1) << slash
		<< (timeStruct->tm_mday) << slash;

	return baseDir.str();

}

std::string DocumentationSettings_i::getDTDFileAbsDir() const
{
	return dtdFileAbsDir;
}

std::string DocumentationSettings_i::getTimingFilesRelDir() const
{
	return timingFilesRelDir;
}

std::string DocumentationSettings_i::getDataFilesRelDir() const
{
	return dataFilesRelDir;
}

std::string DocumentationSettings_i::getExperimentFilesRelDir() const
{
	return experimentFilesRelDir;
}

std::string DocumentationSettings_i::getSequenceFilesRelDir() const
{
	return sequenceFilesRelDir;
}

