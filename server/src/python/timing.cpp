/*! \file
 *  \author Olaf Mandel
 *  \author Jason Hogan
 *  \brief Source-file for the python \link timing_module Timing module\endlink
 *  \section license License
 *
 *  Copyright (C) 2008 Olaf Mandel <mandel@stanford.edu>\n
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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
#include "timing.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>

//#ifdef _MSC_VER
//#include <float.h>  // for _isnan() on VC++
//#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#define isnan(x) _isnan(x)  // VC++ uses _isnan() instead of isnan()
//#else
//#include <math.h>  // for isnan() everywhere else
//#endif

#include "devobject.h"
#include "chobject.h"
#include "parser.h"
#include "ParsedTag.h"

using std::ifstream;
using std::stringbuf;
using std::stringstream;
using std::string;
using std::vector;
using libPython::ParsedChannel;
using libPython::ParsedEvent;
using libPython::ParsedPos;
using libPython::ParsedVar;
using libPython::ParsedTag;
using libPython::ParsedValue;
using libPython::Parser;
using libPython::VTobject;

namespace libPythonPrivate
{

bool convertPyObjectToString(PyObject* obj, std::string& result);
bool convertPyObjectToDouble(PyObject* obj, double& result);
bool convertPy2MixedValue(PyObject* obj, MixedValue& result);



/**** Global Variables ****/

/*! \brief Reference to the Python \c __main__ module
 */
static PyObject *mainModule   = NULL;
/*! \brief Reference to the Python \c __main__ module dictionary
 */
static PyObject *mainDict     = NULL;

/*! \brief Reference to the Python \c inspect.stack function
 */
static PyObject *stackFunction     = NULL;
/*! \brief Reference to the Python \c inspect.getfile function
 */
static PyObject *getfileFunction   = NULL;
/*! \brief Reference to the Python \c inspect.getlineno function
 */
static PyObject *getlinenoFunction = NULL;
/*! \brief Reference to the Python \c os.chdir function
 */
static PyObject *chdirFunction     = NULL;
/*! \brief Reference to the Python \c os.getcwd function
 */
static PyObject *getcwdFunction    = NULL;
/*! \brief Reference to the Python \c os.path.abspath function
 */
static PyObject *abspathFunction   = NULL;
/*! \brief Reference to the Python \c os.path.dirname function
 */
static PyObject *dirnameFunction   = NULL;
/*! \brief Reference to the Python \c os.path.normcase function
 */
static PyObject *normcaseFunction  = NULL;

/*! \brief Pointer to an instance of the Parser class
 */
static Parser *parser = NULL;

/*! \brief Stack of already included files
 */
static vector<string> fileStack;

/**** Shared helper functions ****/

/*! \brief Creates a ParsedChannel object from a PyObject of the correct type
 *  \param[in] ch The Python object to translate into a ParsedChannel
 *  \return The initialized ParsedChannel object or an object with
 *      \c id="".
 *
 *  It is assumed that \c ch is of type chType.
 */
static ParsedChannel
getCh(PyObject *ch)
{
    PyObject       *devObj = NULL;
    PyObject       *obj    = NULL;
    char           *str;
    string          id;
    string          addr;
    unsigned short  module;
    unsigned short  nr;

    assert(Py_IsInitialized());
    assert(ch != NULL);

    devObj = PyObject_GetAttrString(ch, "device");
        /* Received new reference */
    if(NULL == devObj)
        goto ErrorHappend;

    obj = PyObject_GetAttrString(devObj, "id");
        /* Received new reference */
    if(NULL == obj)
        goto ErrorHappend;
    str = PyString_AsString(obj);
        /* Only valid while parent object lives */
    if(NULL == str)
        goto ErrorHappend;
    id = str;
    Py_DECREF(obj);

    obj = PyObject_GetAttrString(devObj, "addr");
        /* Received new reference */
    if(NULL == obj)
        goto ErrorHappend;
    str = PyString_AsString(obj);
        /* Only valid while parent object lives */
    if(NULL == str)
        goto ErrorHappend;
    addr = str;
    Py_DECREF(obj);

    obj = PyObject_GetAttrString(devObj, "module");
        /* Received new reference */
    if(NULL == obj)
        goto ErrorHappend;
    if(!PyArg_Parse(obj, "H", &module))
        goto ErrorHappend;
    Py_DECREF(obj);

    Py_DECREF(devObj);

    obj = PyObject_GetAttrString(ch, "nr");
        /* Received new reference */
    if(NULL == obj)
        goto ErrorHappend;
    if(!PyArg_Parse(obj, "H", &nr))
        goto ErrorHappend;
    Py_DECREF(obj);

    return ParsedChannel(id, addr, module, nr);

ErrorHappend:
    Py_XDECREF(devObj);
    Py_XDECREF(obj);
    return ParsedChannel("","",0,0);
}

/*! \brief Creates a ParsedPos object for the current position
 *  \return The initialized ParsedPos object or an object with line=0.
 *
 *  "Current position" is seen as the position from which the current
 *  Python function was called. This is usually from where setvar() or
 *  event() or meas() were called.
 *
 *  This function uses the \c inspect module of Python to find out
 *  where it is.
 *
 *  \todo It is (probably) much easier to read out the frame:
 *        stack is a list and entry is a tuple. You can use integers to
 *        select the entries.
 */
