/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the python class brdObject ("brd")
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
#include "brdobject.h"
#include <cassert>
#include <string>
#if defined(HAVE_LIBPYTHON2_5)
#  ifdef HAVE_PYTHON2_5_STRUCTMEMBER_H
#    include <python2.5/structmember.h>
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
using libPython::Parser;

namespace libPythonPrivate
{

/**** Global Variables ****/

/*! \brief Pointer to an instance of the Parser class
 */
static Parser *parser = NULL;

/*! \brief Numeric constants for all boards.
 *
 *  The entries in this list need to start with 0, so that the constant
 *  boardStrings corresponds with these constants.
 */
typedef enum {din24, dout24, ain2, aout2, aout40, dds4} boardTypes;

static char *boardStrings[] = {
    "DigInx24 v1",   /* din24  */
    "DigOutx24 v1",  /* dout24 */
    "AnInx2 v1",     /* ain2   */
    "AnOutx2 v1",    /* aout2  */
    "AnOutx40 v1",   /* aout40 */
    "DDSx4 v1"       /* dds4   */
};


/*! \brief The brdObject class contains all data needed for a Python
 *      board object
 */
typedef struct {
    PyObject_HEAD
    boardTypes  id;
    char       *addr;
    int         module;
} brdObject;

/*! \brief The \c __new__ function for brdObject
 *  \param[in] type The type we want to create (not neccessarily brd!).
 *  \param[in] args Possible arguments given without keywords. Ignored.
 *  \param[in] kwds Possible arguments given using keywords. Ignored.
 *  \return The new Python brdObject or NULL on failure.
 *
 *  The default values are id = None, addr = None and module = 0.
 */
static PyObject *
brd_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    brdObject *self;

    self = reinterpret_cast<brdObject *>(type->tp_alloc(type, 0));
    if (self != NULL) {
        self->id     = din24;
        self->addr   = strdup("");
        self->module = 0;
    } else
        self->addr   = NULL;

    return reinterpret_cast<PyObject *>(self);
}

/*! \brief The deallocation function for brdObject
 */
static void
brd_dealloc(brdObject* self)
{
    if(self->addr != NULL)
        free(self->addr);
    self->ob_type->tp_free((PyObject*)self);
}

/*! \brief The read access function for brdObject::id
 *  \param[in] self The Python object.
 *  \param[in] closure Unused extra parameter.
 *  \return Object with string corresponding to id.
 *
 *  Returns a Python string with the content from boardTypes[id].
 */
static PyObject *
brd_getid(brdObject *self, void *closure)
{
    assert(self->id >= din24);
    assert(self->id <= dds4);

    return Py_BuildValue("s", boardStrings[self->id]);
}

/*! \brief The write access function for brdObject::id
 *  \param[in,out] self The Python object.
 *  \param[in] value The new value for id.
 *  \param[in] closure Unused extra parameter.
 *  \return 0 on success or -1 on failure.
 *
 *  Filters out all all values that are not contained in boardTypes.
 *  For all remaining, set brdObject::id.
 */
static int
brd_setid(brdObject *self, PyObject *value, void *closure)
{
    char  *id;
    short  i;

    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the id");
        return -1;
    }

    if (!PyString_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The id must be a string");
        return -1;
    }

    id = PyString_AsString(value);
        /* Only valid while parent object lives */
    if(NULL == id)
        return -1;

    for(i=din24; i<=dds4; ++i)
        if(0 == strcmp(id, boardStrings[i]))
            break;
    if(i>dds4) {
        string buf;
        buf = "id=";
        buf += id;
        buf += " is not a recognized board type.";
        PyErr_SetString(PyExc_AttributeError, buf.c_str());
        return -1;
    }
    self->id = static_cast<boardTypes>(i);

    return 0;
}

/*! \brief The \c __init__ function for brdObject
 *  \param[in,out] self The Python object.
 *  \param[in] args Arguments given without keywords.
 *  \param[in] kwds Arguments given using keywords.
 *  \return 0 on success or -1 on failure.
 *
 *  Expects between 0 and 3 parameters under Python: __init__(id,addr,module).
 *  Every missing parameter is not changed. The values for id and addr must
 *  be strings, the value for module must be an integer.
 */
