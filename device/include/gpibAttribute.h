/*! \file gpibAttribute.h
 *  \author David Johnson
 *  \brief Include-file for the class gpibAttribute
 *  \section license License
 *
 *  Copyright (C) 2010 David Johnson <david.m.johnson@stanford.edu>\n
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

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>

#ifndef GPIB_ATTRIBUTE_H
#define GPIB_ATTRIBUTE_H

class gpibAttribute
{
public:
	gpibAttribute();
	gpibAttribute(std::string command, std::string allowed, bool readOnly);
	~gpibAttribute();

	std::string stringValue;
	std::string gpibCommand;
	std::string allowedValues;
	double value;
	bool isReadOnly;

};
#endif

