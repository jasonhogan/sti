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

	tChannelSeq = STI::Types::TChannelSeq_var(new STI::Types::TChannelSeq);
	tEventSeq   = STI::Types::TEventSeq_var( new STI::Types::TEventSeq );
//	partnerEventSeq = STI::Types::TEventSeq_var( new STI::Types::TEventSeq );
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

void Parser_i::setParsedFile(std::string filename)
{
	parsedFilename = filename;
}

std::string Parser_i::getParsedFile()
{
	return parsedFilename;
}

bool Parser_i::parseSequenceTimingFile()
{
	return parseFile(getParsedFile().c_str(), parseMessenger);
}

::CORBA::Boolean Parser_i::parseFile(const char* filename, 
									 STI::Client_Server::Messenger_ptr parserCallback)
{
	parseMessenger = STI::Client_Server::Messenger::_duplicate( parserCallback );

	setParsedFile(filename);

	clearEvents();

	outMessage.str("");

	sti_Server->sendMessageToClient(parserCallback, "Parsing Python...\n");

	bool error = pyParser->parseFile(filename);
	
	if( error )
	{
		outMessage << pyParser->errMsg() << endl;
		sti_Server->sendMessageToClient(parserCallback, outMessage.str().c_str() );
	}

	setupParsedChannels();
	setupParsedEvents();
	
	outMessage << pyParser->outMsg() << endl;
	cout << "Events: " << error << ", " << (pyParser->events())->size() << endl;
	cout << "done parsing. " << endl << "error: " << pyParser->errMsg() << endl << "out: " << pyParser->outMsg()<< endl;


	if(!error) 
	{
		error = sti_Server->setupEventsOnDevices(parserCallback);
	}

	if(error)
		sti_Server->stopServer();

	return error;
}


