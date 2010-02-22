/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the python \link anti_kbd_int Keyboard interrupt
 *      preventer\endlink
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
#include "antikbdint.h"
#include <cassert>
#if defined(HAVE_LIBPYTHON2_5)
#  ifdef HAVE_PYTHON2_5_PYTHON_H
#    include <python2.5/Python.h>
#  elif defined(MSVCPP_HAVE_PYTHON2_5_H)
#    include <Python.h>
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

namespace libPythonPrivate
{

/**** Global Variables ****/

/*! \brief Reference to the Python \c signal.signal function
 */
static PyObject *signalFunction    = NULL;

/*! \brief Reference to the Python \c signal.SIGINT constant
 */
static PyObject *sigintConstant    = NULL;

/*! \brief Reference to the original handler for SIGINT.
 *
 *  This value is set during AntiKbdInt_Initialize() and used to restore the
 *  default in AntiKbdInt_Finalize().
 */
static PyObject *oldSIGINThandler = NULL;

/**** C Functions ****/

/*! \return 0 on success, otherwise -1.
 *
 *  Disables Python grabbing the CTRL-C key.
 *
 *  If an error occured, you do not need to call AntiKbdInt_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
AntiKbdInt_Initialize()
{
    PyObject *signalModule;
    PyObject *sig_dflConstant = NULL;

    assert(Py_IsInitialized());

    /* Initialize global and local C++ references to Python objects */
    signalModule = PyImport_AddModule("signal");
        /* Reference is only borrowed */
    if(NULL == signalModule)
        goto ErrorHappend;
    signalFunction = PyObject_GetAttrString(signalModule, "signal");
        /* Received new reference */
    if(NULL == signalFunction)
        goto ErrorHappend;
    sig_dflConstant = PyObject_GetAttrString(signalModule, "SIG_DFL");
        /* Received new reference */
    if(NULL == sig_dflConstant)
        goto ErrorHappend;
    sigintConstant = PyObject_GetAttrString(signalModule, "SIGINT");
        /* Received new reference */
    if(NULL == sigintConstant)
        goto ErrorHappend;

    /* Make Python ignore SIGINT (Ctrl-C) */
    oldSIGINThandler = PyObject_CallFunctionObjArgs(signalFunction,
        sigintConstant, sig_dflConstant, NULL);
        /* Received new reference */
    if(NULL == oldSIGINThandler)
        goto ErrorHappend;

    return 0;

ErrorHappend:
    Py_XDECREF(sig_dflConstant);
    AntiKbdInt_Finalize();
    return -1;
}

/*!
 *  This reestablishes the original interrupt handler for CTRL-C under
 *  Python.
 */
void
AntiKbdInt_Finalize()
{
    assert(Py_IsInitialized());

    /* Reinstate original handler for SIGINT */
    if(oldSIGINThandler != NULL) {
        PyObject *ret = NULL;
        assert(signalFunction   != NULL);
        assert(sigintConstant   != NULL);
        assert(oldSIGINThandler != NULL);
        ret = PyObject_CallFunctionObjArgs(signalFunction, sigintConstant,
            oldSIGINThandler, NULL);
            /* Received new reference */
        Py_XDECREF(ret);
    }

    /* De-Reference all owned Python objects */
    Py_XDECREF(signalFunction);
    signalFunction    = NULL;
    Py_XDECREF(sigintConstant);
    sigintConstant    = NULL;
    Py_XDECREF(oldSIGINThandler);
    oldSIGINThandler  = NULL;
}

};

/*! \page anti_kbd_int Keyboard interrupt preventer
 *
 *  \todo Add documentation!
 */
