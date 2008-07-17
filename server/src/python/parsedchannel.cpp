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

using std::stringstream;

namespace libPython
{

/*! \param[in] device The initial value for #device.
 *  \param[in] nr     The initial value for #nr.
 */
ParsedChannel::ParsedChannel(const ParsedDevice &device, unsigned short nr)
    : f_nr(nr)
{
    f_device = new ParsedDevice(device);
}

/*! \param[in] src The source to copy from.
 */
ParsedChannel::ParsedChannel(const ParsedChannel &src)
    : f_nr(src.f_nr)
{
    f_device = new ParsedDevice(*src.f_device);
}

ParsedChannel::~ParsedChannel()
{
    delete f_device;
}

/*! \return A constant pointer to #f_device.
 *
 *  This access method prevents unwitting changes to #f_device from outside
 *  code by casting the pointer const.
 */
const ParsedDevice *
ParsedChannel::device() const
{
    return f_device;
}

/*! \return The channel number.
 *
 *  This is identical to what you get by calling \c repr() under Python.
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

    buf << "ch(";
    buf << f_device->str();
    buf << ',' << f_nr << ')';

    return buf.str();
}

/*! \param[in] other The rhs of the comparison.
 *  \return \c true if equal, \c false otherwise.
 *
 *  Compares both #device and #nr. Only \c true if both are equal.
 */
bool
ParsedChannel::operator==(const ParsedChannel &other) const
{
    return *f_device==*other.f_device && f_nr==other.f_nr;
}

};
