/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class ParsedVar
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
#include "parsedvar.h"

using std::string;

namespace libPython
{

/*! \param[in] src The object to copy
 */
ParsedVar::ParsedVar(const ParsedVar &src)
: name(src.name), value(src.value), isOverwritten(src.isOverwritten)
{
    if(src.position != NULL)
        position = new ParsedPos(*src.position);
    else
        position = NULL;
}

/*! \param[in] name     The initial value for #name.
 *  \param[in] value    The initial value for #value.
 */
ParsedVar::ParsedVar(const std::string &name, const ParsedValue &value)
    : name(name), value(value), position(NULL)
{
}

/*! \param[in] name     The initial value for #name.
 *  \param[in] value    The initial value for #value.
 *  \param[in] position The initial value for #position.
 */
ParsedVar::ParsedVar(const std::string &name, const ParsedValue &value,
    const ParsedPos& position, bool overwritten)
    : name(name), value(value), isOverwritten(overwritten)
{
    this->position = new ParsedPos(position);
}

ParsedVar::~ParsedVar()
{
    if(position != NULL)
        delete position;
}

};
