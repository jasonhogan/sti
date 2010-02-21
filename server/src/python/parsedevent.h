/*! \file
 *  \author Olaf Mandel
 *  \author Jason Hogan
 *  \brief Include-file for the class ParsedEvent
 *  \section license License
 *
 *  Copyright (C) 2008 Olaf Mandel <mandel@stanford.edu>\n
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

#ifndef PARSEDEVENT_H
#define PARSEDEVENT_H

#include <string>
#include "parsedpos.h"

#include <MixedValue.h>

namespace libPython
{

/*! \brief The ParsedEvent class represents information for one timing event
 *
 * This class is instantiated by the event() and meas() functions of the
 * \link timing_module Timing module\endlink.
 */
class ParsedEvent
{

public:

    template<class T> 
	ParsedEvent(unsigned channel, double time, const T& value, const ParsedPos &position, bool isMeasurement=false)//For non-measurement events and measurement events with no (explicit) description
		: time(time), channel(channel), position(position), value(value), measureEvent(isMeasurement)
	{
		setDesc("");
	}
    template<class T> 
	ParsedEvent(unsigned channel, double time, const T& value, const ParsedPos &position, std::string desc)//only used for constructing measurement events (with description)
		: time(time), channel(channel), position(position), value(value), measureEvent(true), description(desc)
	{
	}


 //   ParsedEvent(unsigned channel, double time, double number, const ParsedPos &position);
 //   ParsedEvent(unsigned channel, double time, const std::string& text, const ParsedPos &position);
	//ParsedEvent(unsigned channel, double time, const MixedValue& value, const ParsedPos &position);
 //   ParsedEvent(unsigned channel, double time, const ParsedPos &position, const std::string& desc);
    ~ParsedEvent();


    void setNumber(double number);
    void setText(const std::string& text);
 	void setDesc(const std::string &desc);

	MixedValue::MixedValueType type() const;
	double number() const;
    std::string text() const;
	const STI::Types::TValMixed getValue() const;
	std::string desc() const;
	bool isMeasureEvent() const;
 
	/*! \brief Convenience-function to get string representation of value */
    std::string print() const;
    /*! \brief Comparision that for everything but position */
    bool nearlyEqual(const ParsedEvent& rhs) const;

    double    time;
    unsigned  channel;
    ParsedPos position;

private:
    
	/*! \brief The type of this event
     *
     *  This determines, which element, #f_value_number or f_value_text is
     *  valid. It is not possible to change the type of an event once it has
     *  been defined.
     */
	MixedValue value;
	bool measureEvent;
	std::string description;
};

};

#endif
