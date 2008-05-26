/*! \file ModeHandler_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ModeHandler_i
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


#include "ModeHandler_i.h"
#include <iostream>
#include <string>
#include <vector>

ModeHandler_i::ModeHandler_i()
{
	char* dummy = "local dummy test";
	localDummy = dummy;
}


ModeHandler_i::~ModeHandler_i()
{
}


::CORBA::Boolean ModeHandler_i::requestControl(const char* myName)
{
	return true;
}


void ModeHandler_i::answerRequest(::CORBA::Boolean yield)
{
}
char* ModeHandler_i::controller()
{
	char* dummy = "dummy";
	return CORBA::string_dup(dummy);
}


void ModeHandler_i::controller(const char* _v)
{
}


::CORBA::Boolean ModeHandler_i::requestPending()
{
	return true;
}


void ModeHandler_i::requestPending(::CORBA::Boolean _v)
{
}


char* ModeHandler_i::requesterName()
{	
	char* dummy = "dummy";



	return CORBA::string_dup(localDummy);
}


void ModeHandler_i::requesterName(const char* _v)
{
}

