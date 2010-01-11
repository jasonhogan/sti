/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class ParsedValue
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

#ifndef PARSEDVALUE_H
#define PARSEDVALUE_H

#include <string>
#include <vector>

namespace libPython
{

class Parser;

/*! \brief The ValueType enum lists all possible types of a ParsedValue.
 */
enum ValueType {VTnumber, VTstring, VTlist, VTchannel, VTobject};

/*! \brief The ParsedValue class stores a Python value in a union
 *
 *  This class is instantiated by functions in the \link timing_module Timing
 *  module\endlink.
 */
class ParsedValue
{
public:
    /*! \brief Pointer to the Parser class we reference
     *
     *  This is only populated if the #type is VTchannel.
     */
    Parser *parser;
    /*! \brief The type of this value
     *
     * It is automatically set by calling the correct constructor.
     */
    ValueType type;
    /*! \brief A numerical value
     *
     *  This is only populated if the #type is VTnumber.
     */
    double number;
    /*! \brief A string value or the repr() of a list or an object
     *
     *  If this represents a string, it does contain the string directly,
     *  without enclosing quotes.
     *  If this represents a list or an object, it contains the Python repr()
     *  of that list or object.
     *
     *  This is  populated if the #type is VTstring, VTlist or VTobject.
     */
    std::string str_val;
    /*! \brief The list values
     *
     *  This is only populated if the #type is VTlist.
     */
    std::vector<ParsedValue> list;
    /*! \brief Index into Parser::f_channels
     *
     *  This is an index into the list of channels stored in Parser.
     *
     *  This is only populated if the #type is VTchannel.
     */
    int channel;

    /*! \brief Constructor for numerical values */
    ParsedValue(double number);
    /* \brief Constructor for both string and object values */
    ParsedValue(const std::string& str_val, ValueType type=VTstring);
    /* \brief Constructor for list values */
    ParsedValue(const std::vector<ParsedValue> &list, const std::string &repr);
    /* \brief Constructor for a channel */
    ParsedValue(Parser *parser, int channel);
    /*! \brief Destructor */
    ~ParsedValue();

    /*! \brief Outputs the value as a string */
    std::string str() const;
    /*! \brief Outputs the value type as a string */
    std::string typestr() const;

    /*! \brief Equality Operator */
    bool operator==(const ParsedValue &other) const;
    /*! \brief Non-Equality Operator */
    bool operator!=(const ParsedValue &other) const;
};

};

#endif