static int
brd_init(brdObject *self, PyObject *args, PyObject *kwds)
{
    PyObject   *id   = NULL;
    char       *addr = NULL;

    static char *kwlist[] = {"id", "addr", "module", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|Osi:brd.__init__", kwlist, 
        &id, &addr, &self->module))
        /* id is a borrowed reference */
        return -1; 

    if(id != NULL)
        if(brd_setid(self, id, NULL))
            return -1;
    if(addr != NULL) {
        if(self->addr != NULL)
            free(self->addr);
        self->addr = strdup(addr);
    }

    return 0;
}

/*! \brief The \c __repr__ function for brdObject
 *  \param[in] obj Reference to the object to \c repr.
 *  \return Python string object on success or NULL on failure.
 *
 *  The format of the output can be used to create the object from the
 *  string.
 */
static PyObject *
brd_repr(brdObject *obj)
{
    PyObject *result;

    assert(obj->id >= din24);
    assert(obj->id <= dds4);

    /* Create result */
    result = PyString_FromFormat("brd('%s','%s',%d)", boardStrings[obj->id],
        obj->addr, obj->module);
        /* Received new reference */
    return result;
}

/*! \brief The \c __str__ function for brdObject
 *  \param[in] obj Reference to the object to \c str.
 *  \return Python string object on success or NULL on failure.
 *
 *  This version is friendlier on the eye than brd_repr(), but it is not
 *  valid input to Python.
 */
static PyObject *
brd_str(brdObject *obj)
{
    PyObject *result;

    assert(obj->id >= din24);
    assert(obj->id <= dds4);

    /* Create result */
    result = PyString_FromFormat("brd(%s, %s, %d)", boardStrings[obj->id],
        obj->addr, obj->module);
        /* Received new reference */
    return result;
}

/*! \brief Returns the duration of a single time step
 *  \param[in] self The Python object for this class.
 *  \return The result as a Python object or NULL on error.
 *
 *  This function at the moment just returns a constant 1.
 *
 *  \todo Implement a real lookup!
 */
static PyObject *
brd_dt(brdObject* self)
{
    return Py_BuildValue("f", 1.);
}

/*! \brief The data structure used to describe all class members of the
 *      brdObject class
 */
static PyMemberDef brdMembers[] = {
    {"addr", T_STRING, offsetof(brdObject, addr), 0, NULL},
    {"module", T_INT, offsetof(brdObject, module), 0, NULL},
    {NULL}
};

static PyGetSetDef brdGetseters[] = {
    {"id", 
     (getter)brd_getid, (setter)brd_setid,
     "Board type id",
     NULL},
    {NULL}  /* Sentinel */
};

/*! \brief The data structure used to describe all methods of the brdObject
 *      class
 */
static PyMethodDef brdMethods[] = {
    {"dt", (PyCFunction)brd_dt, METH_NOARGS, NULL},
    {NULL}
};

/*! \brief The data structure used to register the brdObject class with
 *      Python
 *
 *   You can use this structure to detect instances of this Object in your own
 *   code.
 */
PyTypeObject brdType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "timing.brd",              /*tp_name*/
    sizeof(brdObject),         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)brd_dealloc,   /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    (reprfunc)brd_repr,        /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)brd_str,         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A hardware board providing some channels.\n", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    brdMethods,                /* tp_methods */
    brdMembers,                /* tp_members */
    brdGetseters,              /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)brd_init,        /* tp_init */
    0,                         /* tp_alloc */
    brd_new,                   /* tp_new */
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
 *  If an error occured, you do not need to call brdObject_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
brdObject_Initialize(PyObject *module, libPython::Parser *parser)
{
    assert(Py_IsInitialized());
    assert(module != NULL);

    /* Prepare global class object structures */
    if(PyType_Ready(&brdType))
        goto ErrorHappend;

    /* Add class variables to python */
    Py_INCREF(reinterpret_cast<PyObject *>(&brdType));
    if(PyModule_AddObject(module, "brd",
        reinterpret_cast<PyObject *>(&brdType)))
        goto ErrorHappend;

    return 0;

ErrorHappend:
    brdObject_Finalize();
    return -1;
}

/*!
 *  This will make the class non-usable.
 */
void
brdObject_Finalize()
{
    /* Get rid of all global variables */
    parser = NULL;
}

};
