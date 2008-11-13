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

#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "parsedvalue.h"
#include <cassert>
#include <sstream>
#include "parser.h"

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
 *
 *  This will create a local copy of the list.
 *  The str() method will return something of this form:
 *  [foo, bar, baz]
 */
ParsedValue::ParsedValue(const std::vector<ParsedValue> &list)
    : parser(NULL), type(VTlist), number(0), str_val(), list(list), channel(0)
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

/*! This will return a string representation of the current value.
 */
std::string
ParsedValue::str() const
{
    stringstream                        buf;
    vector<ParsedValue>::const_iterator i;

    switch(type) {
    case VTnumber:
        buf << number;
        return buf.str();
        break;
    case VTstring:
    case VTobject:
        return str_val;
        break;
    case VTlist:
        buf << "[";
        if(list.size()>0) {
            i=list.begin();
            buf << i->str();
            for(i++; i<list.end(); i++)
                buf << "," << i->str();
        }
        buf << "]";
        return buf.str();
        break;
    case VTchannel:
        assert(parser != NULL);
        return parser->channels()->at(channel).str();
        break;
    default:
        assert(false);  // This should never happen
        break;
    }
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if equal, \c false otherwise.
 */
bool
ParsedValue::operator==(const ParsedValue &other) const
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
