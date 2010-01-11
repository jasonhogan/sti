/*! \file
 *  \author Olaf Mandel
 *  \brief Main-file of the debug-tool for the python library
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
#include <cstdlib>
#include <iostream>
#include "parser.h"

using namespace std;
using namespace libPython;

/*! \brief Main function of the tool
 *  \param[in] argc Number of command-line arguments in \a argv
 *  \param[in] argv List of C-type strings, each containing one command-line
 *      parameter
 *  \return The exit or status value of the program
 *
 *  Initializes the Python environment, then runs the file given as the
 *  first parameter in Python.
 */
int
main(int argc, char *argv[])
{
    Parser                                      parser;
    vector<string>::const_iterator              i, imax;
    vector<ParsedVar>::const_iterator           j, jmax;
    vector<ParsedEvent>::const_iterator         k, kmax;
    vector<const ParsedEvent *>::const_iterator l, lmax;

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <timing-file>" << endl;
        return EXIT_FAILURE;
    }

    /* Overload the variable MOT_Current */
    parser.overwritten["MOT_Current"] = "2";

    /* Execute the file argv[1] */
/*    for(int i=0; i<2000; ++i) {
        cout << i << endl;
        if(parser.parseFile(argv[1]))
            cerr << parser.errMsg();
    } */
    if(parser.parseFile(argv[1])) {
        cerr << "Couldn't run file \"" << argv[1] << "\"." << endl;
        cerr << parser.errMsg();
        return EXIT_FAILURE;
    }

    /* Show output of script */
    cout << "Standard output:" << endl;
    cout << parser.outMsg();
    cout << endl << "Error output:" << endl;
    cout << parser.errMsg();

    /* Show list of files */
    if(!parser.files()->empty())
        cout << endl << "Files visited during parsing:" << endl;
    for(i=parser.files()->begin(), imax=parser.files()->end(); i!=imax; ++i)
        cout << *i << endl;

    /* Show list of variables */
    if(!parser.variables()->empty())
        cout << endl << "Variables defined:" << endl;
    for(j=parser.variables()->begin(), jmax=parser.variables()->end(); j!=jmax;
        ++j)
        if(j->position != NULL)
            cout << j->position->str() << ": " << j->value.typestr() << ": "
                << j->name << " = " << j->value.str() << endl;
        else
            cout << "EOF: " << j->value.typestr() << ": " << j->name << " = "
                << j->value.str() << endl;

    /* Show list of events */
    if(!parser.events()->empty())
        cout << endl << "Events defined:" << endl;
    for(k=parser.events()->begin(), kmax=parser.events()->end(); k!=kmax; ++k)
        cout << k->position.str() << ": "
            << parser.channels()->at(k->channel).str()
            << " @ " << k->time << "s = " << k->value() << endl;

    /* Show list of measurements */
    if(!parser.measurements()->empty())
        cout << endl << "Measurements defined:" << endl;
    for(l=parser.measurements()->begin(), lmax=parser.measurements()->end();
        l!=lmax; ++l)
        cout << (*l)->position.str() << ": "
            << parser.channels()->at((*l)->channel).str()
            << " @ " << (*l)->time << "s : \"" << (*l)->desc() << "\"" << endl;

    return EXIT_SUCCESS;
}
