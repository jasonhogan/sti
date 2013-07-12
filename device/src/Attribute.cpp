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


#include "Attribute.h"
#include <string>

#include <iostream>
using std::string;
using std::cerr;
using std::cout;
using std::endl;


using STI::Device::Attribute;

Attribute::Attribute()
{
	value_l = "";
}


Attribute::Attribute(const std::string& initialValue, const std::string& values)
{
	string::size_type comma = values.find_first_not_of(",", 0);
	string::size_type space = values.find_first_not_of(" ", 0);

	if(comma != string::npos && space != string::npos)
	{
		//Found something other than comma or space
		setAllowedValues(values);
	}

	if( isAllowed(initialValue) )
	{
		setValue(initialValue);
	}
	else
	{
		// Use first value in the allowed values list for the initial value.
		// There is at least one allowed value if isAllowed() == false.

		setValue(valuelist_l[0]);

		cerr << "Warning: Attribute initial value '" << initialValue 
			<< "' is not an allowed value." << endl;
	}
}

void Attribute::setAllowedValues(const std::string& values)
{
	string::size_type tBegin = 0;
	string::size_type tEnd = 0;

	// splits the string at every comma
	while(tEnd != string::npos)
	{
		tBegin = values.find_first_not_of(",", tEnd);
		tBegin = values.find_first_not_of(" ", tBegin); // eat initial spaces
		tEnd = values.find_first_of(",", tBegin);
		
		valuelist_l.push_back(values.substr(tBegin, tEnd - tBegin));
	}
}




const string& Attribute::value() const
{
	return value_l;
}


const std::vector<std::string>& Attribute::valuelist() const
{
	return valuelist_l;
}


void Attribute::setValue(const std::string& newValue)
{
	value_l = newValue;
}


bool Attribute::isAllowed(const std::string& value)
{
	bool allowed = false;
	unsigned i;
	
	for(i = 0; i < valuelist_l.size(); i++)
	{
		if(valuelist_l[i].compare(value) == 0) // strings are the same
			allowed = true;		// value found in allowed value list
	}

	if(valuelist_l.size() == 0)
	{
		//All values are allowed if no allowed value list is specified
		allowed = true;
	}

	return allowed;
}

void Attribute::printAllowedValues() const
{
	cout << "Allowed Values: ";
	for(unsigned i = 0; i < valuelist_l.size(); i++)
	{
		cout << valuelist_l[i] << " ";
	}
	cout << endl;
};

