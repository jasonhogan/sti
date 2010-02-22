/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the python class devObject ("dev")
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
#include "devobject.h"
#include <cassert>
#include <string>
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
using libPython::Parser;

namespace libPythonPrivate
{

/**** Global Variables ****/

/*! \brief Pointer to an instance of the Parser class
 */
static Parser *parser = NULL;

/*! \todo get rid of this list, implement this differently */
//static char *deviceStrings[] = {
//    "DigInx24 v1",   /* din24  */
//    "DigOutx24 v1",  /* dout24 */
//    "AnInx2 v1",     /* ain2   */
//    "AnOutx2 v1",    /* aout2  */
//    "AnOutx40 v1",   /* aout40 */
//    "DDSx4 v1",      /* dds4   */
//    "AndorCam v1"    /* cam1   */
//};

/*! \brief The devObject class contains all data needed for a Python
 *      device object
 */
typedef struct {
    PyObject_HEAD
    char *id;
    char *addr;
    int   module;
} devObject;

/*! \brief The \c __new__ function for devObject
 *  \param[in] type The type we want to create (not neccessarily dev!).
 *  \param[in] args Possible arguments given without keywords. Ignored.
 *  \param[in] kwds Possible arguments given using keywords. Ignored.
 *  \return The new Python devObject or NULL on failure.
 *
 *  The default values are id = None, addr = None and module = 0.
 */
static PyObject *
dev_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    devObject *self;

    self = reinterpret_cast<devObject *>(type->tp_alloc(type, 0));
    if (self != NULL) {
        self->id     = strdup("");
        self->addr   = strdup("");
        self->module = 0;
    } else
        self->addr   = NULL;

    return reinterpret_cast<PyObject *>(self);
}

/*! \brief The deallocation function for devObject
 */
static void
dev_dealloc(devObject* self)
{
    if(self->id != NULL)
        free(self->id);
    if(self->addr != NULL)
        free(self->addr);
    self->ob_type->tp_free((PyObject*)self);
}

/*! \brief The read access function for devObject::id
 *  \param[in] self The Python object.
 *  \param[in] closure Unused extra parameter.
 *  \return Object with string corresponding to id.
 *
 *  Returns a Python string with the content from id.
 */
static PyObject *
dev_getid(devObject *self, void *closure)
{
    return Py_BuildValue("s", self->id);
}

/*! \brief The write access function for devObject::id
 *  \param[in,out] self The Python object.
 *  \param[in] value The new value for id.
 *  \param[in] closure Unused extra parameter.
 *  \return 0 on success or -1 on failure.
 *
 *  For all remaining, set devObject::id.
 *  \todo Need to implement filters to only accept valid id strings
 */
static int
dev_setid(devObject *self, PyObject *value, void *closure)
{
    char  *id;

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

    if(false) {
        string buf;
        buf = "id=";
        buf += id;
        buf += " is not a recognized device type.";
        PyErr_SetString(PyExc_AttributeError, buf.c_str());
        return -1;
    }

    if(self->id != NULL)
        free(self->id);
    self->id = strdup(id);

    return 0;
}

/*! \brief The \c __init__ function for devObject
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
dev_init(devObject *self, PyObject *args, PyObject *kwds)
{
    PyObject   *id   = NULL;
    char       *addr = NULL;

    static const char *kwlist[] = {"id", "addr", "module", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|Osi:dev.__init__",
        const_cast<char**>(kwlist), &id, &addr, &self->module))
        /* id is a borrowed reference */
        return -1;

    if(id != NULL)
        if(dev_setid(self, id, NULL))
            return -1;
    if(addr != NULL) {
        if(self->addr != NULL)
            free(self->addr);
        self->addr = strdup(addr);
    }

    return 0;
}

/*! \brief The \c __repr__ function for devObject
 *  \param[in] obj Reference to the object to \c repr.
 *  \return Python string object on success or NULL on failure.
 *
 *  The format of the output can be used to create the object from the
 *  string.
 */
static PyObject *
dev_repr(devObject *obj)
{
    PyObject *result;

    /* Create result */
    result = PyString_FromFormat("dev('%s','%s',%d)", obj->id, obj->addr,
        obj->module);
        /* Received new reference */
    return result;
}

/*! \brief The \c __str__ function for devObject
 *  \param[in] obj Reference to the object to \c str.
 *  \return Python string object on success or NULL on failure.
 *
 *  This version is friendlier on the eye than dev_repr(), but it is not
 *  valid input to Python.
 */
static PyObject *
dev_str(devObject *obj)
{
    PyObject *result;

    /* Create result */
    result = PyString_FromFormat("dev(%s, %s, %d)", obj->id, obj->addr,
        obj->module);
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
dev_dt(devObject* self)
{
    return Py_BuildValue("f", 1.);
}

/*! \brief The data structure used to describe all class members of the
 *      devObject class
 */
static PyMemberDef devMembers[] = {
    {const_cast<char*>("addr"), T_STRING, offsetof(devObject, addr), 0, NULL},
    {const_cast<char*>("module"), T_INT, offsetof(devObject, module), 0, NULL},
    {NULL}
};

static PyGetSetDef devGetseters[] = {
    {const_cast<char*>("id"), (getter)dev_getid, (setter)dev_setid,
         const_cast<char*>("Device type id"), NULL},
    {NULL}  /* Sentinel */
};

/*! \brief The data structure used to describe all methods of the devObject
 *      class
 */
static PyMethodDef devMethods[] = {
    {"dt", (PyCFunction)dev_dt, METH_NOARGS, NULL},
    {NULL}
};

/*! \brief The data structure used to register the devObject class with
 *      Python
 *
 *   You can use this structure to detect instances of this Object in your own
 *   code.
 */
PyTypeObject devType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "timing.dev",              /*tp_name*/
    sizeof(devObject),         /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)dev_dealloc,   /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    (reprfunc)dev_repr,        /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)dev_str,         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "A hardware device providing some channels.\n", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    devMethods,                /* tp_methods */
    devMembers,                /* tp_members */
    devGetseters,              /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)dev_init,        /* tp_init */
    0,                         /* tp_alloc */
    dev_new,                   /* tp_new */
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
 *  If an error occured, you do not need to call devObject_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
devObject_Initialize(PyObject *module, libPython::Parser *parser)
{
    assert(Py_IsInitialized());
    assert(module != NULL);

    /* Prepare global class object structures */
    if(PyType_Ready(&devType))
        goto ErrorHappend;

    /* Add class variables to python */
    Py_INCREF(reinterpret_cast<PyObject *>(&devType));
    if(PyModule_AddObject(module, "dev",
        reinterpret_cast<PyObject *>(&devType)))
        goto ErrorHappend;

    return 0;

ErrorHappend:
    devObject_Finalize();
    return -1;
}

/*!
 *  This will make the class non-usable.
 */
void
devObject_Finalize()
{
    /* Get rid of all global variables */
    parser = NULL;
}

};
