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
#include <cassert>

#include "Control_i.h"
#include "STI_Server.h"
#include <ExperimentDocumenter.h>
#include <SequenceDocumenter.h>

Control_i::Control_i(STI_Server* server) : sti_Server(server)
{
	modeHandler = NULL;
	expSequence = NULL;
	parser = NULL;
}


Control_i::~Control_i()
{
}



void Control_i::add_Parser(Parser_i* var)
{
	assert(var != NULL);

	if(parser != NULL)
	{
		// Remove reference to the current ModeHandler_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		parser->_remove_ref();
	}

	parser = var;
	parser->_add_ref();
}

void Control_i::remove_Parser()
{
	if(parser != NULL)
	{
		parser->_remove_ref();
	}

	parser = NULL;
}

void Control_i::add_ExpSequence(ExpSequence_i* var)
{
	assert(var != NULL);

	if(expSequence != NULL)
	{
		// Remove reference to the current ModeHandler_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		parser->_remove_ref();
	}

	expSequence = var;
	expSequence->_add_ref();
}

void Control_i::remove_ExpSequence()
{
	if(expSequence != NULL)
	{
		expSequence->_remove_ref();
	}

	expSequence = NULL;
}



void Control_i::add_ModeHandler(ModeHandler_i* var)
{
	assert(var != NULL);

	if(modeHandler != NULL)
	{
		// Remove reference to the current ModeHandler_i servant, allowing
		// for the possibility that var is a new instance of the servant.
		modeHandler->_remove_ref();
	}

	modeHandler = var;
	modeHandler->_add_ref();
}

void Control_i::remove_ModeHandler()
{
	if(modeHandler != NULL)
	{
		modeHandler->_remove_ref();
	}

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


void Control_i::runSingle(::CORBA::Boolean documented, const STI_Client_Server::TExpRunInfo& info)
{
	sti_Server->playEvents();
	cout << "played" << endl;
	
	sti_Server->waitForEventsToFinish();


	if (documented)
	{
		ExperimentDocumenter documenter(info);
		documenter.writeToDisk();
	}
}


void Control_i::runSequence(::CORBA::Boolean documented, const STI_Client_Server::TExpSequenceInfo& info)
{
/*
<mySequences>
	*mySequence.xml
		<mySequence>
			* myTrial_1.xml
			* myTrial_2.xml
			<timing>
				* myTiming.py
				* myChannels.py
*/

	STI_Client_Server::TExpRunInfo currentExperimentInfo;
	currentExperimentInfo.isSequenceMember = true;

	SequenceDocumenter sequence(info, parser);

	if(documented)
	{
		sequence.writeDirectoryStructureToDisk();
		sequence.copyTimingFiles();
		sequence.createSequenceXML();
	}

	bool runsRemaining = true;

	while(runsRemaining)
	{
		unsigned experimentNumber = 0;
		if(documented)
		{
			currentExperimentInfo.filename 
				= sequence.generateExperimentFilename("_" + experimentNumber).c_str();
			
			currentExperimentInfo.serverBaseDirectory 
				= sequence.getExperimentAbsDirectory().c_str();
			
			currentExperimentInfo.sequenceRelativePath 
				= sequence.getSequenceRelativePath().c_str();	//includes directory and filename
		}

		runSingle(documented, currentExperimentInfo);

		if(documented)
			sequence.addExperiment(currentExperimentInfo);
	}

	if(documented)
	{
		sequence.writeSequenceXML();
	}
}


void Control_i::_cxx_continue()
{
}


void Control_i::stop()
{
	sti_Server->stopServer();
	sti_Server->stopAllDevices();
}

void Control_i::pause()
{
	sti_Server->pauseAllDevices();
}

void Control_i::resume()
{
	sti_Server->playEvents();
}


STI_Client_Server::ExpSequence_ptr Control_i::expSeq()
{
	STI_Client_Server::ExpSequence_ptr dummy = 0;
	return dummy;
}

char* Control_i::errMsg()
{
	const char* dummy = "dummy";
	return CORBA::string_dup(dummy);
}

char* Control_i::transferErr(const char* deviceID)
{
	CORBA::String_var error( sti_Server->getTransferErrLog(deviceID).c_str() );
	return error._retn();
}

STI_Client_Server::TExpRunInfo* Control_i::getDefaultRunInfo()
{
	std::string defaultSingleRunPath = "c:/code";
	std::string defaultSingleRunFilename = "trial";


	using STI_Client_Server::TExpRunInfo;
	using STI_Client_Server::TExpRunInfo_var;
	
	TExpRunInfo_var tRunInfo( new TExpRunInfo() );

	tRunInfo->serverBaseDirectory = defaultSingleRunPath.c_str();
	tRunInfo->filename = defaultSingleRunFilename.c_str();
	tRunInfo->description = (parser->getParsedDescription()).c_str();

	return tRunInfo._retn();
}

STI_Client_Server::TExpSequenceInfo* Control_i::getDefaultSequenceInfo()
{
	std::string defaultSequenceFilename = "timingSeq.xml";
	std::string defaultSequencePath = "c:/code";
	std::string defaultSequenceFilenameBase = "timingSeq";


	using STI_Client_Server::TExpSequenceInfo;
	using STI_Client_Server::TExpSequenceInfo_var;
	
	TExpSequenceInfo_var tSeqInfo( new TExpSequenceInfo() );

	tSeqInfo->sequenceDescription = "";
	tSeqInfo->filename = defaultSequenceFilename.c_str();
	tSeqInfo->serverBaseDirectory = defaultSequencePath.c_str();
	tSeqInfo->trialFilenameBase = defaultSequenceFilenameBase.c_str();

	return tSeqInfo._retn();

}