static ParsedPos
getPos()
{
    PyObject *stack   = NULL;
    PyObject *entry   = NULL;
    PyObject *frame   = NULL;
    PyObject *number  = NULL;
    PyObject *fileobj = NULL;
    char     *file;
    string    filestr;
    PyObject *lineobj = NULL;
    unsigned  line;

    assert(Py_IsInitialized());
    assert(stackFunction     != NULL);
    assert(getfileFunction   != NULL);
    assert(getlinenoFunction != NULL);

    /* Get the execution frame of the caller (as seen from Python) */
    stack = PyObject_CallFunction(stackFunction, NULL);
        /* Received new reference */
    if(NULL == stack)
        goto ErrorHappend;
    number = Py_BuildValue("i", 0);
        /* Received new reference */
    if(NULL == number)
        goto ErrorHappend;
    entry = PyObject_GetItem(stack, number);
        /* Received new reference */
    if(NULL == entry)
        goto ErrorHappend;
    Py_DECREF(stack);
    Py_DECREF(number);
    number = Py_BuildValue("i", 0);
        /* Received new reference */
    if(NULL == number)
        goto ErrorHappend;
    frame = PyObject_GetItem(entry, number);
        /* Received new reference */
    if(NULL == frame)
        goto ErrorHappend;
    Py_DECREF(entry);
    Py_DECREF(number);

    /* Get file name */
    fileobj = PyObject_CallFunctionObjArgs(getfileFunction, frame, NULL);
        /* Received new reference */
    if(NULL == fileobj)
        goto ErrorHappend;
    file = PyString_AsString(fileobj);
        /* Only valid while parent object lives */
    if(NULL == file)
        goto ErrorHappend;
    filestr=file;
    Py_DECREF(fileobj);

    /* Get line number */
    lineobj = PyObject_CallFunctionObjArgs(getlinenoFunction, frame, NULL);
        /* Received new reference */
    if(NULL == lineobj)
        goto ErrorHappend;
    if(!PyArg_Parse(lineobj, "I", &line))
        goto ErrorHappend;
    Py_DECREF(lineobj);
    Py_DECREF(frame);

    return ParsedPos(parser, filestr, line);

ErrorHappend:
    Py_XDECREF(stack);
    Py_XDECREF(entry);
    Py_XDECREF(frame);
    Py_XDECREF(number);
    Py_XDECREF(fileobj);
    Py_XDECREF(lineobj);
    return ParsedPos(parser);
}

/*! \brief Creates a ParsedValue object from a PyObject
 *  \param[in]  value  The Python object to translate.
 *  \param[out] result The resulting ParsedValue object.
 *  \return \c true if an error occured, \c false otherwise.
 */
static bool Py2Cvalue(PyObject *value, ParsedValue &result)
{
	//PyInt_Check(value);
	//PyFloat_Check(value);

    if(PyNumber_Check(value)) {
        PyObject *valueFloat;
        valueFloat = PyNumber_Float(value);
            /* Received new reference */
        if(NULL == valueFloat)
            return true;
        result = ParsedValue(PyFloat_AsDouble(valueFloat));
        Py_DECREF(valueFloat);
    } else if(PyString_Check(value)) {
        result = ParsedValue(PyString_AsString(value));
    } else if(PyTuple_Check(value) || PyList_Check(value)) {
        vector<ParsedValue>  list;
        ParsedValue          elem = ParsedValue(0);
        Py_ssize_t           i;
        PyObject            *item;
        PyObject            *valueRepr;
        

		bool isTuple = PyTuple_Check(value);
		unsigned size = isTuple ? PyTuple_Size(value) : PyList_Size(value);
		
		for(i = 0; i < size; i++) 
		{
			item = isTuple ? PyTuple_GetItem(value, i) : PyList_GetItem(value, i); //holds borrowed reference
			/* item is a borrowed reference */
            if(NULL == item)
                return true;
            if(Py2Cvalue(item, elem))
                return true;
            list.push_back(elem);
        }
        valueRepr = PyObject_Repr(value);
            /* Received new reference */
        if(NULL == valueRepr)
            return true;
        result = ParsedValue(list, PyString_AsString(valueRepr));
        Py_DECREF(valueRepr);
    } else if(PyObject_TypeCheck(value, &chType)) {
        ParsedChannel valueChan = ParsedChannel("","",0,0);
        valueChan = getCh(value);
        if(valueChan.id().empty())
            return true;
        result = ParsedValue(parser, parser->whichChannel(valueChan));
    } else {
        PyObject *valueRepr;
        valueRepr = PyObject_Repr(value);
            /* Received new reference */
        if(NULL == valueRepr)
            return true;
        result = ParsedValue(PyString_AsString(valueRepr), VTobject);
        Py_DECREF(valueRepr);
    }

    return false;
}

/**** Python Functions ****/

