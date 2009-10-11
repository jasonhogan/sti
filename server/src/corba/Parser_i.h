/*! \file Parser_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Parser_i
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

#ifndef PARSER_I_H
#define PARSER_I_H

#include "client.h"
#include "ExpSequence_i.h"
#include <parser.h>
#include <sstream>

class STI_Server;

class Parser_i : public POA_STI::Client_Server::Parser
{
public:

	Parser_i(STI_Server* server);
	~Parser_i();

	::CORBA::Boolean parseFile(const char* filename, STI::Client_Server::Messenger_ptr parserCallback);
    ::CORBA::Boolean parseString(const char* code);
	::CORBA::Boolean parseLoopScript(const char* script);

    STI::Types::TOverwrittenSeq* overwritten();
    void overwritten(const STI::Types::TOverwrittenSeq& _v);
    ::CORBA::Boolean lockOnParse();
    void lockOnParse(::CORBA::Boolean _v);
    char* outMsg();
    char* errMsg();
    char* mainFile();
    STI::Types::TChannelSeq* channels();
    STI::Types::TStringSeq* files();
    STI::Types::TVariableSeq* variables();
    STI::Types::TEventSeq* events();


	libPython::Parser * pyParser;

	STI::Types::TChannelSeq& getParsedChannels();
	const STI::Types::TEventSeq& getParsedEvents() const;
	const std::string getParsedDescription() const;
	
	void add_ExpSequence(ExpSequence_i* var);
	void remove_ExpSequence();

private:

	void setTVarMixed( STI::Types::TVarMixed &destination, const libPython::ParsedValue source);

	STI_Server* sti_Server;
	ExpSequence_i* expSequence;
	
	void removeCarriageReturns(std::string &code);
	bool lockOnParse_l;
	std::stringstream outMessage;

	void setupParsedChannels();
	void setupParsedEvents();

	STI::Types::TChannelSeq_var tChannelSeq;
	STI::Types::TEventSeq_var tEventSeq;
};

#endif
