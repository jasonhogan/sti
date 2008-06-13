/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class ParsedEvent
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

#ifndef PARSEDEVENT_H
#define PARSEDEVENT_H

#include <string>
#include "parsedpos.h"

namespace libPython
{

typedef enum {NumberEvent, TextEvent, DDSEvent, MeasureEvent} EventType;

/*! \brief The ParsedEvent class represents information for one timing event
 *
 * This class is instantiated by the event() function of the
 * \link timing_module Timing module\endlink.
 */
class ParsedEvent
{
    /*! \brief The type of this event
     *
     *  This determines, which element, #f_value_number or f_value_text is
     *  valid. It is not possible to change the type of an event once it has
     *  been defined.
     */
    EventType f_type;
    /*! \brief The numerical values.
     *
     *  This holds the value for types NumberEvent and DDSEvent. In case of
     *  NumberEvent, only element 0 is used.
     */
    double      f_value_number[3];
    /*! \brief The text values.
     *
     *  This holds the value for types TextEvent and MeasureEvent.
     */
    std::string f_value_string;

public:
    unsigned  channel;
    double    time;
    ParsedPos position;

    /*! \brief Contructor for type NumericEvent */
    ParsedEvent(unsigned channel, double time, double number,
                const ParsedPos &position);
    /*! \brief Contructor for type TextEvent */
    ParsedEvent(unsigned channel, double time, const std::string &text,
                const ParsedPos &position);
    /*! \brief Contructor for type DDSEvent */
    ParsedEvent(unsigned channel, double time, double freq, double phase,
                double ampl, const ParsedPos &position);
    /*! \brief Contructor for type MeasureEvent */
    ParsedEvent(unsigned channel, double time, const ParsedPos &position,
                const std::string &desc);
    ~ParsedEvent();

    /*! \brief Access-function (read) for #f_type */
    EventType type() const;
    /*! \brief Access-function (read) for #f_value_number[0] */
    double number() const;
    /*! \brief Access-function (read,write) for #f_value_number[0] */
    void setNumber(double number);
    /*! \brief Access-function (read) for #f_value_string */
    const std::string &text() const;
    /*! \brief Access-function (write) for #f_value_string */
    void setText(const std::string &text);
    /*! \brief Access-function (read) for #f_value_number[n] */
    double dds(unsigned short n) const;
    /*! \brief Access-function (write) for #f_value_number[n] */
    void setDds(unsigned short n, double value);
    /*! \brief Access-function (read) for #f_value_number[0] */
    double freq() const;
    /*! \brief Access-function (write) for #f_value_number[0] */
    void setFreq(double freq);
    /*! \brief Access-function (read) for #f_value_number[1] */
    double phase() const;
    /*! \brief Access-function (write) for #f_value_number[1] */
    void setPhase(double phase);
    /*! \brief Access-function (read) for #f_value_number[2] */
    double ampl() const;
    /*! \brief Access-function (write) for #f_value_number[2] */
    void setAmpl(double ampl);
    /*! \brief Access-function (read) for #f_value_string */
    const std::string &desc() const;
    /*! \brief Access-function (write) for #f_value_string */
    void setDesc(const std::string &desc);
    /*! \brief Convenience-function to get string representation of value */
    std::string value() const;
    /*! \brief Comparision that for everything but position */
    bool nearlyEqual(const ParsedEvent &rhs) const;
};

};

#endif
