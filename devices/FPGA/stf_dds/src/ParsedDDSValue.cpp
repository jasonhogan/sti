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
	
ParsedDDSValue::ParsedDDSValue(double value) : errorOccured(false)
{
	setValue(value);
}

ParsedDDSValue::ParsedDDSValue(const MixedValue& value) : errorOccured(false)
{
	setValue(value);
}

ParsedDDSValue::~ParsedDDSValue()
{
}

bool ParsedDDSValue::parsingErrorOccured() const
{
	return errorOccured;
}

std::string ParsedDDSValue::errorMessage() const
{
	return errMsg;
}

void ParsedDDSValue::setValue(double number)
{
	errorOccured = false;
	type = DDSNumber;

	_number = number;
}

void ParsedDDSValue::setValue(const MixedValue& value)
{
	errorOccured = false;

	switch(value.getType())
	{
	case MixedValue::Double:
		setValue( value.getDouble() );
		break;
	case MixedValue::Vector:
		if(value.getVector().size() != 3)
		{
			errorOccured = true;
			errMsg = "A DDS sweep tuple must have exactly 3 elements.";
		}
		else
		{
			for(unsigned i = 0; i < value.getVector().size(); i++)
			{
				if(value.getVector().at(i).getType() != MixedValue::Double)
				{
					errorOccured = true;
					errMsg = "DDS sweep parameters must be doubles.";
					break;
				}
			}
			if( !errorOccured )
			{
				setValue(
					value.getVector().at(0).getDouble(),	//startValue
					value.getVector().at(1).getDouble(),	//endValue
					value.getVector().at(2).getDouble()		//rampTime
					);
			}
		}
		break;
	default:
		errorOccured = true;
		errMsg = "A DDS value must be a double or a 3-tuple.";
		break;
	}
}


void ParsedDDSValue::setValue(double startValue, double endValue, double rampTime)
{
	errorOccured = false;
	type = DDSSweep;

	_startValue = startValue;
	_endValue   = endValue;
	_rampTime   = rampTime;
}

void ParsedDDSValue::setValueToNoChange()
{
	errorOccured = false;
	type = DDSNoChange;
}

ParsedDDSValue::TDDSType ParsedDDSValue::getType() const
{
	return type;
}

MixedValue ParsedDDSValue::getMixedValue() const
{
	MixedValue val;
	
	switch(getType())
	{
	case ParsedDDSValue::DDSNumber:
		val.setValue( getNumber() );
		break;
	case ParsedDDSValue::DDSNoChange:
		val.setValue(true);
		break;
	case ParsedDDSValue::DDSSweep:
		val.addValue( getStartValue() );
		val.addValue( getEndValue() );
		val.addValue( getRampTime() );
		break;
	default:
		break;
	}

	return val;
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
	if(type == DDSNoChange)
		result << "";
	else if(type == DDSNumber)
		result << getNumber();
	else if(type == DDSSweep)
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
	case DDSNoChange:
		equal = true;
		break;
	case DDSNumber:
		equal = (getNumber() == other.getNumber());
		break;
	case DDSSweep:
		equal = (getStartValue() == other.getStartValue()) &&
			(getEndValue() == other.getEndValue()) &&
			(getRampTime() == other.getRampTime());
		break;
	}
	return equal;
}

