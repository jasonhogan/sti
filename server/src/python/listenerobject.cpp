/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the python class listenerObject ("listener")
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
#include "listenerobject.h"
#include <cassert>
#if defined(HAVE_LIBPYTHON2_5)
#  ifdef HAVE_PYTHON2_5_STRUCTMEMBER_H
#    include <python2.5/structmember.h>
#  elif defined MSVCPP_HAVE_PYTHON2_5_STRUCTMEMBER_H
#    include <structmember.h>
#  else
#    error Need include file python2.5/structmember.h
#  endif
#elif defined(HAVE_LIBPYTHON2_4)
#  ifdef HAVE_PYTHON2_4_STRUCTMEMBER_H
#    include <python2.4/structmember.h>
#  else
#    error Need include file python2.4/structmember.h
#  endif
#else
#  error Need a python library
#endif

using std::string;

namespace libPythonPrivate
{

/**** Global Variables ****/

/*! \brief The original handler for the stderr channel.
 */
static PyObject *olderr = NULL;
/*! \brief The original handler for the stdout channel.
 */
static PyObject *oldout = NULL;

/*! \brief The listenerObject class contains all data needed for a Python
 *      listener object
 *
 *  The purpose of this object is to act as a receiver for output into
 *  the stdout or the stderr channels of Python. It uses its internal
 *  #buffer variable to collect the output.
 *
 *  Use listenerObject_Initialize() and listenerObject_Finalize() to
 *  install or remove the objects.
 */
typedef struct {
    PyObject_HEAD
    string *buffer;
    int     softspace;
} listenerObject;

/*! \brief The \c __new__ function for listenerObject
 *  \param[in] type The type we want to create (not neccessarily listener!).
 *  \param[in] args Possible arguments given without keywords. Ignored.
 *  \param[in] kwds Possible arguments given using keywords. Ignored.
 *  \return The new Python listenerObject or NULL on failure.
 *
 *  The default values are buffer = NULL and softspace = 0.
 */
static PyObject *
listener_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    listenerObject *self;

    self = reinterpret_cast<listenerObject *>(type->tp_alloc(type, 0));
    if (self != NULL) {
        self->buffer    = NULL;
        self->softspace = 0;
    }

    return reinterpret_cast<PyObject *>(self);
}

/*! \brief The deallocation function for listenerObject
 */
static void
listener_dealloc(listenerObject* self)
{
    self->ob_type->tp_free((PyObject*)self);
}

/*! \brief The \c __init__ function for listenerObject
 *  \param[in,out] self The Python object.
 *  \param[in] args Arguments given without keywords.
 *  \param[in] kwds Arguments given using keywords.
 *  \return 0 on success or -1 on failure.
 *
 *  Expects between 0 and 1 parameters under Python: __init__(softspace).
 *  Every missing parameter is not changed. The value for softspace must be an
 *  integer.
 *
 *  \note It is not possible to change buffer from within Python!
 */
static int
listener_init(listenerObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"softspace", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|i:listener.__init__",
        const_cast<char**>(kwlist), &self->softspace))
        return -1;

    return 0;
}

/*! \brief Appends the string \a output to listenerObject#buffer
 *  \param[in] self The Python object for this class.
 *  \param[in] output The Python string object to be output.
 *  \return Py_None on success or NULL on error.
 *
 *  If listenerObject#buffer == NULL, do nothing.
 */
static PyObject *
listener_write(listenerObject* self, PyObject *output)
{
    char *buffer;

    if(self->buffer == NULL)
        Py_RETURN_NONE;

    buffer = PyString_AsString(output);
        /* Only valid while parent object lives */
    if(NULL == buffer)
        return NULL;

    self->buffer->append(buffer);

    Py_RETURN_NONE;
}

/*! \brief The data structure used to describe all class members of the
 *      listenerObject class
 */
static PyMemberDef listenerMembers[] = {
    {const_cast<char*>("softspace"), T_INT,
        offsetof(listenerObject, softspace), 0, NULL},
    {NULL}
};