/*! \brief Implementation of the python \c event(channel,time,val) function
 *  \param[in] self Reference to class for methods. Unused here.
 *  \param[in] args List of unnamed function arguments.
 *  \param[in] kwds List of function arguments named with keywords.
 *  \return Py_NONE on success, NULL otherwise.
 *
 *  The three values channel, time and value are stored in Timing_parser,
 *  the data types are unsigned, double and double, respectively. Additionally,
 *  the position in the python script that caled this function gets stored.
 */
static PyObject* event(PyObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"channel", "time", "val", NULL};
    PyObject          *channelObj;
    ParsedChannel      chan     = ParsedChannel("","",0,0);
    unsigned           channel;
    double             time;
    PyObject          *valObj;

	ParsedPos          pos      = ParsedPos(parser);
    ParsedEvent       *event;

    assert(parser != NULL);

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!dO:event",
        const_cast<char**>(kwlist), &chType, &channelObj, &time, &valObj))
        /* channelObj is a borrowed reference */
        /* valObj is a borrowed reference */
        return NULL;

    /* Find channel number */
    chan = getCh(channelObj);
    if(chan.id().empty())
        return NULL;
    channel = parser->whichChannel(chan);

    /* Get current position in file */
    pos = getPos();
    if(pos.line == 0)
        return NULL;

    /* Check type of val argument, create ParsedEvent */
    if(PyNumber_Check(valObj)) {
        PyObject *valFloat;
        valFloat = PyNumber_Float(valObj);
            /* Received new reference */
        if(NULL == valFloat)
            return NULL;
        event = new ParsedEvent(channel, time, PyFloat_AsDouble(valFloat),
            pos);
        Py_DECREF(valFloat);
    } 
	else if( PyTuple_Check(valObj) || PyList_Check(valObj) ) 
	{
		//found a vector
		MixedValue vectorVal;

		convertPy2MixedValue(valObj, vectorVal);

  		event = new ParsedEvent(channel, time, vectorVal, pos);
	}
	else if(PyString_Check(valObj))
	{
		event = new ParsedEvent(channel, time, std::string( PyString_AsString(valObj) ), pos);
    } else {
        PyErr_SetString(PyExc_RuntimeError,
            "Value must be a number, a string or a tuple.");
        return NULL;
    }

    /* Add to the list of events */
    if(parser->addEvent(*event)) {
        stringstream buf;
        buf << "Tried to redefine event on channel ";
        buf << parser->channels()->at(channel).str();
        buf << " at ";
        buf << time;
        buf << "s";
        PyErr_SetString(PyExc_RuntimeError, buf.str().c_str());
        delete event;
        return NULL;
    }

    delete event;
    Py_RETURN_NONE;
}



bool convertPy2MixedValue(PyObject* obj, MixedValue& result)
{
	bool success = false;

	if(PyNumber_Check(obj))
	{
		double temp_d;
		if(convertPyObjectToDouble(obj, temp_d))
		{
			result = temp_d;
			success = true;
		}
	}
	else if(PyString_Check(obj))
	{
		std::string temp_s;
		if(convertPyObjectToString(obj, temp_s))
		{
			result = temp_s;
			success = true;
		}
	}
	else if(PyTuple_Check(obj) || PyList_Check(obj))
	{
		bool isTuple = PyTuple_Check(obj);
		
		unsigned size = isTuple ? PyTuple_Size(obj) : PyList_Size(obj);
		MixedValue value_i;

		for(unsigned i = 0; i < size; i++)
		{
			value_i.clear();

			PyObject* obj_i;	
			obj_i = isTuple ? PyTuple_GetItem(obj, i) : PyList_GetItem(obj, i); //holds borrowed reference
			success = convertPy2MixedValue( obj_i, value_i );

			if(success)
				result.addValue( value_i );
			else
				break;
		}
	}
	return success;
}

bool convertPyObjectToString(PyObject* obj, std::string& result)
{
	if( !PyString_Check(obj) )
		return false;

	result = std::string( PyString_AsString(obj) );

	return true;
}


bool convertPyObjectToDouble(PyObject* obj, double& result)
{
	if( !PyNumber_Check(obj) )
		return false;

	PyObject *temp;
	temp = PyNumber_Float(obj);
    /* Received new reference */
	if(NULL == temp)
	{
		Py_DECREF(temp);
	    return false;
	}

	result = PyFloat_AsDouble(temp);
	Py_DECREF(temp);

	return true;
}


/*! \brief Implementation of the python \c include(filename) function
 *  \param[in] self Reference to class for methods. Unused here.
 *  \param[in] args List of unnamed function arguments.
 *  \param[in] kwds List of function arguments named with keywords.
 *  \exception Various Exceptions thrown by Timing_readFile().
 *  \return Py_NONE on success, NULL otherwise.
 *
 *  Calls Timing_readFile().
 */
static PyObject* include(PyObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"file", NULL};
    char              *filename;

    assert(parser != NULL);

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s:include",
        const_cast<char**>(kwlist), &filename))
        return NULL;
    if(Timing_readFile(filename))
        return NULL;
    Py_RETURN_NONE;
}

