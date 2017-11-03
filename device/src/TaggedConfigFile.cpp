/*! \file ConfigFile.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ConfigFile
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

#include "TaggedConfigFile.h"
#include <fstream>
#include <iostream>


TaggedConfigFile::TaggedConfigFile(std::string filename, std::string beginTag, std::string endTag) :
filename_(filename), beginTag_(beginTag), endTag_(endTag),
parsed(false)
{
	parse();
}

TaggedConfigFile::~TaggedConfigFile()
{
}

void TaggedConfigFile::parse()
{
	// Nabbed from ConfigFile.cpp, parse()
	std::fstream configFile(filename_.c_str(), std::fstream::in);

	if( !configFile.is_open() )
	{
		parsed = false;
		std::cerr << "Error opening FieldConfig file '" << filename_ << "'." << std::endl;
		return;
	}

	std::string line;
	bool success = true;
	std::size_t commentLoc;
	bool recording = false;
	parsed = true;	//unless there's a problem
	
	std::string fieldNameBegin = "";
	std::string fieldNameEnd = "";
	std::vector <std::string> field;

    while( success && getline(configFile, line) )
	{
		if (isEnd(line, fieldNameEnd)) {
			recording = false;
			if (fieldNameEnd.compare(fieldNameBegin) != 0)
			{
				success = false;
				break;
			}
			else 
			{
				fields.insert (std::pair<std::string, std::vector <std::string> > (fieldNameBegin, field) );
				fieldNameBegin = "";
				fieldNameEnd = "";
				field.clear();
			}
		}
		else if (recording == true)
		{
			commentLoc = line.find_first_of("#");
			if (commentLoc != 0)
				field.push_back( line.substr(0,commentLoc) );
		}
		else if (isBegin(line, fieldNameBegin))
			recording = true;
	}

	if( !success )
	{
		std::cerr << "Error parsing config file '" << filename_ << "' at line" << std::endl
			<< ">>> " << line << std::endl;
	}

	parsed = success;
	configFile.close();
}



bool TaggedConfigFile::isBegin(std::string line, std::string &fieldName)
{
	std::size_t fieldNameStart;

	//remove inline comments
	std::size_t commentLoc = line.find_first_of("#");
	std::string code = line.substr(0, commentLoc); 

	std::size_t beginLoc = code.find(beginTag_);
	if (beginLoc != std::string::npos) {
		fieldNameStart = code.find_first_not_of(" ", beginLoc + beginTag_.size());
		fieldName = code.substr(fieldNameStart);
		return true;
	}
	else
		return false;
}

bool TaggedConfigFile::isEnd(std::string line, std::string &fieldName)
{
	std::size_t fieldNameStart;

	//remove inline comments
	std::size_t commentLoc = line.find_first_of("#");
	std::string code = line.substr(0, commentLoc); 

	std::size_t endLoc = code.find(endTag_);
	if (endLoc != std::string::npos) {
		fieldNameStart = code.find_first_not_of(" ", endLoc + endTag_.size());
		fieldName = code.substr(fieldNameStart);
		return true;
	}
	else
		return false;

}

bool TaggedConfigFile::getField(std::string fieldName, std::vector <std::string> &field)
{
	field.clear();
	if (!parsed)
		return false;

	std::map<std::string, std::vector<std::string> >::iterator it = fields.find(fieldName);

	if (it != fields.end())
	{
		field = it->second;
		return true;
	}
	else
		return false;
}

bool TaggedConfigFile::writeField(std::string fieldName, std::vector<std::string> fieldContents)
{
	std::string line;
	bool success = true;

	std::string fieldNameBegin = "";
	std::string fieldNameEnd = "";
	std::vector <std::string> file;

	std::ifstream oldConfigFile(filename_.c_str());

	if( !oldConfigFile.is_open() )
	{
		std::cerr << "Error opening FieldConfig file '" << filename_ << "'." << std::endl;
		return false;
	}

	while( getline(oldConfigFile, line) )
	{		
		if (isBegin(line, fieldNameBegin))
		{
			file.push_back(line);
			if (fieldNameBegin == fieldName)
			{
				//insert new field contents
				file.insert(file.end(), fieldContents.begin(), fieldContents.end());
				//search file for end of field, ignoring lines.
				while(getline(oldConfigFile, line))
				{
					if (isEnd(line, fieldNameEnd)) {
						file.push_back(line);
						break;
					}
				}
			}

		}
		else
			file.push_back(line);
	}

	oldConfigFile.close();

	std::ofstream newConfigFile(filename_.c_str());

	if (newConfigFile.is_open()) {
		for(unsigned int i = 0; i < file.size(); i++)
			newConfigFile << file.at(i) << "\n";
	}
	else
		return false;

	newConfigFile.close();

	return true;
}