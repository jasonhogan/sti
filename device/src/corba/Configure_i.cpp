/*! \file Configure_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Configure_i
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#include "device.h"
#include "Configure_i.h"


Configure_i::Configure_i()
{
}

Configure_i::~Configure_i()
{
}

::CORBA::Boolean Configure_i::setAttribute(const char *key, const char *value)
{
	return true;
}

char* Configure_i::getAttribute(const char *key)
{
	char* dummy = 0;
	return dummy;
}

STI_Server_Device::TStringSeq* Configure_i::attributes()
{
	STI_Server_Device::TStringSeq* dummy = 0;
	return dummy;
}

char* Configure_i::deviceType()
{
	char* dummy = 0;
	return dummy;
}
