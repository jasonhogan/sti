/*! \file DocumentationSettings_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DocumentationSettings_i
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

#ifndef DOCUMENTATIONSETTINGS_I_H
#define DOCUMENTATIONSETTINGS_I_H

#include "client.h"
#include <string>


class DocumentationSettings_i : public POA_STI::Client_Server::DocumentationSettings
{
public:

	DocumentationSettings_i(std::string configFilename);
	~DocumentationSettings_i();

	char* DocumentationBaseAbsDir();
	void DocumentationBaseAbsDir(const char* _v);
	char* DTDFileAbsDir();
	void DTDFileAbsDir(const char* _v);
	char* TimingFilesRelDir();
	void TimingFilesRelDir(const char* _v);
	char* DataFilesRelDir();
	void DataFilesRelDir(const char* _v);
	char* ExperimentFilesRelDir();
	void ExperimentFilesRelDir(const char* _v);
	char* SequenceFilesRelDir();
	void SequenceFilesRelDir(const char* _v);

	std::string getDocumentationBaseAbsDir() const;
	std::string getDTDFileAbsDir() const;
	std::string getTimingFilesRelDir() const;
	std::string getDataFilesRelDir() const;
	std::string getExperimentFilesRelDir() const;
	std::string getSequenceFilesRelDir() const;

	std::string getTodaysBaseAbsDir();

private:

	std::string documentationBaseAbsDir;
	std::string dtdFileAbsDir;
	std::string timingFilesRelDir;
	std::string dataFilesRelDir;
	std::string experimentFilesRelDir;
	std::string sequenceFilesRelDir;

};

#endif