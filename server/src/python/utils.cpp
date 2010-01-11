/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the utilities collection
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

#include "utils.h"
#include <algorithm>
#include <cctype>

using std::equal;

namespace libPythonPrivate
{

/*! \brief Compares two \c chars, ignoring case
 *  \param[in] a The first character to compare
 *  \param[in] b The second character to compare
 *  \return \c true if the characters match, \c false otherwise
 *
 *  This \c static function is called by icompare().
 */
static bool
icomparechar(char a, char b)
{
    return tolower(a)==tolower(b);
}

/*! \param[in] a The first string to compare
 *  \param[in] b The second string to compare
 *  \return \c true if the strings match, \c false otherwise
 */
bool
icompare(const std::string &a, const std::string &b)
{
    if(a.length() != b.length())
        return false;

    return equal(a.begin(), a.end(), b.begin(), &icomparechar);
}

};
