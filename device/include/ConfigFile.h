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

class ConfigFile
{
public:

	ConfigFile(std::string filename);
	~ConfigFile();

	template <class T>
	bool getParameter(std::string name, T& value) const
	{
		std::string strValue;
		if( !getStringValue(name, strValue) )
			return false;
		return stringToValue(strValue, value);
	}

	
	template <class T>
	bool setParameter(std::string name, T value)
	{
		return setStringValue(name, valueToString(value));
	}

	
	bool saveToDisk();
	void parse();
	void setHeader(std::string text);

	bool isParsed() const { return parsed; }

	std::string printParameters();
	
	std::string getFilename() const { return filename_; }

private:
	
	bool assignStringValue(std::string line);
	bool getStringValue(std::string name, std::string &value) const;
	bool setStringValue(std::string name, std::string value);

	std::string header;
	std::map<std::string, std::string> parameters;
	std::string filename_;
	bool parsed;
	
	bool stringToValue(std::string inString, std::string& outValue, std::ios::fmtflags numBase=std::ios::dec) const
	{
		outValue = inString;
		return true;
	}

	template<typename T> bool stringToValue(std::string inString, T& outValue, std::ios::fmtflags numBase=std::ios::dec) const
	{
        //Returns true if the conversion is successful
        std::stringstream tempStream;
        tempStream.setf( numBase, std::ios::basefield );

        tempStream << inString;
        tempStream >> outValue;

        return !tempStream.fail();
	};

	template<typename T> std::string valueToString(T inValue, std::string Default="", std::ios::fmtflags numBase=std::ios::dec) const
	{
		std::string outString;
        std::stringstream tempStream;
        tempStream.setf( numBase, std::ios::basefield );

        tempStream << inValue;
		outString = tempStream.str();

        if( !tempStream.fail() )
			return outString;
		else
			return Default;
	};


};

#endif