/*! \brief Implementation of the python \c meas(channel,time,desc) function
 *  \param[in] self Reference to class for methods. Unused here.
 *  \param[in] args List of unnamed function arguments.
 *  \param[in] kwds List of function arguments named with keywords.
 *  \return Py_NONE on success, NULL otherwise.
 *
 *  The three values channel, time and desc are stored in Timing_parser,
 *  the data types are unsigned, double and string, respectively. Additionally,
 *  the position in the python script that caled this function gets stored.
 *  The third argument, desc, is optional and will be replaced by an empty
 *  string if missing.
 */
static PyObject* meas(PyObject* self, PyObject* args, PyObject* kwds)
{
    static const char* kwlist[] = {"channel", "time", "value", "desc", NULL};
    PyObject*          channelObj;
    ParsedChannel      chan     = ParsedChannel("","",0,0);
    unsigned           channel;
    double             time;
	PyObject*          valObj = NULL;
    const char*        desc     = NULL;
    ParsedPos          pos      = ParsedPos(parser);
	ParsedEvent*       event;

    assert(parser != NULL);

	//if no desc and value is string, send value as string and desc is empty

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O!d|Os:meas",
        const_cast<char**>(kwlist), &chType, &channelObj, &time, &valObj, &desc))
        /* channelObj is a borrowed reference */
		/* valObj is a borrowed reference */
        return NULL;

    /* Find channel number */
    chan = getCh(channelObj);
    if(chan.id().empty())
        return NULL;
    channel = parser->whichChannel(chan);

	/* Get current position in file */
    pos = getPos();
    if(pos.line == 0)
        return NULL;
    
    if(valObj == NULL)
	{
        event = new ParsedEvent(channel, time, MixedValue(), pos, true);	//using Empty MixedValue and no description
	}
	else
	{
		/* Check type of val argument, create ParsedEvent */
		if(PyNumber_Check(valObj)) {
			PyObject *valFloat;
			valFloat = PyNumber_Float(valObj);
				/* Received new reference */
			if(NULL == valFloat)
				return NULL;
			if(desc != NULL)
				event = new ParsedEvent(channel, time, PyFloat_AsDouble(valFloat), pos, std::string(desc));
			else
				event = new ParsedEvent(channel, time, PyFloat_AsDouble(valFloat), pos, true);

			Py_DECREF(valFloat);
		} 
		else if( PyTuple_Check(valObj) || PyList_Check(valObj) ) {
			//found a vector
			MixedValue vectorVal;
			convertPy2MixedValue(valObj, vectorVal);
 			if(desc != NULL)
	 			event = new ParsedEvent(channel, time, vectorVal, pos, std::string(desc));
			else
	 			event = new ParsedEvent(channel, time, vectorVal, pos, true);

		}
		else if(PyString_Check(valObj)) {
 			if(desc != NULL)
				event = new ParsedEvent(channel, time, std::string( PyString_AsString(valObj) ), pos, std::string(desc));
			else
				event = new ParsedEvent(channel, time, std::string( PyString_AsString(valObj) ), pos, true);	//valObj MIGHT be a description; let the STI_Device parser decide based on the device's channel type...
		} 
		else {
			PyErr_SetString(PyExc_RuntimeError,
				"Value must be a number, a string or a tuple.");
			return NULL;
		}
	}

	/* Add to the list of events */
	if(parser->addEvent(*event)) {
        stringstream buf;
        buf << "Tried to redefine measurement on channel ";
        buf << parser->channels()->at(channel).str();
        buf << " at ";
        buf << time;
        buf << "s";
        PyErr_SetString(PyExc_RuntimeError, buf.str().c_str());
		delete event;
        return NULL;
    }

	delete event;
    Py_RETURN_NONE;
}

/*! \brief Implementation of the python \c setvar(name,val) function
 *  \param[in] self Reference to class for methods. Unused here.
 *  \param[in] args List of unnamed function arguments.
 *  \param[in] kwds List of function arguments named with keywords.
 *  \exception RuntimeError value missing and no default available.
 *  \exception Various Exceptions thrown by PyRun_String.
 *  \return Py_NONE on success, NULL otherwise.
 */
static PyObject *
setvar(PyObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"name", "val", NULL};
    char              *name;
    PyObject          *value    = NULL;
    ParsedValue        valueC   = ParsedValue(0);
    ParsedPos          pos      = ParsedPos(parser);
	bool               isOverwritten = false;

    assert(mainModule != NULL);
    assert(parser     != NULL);

