/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class ParsedEvent
 *  \section license License
 *
 *  Copyright (C) 2008 Olaf Mandel <mandel@stanford.edu>\n
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "parsedevent.h"
#include <cassert>
#include <sstream>

using std::string;
using std::stringstream;

namespace libPython
{

///*! \param[in] channel  The initial value for #channel.
// *  \param[in] time     The initial value for #time.
// *  \param[in] number   The initial value for #f_value_number.
// *  \param[in] position The initial value for #position.
// */
//ParsedEvent::ParsedEvent(unsigned channel, double time, double number, const ParsedPos &position)
//: time(time), channel(channel), position(position), measureEvent(false)
//{
//    value.setValue(number);
//}
//
///*! \param[in] channel  The initial value for #channel.
// *  \param[in] time     The initial value for #time.
// *  \param[in] text     The initial value for #f_value_string.
// *  \param[in] position The initial value for #position.
// */
//ParsedEvent::ParsedEvent(unsigned channel, double time, const std::string& text, const ParsedPos &position)
//: time(time), channel(channel), position(position), measureEvent(false)
//{
//    value.setValue(text);
//}
//
///*! \param[in] channel  The initial value for #channel.
// *  \param[in] time     The initial value for #time.
// *  \param[in] desc     The initial value for #f_value_string.
// *  \param[in] position The initial value for #position.
// *
// *  The differing order of arguments distinguishes this from the constructor
// *  for a TextEvent.
// */
//ParsedEvent::ParsedEvent(unsigned channel, double time, const MixedValue& value, const ParsedPos &position)
//: time(time), channel(channel), position(position), measureEvent(false)
//
//{
//    this->value = value;
//}
//
//ParsedEvent::ParsedEvent(unsigned channel, double time, const ParsedPos &position, const std::string& desc)
//: time(time), channel(channel), position(position), measureEvent(true)
//{
//    value.setValue(desc);
//}

ParsedEvent::~ParsedEvent()
{
}


const STI::Types::TValMixed ParsedEvent::getValue() const
{
	return value.getTValMixed();
}

double ParsedEvent::number() const
{
    return value.getNumber();
}

void ParsedEvent::setNumber(double number)
{
    value.setValue(number);
}

 std::string ParsedEvent::text() const
{
    return value.getString();
}

bool ParsedEvent::isMeasureEvent() const
{
	return measureEvent;
}

void ParsedEvent::setText(const std::string& text)
{
    value.setValue(text);
}

std::string ParsedEvent::desc() const
{
	return description;
}

void ParsedEvent::setDesc(const std::string& desc)
{
	description = desc;
}

/*! \return A string representation of this events value.
 *
 *  To distinguish TextEvent and MeasureEvent, the string for a TextEvent
 *  is given with double-quotes, while the description of a MeasureEvent is
 *  given as \a meas("desc"). For DDSEvent, the string is a tuple enclosed by
 *  brackets. All numbers follow the default formating of std::StringStream.
 */
std::string ParsedEvent::print() const
{
    stringstream buf;

	if(measureEvent)
	{
		buf << "meas('" << value.print()  << ", " << description << "')";
	}
	else
	{
		buf << value.print();
	}

    return buf.str();
}

/*! \param[in] rhs The rhs of the comparision.
 *  \return \a True if all properties except #position match, \a False
 *          otherwise.
 *
 *  This behaves nearly equal to operator==(rhs), but the value of #position
 *  is ignored. The comparison utilizes the fact that the unused entries
 *  of #f_value_number and #f_value_string are alvays initialized to the
 *  same default.
 */
bool ParsedEvent::nearlyEqual(const ParsedEvent& rhs) const
{
	return (channel == rhs.channel && time == rhs.time && measureEvent == rhs.measureEvent && value == rhs.value);
}

};
