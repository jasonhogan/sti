/*! \file FieldConfigFile.h
 *  \author Susannah Dickerson (modified from ConfigFile by Jason Hogan)
 *  \brief Include-file for the class FieldConfigFile
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
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

#ifndef FIELDCONFIGFILE_H
#define FIELDCONFIGFILE_H

#include <map>
#include <string>
#include <sstream>
#include <vector>

class TaggedConfigFile
{
public:

	TaggedConfigFile(std::string filename, std::string beginKey = "begin", std::string endKey = "end");
	~TaggedConfigFile();

	void parse();
	bool getField(std::string fieldName, std::vector<std::string> &field);

private:

	std::map<std::string, std::vector<std::string> > fields;
	
	std::string filename_;
	std::string beginTag_;
	std::string endTag_;
	bool parsed;

	bool isBegin(std::string line, std::string &fieldName);
	bool isEnd(std::string line, std::string &fieldName);

};

#endif