//	cout << "setvar(" << args

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s|O:setvar",
        const_cast<char**>(kwlist), &name, &value))
        return NULL;
        /* value is a borrowed reference or NULL */
    if(parser->overwritten.count(name) == 0)
        if(value == NULL) {
            string buf;
            buf = "Defined variable ";
            buf += name;
            buf += " without a value";
            PyErr_SetString(PyExc_RuntimeError, buf.c_str());
            return NULL;
        } else
            Py_INCREF(value); /* Got value as parameter, need to temporarily
                                 take posession of it (to have a valid DECREF
                                 later) */
    else {  /* Have valid overwrite, ignore value parameter */
        value = PyRun_String(parser->overwritten[name].c_str(), Py_eval_input,
            mainDict, mainDict);
		isOverwritten = true;
        /* Received new reference */
        if(value == NULL)
            return NULL;
    }

    /* Store the value in the dictionary */
    if(PyObject_SetAttrString(mainModule, name, value)) {
        Py_DECREF(value);
        return NULL;
    }

    /* Create valueC, based on value typ and read current position */
    if(Py2Cvalue(value, valueC)) {
        Py_DECREF(value);
        return NULL;
    }
    Py_DECREF(value);
    pos = getPos();
    if(pos.line == 0)
        return NULL;

    /* Store variable in list of variables */
    if(parser->addVariable(ParsedVar(name, valueC, pos, isOverwritten))) {
        string buf;
        buf = "Tried to redefine variable ";
        buf += name;
        PyErr_SetString(PyExc_RuntimeError, buf.c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}


/*! \brief Implementation of the python \c setvar(name,val) function
 *  \param[in] self Reference to class for methods. Unused here.
 *  \param[in] args List of unnamed function arguments.
 *  \param[in] kwds List of function arguments named with keywords.
 *  \exception RuntimeError value missing and no default available.
 *  \exception Various Exceptions thrown by PyRun_String.
 *  \return Py_NONE on success, NULL otherwise.
 */



static PyObject *
settag(PyObject *self, PyObject *args, PyObject *kwds)
{
    static const char *kwlist[] = {"name", "time", NULL};
    char              *name;
    PyObject          *timeObj = NULL;
    ParsedPos          pos      = ParsedPos(parser);

    assert(mainModule != NULL);
    assert(parser     != NULL);

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "s|O:settag",
        const_cast<char**>(kwlist), &name, &timeObj))
        return NULL;

	bool timeIncluded = false;
	double time = 0;
	
	if(timeObj != NULL) {
		timeIncluded = convertPyObjectToDouble(timeObj, time);
	}

    pos = getPos();
    if(pos.line == 0)
        return NULL;

    /* Store tag in list of tags */
	if(parser->addTag(ParsedTag(name, time, pos, timeIncluded, parser->events()->size()))) {

		//This tag is a duplicate.
		//Quietly ignore this settag command.

        //string buf;
        //buf = "Tried to redefine tag ";
        //buf += name;
        //PyErr_SetString(PyExc_RuntimeError, buf.c_str());
        //return NULL;
    }

    Py_RETURN_NONE;
}


/*! \brief The data structure used to register the \link timing_module Timing
 *      module\endlink with Python
 */
static PyMethodDef methods[] = {
    {"event",   (PyCFunction)event,   METH_VARARGS|METH_KEYWORDS,
     "Create an event on a channel and time.\n"
     "\n"
     "The value on the channel changes instantaniously at time.\n"},
    {"include", (PyCFunction)include, METH_VARARGS|METH_KEYWORDS,
     "Reads in a file and executes it.\n"
     "\n"
     "The dictionary during evaluating the script is set to global.\n"},
    {"meas",    (PyCFunction)meas,    METH_VARARGS|METH_KEYWORDS,
     "Schedule a measurement on a channel and time.\n"
     "\n"
     "The measurement is single-shot, i.e. no measurement curve is taken.\n"},
    {"setvar",  (PyCFunction)setvar,  METH_VARARGS|METH_KEYWORDS,
     "Sets a constant to a value.\n"
     "\n"
     "The name of the constant must be given as a string, the resulting\n"
     "variable is put in the global context. If the variable is contained in\n"
     "the list of overwritten variables, then use that value instead of val\n"
     "to define the variable.\n"},
	 {"settag",  (PyCFunction)settag,  METH_VARARGS|METH_KEYWORDS,
     "Defines a named tag.\n"
     "\n"
     "The name of the tag must be given as a string. A time can be optionally\n"
     "provided. If no time is given, the time of the tag is determined by the\n"
     "times of the events surrounding the tag in the timing file.\n"},
    {NULL, NULL, 0, NULL}
};

/**** C Functions ****/

/*! \param[in] parser Pointer to the instance of Parser that should be used
 *      in the module.
 *  \return 0 on success, otherwise -1.
 *
 *  - Initializes all global variables of the \link timing_module Timing
 *    module\endlink.
 *  - Registers all functions and classes with Python.
 *
 *  The \a parser object is stored in ::parser. It needs to stay valid until
 *  after the program is finished using the module.
 *
 *  If an error occured, you do not need to call Timing_Finalize().
 *
 *  \warning It is probably wrong to call this function more than once per
 *  run of the Python environment.
 */
