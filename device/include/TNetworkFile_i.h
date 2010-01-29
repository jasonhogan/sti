/*! \file TNetworkFile_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class TNetworkFile_i
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef TNETWORKFILE_I_H
#define TNETWORKFILE_I_H

#include "orbTypes.h"

#include <string>


class TNetworkFile_i : public POA_STI::Types::TNetworkFile
{
public:

	TNetworkFile_i(std::string filename);
	~TNetworkFile_i();

	::CORBA::Boolean getBytes(::CORBA::Long number, STI::Types::TOctetSeq_out bytes);
	char* fileName();
	::CORBA::Long length();

private:

	std::string filename_l;

};

#endif
