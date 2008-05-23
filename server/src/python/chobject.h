/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the python class chObject ("ch")
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

#ifndef CHOBJECT_H
#define CHOBJECT_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#ifdef HAVE_PYTHON2_4_PYTHON_H
#  include <python2.4/Python.h>
#endif
#ifdef HAVE_PYTHON2_5_PYTHON_H
#  include <python2.5/Python.h>
#endif
#include "parser.h"

extern PyTypeObject chType;

/*! \brief Initalizes the Python class chObject
 */
int chObject_Initialize(PyObject *module, Parser *parser);

/*! \brief Releases all ressources used by the Python class chObject
 */
void chObject_Finalize();

#endif
