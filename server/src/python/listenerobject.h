/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the python class listenerObject ("listener")
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

#ifndef LISTENEROBJECT_H
#define LISTENEROBJECT_H

#if defined(HAVE_LIBPYTHON2_5)
#  ifdef HAVE_PYTHON2_5_PYTHON_H
#    include <python2.5/Python.h>
#  else
#    error Need include file python2.5/Python.h
#  endif
#elif defined(HAVE_LIBPYTHON2_4)
#  ifdef HAVE_PYTHON2_4_PYTHON_H
#    include <python2.4/Python.h>
#  else
#    error Need include file python2.4/Python.h
#  endif
#else
#  error Need a python library
#endif
#include <string>

namespace libPythonPrivate
{

extern PyTypeObject listenerType;

/*! \brief Initalizes the Python class listenerObject
 */
int listenerObject_Initialize(std::string *outStr, std::string *errStr);

/*! \brief Releases all ressources used by the Python class listenerObject
 */
void listenerObject_Finalize();

};

#endif
