/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the python class chObject ("ch")
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
#include "chobject.h"
#include <cassert>
#ifdef HAVE_PYTHON2_4_STRUCTMEMBER_H
#  include <python2.4/structmember.h>
#endif
#ifdef HAVE_PYTHON2_5_STRUCTMEMBER_H
#  include <python2.5/structmember.h>
#endif
#include "brdobject.h"

using libPython::Parser;

namespace libPythonPrivate
{

/**** Global Variables ****/

/*! \brief Pointer to an instance of the Parser class
 */
static Parser *parser = NULL;

/*! \brief The chObject class contains all data needed for a Python
 *      channel object
 */
typedef struct {
    PyObject_HEAD
    PyObject *board;
    int       nr;
} chObject;

/*! \brief The \c __new__ function for chObject
 *  \param[in] type The type we want to create (not neccessarily ch!).
 *  \param[in] args Possible arguments given without keywords. Ignored.
 *  \param[in] kwds Possible arguments given using keywords. Ignored.
 *  \return The new Python chObject or NULL on failure.
 *
 *  The default values are board = None and nr = 0.
 */
static PyObject *
ch_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    chObject *self;

    self = reinterpret_cast<chObject *>(type->tp_alloc(type, 0));
    if (self != NULL) {
        self->board = Py_None;
        Py_INCREF(self->board);
        self->nr = 0;
    }

    return reinterpret_cast<PyObject *>(self);
}

/*! \brief The deallocation function for chObject
 *
 *  \bug Why may we not call Py_DECREF here? If we do, Python crashes after
 *      several iterations!
 */
static void
ch_dealloc(chObject* self)
{
/*    Py_XDECREF(self->board); */
    self->ob_type->tp_free((PyObject*)self);
}

/*! \brief The \c __init__ function for chObject
 *  \param[in,out] self The Python object.
 *  \param[in] args Arguments given without keywords.
 *  \param[in] kwds Arguments given using keywords.
 *  \return 0 on success or -1 on failure.
 *
 *  Expects between 0 and 2 parameters under Python: __init__(board,nr).
 *  Every missing parameter is not changed. The value for nr must be an
 *  integer.
 */
static int
ch_init(chObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"board", "nr", NULL};
    PyObject *board       = NULL;
    PyObject *tmp;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|O!i:ch.__init__", kwlist, 
        &brdType, &board, &self->nr))
        return -1; 

    if(board != NULL) {
        tmp = self->board;
        Py_INCREF(board);
        self->board = board;
        Py_XDECREF(board);
    }

    return 0;
}

/*! \brief The \c __repr__ function for chObject
 *  \param[in] obj Reference to the object to \c repr.
 *  \return Python string object on success or NULL on failure.
 *
 *  The format of the output can be used to create the object from the
 *  string.
 */
static PyObject *
ch_repr(chObject *obj)
{
    PyObject *brdObj;
    char     *brdStr;
    PyObject *result;

    /* Get string for board */
    brdObj = PyObject_Repr(obj->board);
        /* Received new reference */
    if(NULL == brdObj)
        return NULL;
    brdStr = PyString_AsString(brdObj);
        /* Only valid while parent object lives */
    if(NULL == brdStr) {
        Py_DECREF(brdObj);
        return NULL;
    }

    /* Create result */
    result = PyString_FromFormat("ch(%s,%d)", brdStr, obj->nr);
        /* Received new reference */
    Py_DECREF(brdObj);
    return result;
}

/*! \brief The \c __str__ function for chObject
 *  \param[in] obj Reference to the object to \c str.
 *  \return Python string object on success or NULL on failure.
 *
 *  This version is friendlier on the eye than ch_repr(), but it is not
 *  valid input to Python.
 */
static PyObject *
ch_str(chObject *obj)
{
    PyObject *brdObj;
    char     *brdStr;
    PyObject *result;

    /* Get string for board */
    brdObj = PyObject_Str(obj->board);
        /* Received new reference */
    if(NULL == brdObj)
        return NULL;
    brdStr = PyString_AsString(brdObj);
        /* Only valid while parent object lives */
    if(NULL == brdStr) {
        Py_DECREF(brdObj);
        return NULL;
    }

    /* Create result */
    result = PyString_FromFormat("ch(%s, %d)", brdStr, obj->nr);
        /* Received new reference */
    Py_DECREF(brdObj);
    return result;
}

/*! \brief Returns the duration of a single time step
 *  \param[in] self The Python object for this class.
 *  \return The result as a Python object or NULL on error.
 *
 *  This function calls brdObject::dt() via the Python parser.
 *
 *  \todo Make ch.dt() be faster by calling brdObject.dt() directly in C.
 */
static PyObject *
ch_dt(chObject* self)
{
    if (self->board == NULL) {
        PyErr_SetString(PyExc_AttributeError, "board");
        return NULL;
    }

    return PyObject_CallMethod(self->board, "dt", NULL);
}

/*! \brief The data structure used to describe all class members of the
 *      chObject class
 */
static PyMemberDef chMembers[] = {
    {"board", T_OBJECT_EX, offsetof(chObject, board), 0, NULL},
    {"nr", T_INT, offsetof(chObject, nr), 0, NULL},
    {NULL}
};

/*! \brief The data structure used to describe all methods of the chObject
 *      class
 */
static PyMethodDef chMethods[] = {
    {"dt", (PyCFunction)ch_dt, METH_NOARGS, NULL},
    {NULL}
};

/*! \brief The data structure used to register the chObject class with
 *      Python
 *
 *   You can use this structure to detect instances of this Object in your own
 *   code.
 */
PyTypeObject chType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "timing.ch",               /*tp_name*/
    sizeof(chObject),          /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ch_dealloc,    /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    (reprfunc)ch_repr,         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)ch_str,          /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A hardware channel.\n",   /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    chMethods,                 /* tp_methods */
    chMembers,                 /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)ch_init,         /* tp_init */
    0,                         /* tp_alloc */
    ch_new,                    /* tp_new */
};

/**** C Functions ****/

/*! \param[in] module The Python module into which to put the class
 *  \param[in] parser Pointer to the instance of Parser that should be used
 *      in the module.
 *  \return 0 on success, otherwise -1.
 *
 *  - Initializes the global parser variable
 *  - Registers the class with Python in \a module
 *
 *  The \a parser object is stored in ::parser. It needs to stay valid until
 *  after the program is finished using the class.
 *
 *  If an error occured, you do not need to call chObject_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
chObject_Initialize(PyObject *module, libPython::Parser *parser)
{
    assert(Py_IsInitialized());
    assert(module != NULL);

    /* Prepare global class object structures */
    if(PyType_Ready(&chType))
        goto ErrorHappend;

    /* Add class variables to python */
    Py_INCREF(reinterpret_cast<PyObject *>(&chType));
    if(PyModule_AddObject(module, "ch",
        reinterpret_cast<PyObject *>(&chType)))
        goto ErrorHappend;

    return 0;

ErrorHappend:
    chObject_Finalize();
    return -1;
}

/*!
 *  This will make the class non-usable.
 */
void
chObject_Finalize()
{
    /* Get rid of all global variables */
    parser = NULL;
}

};
