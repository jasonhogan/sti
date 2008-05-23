/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the python \link timing_module Timing
 *      module\endlink
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

#ifndef TIMING_H
#define TIMING_H

#include <string>
#include "parser.h"

namespace libPythonPrivate
{

/*! \brief Initalizes the Python timing module
 */
int Timing_Initialize(libPython::Parser *parser);

/*! \brief Releases all ressources used by the Python timing module
 */
void Timing_Finalize();

/*! \brief Reads in a file and parses it
 */
int Timing_readFile(const std::string &filename);

/*! \brief Evaluates the code
 */
int Timing_evaluate(const std::string &code);

};

#endif
