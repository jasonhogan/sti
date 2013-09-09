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

#include <ConfigFile.h>
#include <fstream>
#include <iostream>

using STI::Utils::ConfigFile;

ConfigFile::ConfigFile(const std::string& filename) :
filename_(filename),
parsed(false)
{
	header = "";
	parse();
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::setHeader(const std::string& text)
{
	header = text;
}

bool ConfigFile::saveToDisk() const
{
	std::fstream configFile(filename_.c_str(), std::fstream::out);

	if( !configFile.is_open() )
	{
		return false;
	}

	if(header.compare("") != 0)
	{
		configFile << header << std::endl;
	}

	configFile << printParameters();

	return true;
}

void ConfigFile::parse()
{
	parameters.clear();

	std::fstream configFile(filename_.c_str(), std::fstream::in);

	if( !configFile.is_open() )
	{
		parsed = false;
		std::cerr << "Error opening config file '" << filename_ << "'." << std::endl;
		return;
	}

	std::string line;
	bool success = true;
	std::size_t commentLoc;
	parsed = true;	//unless there's a problem

    while( success && getline(configFile, line) )
	{
		commentLoc = line.find_first_of("#");
		success = assignStringValue( line.substr(0, commentLoc) );
	}

	if( !success )
	{
		std::cerr << "Error parsing config file '" << filename_ << "' at line" << std::endl
			<< ">>> " << line << std::endl;
	}

	parsed = success;
	configFile.close();
}


bool ConfigFile::isParsed() const
{
	return parsed;
}

std::string ConfigFile::printParameters() const
{
	std::stringstream printout;

	for(std::map<std::string, std::string>::const_iterator it = parameters.begin(); it != parameters.end(); it++)
	{
		printout << it->first << " = " << it->second << std::endl;
	}

	return printout.str();
}


bool ConfigFile::assignStringValue(std::string line)
{
	std::size_t nameStart = line.find_first_not_of(" ");

	if(line.length() == 0 || nameStart == std::string::npos )	//blank line
		return true;

	std::size_t equalsLoc = line.find_first_of("=");

	if(equalsLoc < 1 || equalsLoc == std::string::npos || equalsLoc == nameStart)
		return false;

	std::size_t nameEnd = line.find_last_not_of(" ", equalsLoc - 1);

	if(equalsLoc == line.length() - 1)
		line.append("");

	std::size_t valueStart = line.find_first_not_of(" ", equalsLoc + 1);

	if(valueStart == std::string::npos)
		valueStart = equalsLoc + 1;

	return setStringValue( line.substr(nameStart, nameEnd + 1), line.substr(valueStart) );
}

bool ConfigFile::getStringValue(const std::string& name, std::string& value) const
{
	if( !parsed )
		return false;

	std::map<std::string, std::string>::const_iterator param = parameters.find(name);

	if(param == parameters.end())
	{
		return false;
	}
	else
	{
		value = param->second;
		return true;
	}
}

bool ConfigFile::setStringValue(const std::string& name, const std::string& value)
{
	if( !parsed )
		return false;

	parameters[name] = value;

	return true;
}

