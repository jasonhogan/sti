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
#include "STI_Server.h"

#include <iostream>
using namespace std;

Parser_i::Parser_i(STI_Server* server) : sti_Server(server)
{
	outMessage.str("");
	pyParser = new libPython::Parser();
	expSequence = NULL;

	tChannelSeq = STI_Client_Server::TChannelSeq_var(new STI_Client_Server::TChannelSeq);
	tEventSeq   = STI_Client_Server::TEventSeq_var( new STI_Client_Server::TEventSeq );
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

	//return pyParser->parseFile(filename);

	outMessage.str("");

	outMessage << "Parsing..." << endl;

	bool error = pyParser->parseFile(filename);
	setupParsedChannels();
	setupParsedEvents();

	outMessage << pyParser->outMsg() << endl;

	cout << "Events: " << error << ", " << (pyParser->events())->size() << endl;
	cout << "done parsing. " << endl << "error: " << pyParser->errMsg() << endl << "out: " << pyParser->outMsg()<< endl;

	if(!error) {
		outMessage << "Checking channels..." << endl;
		error = sti_Server->checkChannelAvailability(outMessage);
	}

	cout << endl << endl << "Parsed" << endl;

	//TEMPORARY
	if(!error) 
	{
		sti_Server->divideEventList();
	cout << "Divided" << endl;
		sti_Server->transferEvents();
	cout << "transfered" << endl;
		sti_Server->loadEvents();
	cout << "loaded" << endl;
		sti_Server->playEvents();
	cout << "played" << endl;
	}

	return error;
}


::CORBA::Boolean Parser_i::parseString(const char* code)
{
	bool error = pyParser->parseString(code);
	setupParsedChannels();
	return error;
}

void Parser_i::removeCarriageReturns(string &code)
{
	string::size_type loc = 0;
	char cr = 13;	//carriage return

	while(loc != string::npos)
	{
		loc = code.find_first_of(cr);
		if( loc != string::npos )
			code.replace(loc, 1, "");	//remove carriage returns
	}
}

::CORBA::Boolean Parser_i::parseLoopScript(const char* script)
{
	bool error = false;
	bool pythonError = false;

	string code(script);
	removeCarriageReturns(code);

//	for(unsigned i=0; i < 90; i++)
//		cerr << "(" << code.c_str()[i] << ", " << static_cast<unsigned>(code.c_str()[i]) << ")" << endl;
//	cerr << endl << endl;

	pythonError = parseString(code.c_str());	//this never returns if cin is waiting in another thread
	setupParsedChannels();

	std::vector<libPython::ParsedVar> const & vars = *pyParser->variables();
	std::vector<libPython::ParsedVar>::const_iterator iter;

	for(iter=vars.begin(); iter != vars.end(); iter++)
	{
//		cerr << "name: " << iter->name;
		if(iter->name == string("variables"))
		{		
			if(iter->position == NULL)	//the final value after parsing, not the setvar() value
			{
				if(iter->value.type == libPython::VTlist)
					expSequence->setupVariables( iter->value.list );
				else
					error = true;
			}
		}
	}
	for(iter=vars.begin(); iter != vars.end(); iter++)
	{
		if(iter->name == string("experiments"))
		{
			if(iter->position == NULL)	//the final value after parsing, not the setvar() value
			{
				if(iter->value.type == libPython::VTlist)
					error = expSequence->setupExperiments( iter->value.list );
				else
					error = true;
			}
		}
	}
		cerr << endl;

	cerr << "done parsing. " << endl << "error: " << pyParser->errMsg() << endl << "out: " << pyParser->outMsg()<< endl;
	//expSequence->setExpSequence();

//	pyParser->parseLoopScript(code, {variables, experiments});

	return error || pythonError;
}