int
Timing_Initialize(libPython::Parser *parser)
{
    PyObject *inspectModule   = NULL;
//    PyObject *numpyModule     = NULL;
    PyObject *osModule;
    PyObject *ospathModule;
    PyObject *timingModule;

    assert(Py_IsInitialized());

    /* Initialize global and local C++ references to Python objects */
    mainModule = PyImport_AddModule("__main__");
    if(NULL == mainModule)
        goto ErrorHappend;
    Py_INCREF(mainModule); /* Own borrowed reference */
    mainDict = PyModule_GetDict(mainModule);
    if(NULL == mainDict)
        goto ErrorHappend;
    Py_INCREF(mainDict); /* Own borrowed reference */
    inspectModule = PyImport_ImportModule("inspect");
        /* Received new reference */
    if(NULL == inspectModule)
        goto ErrorHappend;
//	numpyModule = PyImport_ImportModule("numpy");
        /* Received new reference */
//    if(NULL == numpyModule)
//        goto ErrorHappend;
    stackFunction = PyObject_GetAttrString(inspectModule, "stack");
        /* Received new reference */
    if(NULL == stackFunction)
        goto ErrorHappend;
    getfileFunction = PyObject_GetAttrString(inspectModule, "getfile");
        /* Received new reference */
    if(NULL == getfileFunction)
        goto ErrorHappend;
    getlinenoFunction = PyObject_GetAttrString(inspectModule, "getlineno");
        /* Received new reference */
    if(NULL == getlinenoFunction)
        goto ErrorHappend;
    Py_DECREF(inspectModule);
    osModule = PyImport_AddModule("os");
        /* Reference is only borrowed */
    if(NULL == osModule)
        goto ErrorHappend;
    chdirFunction = PyObject_GetAttrString(osModule, "chdir");
        /* Received new reference */
    if(NULL == chdirFunction)
        goto ErrorHappend;
    getcwdFunction = PyObject_GetAttrString(osModule, "getcwd");
        /* Received new reference */
    if(NULL == getcwdFunction)
        goto ErrorHappend;
    ospathModule = PyImport_AddModule("os.path");
        /* Reference is only borrowed */
    if(NULL == ospathModule)
        goto ErrorHappend;
    abspathFunction = PyObject_GetAttrString(ospathModule, "abspath");
        /* Received new reference */
    if(NULL == abspathFunction)
        goto ErrorHappend;
    dirnameFunction = PyObject_GetAttrString(ospathModule, "dirname");
        /* Received new reference */
    if(NULL == dirnameFunction)
        goto ErrorHappend;
    normcaseFunction = PyObject_GetAttrString(ospathModule, "normcase");
        /* Received new reference */
    if(NULL == normcaseFunction)
        goto ErrorHappend;
    libPythonPrivate::parser = parser;

    /* Add module functions to python */
    timingModule = Py_InitModule3("stipy", methods,
        "Timing related functions.\n"
        "\n"
        "This is a dummy file used to provide a simulation of using the\n"
        "timing module provided by the C-code to the embedded Python\n"
        "environment. With this file, you can test out your code without the\n"
        "embedded environment.\n");
        /* Reference is only borrowed */
    if(NULL == timingModule)
        goto ErrorHappend;
	
	//// required for numPy
	//// overriding import_array1(-1):
	//if (_import_array() < 0) 
	//{
	//	PyErr_Print(); 
	//	PyErr_SetString(PyExc_ImportError, "numpy.core.multiarray failed to import"); 
	////	return ret; 
 //       goto ErrorHappend;
	//}

	/* Add class variables to python */
    if(chObject_Initialize(timingModule, parser))
        goto ErrorHappend;
    if(devObject_Initialize(timingModule, parser))
        goto ErrorHappend;

    return 0;

ErrorHappend:
    Py_XDECREF(inspectModule);
//	Py_XDECREF(numpyModule);
    Timing_Finalize();
    return -1;
}

/*!
 *  This will make the module non-usable. You should do it before calling
 *  \c PyFinalize().
 */
void
Timing_Finalize()
{
    assert(Py_IsInitialized());

    /* De-Reference all owned Python objects */
    Py_XDECREF(mainDict);
    mainDict          = NULL;
    Py_XDECREF(mainModule);
    mainModule        = NULL;
    Py_XDECREF(stackFunction);
    stackFunction     = NULL;
    Py_XDECREF(getfileFunction);
    getfileFunction   = NULL;
    Py_XDECREF(getlinenoFunction);
    getlinenoFunction = NULL;
    Py_XDECREF(chdirFunction);
    chdirFunction     = NULL;
    Py_XDECREF(getcwdFunction);
    getcwdFunction    = NULL;
    Py_XDECREF(abspathFunction);
    abspathFunction   = NULL;
    Py_XDECREF(dirnameFunction);
    dirnameFunction   = NULL;
    Py_XDECREF(normcaseFunction);
    normcaseFunction  = NULL;

    /* Deregister objects from Python */
    chObject_Finalize();
    devObject_Finalize();

    /* Get rid of all other global variables */
    parser           = NULL;
}

/*! \brief Identical to \c PyRun_String(), but sets a custom file name.
 *  \param[in] str      The string to evaluate.
 *  \param[in] filename The filename to use.
 *  \param[in] start    One of \c Py_eval_input, \c Py_file_input,
 *                      \c Py_single_input.
 *  \param[in] globals  The global dictionary.
 *  \param[in] locals   The local dictionary or NULL.
 *
 *  This function should behave identically to \c PyRun_String(), but allows
 *  to set the \a filename where the string comes from. The normal
 *  \c PyRun_String() function always sets this to "<string>".
 *
 *  \note This function is basically a direct copy from Python/pythonrun.c
 *      from the Python2.4 sources. So all functionallity is done by those
 *      authors, all bugs are by Olaf Mandel.
 */
