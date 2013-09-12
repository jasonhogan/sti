/*! \file ConfigFile.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ConfigFile
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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <string>
#include <sstream>

#include "utils.h"

namespace STI
{
namespace Utils
{

class ConfigFile
{
public:

	ConfigFile(const std::string& filename);
	~ConfigFile();

	template <class T>
	bool getParameter(const std::string& name, T& value) const
	{
		std::string strValue;
		if( !getStringValue(name, strValue) )
			return false;
		return STI::Utils::stringToValue(strValue, value);
	}

	
	template <class T>
	bool setParameter(const std::string& name, T value)
	{
		return setStringValue(name, valueToString(value));
	}

	
	bool saveToDisk() const;
	void parse();
	void setHeader(const std::string& text);
	bool isParsed() const;

	std::string printParameters() const;

private:
	
	bool assignStringValue(std::string line);
	bool getStringValue(const std::string& name, std::string& value) const;
	bool setStringValue(const std::string& name, const std::string& value);

	std::string header;
	std::map<std::string, std::string> parameters;
	std::string filename_;
	bool parsed;

};

}
}

#endif