STI_Client_Server::TOverwrittenSeq* Parser_i::overwritten()
{
	using STI_Client_Server::TOverwrittenSeq;
	using STI_Client_Server::TOverwrittenSeq_var;

	unsigned i;
	std::map<std::string,std::string>::iterator iter;
	
	TOverwrittenSeq_var overwrittenSeq( new TOverwrittenSeq );

//	overwrittenSeq = new TOverwrittenSeq();

	overwrittenSeq->length( pyParser->overwritten.size() );

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
	CORBA::String_var msg( outMessage.str().c_str() );
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


void Parser_i::setupParsedChannels()
{
	using STI_Client_Server::TChannelSeq;
	using STI_Client_Server::TChannelSeq_var;

	unsigned i;
	const std::vector<libPython::ParsedChannel>& channels = *pyParser->channels();

	tChannelSeq->length(channels.size());

	for(i = 0; i < channels.size(); i++)
	{
		//temporary; server should look for device first
		tChannelSeq[i].outputType	   = STI_Server_Device::ValueMeas;
		tChannelSeq[i].inputType	   = STI_Server_Device::DataNone;
		tChannelSeq[i].type			   = STI_Server_Device::Unknown;

		tChannelSeq[i].channel           = channels[i].nr();
		tChannelSeq[i].device.deviceName = CORBA::string_dup( channels[i].id().c_str() );
		tChannelSeq[i].device.address    = CORBA::string_dup( channels[i].addr().c_str() );
		tChannelSeq[i].device.moduleNum  = channels[i].module();

		string dummy = "";
        tChannelSeq[i].device.deviceID = CORBA::string_dup( dummy.c_str() );
        tChannelSeq[i].device.deviceContext = CORBA::string_dup( dummy.c_str() );
	}
}

STI_Client_Server::TChannelSeq& Parser_i::getParsedChannels()
{
	return tChannelSeq;
}


STI_Client_Server::TChannelSeq* Parser_i::channels()
{
	using STI_Client_Server::TChannelSeq;
	using STI_Client_Server::TChannelSeq_var;

	TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length( tChannelSeq->length() );

	for(unsigned i=0; i < channelSeq->length(); i++)
	{
		channelSeq[i] = tChannelSeq[i];
	}

	return channelSeq._retn();
}

/*
STI_Client_Server::TChannelSeq* Parser_i::channels()
{
	using STI_Client_Server::TChannelSeq;
	using STI_Client_Server::TChannelSeq_var;

	unsigned i;
	std::vector<libPython::ParsedChannel> const & channels = *pyParser->channels();
	TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length(channels.size());

	for(i = 0; i < channels.size(); i++)
	{
		//temporary; server should look for device first
		channelSeq[i].outputType	   = STI_Server_Device::ValueMeas;
		channelSeq[i].inputType		   = STI_Server_Device::DataNone;
		channelSeq[i].type			   = STI_Server_Device::Unknown;

		channelSeq[i].channel          = channels[i].nr();  //Does this agree with the STI_Device's channel numbering?
		channelSeq[i].device.address   = CORBA::string_dup( channels[i].addr().c_str() );
		channelSeq[i].device.moduleNum = channels[i].module();

		string dummy = "";
		channelSeq[i].device.deviceName = CORBA::string_dup( dummy.c_str() );
        channelSeq[i].device.deviceID = CORBA::string_dup( dummy.c_str() );
        channelSeq[i].device.deviceContext = CORBA::string_dup( dummy.c_str() );
	}
	return channelSeq._retn();
}
*/

STI_Client_Server::TStringSeq* Parser_i::files()
{
	using STI_Client_Server::TStringSeq;

	unsigned i;
	const std::vector<std::string>& files = *pyParser->files();

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

	const std::vector<libPython::ParsedVar>& vars = *pyParser->variables();
	
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
			setTVarMixed( variableSeq[j].value, vars[i].value );
			variableSeq[j].name     = CORBA::string_dup( vars[i].name.c_str() );
			variableSeq[j].pos.file = vars[i].position->file;
			variableSeq[j].pos.line = vars[i].position->line;
			
			j++;
		}
	}
	return variableSeq._retn();
}

void Parser_i::setTVarMixed( STI_Client_Server::TVarMixed &destination, 
							const libPython::ParsedValue source)
{
	using STI_Client_Server::TVarMixedSeq;
	using STI_Client_Server::TVarMixedSeq_var;
	
	TVarMixedSeq_var varMixedSeq( new TVarMixedSeq );
	unsigned listLength;

	switch(source.type)
	{
	case libPython::VTnumber:
		destination.number( source.number );
		destination._d( STI_Client_Server::TypeNumber );
		break;
	case libPython::VTstring:
		destination.stringVal( source.str().c_str() );
		destination._d( STI_Client_Server::TypeString );
		break;
	case libPython::VTlist:
		listLength = source.list.size();
		varMixedSeq->length(listLength);

		for(unsigned i = 0; i < listLength; i++)
		{
			setTVarMixed( varMixedSeq[i], source.list[i] );
		}

		destination.list( varMixedSeq );
		destination._d( STI_Client_Server::TypeList );
		break;
	case libPython::VTchannel:
		destination.channel( source.channel );
		destination._d( STI_Client_Server::TypeChannel );
		break;
	case libPython::VTobject:
		destination.objectVal( source.str().c_str() );
		destination._d( STI_Client_Server::TypeObject );
		break;
	default:
		std::string error = "Server-side type error";
		destination.stringVal( error.c_str() );
		destination._d( STI_Client_Server::TypeString );
		break;
	}
}

const STI_Client_Server::TEventSeq& Parser_i::getParsedEvents() const
{
	return tEventSeq;
}


STI_Client_Server::TEventSeq* Parser_i::events()
{
	using STI_Client_Server::TEventSeq;
	using STI_Client_Server::TEventSeq_var;

	TEventSeq_var eventSeq( new TEventSeq );
	eventSeq->length( tEventSeq->length() );

	for(unsigned i=0; i < eventSeq->length(); i++)
	{
		eventSeq[i] = tEventSeq[i];
	}

	return eventSeq._retn();
}


void Parser_i::setupParsedEvents()
{
	using STI_Client_Server::TEventSeq;
	using STI_Client_Server::TEventSeq_var;
	using libPython::EventType;

	unsigned i;
	std::vector<libPython::ParsedEvent> const & events = *pyParser->events();

	tEventSeq->length(events.size());

	for(i = 0; i < events.size(); i++)
	{
		tEventSeq[i].channel = events[i].channel;
		tEventSeq[i].time    = events[i].time;

		tEventSeq[i].pos.file = events[i].position.file;
		tEventSeq[i].pos.line = events[i].position.line;

		switch(events[i].type())
		{
		case libPython::NumberEvent:
			tEventSeq[i].value.number( events[i].number() );
			tEventSeq[i].value._d( STI_Server_Device::ValueNumber );
			break;
		case libPython::TextEvent:
			tEventSeq[i].value.stringVal( events[i].text().c_str() );
			tEventSeq[i].value._d( STI_Server_Device::ValueString );
			break;
		case libPython::DDSEvent:
			tEventSeq[i].value.triplet().freq  = events[i].freq();
			tEventSeq[i].value.triplet().phase = events[i].phase();
			tEventSeq[i].value.triplet().ampl  = events[i].ampl();
			tEventSeq[i].value._d( STI_Server_Device::ValueDDSTriplet );
			break;
		case libPython::MeasureEvent:
		default:
			tEventSeq[i].value.meas(true);
			tEventSeq[i].value._d( STI_Server_Device::ValueMeas );
			break;
		}
	}
}

