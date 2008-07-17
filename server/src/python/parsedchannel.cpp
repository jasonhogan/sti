/*! \file
 *  \author Olaf Mandel
 *  \brief Source-file for the class ParsedChannel
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
#include "parsedchannel.h"
#include <cassert>
#include <sstream>
#include "utils.h"

using std::stringstream;
using libPythonPrivate::icompare;

namespace libPython
{

/*! \param[in] id     The initial value for #id.
 *  \param[in] addr   The initial value for #addr.
 *  \param[in] module The initial value for #module.
 *  \param[in] nr     The initial value for #nr.
 *
 *  The values for \c id and \c addr have to follow certain rules (be part of
 *  a set of fixed strings or be a FQDN, respectively). This is not checked
 *  here, though.
 */
ParsedChannel::ParsedChannel(const std::string &id, const std::string &addr,
    unsigned short module, unsigned short nr)
    : f_id(id), f_addr(addr), f_module(module), f_nr(nr)
{
}

ParsedChannel::~ParsedChannel()
{
}

/*! \return A constant pointer to #f_id.
 *
 *  This access method prevents unwitting changes to #f_id from outside
 *  code by casting the pointer const.
 */
const std::string &
ParsedChannel::id() const
{
    return f_id;
}

/*! \return A constant pointer to #f_addr.
 *
 *  This access method prevents unwitting changes to #f_addr from outside
 *  code by casting the pointer const.
 */
const std::string &
ParsedChannel::addr() const
{
    return f_addr;
}

/*! \return The device number #f_module.
 */
unsigned short
ParsedChannel::module() const
{
    return f_module;
}

/*! \return The channel number #f_nr.
 */
unsigned short
ParsedChannel::nr() const
{
    return f_nr;
}

/*! \return A string in the format "ch(dev('id','addr',module),nr)".
 */
std::string
ParsedChannel::str() const
{
    stringstream buf;

    buf << "ch(dev('" << f_id;
    buf << "','" << f_addr;
    buf << "'," << f_module;
    buf << ")," << f_nr << ')';

    return buf.str();
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if equal, \c false otherwise.
 *
 *  Compares all of #id, #addr, #module and #nr. The comparison for #addr
 *  is not case sensitive. Only \c true if all are equal.
 */
bool
ParsedChannel::operator==(const ParsedChannel &other) const
{
    return f_module==other.f_module && f_nr==other.f_nr && f_id==other.f_id
        && icompare(f_addr,other.f_addr);
}

};
