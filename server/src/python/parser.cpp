/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class Parser
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
#include "parser.h"
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
#include "antikbdint.h"
#include "listenerobject.h"
#include "timing.h"

//#include <windows.h>

using std::string;
using std::vector;
using libPythonPrivate::AntiKbdInt_Initialize;
using libPythonPrivate::AntiKbdInt_Finalize;
using libPythonPrivate::listenerObject_Initialize;
using libPythonPrivate::listenerObject_Finalize;
using libPythonPrivate::Timing_Initialize;
using libPythonPrivate::Timing_Finalize;
using libPythonPrivate::Timing_readFile;
using libPythonPrivate::Timing_evaluate;
using libPythonPrivate::Timing_readvars;

namespace libPython
{

/*!
 *  Initializes the parser with all values empty.
 */
Parser::Parser()
{
    f_channels     = new vector<ParsedChannel>;
    f_events       = new vector<ParsedEvent>;
    f_measurements = NULL;
    f_files        = new vector<string>;
    f_variables    = new vector<ParsedVar>;
}

/*!
 *  Frees up the parser.
 */
Parser::~Parser()
{
    delete f_channels;
    delete f_events;
    if(f_measurements != NULL)
        delete f_measurements;
    delete f_files;
    delete f_variables;
}

/*! \param[in] filename The name of the file to parse.
 *  \return \c false on success, otherwise \c true.
 *
 *  - Clears all internal variables.
 *  - Brings up the Python environment
 *  - Calls Timing_readFile()
 *  - Reads all Python variables
 *  - Sets #f_mainFile
 *  - Takes down the Python environment
 *
 *  Bringing up and taking down the Python environment for every run are done
 *  to ensure that no global variables remain between runs.
 *
 *  If an error occured, the internal lists are cleared again, but the message
 *  strings are left intact.
 *
 *  \bug There is a memory leak in Python! Just calling \c Py_Initialize()
 *      and \c Py_Finalize() will waste nearly 9kB of RAM!!!
 */
bool Parser::parseFile(std::string filename)
{
    cleanup();
    f_errMsg.erase();
    f_outMsg.erase();

    if(PythonUp())
        return true;
    if(Timing_readFile(filename) != 0) {
		if(PyErr_Occurred())
			PyErr_Print();
        PythonDown();
        cleanup();
        return true;
    }
    if(Timing_readvars() != 0) {
        if(PyErr_Occurred())
            PyErr_Print();
        PythonDown();
        cleanup();
        return true;
    }
    f_mainFile = f_files->at(0);
    PythonDown();

    return false;
}

/*! \param[in] code The verbatim Python code to work with.
 *  \return \c false on success, otherwise \c true.
 *
 *  - Clears all internal variables.
 *  - Brings up the Python environment
 *  - Calls Timing_evaluate()
 *  - Reads all Python variables
 *  - Sets #f_code
 *  - Takes down the Python environment
 *
 *  Bringing up and taking down the Python environment for every run are done
 *  to ensure that no global variables remain between runs.
 *
 *  If an error occured, the internal lists are cleared again, but the message
 *  strings are left intact.
 *
 *  \bug There is a memory leak in Python! Just calling \c Py_Initialize()
 *      and \c Py_Finalize() will waste nearly 9kB of RAM!!!
 */
bool Parser::parseString(std::string code)
{
    cleanup();
    f_errMsg.erase();
    f_outMsg.erase();

    if(PythonUp())
        return true;
    if(Timing_evaluate(code.c_str()) != 0) {
        if(PyErr_Occurred())
            PyErr_Print();
        PythonDown();
        cleanup();
        return true;
    }
    if(Timing_readvars() != 0) {
        if(PyErr_Occurred())
            PyErr_Print();
        PythonDown();
        cleanup();
        return true;
    }
    f_code = code;
    PythonDown();

    return false;
}

/*! \return A constant pointer to #f_channels.
 *
 *  This access method prevents unwitting changes to #f_channels from outside
 *  code by casting the pointer const.
 */
const vector<ParsedChannel>* Parser::channels() const
{
    return f_channels;
}

/*! \param[in] channel The channel whose address in #f_channels to search.
 *  \return index in #f_channels.
 *
 *  Searches for \a channel in #f_channels. If it is present, return its
 *  position. If it is missing, append it to the list and return that position.
 */
int Parser::whichChannel(const ParsedChannel &channel)
{
    int i;

    for(i=f_channels->size()-1; i>=0; --i)
        if(f_channels->at(i) == channel)
            return i;

    f_channels->push_back(channel);
    return f_channels->size()-1;
}

/*! \return A constant pointer to #f_events.
 *
 *  This access method prevents unwitting changes to #f_events from outside
 *  code by casting the pointer const.
 */
const vector<ParsedEvent>* Parser::events() const
{
    return f_events;
}

/*! \param[in] event The event to add to #f_events.
 *  \return \c false on success, \c true otherwise.
 *
 *  Adds \a event to #f_events, if it is not already contained in the vector.
 *  If it is, returns \c true and does nothing. Also, if #f_events was
 *  changed, delete the buffered #f_measurements.
 *
 *  For determining if an event is already contained, the event channel,
 *  time and value must match but the position must mismatch.
 */
bool Parser::addEvent(const ParsedEvent& event)
{
    vector<ParsedEvent>::const_iterator i, imax;

    for(i=f_events->begin(), imax=f_events->end(); i!=imax; ++i)
        if(i->nearlyEqual(event) && i->position != event.position)
            return true;

    f_events->push_back(event);
    if(f_measurements != NULL) {
        delete f_measurements;
        f_measurements = NULL;
    }
    return false;
}

/*! \return A constant pointer to #f_measurements.
 *
 *  This filters the #f_events list and buffers the result in #f_measurements.
 *  The filter accepts only events of type MeasureEvent.
 *
 *  This method prevents unwitting changes to #f_measurements from outside
 *  code by casting the pointer const.
 */
const vector<const ParsedEvent *>* Parser::measurements()
{
    vector<ParsedEvent>::const_iterator i, imax;

    if(f_measurements != NULL)
        return f_measurements;

    f_measurements = new vector<const ParsedEvent *>;
    for(i=f_events->begin(), imax=f_events->end(); i!=imax; ++i)
		if( i->isMeasureEvent() )
            f_measurements->push_back(&(*i));

    return f_measurements;
}

/*! \return A constant pointer to #f_files.
 *
 *  This access method prevents unwitting changes to #f_files from outside
 *  code by casting the pointer const.
 */
const vector<std::string>* Parser::files() const
{
    return f_files;
}

/*! \param[in] file The file to whose address to search in #f_files.
 *  \return index in #f_files or -1 if \a has not been added to #f_files.
 *
 *  Searches for \a file in #f_files. If it is present, return its position.
 *  If it is missing, append it to the list and return that position.
 *  This method will not add the file name "<string>" to the list, because
 *  this is the file name used by Python to mark verbatim code input.
 */
int
Parser::whichFile(const std::string &file)
{
    int i;

    if(file == "<string>")
        return -1;

    for(i=f_files->size()-1; i>=0; --i)
        if(f_files->at(i) == file)
            return i;

    f_files->push_back(file);
    return f_files->size()-1;
}

/*! \return A constant pointer to #f_mainFile.
 *
 *  This access method prevents unwitting changes to #f_mainFile from outside
 *  code by casting the reference const.
 */
const std::string& Parser::mainFile() const
{
    return f_mainFile;
}

/*! \return A constant pointer to #f_variables.
 *
 *  This access method prevents unwitting changes to #f_variables from outside
 *  code by casting the pointer const.
 */
const vector<ParsedVar>* Parser::variables() const
{
    return f_variables;
}

/*! \param[in] variable The variable to add to #f_variables.
 *  \return \c false on success, \c true otherwise.
 *
 *  Adds \a variable to #f_variables, if the variable is not yet in the list.
 *  If it is, does nothing and returns \c true if the old and new values or
 *  positions differ and \c false if they are identical.
 *
 *  For determining if a variable is already contained, the variable name
 *  must match and the position must either be present or absent in both
 *  the entry in the list and the new variable.
 */
bool Parser::addVariable(const ParsedVar &variable)
{

    std::vector<ParsedVar>::const_iterator i, imax;

    for(i=f_variables->begin(), imax=f_variables->end(); i!=imax; ++i)
        if(i->name == variable.name) {
            if (i->position == NULL && variable.position == NULL)
                return i->value != variable.value;
            if (i->position != NULL && variable.position != NULL)
                return (i->value != variable.value)
                    || (i->position != variable.position);
        }

    f_variables->push_back(variable);

    return false;
}

/*! \return A constant pointer to the content of variable "description".
 *
 *  Searches for the variable "description" in #f_variables. If such
 *  a variable does not exist, return a reference to an empty string
 *  instead.
 *
 *  It is not possible to destinguish between an empty description and
 *  a non-existing description using this function. Use variables() in
 *  this case.
 */
const std::string Parser::description() const
{
    static string empty;
    std::vector<ParsedVar>::const_iterator i, imax;

    for(i=f_variables->begin(), imax=f_variables->end(); i!=imax; ++i)
        if(i->name == "description")
            return i->value.str();

    return empty;
}

/*! \return A constant pointer to #f_errMsg.
 *
 *  This access method prevents unwitting changes to #f_errMsg from outside
 *  code by casting the reference const.
 */
const std::string &
Parser::errMsg() const
{
    return f_errMsg;
}

/*! \return A constant pointer to #f_outMsg.
 *
 *  This access method prevents unwitting changes to #f_outMsg from outside
 *  code by casting the reference const.
 */
const std::string &
Parser::outMsg() const
{
    return f_outMsg;
}

/*! \return \c false on success, \c true otherwise
 *
 *  This method brings up Python. In detail, this includes:
 *
 *  - Initializes the Python environment.
 *  - Connects two listenerObject to get #f_errMsg and #f_outMsg.
 *  - Switches off CTRL-C
 *  - Registers the \link timing_module Timing module\endlink.
 *
 *  If an error occurs, Python is not up (you do not need to call
 *  PythonDown() ), but #f_errMsg may contain the error message.
 */
bool
Parser::PythonUp()
{
    assert(!Py_IsInitialized());

    Py_Initialize();
    if(listenerObject_Initialize(&f_outMsg, &f_errMsg)) {
        PyErr_Clear();
        Py_Finalize();
        return true;
    }
    if(AntiKbdInt_Initialize()) {
        if(PyErr_Occurred())
            PyErr_Print();
        listenerObject_Finalize();
        PyErr_Clear();
        Py_Finalize();
        return true;
    }
    if(Timing_Initialize(this)) {
        if(PyErr_Occurred())
            PyErr_Print();
        AntiKbdInt_Finalize();
        if(PyErr_Occurred())
            PyErr_Print();
        listenerObject_Finalize();
        PyErr_Clear();
        Py_Finalize();
        return true;
    }

    return false;
}

/*!
 *  This method takes down Python. In detail, this includes:
 *
 *  - Unregisters the \link timing_module Timing module\endlink.
 *  - Re-enables CTRL-C
 *  - Disconnects the two listenerObject.
 *  - Finalizes the Python environment.
 *
 *  If an error occurs, #f_errMsg may contain the error message.
 */
void
Parser::PythonDown()
{
    assert(Py_IsInitialized());

    Timing_Finalize();
    if(PyErr_Occurred())
        PyErr_Print();
    AntiKbdInt_Finalize();
    if(PyErr_Occurred())
        PyErr_Print();
    listenerObject_Finalize();
    PyErr_Clear();
    Py_Finalize();

	    
	//if (_multiarray_module_loaded) {

 //     HINSTANCE hDLL = NULL;

	//  LPCTSTR buf = string("multiarraymodule.dll").c_str();
 //     hDLL = LoadLibraryEx(buf, NULL,LOAD_WITH_ALTERED_SEARCH_PATH);

 //     FreeLibrary(hDLL);

 //     FreeLibrary(hDLL);

 //   }

}

/*!
 *  This cleans up all internal variables that are populated by a call to
 *  parseFile() or parseString().
 */
void
Parser::cleanup()
{
    f_channels->clear();
    f_code.erase();
    f_events->clear();
    if(f_measurements != NULL) {
        delete f_measurements;
        f_measurements = NULL;
    }
    f_files->clear();
    f_mainFile.erase();
    f_variables->clear();
}

};

/*! \namespace libPython
 *  \brief The namespace provided by the Python library
 *
 *  You can find all functionality that the Python library provides to
 *  you under this namespace, beginning from the Parser object.
 */
/*! \namespace libPythonPrivate
 *  \brief The namespace internally used by the Python library
 *
 *  The elements in this namespace are internal to the library, you
 *  should not have to use them.
 */