PyObject *
PyRun_StringFilename(const char *str, const char *filename, int start,
    PyObject *globals, PyObject *locals)
{
#ifdef HAVE_LIBPYTHON2_5
    PyObject *ret = NULL;
    _mod     *mod;
    PyArena  *arena;

    arena = PyArena_New();
    if(arena == NULL)
        return NULL;
    mod = PyParser_ASTFromString(str, filename, start, NULL, arena);
    if(mod != NULL) {
        PyCodeObject *co;
        co = PyAST_Compile(mod, filename, NULL, arena);
        if(co == NULL)
            return NULL;
        ret = PyEval_EvalCode(co, globals, locals);
        Py_DECREF(co);
    }
    PyArena_Free(arena);
    return ret;
#endif
#ifdef HAVE_LIBPYTHON2_4
    node         *n;
    PyCodeObject *co;
    PyObject     *v;

    n = PyParser_SimpleParseStringFlagsFilename(str, filename, start, 0);
    if(NULL == n)
        return NULL;
    co = PyNode_Compile(n, filename);
    PyNode_Free(n);
    if(co == NULL)
        return NULL;
    v = PyEval_EvalCode(co, globals, locals);
    Py_DECREF(co);
    return v;
#endif
}

/*! \param[in] filename The (relative or absolute) name of the file to read
 *      in and parse.
 *  \exception RuntimeError circular include detected.
 *  \exception Various Everything that the Python parser creates while reading
 *             the file.
 *  \return 0 if successfull, -1 otherwise.
 *
 *  The working directory gets temporarily changed to the directory where the
 *  file resides. Please use the Unix name convention ('/' instead of '\').
 *
 *  \note We do not use \c PyRun_FileEx(), because under Windows all calls
 *      to functions with \c FILE* parameters are fragile and can cause a
 *      crash of the system (happens because the library and the program
 *      are compiled with different internal representations of the
 *      \c FILE structure). We use PyRun_StringFilename instead.
 *  \bug  Needs to use the base path at all.
 *  \todo Needs to check if we leave the base path.
 */
int
Timing_readFile(const std::string &filename)
{
    PyObject *ret1        = NULL;
    PyObject *ret2        = NULL;
    PyObject *oldpwd      = NULL;
    PyObject *fullpathobj = NULL;
    char     *fullpath;
    string    fullpathstr;
    ifstream  strm;
    stringbuf buffer;
    vector<string>::const_iterator i;

    assert(Py_IsInitialized());
    assert(mainDict         != NULL);
    assert(chdirFunction    != NULL);
    assert(getcwdFunction   != NULL);
    assert(abspathFunction  != NULL);
    assert(dirnameFunction  != NULL);
    assert(normcaseFunction != NULL);

    /* Use python to get the current-working-directory */
    oldpwd = PyObject_CallObject(getcwdFunction, NULL);
        /* Received new reference */
    if(NULL == oldpwd)
        goto ErrorHappend;

    /* Use python to get the absolute path of the file */
    ret1 = PyObject_CallFunction(abspathFunction, const_cast<char*>("s"),
        filename.c_str());
        /* Received new reference */
    if(NULL == ret1)
        goto ErrorHappend;
    fullpathobj = PyObject_CallFunctionObjArgs(normcaseFunction, ret1, NULL);
        /* Received new reference */
    if(NULL == fullpathobj)
        goto ErrorHappend;
    Py_DECREF(ret1);
    ret1 = NULL;
    fullpath = PyString_AsString(fullpathobj);
        /* Only valid while parent object lives */
    if(NULL == fullpath)
        goto ErrorHappend;

    /* Check for circular includes */
    fullpathstr = string(fullpath); /* For speedup in the loop */
    for(i=fileStack.begin(); i != fileStack.end(); ++i)
//        if(*i == fullpathstr)
		if(i->compare(fullpathstr) == 0)
            break;
    if(i != fileStack.end()) {
        string buf;
        buf="Circular include: ";
        for( ; i != fileStack.end(); ++i)
            buf+=string(*i)+" ";
        buf+=fullpathstr;
        PyErr_SetString(PyExc_RuntimeError, buf.c_str());
        goto ErrorHappend;
    }

    /* Change the directory to our new home. */
    ret1 = PyObject_CallFunctionObjArgs(dirnameFunction, fullpathobj, NULL);
        /* Received new reference */
    if(NULL == ret1)
        goto ErrorHappend;
    ret2 = PyObject_CallFunctionObjArgs(chdirFunction, ret1, NULL);
        /* Received new reference */
    if(NULL == ret2)
        goto ErrorHappend;
    Py_DECREF(ret1);
    ret1 = NULL;
    Py_DECREF(ret2);
    ret2 = NULL;

    /* Open and run the file (adding the file to the lists) */
    strm.open(fullpath);
    if(!strm) {
        string buf;
        buf="Couldn't open file: ";
        buf+=fullpathstr;
        PyErr_SetString(PyExc_IOError, buf.c_str());
        goto ErrorHappend;
    }
    strm >> &buffer;
    /* This hack is needed for at least one version of the stdlib:
     * eof() does not get set by operator>>(streambuf *) */
    if(strm.good() && !strm.eof())
        strm.get();
    if(!strm.eof()) {
        strm.close();
        string buf;
        buf="Couldn't read complete file: ";
        buf+=fullpathstr;
        PyErr_SetString(PyExc_IOError, buf.c_str());
        goto ErrorHappend;
    }
    strm.close();
    parser->whichFile(fullpathstr);
    fileStack.push_back(fullpathstr);
    Py_DECREF(fullpathobj);
    fullpathobj = NULL;
    ret1 = PyRun_StringFilename(buffer.str().c_str(), fullpathstr.c_str(),
        Py_file_input, mainDict, NULL);
        /* Received new reference */
    if(NULL == ret1)
        goto ErrorHappend;
    Py_DECREF(ret1);
    ret1 = NULL;

    /* Go back to our old home. Clean fileStack. */
    ret1 = PyObject_CallFunctionObjArgs(chdirFunction, oldpwd, NULL);
        /* Received new reference */
    if(NULL == ret1)
        goto ErrorHappend;
    fileStack.pop_back();

    Py_DECREF(ret1);
    ret1 = NULL;
    Py_DECREF(oldpwd);
    oldpwd = NULL;

    return 0;

ErrorHappend:
	//Added by Jason Hogan to allow exceptions and parsing errors to be reported.
	//In the event of a parsing error, PyErr_Occurred() and PyErr_Print() must 
	//be called before Py_XDECREF because Py_XDECREF seems to delete all errors.
	if(PyErr_Occurred())
		PyErr_Print();
    if(oldpwd != NULL) {
        Py_XDECREF(PyObject_CallFunctionObjArgs(chdirFunction, oldpwd, NULL));
    }
	fileStack.clear();
    Py_XDECREF(ret1);
    Py_XDECREF(ret2);
    Py_XDECREF(oldpwd);
    Py_XDECREF(fullpathobj);
    return -1;
}

