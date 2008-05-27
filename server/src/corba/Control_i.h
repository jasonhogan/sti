/*! \file Control_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Control_i
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

#ifndef CONTROL_I_H
#define CONTROL_I_H

#include "client.h"
#include "ExpSequence_i.h"
#include "Parser_i.h"


class Control_i : public POA_STI_Client_Server::Control
{
public:

	Control_i();
	virtual ~Control_i();


    virtual STI_Client_Server::TStatus status();
    virtual void reset();
    virtual void setDirect();
    virtual void runSingle();
    virtual void runSequence();
    virtual void _cxx_continue();
    virtual void stop();
    virtual STI_Client_Server::ExpSequence_ptr expSeq();
    virtual char* errMsg();



	void add_Parser(Parser_i* var);
	void remove_Parser();
	void add_ExpSequence(ExpSequence_i* var);
	void remove_ExpSequence();
	void add_ModeHandler(ModeHandler_i* var);
	void remove_ModeHandler();

private:

	bool have_ExpSequence;
	bool have_Parser;
	bool have_ModeHandler;

	ExpSequence_i* expSequence;
	Parser_i* parser;
	ModeHandler_i* modeHandler;

};

#endif
