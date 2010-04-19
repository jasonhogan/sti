/*! \file ServerTimingSeqControl_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ServerTimingSeqControl_i
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

//was 'Control_i' -- 2/6/2010

#ifndef CONTROL_I_H
#define CONTROL_I_H

#include "client.h"
#include "ModeHandler_i.h"
#include "ExpSequence_i.h"
#include "Parser_i.h"

class STI_Server;

class ServerTimingSeqControl_i : public POA_STI::Client_Server::ServerTimingSeqControl
{
public:

	ServerTimingSeqControl_i(STI_Server* server);
	~ServerTimingSeqControl_i();


    STI::Types::TStatus status();
    void reset();
    void setDirect();
	void runSingle(::CORBA::Boolean documented);
	void runSequence(::CORBA::Boolean documented);
	void runSingleContinuous();
    void _cxx_continue();
	void pause();
	void resume();
    void stop();
    STI::Client_Server::ExpSequence_ptr expSeq();
    char* errMsg();
	char* transferErr(const char* deviceID);
	STI::Types::TExpRunInfo* getDefaultRunInfo();
	STI::Types::TExpSequenceInfo* getDefaultSequenceInfo();


	void add_Parser(Parser_i* var);
	void remove_Parser();
	void add_ExpSequence(ExpSequence_i* var);
	void remove_ExpSequence();
	void add_ModeHandler(ModeHandler_i* var);
	void remove_ModeHandler();

private:

	void runSingleExperiment(bool documented);

	STI_Server* sti_Server;

	ExpSequence_i* expSequence;
	Parser_i* parser;
	ModeHandler_i* modeHandler;

	bool stopSequence;

};

#endif
