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
 *  \param[in] number   The initial value for #f_value_number[0].
 *  \param[in] position The initial value for #position.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time, double number,
                         const ParsedPos &position)
    : f_type(NumberEvent), f_value_string(), channel(channel), time(time),
      position(position)
{
    f_value_number[0] = number; 
    f_value_number[1] = 0; 
    f_value_number[2] = 0; 
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
    f_value_number[0] = 0; 
    f_value_number[1] = 0; 
    f_value_number[2] = 0; 
}

/*! \param[in] channel  The initial value for #channel.
 *  \param[in] time     The initial value for #time.
 *  \param[in] freq     The initial value for #f_value_number[0].
 *  \param[in] phase    The initial value for #f_value_number[1].
 *  \param[in] ampl     The initial value for #f_value_number[2].
 *  \param[in] position The initial value for #position.
 */
ParsedEvent::ParsedEvent(unsigned channel, double time, double freq,
                         double phase, double ampl, const ParsedPos &position)
    : f_type(DDSEvent), f_value_string(), channel(channel), time(time),
      position(position)
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
    f_value_number[0] = 0; 
    f_value_number[1] = 0; 
    f_value_number[2] = 0; 
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

/*! \return #f_value_number[0].
 *
 *  It is only allowed to call this method for events of type NumberEvent.
 */
double
ParsedEvent::number() const
{
    assert(f_type == NumberEvent);

    return f_value_number[0];
}

/*!
 *  It is only allowed to call this method for events of type NumberEvent.
 */
void
ParsedEvent::setNumber(double number)
{
    assert(f_type == NumberEvent);

    f_value_number[0] = number;
}

/*! \return #f_value_string.
 *
 *  It is only allowed to call this method for events of type TextEvent.
 */
const std::string &
ParsedEvent::text() const
{
    assert(f_type == TextEvent);

    return f_value_string;
}

/*!
 *  It is only allowed to call this method for events of type TextEvent.
 */
void
ParsedEvent::setText(const std::string &text)
{
    assert(f_type == TextEvent);

    f_value_string = text;
}

/*! \param[in] n Selects which element to get.
 *  \return #f_value_number[n].
 *
 *  It is only allowed to call this method for events of type DDSEvent. The
 *  value of \a n must be one of:
 *  - 0: Frequency
 *  - 1: Phase
 *  - 2: Amplitude
 */
double
ParsedEvent::dds(unsigned short n) const
{
    assert(f_type == DDSEvent);
    assert(n < 3);

    return f_value_number[n];
}

/*!
 *  It is only allowed to call this method for events of type DDSEvent. The
 *  value of \a n must be one of:
 *  - 0: Frequency
 *  - 1: Phase
 *  - 2: Amplitude
 */
void
ParsedEvent::setDds(unsigned short n, double value)
{
    assert(f_type == DDSEvent);
    assert(n < 3);

    f_value_number[n] = value;
}

/*! \return #f_value_number[0] (the frequency).
 *
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double
ParsedEvent::freq() const
{
    assert(f_type == DDSEvent);

    return f_value_number[0];
}

/*!
 *  It is only allowed to call this method for events of type DDSEvent.
 */
void
ParsedEvent::setFreq(double freq)
{
    assert(f_type == DDSEvent);

    f_value_number[0] = freq;
}

/*! \return #f_value_number[1] (the phase).
 *
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double
ParsedEvent::phase() const
{
    assert(f_type == DDSEvent);

    return f_value_number[1];
}

/*!
 *  It is only allowed to call this method for events of type DDSEvent.
 */
void
ParsedEvent::setPhase(double phase)
{
    assert(f_type == DDSEvent);

    f_value_number[1] = phase;
}

/*! \return #f_value_number[2] (the amplitude).
 *
 *  It is only allowed to call this method for events of type DDSEvent.
 */
double
ParsedEvent::ampl() const
{
    assert(f_type == DDSEvent);

    return f_value_number[2];
}

/*!
 *  It is only allowed to call this method for events of type DDSEvent.
 */
void
ParsedEvent::setAmpl(double ampl)
{
    assert(f_type == DDSEvent);

    f_value_number[2] = ampl;
}

/*! \return #f_value_string.
 *
 *  It is only allowed to call this method for events of type MeasureEvent.
 */
const std::string &
ParsedEvent::desc() const
{
    assert(f_type == MeasureEvent);

    return f_value_string;
}

/*!
 *  It is only allowed to call this method for events of type MeasureEvent.
 */
void
ParsedEvent::setDesc(const std::string &desc)
{
    assert(f_type == MeasureEvent);

    f_value_string = desc;
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

/*! \param[in] rhs The rhs of the comparision.
 *  \return \a True if all properties except #position match, \a False
 *          otherwise.
 *
 *  This behaves nearly equal to operator==(rhs), but the value of #position
 *  is ignored. The comparison utilizes the fact that the unused entries
 *  of #f_value_number and #f_value_string are alvays initialized to the
 *  same default.
 */
bool
ParsedEvent::nearlyEqual(const ParsedEvent &rhs) const
{
    return channel == rhs.channel && time == rhs.time && f_type == rhs.f_type
      && f_value_number[0] == rhs.f_value_number[0]
      && f_value_number[1] == f_value_number[1]
      && f_value_number[2] == f_value_number[2]
      && f_value_string == f_value_string;
}

};
