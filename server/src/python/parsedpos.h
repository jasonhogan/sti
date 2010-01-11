/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class ParsedPos
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

#ifndef PARSEDPOS_H
#define PARSEDPOS_H

#include <string>

namespace libPython
{

class Parser;

/*! \brief The ParsedPos class stores the position inside of a text file
 *
 *  This class is instantiated by functions in the \link timing_module Timing
 *  module\endlink and by shorthands in ParsedEvent and ParsedVar.
 */
class ParsedPos
{
public:
    /*! \brief Pointer to the Parser class we reference
     *
     *  This is only needed for the string-constructor and for the str()
     *  method.
     */
    Parser *parser;
    /*! \brief Index into Parser::f_files.
     *
     *  This is an index into the list of files stored in Parser.
     *  It can also have the value -1, which would mean that there is
     *  no filename in Parser.
     */
    int file;
    /*! \brief The line number within the file.
     *
     *  Counting starts with 1, all newline sequences recognized by
     *  Python are used to count. If no line is supposed to be given,
     *  set this to 0.
     */
    unsigned line;

    /*! \brief This constructor initializes the position to "<string>" */
    ParsedPos(Parser *parser);
    /*! \brief Standard constructor */
    ParsedPos(Parser *parser, int file, unsigned line);
    /*! \brief This constructor looks up the file number in parser */
    ParsedPos(Parser *parser, const std::string &file, unsigned line);
    /*! \brief Destructor */
    ~ParsedPos();

    /*! \brief Outputs the position as a string */
    std::string str() const;

    /*! \brief Equality Operator */
    bool operator==(const ParsedPos &other) const;
    /*! \brief Non-Equality Operator */
    bool operator!=(const ParsedPos &other) const;
};

};

#endif
