/*! \file
 *  \author Olaf Mandel
 *  \brief Include-file for the class ParsedChannel
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

#ifndef PARSEDCHANNEL_H
#define PARSEDCHANNEL_H

#include <string>

namespace libPython
{

/*! \brief The ParsedChannel class represents information for one hardware
 *         channel
 *
 *  This class is instantiated by the getCh() function of the
 *  \link timing_module Timing module\endlink.
 *
 */
class ParsedChannel
{
    /*! \brief Internal variable for the type of the device
     *
     *  There are some valid strings. No checking is performed here, if
     *  this is one of them.
     *
     *  This variable is accessible through id(). No means to edit the
     *  variable is provided.
     */
    std::string f_id;
    /*! \brief Internal variable for the address of the device
     *
     *  This string must contain a valid internet address, either as a FQDN
     *  or as a numeric IP address. It is strongly suggested to call each
     *  device with only one name.
     *
     *  This variable is accessible through addr(). No means to edit the
     *  variable is provided.
     */
    std::string f_addr;
    /*! \brief Internal variable for the number of the device
     *
     *  This is the number of the devide at the given address. Counting
     *  starts with 0. So if this is the third device at addr=127.0.0.1,
     *  the value is two.
     *
     *  This variable is accessible through module(). No means to edit the
     *  variable is provided.
     */
    unsigned short f_module;
    /*! \brief Internal variable for the channel number
     *
     *  This is the number of the channel on the device. Counting starts with
     *  0. So if this is the third channel on the device, the value is two.
     *
     *  This variable is accessible through nr(). No means to edit the number
     *  is provided.
     */
    unsigned short f_nr;

public:
    /*! \brief Standard Contructor */
    ParsedChannel(const std::string &id, const std::string &addr,
        unsigned short module, unsigned short nr);
    /*! \brief Destructor */
    ~ParsedChannel();

    /*! \brief Access method (read) for #f_id */
    const std::string &id() const;
    /*! \brief Access method (read) for #f_addr */
    const std::string &addr() const;
    /*! \brief Access method (read) for #f_module */
    unsigned short module() const;
    /*! \brief Access method (read) for #f_nr */
    unsigned short nr() const;

    /*! \brief Outputs the channel as a string */
    std::string str() const;

    /*! \brief Equality Operator */
    bool operator==(const ParsedChannel &other) const;
};

};

#endif
