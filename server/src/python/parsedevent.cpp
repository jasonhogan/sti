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

/*! \param[in] channel  The initial value for #channel.
 *  \param[in] time     The initial value for #time.
 *  \param[in] number   The initial value for #f_value.number.
 *  \param[in] position The initial value for #position.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time, double number,
                         const ParsedPos &position)
    : f_type(NumberEvent), channel(channel), time(time), position(position)
{
    f_value_number[0] = number; 
}

/*! \param[in] channel  The initial value for #channel.
 *  \param[in] time     The initial value for #time.
 *  \param[in] text     The initial value for #f_value.text.
 *  \param[in] position The initial value for #position.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time,
                         const std::string &text, const ParsedPos &position)
    : f_type(TextEvent), f_value_string(text), channel(channel), time(time),
      position(position)
{
}

/*! \param[in] channel  The initial value for #channel.
 *  \param[in] time     The initial value for #time.
 *  \param[in] freq     The initial value for #f_value.dds[0].
 *  \param[in] phase    The initial value for #f_value.dds[1].
 *  \param[in] ampl     The initial value for #f_value.dds[2].
 *  \param[in] position The initial value for #position.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time, double freq,
                         double phase, double ampl, const ParsedPos &position)
    : f_type(DDSEvent), channel(channel), time(time), position(position)
{
    f_value_number[0] = freq;
    f_value_number[1] = phase;
    f_value_number[2] = ampl;
}

/*! \param[in] channel  The initial value for #channel.
 *  \param[in] time     The initial value for #time.
 *  \param[in] desc     The initial value for #f_value.desc.
 *  \param[in] position The initial value for #position.
 *
 *  The differing order of arguments distinguishes this from the constructor
 *  for a TextEvent.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time,
                         const ParsedPos &position, const std::string &desc)
    : f_type(MeasureEvent), f_value_string(desc), channel(channel), time(time),
      position(position)
{
}

ParsedEvent::~ParsedEvent()
{
}

/*! \return The type of this event from #f_type.
 */
EventType
ParsedEvent::type() const
{
    return f_type;
}

/*! \return A reference to #f_value_number[0].
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type NumberEvent.
 */
double &
ParsedEvent::number()
{
    assert(f_type == NumberEvent);

    return f_value_number[0];
}

/*! \return A reference to #f_value_string.
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type TextEvent.
 */
std::string &
ParsedEvent::text()
{
    assert(f_type == TextEvent);

    return f_value_string;
}

/*! \param[in] n Selects which element to get.
 *  \return A reference to #f_value_number[n].
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type DDSEvent. The
 *  value of \a n must be one of:
 *  - 0: Frequency
 *  - 1: Phase
 *  - 2: Amplitude
 */
double &
ParsedEvent::dds(unsigned short n)
{
    assert(f_type == DDSEvent);
    assert(n < 3);

    return f_value_number[n];
}

/*! \return A reference to #f_value_number[0] (the frequency).
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double &
ParsedEvent::freq()
{
    assert(f_type == DDSEvent);

    return f_value_number[0];
}

/*! \return A reference to #f_value_number[1] (the phase).
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double &
ParsedEvent::phase()
{
    assert(f_type == DDSEvent);

    return f_value_number[1];
}

/*! \return A reference to #f_value_number[2] (the amplitude).
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double &
ParsedEvent::ampl()
{
    assert(f_type == DDSEvent);

    return f_value_number[2];
}

/*! \return A reference to #f_value_string.
 *
 *  Returns a reference, this allows to assign a new value to this reference.
 *  It is only allowed to call this method for events of type MeasureEvent.
 */
std::string &
ParsedEvent::desc()
{
    assert(f_type == MeasureEvent);

    return f_value_string;
}

/*! \return A string representation of this events value.
 *
 *  To distinguish TextEvent and MeasureEvent, the string for a TextEvent
 *  is given with double-quotes, while the description of a MeasureEvent is
 *  given as \a meas("desc"). For DDSEvent, the string is a tuple enclosed by
 *  brackets. All numbers follow the default formating of std::StringStream.
 */
std::string
ParsedEvent::value() const
{
    stringstream buf;

    switch(f_type) {
    case NumberEvent:
        buf << f_value_number;
        break;
    case TextEvent:
        buf << "\"" << f_value_string << "\"";
        break;
    case DDSEvent:
        buf << "(" << f_value_number[0] << ", " << f_value_number[1] << ", "
            << f_value_number[2] << ")";
        break;
    default:  //This must be MeasureEvent
        buf << "meas(\"" << f_value_string << "\")";
        break;
    };

    return buf.str();
}

};