/*! \brief The data structure used to describe all methods of the
 *      listenerObject class
 */
static PyMethodDef listenerMethods[] = {
    {const_cast<char*>("write"), (PyCFunction)listener_write, METH_O, NULL},
    {NULL}
};

/*! \brief The data structure used to register the listenerObject class with
 *      Python
 *
 *   You can use this structure to detect instances of this Object in your own
 *   code.
 */
PyTypeObject listenerType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "listener",                /*tp_name*/
    sizeof(listenerObject),    /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)listener_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Redirects stderr or stdout to a C++ string.\n", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    listenerMethods,           /* tp_methods */
    listenerMembers,           /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)listener_init,   /* tp_init */
    0,                         /* tp_alloc */
    listener_new,              /* tp_new */
};

/**** C Functions ****/

/*! \param[in] outStr String buffer for stdout
 *  \param[in] errStr String buffer for stderr
 *  \return 0 on success, otherwise -1.
 *
 *  - Registers the class with Python
 *  - Stores the current values of \c sys.stdout and \c sys.stderr
 *  - Overwrites \c sys.stdout and \c sys.stderr with two instances of
 *    the class
 *
 *  If an error occured, you do not need to call listenerObject_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
listenerObject_Initialize(std::string *outStr, std::string *errStr)
{
    PyObject       *module;
    listenerObject *listener = NULL;

    assert(Py_IsInitialized());

    /* Prepare global class object structures */
    if(PyType_Ready(&listenerType))
        goto ErrorHappend;

    /* Get sys module */
    module = PyImport_AddModule("sys");
        /* Reference is only borrowed */
    if(NULL == module)
        goto ErrorHappend;

    /* Add class variable to python */
    Py_INCREF(reinterpret_cast<PyObject *>(&listenerType));

    /* Get old values of stdout and stderr */
    oldout = PyObject_GetAttrString(module, "stdout");
        /* Received new reference */
    if(NULL == oldout)
        goto ErrorHappend;
    olderr = PyObject_GetAttrString(module, "stderr");
        /* Received new reference */
    if(NULL == olderr)
        goto ErrorHappend;

    /* Overwrite with new object instances */
    listener = PyObject_New(listenerObject, &listenerType);
        /* Received new reference */
    if(NULL == listener)
        goto ErrorHappend;
    listener->buffer = outStr;
    if(PyObject_SetAttrString(module, "stdout",
        reinterpret_cast<PyObject *>(listener)))
        goto ErrorHappend;
    Py_DECREF(listener);
    listener = PyObject_New(listenerObject, &listenerType);
        /* Received new reference */
    if(NULL == listener)
        goto ErrorHappend;
    listener->buffer = errStr;
    if(PyObject_SetAttrString(module, "stderr",
        reinterpret_cast<PyObject *>(listener)))
        goto ErrorHappend;
    Py_DECREF(listener);

    return 0;

ErrorHappend:
    Py_XDECREF(listener);
    listenerObject_Finalize();
    return -1;
}

/*!
 *  This will make the class non-usable. The steps are:
 *
 *  - Restores the original values of \c sys.stdout and \c sys.stderr
 *  - Deregisters the class with Python
 */
void
listenerObject_Finalize()
{
    PyObject *module;

    assert(Py_IsInitialized());

    /* Restore original values for sys.stdout and sys.stderr */
    module = PyImport_AddModule("sys");
        /* Reference is only borrowed */
    if(module != NULL && oldout != NULL)
        PyObject_SetAttrString(module, "stdout", oldout);
    Py_XDECREF(oldout);
    oldout = NULL;
    if(module != NULL && olderr != NULL)
        PyObject_SetAttrString(module, "stderr", olderr);
    Py_XDECREF(olderr);
    olderr = NULL;

    /* De-Reference Python type variable */
    if(listenerType.ob_refcnt>1) {
        Py_XDECREF(reinterpret_cast<PyObject *>(&listenerType));
    }
}

};
