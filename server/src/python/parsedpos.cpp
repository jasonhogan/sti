/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class ParsedPos
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
#include "parsedpos.h"
#include <cassert>
#include <sstream>
#include "parser.h"

using std::stringstream;

namespace libPython
{

/*! \param[in] parser The initial value for #parser.
 *
 *  The str() method would return "<string>" for this position.
 */
ParsedPos::ParsedPos(Parser *parser)
    : parser(parser), file(-1), line(0)
{
    assert(parser != NULL);
}

/*! \param[in] parser The initial value for #parser.
 *  \param[in] file   The initial value for #file.
 *  \param[in] line   The initial value for #line.
 */
ParsedPos::ParsedPos(Parser *parser, int file, unsigned line)
    : parser(parser), file(file), line(line)
{
    assert(parser != NULL);
}

/*! \param[in] parser The initial value for #parser.
 *  \param[in] file   The file name to look up / add to parser.
 *  \param[in] line   The initial value for #line.
 */
ParsedPos::ParsedPos(Parser *parser, const std::string &file, unsigned line)
    : parser(parser), line(line)
{
    assert(parser != NULL);

    this->file = parser->whichFile(file);
}

ParsedPos::~ParsedPos()
{
}

/*!
 *  This will return a file position in the format "filename(linenumber)".
 *  If the line number is 0, that part is left out. If the file number
 *  is 0, the filename output is "<string>", otherwise it is the file name
 *  as found in Parser.
 */
std::string
ParsedPos::str() const
{
    stringstream buf;

    assert(parser != NULL);

    if(file != -1)
        buf << parser->files()->at(file);
    else
        buf << "<string>";
    if(line != 0)
        buf << '(' << line << ')';

    return buf.str();
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if equal, \c false otherwise.
 *
 *  Compares both #file and #line. Only \c true if both are equal.
 *  This does not actually compare #parser, it is assumed that they
 *  are identical or at least contain the same stuff...
 */
bool
ParsedPos::operator==(const ParsedPos &other) const
{
    return file==other.file && line==other.line;
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if not equal, \c false otherwise.
 *
 *  Uses operator==() and negates that result.
 */
bool
ParsedPos::operator!=(const ParsedPos &other) const
{
    return !(*this == other);
}

};
