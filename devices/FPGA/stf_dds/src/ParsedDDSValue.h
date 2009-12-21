/*! \file ParsedDDSValue.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ParsedDDSValue
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


#ifndef PARSEDDDSVALUE_H
#define PARSEDDDSVALUE_H

#include <string>
#include <MixedValue.h>

class ParsedDDSValue
{
public:

	ParsedDDSValue(double value);
	ParsedDDSValue(const MixedValue& value);
	~ParsedDDSValue();

	enum TDDSType {DDSNumber, DDSSweep, DDSNoChange};

	bool operator==(const ParsedDDSValue &other) const;

	void setValue(double number);
	void setValue(const MixedValue& value);
	void setValue(double startValue, double endValue, double rampTime);
	void setValueToNoChange();

	TDDSType getType() const;

	double getNumber() const;

	double getStartValue() const;
	double getEndValue() const;
	double getRampTime() const;

	const std::string print() const;

	bool parsingErrorOccured() const;
	std::string errorMessage() const;

	MixedValue getMixedValue() const;

private:

	TDDSType type;

	double _number;

	double _startValue;
	double _endValue;
	double _rampTime;

	std::string errMsg;
	bool errorOccured;
};

#endif
