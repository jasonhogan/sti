/*! \file Parser_i.cpp
 *  \author Jonathan David Harvey
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Parser_i
 *  \section license License
 *
 *  Copyright (C) 2008 Jonathan Harvey <harv@stanford.edu>\n
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


#include <cassert>
#include "client.h"
#include "ExpSequence_i.h"
#include "Parser_i.h"


#include <iostream>
using namespace std;

Parser_i::Parser_i()
{
	pyParser = new libPython::Parser();
	expSequence = NULL;
}


Parser_i::~Parser_i()
{
	remove_ExpSequence();
}


void Parser_i::add_ExpSequence(ExpSequence_i* var)
{
	assert(var != NULL);

	if(expSequence != NULL)
	{
		// Remove reference to the current ExpSequence_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		expSequence->_remove_ref();
	}

	expSequence = var;
	expSequence->_add_ref();
}

void Parser_i::remove_ExpSequence()
{
	if(expSequence != NULL)
	{
		expSequence->_remove_ref();
	}

	expSequence = NULL;
}


::CORBA::Boolean Parser_i::parseFile(const char* filename)
{
	return pyParser->parseFile(filename);
}


::CORBA::Boolean Parser_i::parseString(const char* code)
{
	return pyParser->parseString(code);
}


::CORBA::Boolean Parser_i::parseLoopScript(const char* script)
{
	bool error = false;
//	pyParser->record("variables");

	cerr << "parseLoopScript(" << script << ")" << endl;

	error = parseString(script);	//this never returns!!!

	cerr << "done parsing" << endl;
	//expSequence->setExpSequence();

//	pyParser->parseLoopScript(code, {variables, experiments});

	return false;
}


STI_Client_Server::TOverwrittenSeq* Parser_i::overwritten()
{
	using STI_Client_Server::TOverwrittenSeq;
	using STI_Client_Server::TOverwrittenSeq_var;

	unsigned i;
	std::map<std::string,std::string>::iterator iter;
	
	TOverwrittenSeq_var overwrittenSeq( 
		new TOverwrittenSeq( pyParser->overwritten.size() ) );

	for(iter = pyParser->overwritten.begin(), i = 0;
		iter != pyParser->overwritten.end(); iter++, i++)
	{
		overwrittenSeq[i].name  = CORBA::string_dup( iter->first.c_str() );
		overwrittenSeq[i].value = CORBA::string_dup( iter->second.c_str() );
	}
	return overwrittenSeq._retn();
}


void Parser_i::overwritten(const STI_Client_Server::TOverwrittenSeq& _v)
{
	unsigned i;
	
	//reset the overwritten list before beginning a new run
	pyParser->overwritten.clear();

	for(i = 0; i < _v.length(); i++)
	{
		pyParser->overwritten[CORBA::string_dup(_v[i].name)] = _v[i].value;
	}
}


::CORBA::Boolean Parser_i::lockOnParse()
{
	return lockOnParse_l;
}


void Parser_i::lockOnParse(::CORBA::Boolean _v)
{
	lockOnParse_l = _v;
}


char* Parser_i::outMsg()
{
	CORBA::String_var msg( pyParser->outMsg().c_str() );
	return msg._retn();
}


char* Parser_i::errMsg()
{
	CORBA::String_var msg( pyParser->errMsg().c_str() );
	return msg._retn();
}


char* Parser_i::mainFile()
{
	CORBA::String_var file( pyParser->mainFile().c_str() );
	return file._retn();
}


STI_Client_Server::TChannelSeq* Parser_i::channels()
{
	using STI_Client_Server::TChannelSeq;
	using STI_Client_Server::TChannelSeq_var;

	unsigned i;
	std::vector<libPython::ParsedChannel> const & channels = *pyParser->channels();
	TChannelSeq_var channelSeq( new TChannelSeq(channels.size()) );

	for(i = 0; i < channels.size(); i++)
	{
		channelSeq[i].channel          = channels[i].nr();  //Does this agree with the STI_Device's channel numbering?
		channelSeq[i].device.address   = CORBA::string_dup( channels[i].addr().c_str() );
		channelSeq[i].device.moduleNum = channels[i].module();
	}
	return channelSeq._retn();
}


STI_Client_Server::TStringSeq* Parser_i::files()
{
	using STI_Client_Server::TStringSeq;

	unsigned i;
	std::vector<std::string> const & files = *pyParser->files();

	STI_Client_Server::TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length(files.size());

	for(i = 0; i < files.size(); i++)
	{
		stringSeq[i] = CORBA::String_var( files[i].c_str() );
//		stringSeq[i] = CORBA::string_dup( files[i].c_str() );
	}
	return stringSeq._retn();
}


STI_Client_Server::TVariableSeq* Parser_i::variables()
{
	using STI_Client_Server::TVariableSeq;
	using STI_Client_Server::TVariableSeq_var;

	unsigned i,j;
	unsigned varLength = 0;

	std::vector<libPython::ParsedVar> const & vars = *pyParser->variables();
	
	// The client only gets variables that have a non-NULL position.
	// These correspond to variables that are defined in the timing file using setVar().
	for(i = 0; i < vars.size(); i++)
		if(vars[i].position != NULL)
			varLength++;			//only count the varible with a defined postion

	TVariableSeq_var variableSeq( new TVariableSeq );
	variableSeq->length(varLength);

	for(i=0, j=0; i < vars.size(); i++)	//look through all the python variables
	{
		if(vars[i].position != NULL)	//only copy the setVar() variables
		{
			variableSeq[j].value.channel(1);		//dummy

//**** Need to actually add values of TVarMixed ****//

			variableSeq[j].name     = CORBA::string_dup( vars[i].name.c_str() );
			variableSeq[j].pos.file = vars[i].position->file;
			variableSeq[j].pos.line = vars[i].position->line;
			
			j++;
		}
	}
	return variableSeq._retn();
}


STI_Client_Server::TEventSeq* Parser_i::events()
{
	using STI_Client_Server::TEventSeq;
	using STI_Client_Server::TEventSeq_var;
	using libPython::EventType;

	unsigned i;
	std::vector<libPython::ParsedEvent> const & events = *pyParser->events();

	TEventSeq_var eventSeq( new TEventSeq(events.size()) );

	for(i = 0; i < events.size(); i++)
	{
		eventSeq[i].channel = events[i].channel;
		eventSeq[i].time    = events[i].time;

		switch(events[i].type())
		{
		case libPython::NumberEvent:
			eventSeq[i].value.number( events[i].number() );
			eventSeq[i].value._d( STI_Server_Device::ValueNumber );
			break;
		case libPython::TextEvent:
			eventSeq[i].value.stringVal( events[i].text().c_str() );
			eventSeq[i].value._d( STI_Server_Device::ValueString );
			break;
		case libPython::DDSEvent:
			eventSeq[i].value.triplet().freq  = events[i].freq();
			eventSeq[i].value.triplet().phase = events[i].phase();
			eventSeq[i].value.triplet().ampl  = events[i].ampl();
			eventSeq[i].value._d( STI_Server_Device::ValueDDSTriplet );
			break;
		case libPython::MeasureEvent:
		default:
			eventSeq[i].value.meas(true);
			eventSeq[i].value._d( STI_Server_Device::ValueMeas );
			break;
		}
	}
	return eventSeq._retn();
}

