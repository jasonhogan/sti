/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class ParsedVar
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

#ifndef PARSEDVAR_H
#define PARSEDVAR_H

#include <string>
#include "parsedpos.h"
#include "parsedvalue.h"

namespace libPython
{

/*! \brief The ParsedVar class represents one timing variable
 *
 *  This class is instantiated by the setvar() and Timing_readvars() functions
 *  in the \link timing_module Timing module\endlink.
 */
class ParsedVar
{
public:
    /*! \brief The name of the variable
     *
     *  The variable is always assumed to be in the \c __main__ module of
     *  Python, so the content is \c "spam", not \c "__main__.spam".
     *
     *  \warning This value should not be used to rename the variable.
     */
    std::string name;
    /*! \brief The value of the variable
     *
     *  This is the string representation as returned by the Python \c repr()
     *  function. The \c str() function is not used, because one cannot
     *  differentiate between strings and numbers with it.
     *
     *  \warning This only contains the value of the variable when it was
     *           defined. It might very well change its value during the
     *           run of the Python script.
     *  \warning This value should not be used to edit the variable.
     */
    ParsedValue value;

    /*! \brief The filename and position in the file
     *
     *  The position can be NULL. This would mean a variable not created with
     *  Python setvar(). This happens if created by readvars().
     *
     *  \warning You have to always check if this object is NULL or not.
     */
    ParsedPos   *position;

    /*! \brief Copy constructor */
    ParsedVar(const ParsedVar &src);
    /*! \brief Constructor for variables without position */
    ParsedVar(const std::string &name, const ParsedValue &value);
    /*! \brief Stanfard contructor */
    ParsedVar(const std::string &name, const ParsedValue &value,
        const ParsedPos &position);
    /*! \brief Destructor */
    ~ParsedVar();
};

};

#endif
