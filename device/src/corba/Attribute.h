/*! \file Attribute.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Attribute
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>


class Attribute
{
public:

	Attribute();
	Attribute(const std::string initialValue, const char* values[]=0);
	Attribute(const std::string initialValue, const std::string values=0);
	~Attribute();

	std::vector<std::string> const * valuelist() const;
	std::string value() const;
	void setValue(std::string newValue);


private:

	void setAllowedValues(const std::string values);

	std::string value_l;
	std::vector<std::string> valuelist_l;
};

#endif
