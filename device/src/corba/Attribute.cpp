/*! \file Attribute.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Attribute
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
#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif

#include "Attribute.h"
#include <string>
using std::string;

#include <iostream>
using namespace std;


Attribute::Attribute()
{
}

Attribute::Attribute(const std::string initialValue, const char* values[])
{
	cerr << "Attribute Constructor 1 size : " << sizeof(values) << " -> ";

	// Warning, this doesn't actually work!! sizeof(values) is not the size of the array.
	for(int i = 0; i < sizeof(values) / sizeof(char*); i++)
	{
		cerr << i << " = " << values[i] << ", ";
		valuelist_l.push_back(values[i]);
	}

	cerr << endl;

	setValue(initialValue);
}

Attribute::Attribute(const std::string initialValue, const std::string values)
{
	cerr << "Attribute Constructor 2: " << values << endl;
	setValue(initialValue);
	setAllowedValues(values);
}

void Attribute::setAllowedValues(const std::string values)
{
	string::size_type tBegin = 0; 
	string::size_type tEnd = 0;

	// splits the sting at every comma
	while(tEnd != string::npos)
	{
		tBegin = values.find_first_not_of(",", tEnd);
		tEnd = values.find_first_of(",", tBegin);

		valuelist_l.push_back(values.substr(tBegin, tEnd - tBegin));
	}
}

Attribute::~Attribute()
{
}

string Attribute::value() const
{
	return value_l;
}

const std::vector<std::string> * Attribute::valuelist() const
{
	return &valuelist_l;
}

void Attribute::setValue(std::string newValue)
{
	value_l = newValue;
}

bool Attribute::isAllowed(std::string value)
{
	bool found = false;
	int i;
	for(i = 0; i < valuelist_l.size(); i++)
	{
		if(valuelist_l[i].compare(value) == 0) // strings are the same
			found = true;
	}
	return found;
}
