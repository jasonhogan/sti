/*! \file TNetworkFile_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class TNetworkFile_i
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

#include "TNetworkFile_i.h"


TNetworkFile_i::TNetworkFile_i(std::string filename) : filename_l(filename)
{
}

TNetworkFile_i::~TNetworkFile_i()
{
}
::CORBA::Boolean TNetworkFile_i::getBytes(::CORBA::Long number, STI::Types::TOctetSeq_out bytes)
{
	return false;
}

char* TNetworkFile_i::fileName()
{
	CORBA::String_var name( filename_l.c_str() );
	return name._retn();
}

::CORBA::Long TNetworkFile_i::length()
{
	return 0;
}
