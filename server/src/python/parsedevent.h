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

#include "ParsedDDSValue.h"

namespace libPython
{

typedef enum {NumberEvent, TextEvent, DDSEvent, MeasureEvent} EventType;

/*! \brief The ParsedEvent class represents information for one timing event
 *
 * This class is instantiated by the event() and meas() functions of the
 * \link timing_module Timing module\endlink.
 */
class ParsedEvent
{

public:

    /*! \brief Contructor for type NumericEvent */
    ParsedEvent(unsigned channel, double time, double number,
                const ParsedPos &position);

	/*! \brief Contructor for type TextEvent */
    ParsedEvent(unsigned channel, double time, const std::string &text,
                const ParsedPos &position);
   
	/*! \brief Contructor for type DDSEvent */
   	ParsedEvent(unsigned channel, double time, 
		ParsedDDSValue &freq, ParsedDDSValue &ampl, ParsedDDSValue &phase, 
		const ParsedPos &position);
	
	/*! \brief Contructor for type MeasureEvent */
    ParsedEvent(unsigned channel, double time, const ParsedPos &position,
                const std::string &desc);
    ~ParsedEvent();


    EventType type() const;
    double number() const;
    void setNumber(double number);
    const std::string &text() const;
    void setText(const std::string &text);
   
//    double dds(unsigned short n) const;
//    void setDds(unsigned short n, double value);
    
	const ParsedDDSValue& freq() const;
    void setFreq(ParsedDDSValue& freq);

	const ParsedDDSValue& ampl() const;
	void setAmpl(ParsedDDSValue& ampl);
    
	const ParsedDDSValue& phase() const;
    void setPhase(ParsedDDSValue& phase);
    
  
	const std::string &desc() const;
 	void setDesc(const std::string &desc);
 
	/*! \brief Convenience-function to get string representation of value */
    std::string value() const;
    /*! \brief Comparision that for everything but position */
    bool nearlyEqual(const ParsedEvent &rhs) const;

    unsigned  channel;
    double    time;
    ParsedPos position;

private:
    
	/*! \brief The type of this event
     *
     *  This determines, which element, #f_value_number or f_value_text is
     *  valid. It is not possible to change the type of an event once it has
     *  been defined.
     */
    EventType   f_type;
    /*! \brief The numerical values.
     *
     *  This holds the value for type NumberEvent.
     */
    double      f_value_number;

	ParsedDDSValue ddsValues[3]; // {frequency, amplitude, phase}

    /*! \brief The text values.
     *
     *  This holds the value for types TextEvent and MeasureEvent.
     */
    std::string f_value_string;

};

};

#endif
