/*! \file ParsedDDSValue.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ParsedDDSValue
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

#include "ParsedDDSValue.h"

#include <sstream>

ParsedDDSValue::ParsedDDSValue() :
type(STI::Types::DDSNoChange)
{
}

ParsedDDSValue::~ParsedDDSValue()
{
}

void ParsedDDSValue::setValue(double number)
{
	type = STI::Types::DDSNumber;

	_number = number;
}

void ParsedDDSValue::setValue(double startValue, double endValue, double rampTime)
{
	type = STI::Types::DDSSweep;

	_startValue = startValue;
	_endValue   = endValue;
	_rampTime   = rampTime;
}

void ParsedDDSValue::setValueToNoChange()
{
	type = STI::Types::DDSNoChange;
}

TDDSType ParsedDDSValue::getType() const
{
	return type;
}

double ParsedDDSValue::getNumber() const
{
	return _number;
}

double ParsedDDSValue::getStartValue() const
{
	return _startValue;
}

double ParsedDDSValue::getEndValue() const
{
	return _endValue;
}

double ParsedDDSValue::getRampTime() const
{
	return _rampTime;
}

const std::string ParsedDDSValue::print() const
{
	std::stringstream result;
	if(type == STI::Types::DDSNoChange)
		result << "";
	else if(type == STI::Types::DDSNumber)
		result << getNumber();
	else if(type == STI::Types::DDSSweep)
		result << "(" << getStartValue()
		<< "," << getEndValue()
		<< "," << getRampTime() << ")";
	
	return result.str();
}

bool ParsedDDSValue::operator==(const ParsedDDSValue &other) const
{
	if( getType() != other.getType() )
		return false;
	
	bool equal = false;
	switch(getType())
	{
	case STI::Types::DDSNoChange:
		equal = true;
		break;
	case STI::Types::DDSNumber:
		equal = (getNumber() == other.getNumber());
		break;
	case STI::Types::DDSSweep:
		equal = (getStartValue() == other.getStartValue()) &&
			(getEndValue() == other.getEndValue()) &&
			(getRampTime() == other.getRampTime());
		break;
	}
	return equal;
}