::CORBA::Boolean Parser_i::parseString(const char* code)
{
	bool error = pyParser->parseString(code);
//	setupParsedChannels();
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


STI::Types::TOverwrittenSeq* Parser_i::overwritten()
{
	using STI::Types::TOverwrittenSeq;
	using STI::Types::TOverwrittenSeq_var;

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

void Parser_i::clearOverwritten()
{
	pyParser->overwritten.clear();
}


void Parser_i::overwritten(const STI::Types::TOverwrittenSeq& _v)
{
	unsigned i;
	
	//reset the overwritten list before beginning a new run
	clearOverwritten();

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
	using STI::Types::TChannelSeq;
	using STI::Types::TChannelSeq_var;

	unsigned i;
	const std::vector<libPython::ParsedChannel>& channels = *pyParser->channels();

	tChannelSeq->length(channels.size());

	for(i = 0; i < channels.size(); i++)
	{
		//temporary; server should look for device first
		tChannelSeq[i].outputType	   = STI::Types::ValueNone;
		tChannelSeq[i].inputType	   = STI::Types::DataNone;
		tChannelSeq[i].type			   = STI::Types::Unknown;

		tChannelSeq[i].channel           = channels[i].nr();
		tChannelSeq[i].device.deviceName = CORBA::string_dup( channels[i].id().c_str() );
		tChannelSeq[i].device.address    = CORBA::string_dup( channels[i].addr().c_str() );
		tChannelSeq[i].device.moduleNum  = channels[i].module();

		string dummy = "";
        tChannelSeq[i].device.deviceID = CORBA::string_dup( dummy.c_str() );
        tChannelSeq[i].device.deviceContext = CORBA::string_dup( dummy.c_str() );
	}
}

const std::string Parser_i::getParsedDescription() const
{
	return pyParser->description();
}

STI::Types::TChannelSeq& Parser_i::getParsedChannels()
{
	return tChannelSeq;
}


STI::Types::TChannelSeq* Parser_i::channels()
{
	using STI::Types::TChannelSeq;
	using STI::Types::TChannelSeq_var;

	unsigned i;
	unsigned tChannelLength = tChannelSeq->length();

	TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length( tChannelLength + deviceGeneratedChannels.size() );

	for(unsigned i=0; i < tChannelLength; i++)
	{
		channelSeq[i] = tChannelSeq[i];
	}

	for(i = 0; i < deviceGeneratedChannels.size(); i++)
	{
		channelSeq[i + tChannelLength] = deviceGeneratedChannels.at(i);
	}


	return channelSeq._retn();
}
unsigned short Parser_i::addDeviceGeneratedChannel(STI::Types::TChannel tChannel)
{
	deviceGeneratedChannels.push_back(tChannel);

	return static_cast<unsigned short>( (tChannelSeq->length() - 1) + (deviceGeneratedChannels.size() - 1) );
}

/*
STI::Types::TChannelSeq* Parser_i::channels()
{
	using STI::Types::TChannelSeq;
	using STI::Types::TChannelSeq_var;

	unsigned i;
	std::vector<libPython::ParsedChannel> const & channels = *pyParser->channels();
	TChannelSeq_var channelSeq( new TChannelSeq );
	channelSeq->length(channels.size());

	for(i = 0; i < channels.size(); i++)
	{
		//temporary; server should look for device first
		channelSeq[i].outputType	   = STI::Types::ValueMeas;
		channelSeq[i].inputType		   = STI::Types::DataNone;
		channelSeq[i].type			   = STI::Types::Unknown;

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

const std::vector<std::string>& Parser_i::getTimingFiles() const
{
	return *pyParser->files();
}

STI::Types::TStringSeq* Parser_i::files()
{
	using STI::Types::TStringSeq;

	unsigned i;
	const std::vector<std::string>& files = *pyParser->files();

	STI::Types::TStringSeq_var stringSeq( new TStringSeq );
	stringSeq->length(files.size());

	for(i = 0; i < files.size(); i++)
	{
		stringSeq[i] = CORBA::String_var( files[i].c_str() );
//		stringSeq[i] = CORBA::string_dup( files[i].c_str() );
	}
	return stringSeq._retn();
}

const std::vector<libPython::ParsedVar>& Parser_i::getParsedVars() const
{
	return *pyParser->variables();
}

STI::Types::TVariableSeq* Parser_i::variables()
{
	using STI::Types::TVariableSeq;
	using STI::Types::TVariableSeq_var;

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

void Parser_i::setTVarMixed( STI::Types::TVarMixed &destination, 
							const libPython::ParsedValue source)
{
	using STI::Types::TVarMixedSeq;
	using STI::Types::TVarMixedSeq_var;
	
	TVarMixedSeq_var varMixedSeq( new TVarMixedSeq );
	unsigned listLength;

	switch(source.type)
	{
	case libPython::VTnumber:
		destination.number( source.number );
		destination._d( STI::Types::TypeNumber );
		break;
	case libPython::VTstring:
		destination.stringVal( source.str().c_str() );
		destination._d( STI::Types::TypeString );
		break;
	case libPython::VTlist:
		listLength = source.list.size();
		varMixedSeq->length(listLength);

		for(unsigned i = 0; i < listLength; i++)
		{
			setTVarMixed( varMixedSeq[i], source.list[i] );
		}

		destination.list( varMixedSeq );
		destination._d( STI::Types::TypeList );
		break;
	case libPython::VTchannel:
		destination.channel( source.channel );
		destination._d( STI::Types::TypeChannel );
		break;
	case libPython::VTobject:
		destination.objectVal( source.str().c_str() );
		destination._d( STI::Types::TypeObject );
		break;
	default:
		std::string error = "Server-side type error";
		destination.stringVal( error.c_str() );
		destination._d( STI::Types::TypeString );
		break;
	}
}

const STI::Types::TEventSeq& Parser_i::getParsedEvents() const
{
	return tEventSeq;
}



STI::Types::TEventSeq* Parser_i::events()
{
	using STI::Types::TEventSeq;
	using STI::Types::TEventSeq_var;

	unsigned tEventLength = tEventSeq->length();

	TEventSeq_var eventSeq( new TEventSeq );
	eventSeq->length( tEventLength + deviceGeneratedEvents.size() );
	
	unsigned i;
	
	for(i = 0; i < tEventLength; i++)
	{
		eventSeq[i] = tEventSeq[i];
	}
	
	for(i = 0; i < deviceGeneratedEvents.size(); i++)
	{
		eventSeq[i + tEventLength] = deviceGeneratedEvents.at(i);
	}

	return eventSeq._retn();
}

void Parser_i::clearEvents()
{
	deviceGeneratedEvents.clear();
	deviceGeneratedChannels.clear();
}

void Parser_i::addDeviceGeneratedEvent(STI::Types::TPartnerDeviceEvent& generatedEvt, const STI::Types::TEvent& sourceEvt, const RemoteDevice& remoteDevice)
{
	//generatingDeviceID: the deviceID of the device that generated this event
	STI::Types::TEvent newEvent;

	unsigned short channel = addDeviceGeneratedChannel(  remoteDevice.getChannel(generatedEvt.channel) );
		
	newEvent.time = generatedEvt.time;
	newEvent.channel = channel;
	newEvent.value = generatedEvt.value;
	newEvent.pos = sourceEvt.pos;

	newEvent.isMeasurementEvent = sourceEvt.isMeasurementEvent;
	newEvent.description = sourceEvt.description;

	deviceGeneratedEvents.push_back(newEvent);

}


void Parser_i::setupParsedEvents()
{
	using STI::Types::TEventSeq;
	using STI::Types::TEventSeq_var;

	unsigned i;
	const std::vector<libPython::ParsedEvent>& events = *pyParser->events();

	tEventSeq->length(events.size());

	for(i = 0; i < events.size(); i++)
	{
		tEventSeq[i].channel = events.at(i).channel;
		tEventSeq[i].time    = events.at(i).time;

		tEventSeq[i].pos.file = events.at(i).position.file;
		tEventSeq[i].pos.line = events.at(i).position.line;

		tEventSeq[i].value = events.at(i).getValue();

		tEventSeq[i].isMeasurementEvent    = events.at(i).isMeasureEvent();
		tEventSeq[i].description           = CORBA::string_dup(events.at(i).desc().c_str());
	}
}
