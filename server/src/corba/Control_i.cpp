/*! \file Control_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Control_i
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

#include "Control_i.h"


Control_i::Control_i()
{
}


Control_i::~Control_i()
{
}


void Control_i::add_Parser(Parser_i* var)
{	
	if(have_Parser)
	{
		// Remove reference to the current Parser_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		parser->_remove_ref();
	}

	have_Parser = true;
	parser = var;
	parser->_add_ref();
}

void Control_i::remove_Parser()
{
	if(have_Parser)
	{
		parser->_remove_ref();
	}

	have_Parser = false;
	parser = NULL;

}

void Control_i::add_ExpSequence(ExpSequence_i* var)
{	
	if(have_ExpSequence)
	{
		// Remove reference to the current ExpSequence_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		expSequence->_remove_ref();
	}

	have_ExpSequence = true;
	expSequence = var;
	expSequence->_add_ref();
}

void Control_i::remove_ExpSequence()
{
	if(have_ExpSequence)
	{
		expSequence->_remove_ref();
	}

	have_ExpSequence = false;
	expSequence = NULL;
}

void Control_i::add_ModeHandler(ModeHandler_i* var)
{	
	if(have_ModeHandler)
	{
		// Remove reference to the current ModeHandler_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		modeHandler->_remove_ref();
	}

	have_ModeHandler = true;
	modeHandler = var;
	modeHandler->_add_ref();
}

void Control_i::remove_ModeHandler()
{
	if(have_ModeHandler)
	{
		modeHandler->_remove_ref();
	}

	have_ModeHandler = false;
	modeHandler = NULL;
}




STI_Client_Server::TStatus Control_i::status()
{
	STI_Client_Server::TStatus dummy;
	dummy.curTime = 0;
	return dummy;
}


void Control_i::reset()
{
}


void Control_i::setDirect()
{
}


void Control_i::runSingle()
{
}


void Control_i::runSequence()
{
}


void Control_i::_cxx_continue()
{
}


void Control_i::stop()
{
}

STI_Client_Server::ExpSequence_ptr Control_i::expSeq()
{
	STI_Client_Server::ExpSequence_ptr dummy = 0;
	return dummy;
}

char* Control_i::errMsg()
{
	char* dummy = "dummy";
	return CORBA::string_dup(dummy);
}