/*! \param[in] code The Python code to evaluate
 *  \exception Various Everything that the Python parser creates while reading
 *             the code.
 *  \return 0 if successfull, -1 otherwise.
 *
 *  \note We provide this function as an alternative to \c PyRun_SimpleString
 *      to not have the extra newline being output.
 */
int
Timing_evaluate(const std::string &code)
{
    PyObject *ret1        = NULL;

    assert(Py_IsInitialized());
    assert(mainDict         != NULL);

    ret1 = PyRun_String(code.c_str(), Py_file_input, mainDict, NULL);
        /* Received new reference */
    if(NULL == ret1)
        return -1;
    Py_DECREF(ret1);

    return 0;
}

/*! \return 0 if successfull, -1 otherwise.
 *
 *  This will add all variables in the mainDict to the Parser::f_variables
 *  list. The position is left empty, so you can easily distinguish these
 *  entries from the setvar() entries.
 *
 *  \note You can only call this function once, because afterwards the
 *        variables are contained in Parser::f_variables.
 */
int
Timing_readvars()
{
    PyObject    *nameList = NULL;
    Py_ssize_t   i;
    PyObject    *nameObj;
    char        *name;
    PyObject    *value    = NULL;
    ParsedValue  valueC   = ParsedValue(0);

    assert(Py_IsInitialized());
    assert(mainDict != NULL);
    assert(parser   != NULL);

    /* Get all variable names */
    nameList = PyObject_Dir(mainModule);
        /* Received new reference */
    if(NULL == nameList)
        goto ErrorHappened;

    /* Loop through all variables */
    for(i=0; i<PyList_Size(nameList); i++) {

        /* Get the name */
        nameObj = PyList_GetItem(nameList, i);
            /* nameObj is a borrowed reference */
        if(NULL == nameObj)
            goto ErrorHappened;
        name = PyString_AsString(nameObj);
            /* Only valid while parent object lives */
        if(NULL == name)
            goto ErrorHappened;

        /* Get the variable value as repr() */
        value = PyObject_GetAttr(mainModule, nameObj);
            /* Received new reference */
        if(NULL == value)
            goto ErrorHappened;
        if(Py2Cvalue(value, valueC))
            goto ErrorHappened;
        Py_DECREF(value);

        /* Add to parser */
        if(parser->addVariable(ParsedVar(name, valueC))) {
            string buf;
            buf = "Unknown error adding variable ";
            buf += name;
            buf += " to parser list.";
            PyErr_SetString(PyExc_RuntimeError, buf.c_str());
            goto ErrorHappened;
        }
    }

    Py_DECREF(nameList);
    return 0;

ErrorHappened:
    Py_XDECREF(nameList);
    Py_XDECREF(value);
    return -1;
}

};

/*! \page timing_module Timing module
 *
 *  \todo Add documentation!
 */


 /*

 //incorporate eval into setvar definition to extract expression information

def dump(expression):
    result = eval(expression)
    print expression, "=>", result, type(result)
 
 */