/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class Parser
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

#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <string>
#include <vector>
#include "parsedchannel.h"
#include "parsedevent.h"
#include "parsedvar.h"

namespace libPython
{

/*! \brief The Parser class provides the central interface to all parsing
 *      activity and information
 *
 *  This class is used by the \link timing_module Timing module\endlink.
 */
class Parser
{
    /*! \brief Internal variable for the list of channels
     *
     *  This variable is accessible through whichChannel() and channels().
     *  The only way to clear this list is through parseFile() or
     *  parseString(). No means to edit the list are provided.
     */
    std::vector<ParsedChannel> *f_channels;
    /*! \brief Internal variable for verbatim code
     *
     *  This variable is accessible through code(). It gets set by
     *  parseFile() and parseString(). No means to edit the variable are
     *  provided.
     */
    std::string                 f_code;
    /*! \brief Internal variable for the list of events
     *
     *  This variable is accessible through addEvent() and events().
     *  The only way to clear this list is through parseFile() or
     *  parseString(). No means to edit the list are provided.
     */
    std::vector<ParsedEvent>   *f_events;
    /*! \brief Internal variable for the list of measurements
     *
     *  This variable is accessible through measurements().
     *  It is automatically created by measurements() and contains a copy
     *  of all those elements of #f_events that are of type MeasureEvent.
     *  No means to edit the list are provided.
     *
     *  \note This element is not allocated by default. Only when
     *        measurements() is called is this allocated. It gets deallocated
     *        whenever #f_events gets changed.
     */
    std::vector<const ParsedEvent *> *f_measurements;
    /*! \brief Internal variable for the list of files
     *
     *  This variable is accessible through whichFile() and files().
     *  The only way to clear this list is through parseFile() or
     *  parseString(). No means to edit the list are provided.
     */
    std::vector<std::string>   *f_files;
    /*! \brief Internal variable for the main file filename
     *
     *  This variable is accessible through mainFile(). It gets set by
     *  parseFile() and parseString(). No means to edit the variable are
     *  provided.
     *
     *  \note This variable is not identical to #f_files[0] ! If the
     *      input was a verbatim string, it is missing from #f_files.
     *      In that case, #f_mainFile is the empty string.
     */
    std::string                 f_mainFile;
    /*! \brief Internal variable for the list of (Python) variables
     *
     *  This variable is accessible through addVariable() and variables().
     *  The only way to clear this list is through parseFile() or
     *  parseString(). No means to edit the list are provided.
     */
    std::vector<ParsedVar>     *f_variables;
    /*! \brief Internal buffer for the error output of the Python script
     *
     *  This variable is accessible through errMsg(). The only way to clear
     *  this string is through parseFile() or parseString(). No means to edit
     *  this string are provided.
     *
     *  \note To populate the string, the listenerObject is given unrestricted
     *  access to this string through listenerObject_Initialize().
     */
    std::string                 f_errMsg;
    /*! \brief Internal buffer for the standard output of the Python script
     *
     *  This variable is accessible through outMsg(). The only way to clear
     *  this string is through parseFile() or parseString(). No means to edit
     *  this string are provided.
     *
     *  \note To populate the string, the listenerObject is given unrestricted
     *  access to this string through listenerObject_Initialize().
     */
    std::string                 f_outMsg;

    /*! \brief Initializes the Python environment */
    bool PythonUp();
    /*! \brief Take the Python environment down */
    void PythonDown();
    /*! \brief Cleans all valiables to the state before parsing */
    void cleanup();

public:
    /*! \brief The list of overwritten variables

     *  The map works like this: NAME -> VALUE
     *
     *  This object property is completely unprotected, so no assumptions on
     *  the content of this map and the set of variables from the last parsing
     *  run must be made.
     */
    std::map<std::string,std::string> overwritten;

    /*! \brief Copy constructor
     *  \param[in] src The object to be copied.
     *
     *  This is intentionally left unimplemented. This way, the compiler will
     *  not create a default copy constructor, which would not handle the
     *  memory allocation correctly and cause crashes.
     *
     *  \warning Do not implement this constructor.
     */
    Parser(const Parser &src);
    /*! \brief Default constructor */
    Parser();
    /*! \brief Destructor */
    ~Parser();


    /*! \brief Starts parsing the given file */
    bool parseFile(std::string filename);
    /*! \brief Starts parsing the given verbatim string */
    bool parseString(std::string code);

    /*! \brief Access method (read) for #f_channels. */
    const std::vector<ParsedChannel> *channels() const;

    /*! \brief Access method (write,conditional append) for #f_channels. */
    int whichChannel(const ParsedChannel &channel);

    /*! \brief Access method (read) for #f_code. */
    const std::string &code() const;

    /*! \brief Access method (read) for #f_events. */
    const std::vector<ParsedEvent> *events() const;

    /*! \brief Access method (write,append) for #f_events. */
    bool addEvent(const ParsedEvent &event);

    /*! \brief Filterd version of events(), containing only measurements. */
    const std::vector<const ParsedEvent *> *measurements();

    /*! \brief Access method (read) for #f_files. */
    const std::vector<std::string> *files() const;

    /*! \brief Access method (write,conditional append) for #f_files. */
    int whichFile(const std::string &file);

    /*! \brief Access method (read) for #f_mainFile. */
    const std::string &mainFile() const;

    /*! \brief Access method (read) for #f_variables. */
    const std::vector<ParsedVar> *variables() const;

    /*! \brief Access method (write,append) for #f_variables. */
    bool addVariable(const ParsedVar &variable);

    /*! \brief Convenience function to access the variable "description". */
    const std::string description() const;

    /*! \brief Access method (read) for #f_errMsg. */
    const std::string &errMsg() const;

    /*! \brief Access method (read) for #f_outMsg. */
    const std::string &outMsg() const;
};

};

#endif
