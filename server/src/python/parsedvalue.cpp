/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class ParsedValue
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
#include "parsedvalue.h"
#include <cassert>
#include <sstream>
#include "parser.h"

using std::string;
using std::stringstream;
using std::vector;

namespace libPython
{

/* \param[in] number The initial value for #number.
 *
 * The str() method will return a string representation of the number.
 */
ParsedValue::ParsedValue(double number)
    : parser(NULL), type(VTnumber), number(number), str_val(), list(),
      channel(0)
{
}

/* \param[in] str_val The initial value for #str_val.
 * \param[in] type    The initial value for #type.
 *
 * #type needs to be one of VTstr or VTobject. If called with only one
 * argument, the default is to create a VTstring.
 *
 * The str() method will return the string.
 */
ParsedValue::ParsedValue(const std::string &str_val, ValueType type)
    : parser(NULL), type(type), number(0), str_val(str_val), list(), channel(0)
{
    assert(type == VTstring || type == VTobject);
}

/*! \param[in] list The initial value for list.
 *  \param[in] repr The Python string representation of the list.
 *
 *  This will create a local copy of the list.
 *
 *  The str() method will return the value of #repr. This construct was chosen
 *  because it gets difficult to recognize strings otherwise. Just putting
 *  quotes around them is not enough because the string can also contain
 *  quotes. It is better to leave these things to Python.
 *
 *  \note It is the callers responsibility to ensure that #list and #repr
 *        match.
 */
ParsedValue::ParsedValue(const std::vector<ParsedValue> &list,
    const std::string &repr)
    : parser(NULL), type(VTlist), number(0), str_val(repr), list(list),
      channel(0)
{
}

/*! \param[in] parser  The initial value for #parser.
 *  \param[in] channel The initial value for #channel.
 *
 *  The str() method will return the result of ParsedChannel::str().
 */
ParsedValue::ParsedValue(Parser *parser, int channel)
    : parser(parser), type(VTchannel), number(0), str_val(), list(),
      channel(channel)
{
}

ParsedValue::~ParsedValue()
{
}

/*! The returned string is a single word for the type of the value.
 */
std::string ParsedValue::typestr() const
{
    static string NumberStr  = "Number";
    static string StringStr  = "String";
    static string ListStr    = "List";
    static string ChannelStr = "Channel";
    static string ObjectStr  = "Object";

    switch(type) {
    case VTnumber:
        return NumberStr;
        break;
    case VTstring:
        return StringStr;
        break;
    case VTlist:
        return ListStr;
        break;
    case VTchannel:
        return ChannelStr;
        break;
    case VTobject:
        return ObjectStr;
        break;
    default:
        assert(false);  // This should never happen
		return "";
        break;
    }
}

/*! This will return a string representation of the current value.
 */
std::string
ParsedValue::str() const
{
    stringstream buf;

    switch(type) {
    case VTnumber:
        buf << number;
        return buf.str();
        break;
    case VTstring:
    case VTlist:
    case VTobject:
        return str_val;
        break;
    case VTchannel:
        assert(parser != NULL);
        return parser->channels()->at(channel).str();
        break;
    default:
        assert(false);  // This should never happen
        break;
    }
    return ""; // We never reach this, but the compiler does not always
               // realize that.
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if equal, \c false otherwise.
 */
bool ParsedValue::operator==(const ParsedValue &other) const
{
    if(type != other.type)
        return false;
    switch(type) {
    case VTnumber:
        return number == other.number;
        break;
    case VTstring:
    case VTobject:
        return str_val == other.str_val;
        break;
    case VTlist:
        return list == other.list;  // vector::operator==() is your friend!
        break;
    case VTchannel:
        return channel == other.channel;
        break;
    default:
        assert(false);  // This should never happen
		return false;
        break;
    }
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if not equal, \c false otherwise.
 *
 *  Uses operator==() and negates that result.
 */
bool
ParsedValue::operator!=(const ParsedValue &other) const
{
    return !(*this == other);
}

};
