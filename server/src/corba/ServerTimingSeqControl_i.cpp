/*! \file ServerTimingSeqControl_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ServerTimingSeqControl_i
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

#include "ServerTimingSeqControl_i.h"
#include "STI_Server.h"
#include <ExperimentDocumenter.h>
#include <SequenceDocumenter.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

namespace fs = boost::filesystem;

ServerTimingSeqControl_i::ServerTimingSeqControl_i(STI_Server* server) : sti_Server(server)
{
	modeHandler = NULL;
	expSequence = NULL;
	parser = NULL;
	runContinuous = false;
}


ServerTimingSeqControl_i::~ServerTimingSeqControl_i()
{
}



void ServerTimingSeqControl_i::add_Parser(Parser_i* var)
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

void ServerTimingSeqControl_i::remove_Parser()
{
	if(parser != NULL)
	{
		parser->_remove_ref();
	}

	parser = NULL;
}

void ServerTimingSeqControl_i::add_ExpSequence(ExpSequence_i* var)
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

void ServerTimingSeqControl_i::remove_ExpSequence()
{
	if(expSequence != NULL)
	{
		expSequence->_remove_ref();
	}

	expSequence = NULL;
}



void ServerTimingSeqControl_i::add_ModeHandler(ModeHandler_i* var)
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

void ServerTimingSeqControl_i::remove_ModeHandler()
{
	if(modeHandler != NULL)
	{
		modeHandler->_remove_ref();
	}

	modeHandler = NULL;
}






STI::Types::TStatus ServerTimingSeqControl_i::status()
{
	STI::Types::TStatus dummy;
	dummy.curTime = 0;
	return dummy;
}


void ServerTimingSeqControl_i::reset()
{
}


void ServerTimingSeqControl_i::setDirect()
{
}


void ServerTimingSeqControl_i::runSingle(::CORBA::Boolean documented)
{
	parser->clearOverwritten();

	runSingleExperiment(documented);
}


void ServerTimingSeqControl_i::runSingleExperiment(bool documented)
{
	if( !sti_Server->requestPlay() )
		return;

	sti_Server->playEvents();
	
	if (documented)
	{
		//Make directory structure
		std::string baseDirectory = sti_Server->getDocumentationSettings()->getTodaysBaseAbsDir();
		fs::create_directories(fs::path(baseDirectory));

		ExperimentDocumenter documenter(baseDirectory, sti_Server->getDocumentationSettings(), 
			parser->getParsedDescription());

		documenter.addTimingFiles( parser->getTimingFiles() );
		documenter.addVariables( parser->getParsedVars() );

		const std::vector<std::string>& devicesWithEvents = sti_Server->getDevicesWithEvents();
		const RemoteDeviceMap& registeredDevices = sti_Server->getRegisteredDevices();

		//for(unsigned i = 0; i < devicesWithEvents.size(); i++)
		//{
		//	documenter.addDeviceData( *registeredDevices.find(devicesWithEvents.at(i))->second );
		//}
		
		RemoteDeviceMap::const_iterator it;
		for(it = registeredDevices.begin(); it != registeredDevices.end(); it++)
		{
			documenter.addDeviceData( *it->second );
		}
		
		documenter.writeToDisk();
	}
}

void ServerTimingSeqControl_i::runSingleContinuous()
{
	parser->clearOverwritten();
	
	runContinuous = true;
	
	while(runContinuous)
	{
		runSingleExperiment(false);
	}
}

void ServerTimingSeqControl_i::runSequence(::CORBA::Boolean documented)
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



//	STI::Types::TExpRunInfo currentExperimentInfo;
//	currentExperimentInfo.isSequenceMember = true;
		
	std::string baseDirectory = sti_Server->getDocumentationSettings()->getTodaysBaseAbsDir();
	fs::create_directories(fs::path(baseDirectory));

	//ExperimentDocumenter documenter(baseDirectory, sti_Server->getDocumentationSettings(), 
	//	parser->getParsedDescription());

	SequenceDocumenter sequence(baseDirectory, parser, sti_Server->getDocumentationSettings());

	if(documented)
	{
		//Make directory structure
		std::string baseDirectory = sti_Server->getDocumentationSettings()->getTodaysBaseAbsDir();
		fs::create_directories(fs::path(baseDirectory));
	
	//	sequence.writeDirectoryStructureToDisk();
		sequence.copyTimingFiles();
		sequence.createSequenceXML();
	}

	bool parsingSuccess;
	bool runsRemaining = expSequence->getNextExperiment();

	unsigned experimentNumber = 0;

	while(runsRemaining)
	{
		if(documented)
		{
			//currentExperimentInfo.filename 
			//	= sequence.generateExperimentFilename("_" + experimentNumber).c_str();
			//
			//currentExperimentInfo.serverBaseDirectory 
			//	= sequence.getExperimentAbsDirectory().c_str();
			//
			//currentExperimentInfo.sequenceRelativePath 
			//	= sequence.getSequenceRelativePath().c_str();	//includes directory and filename
		}

		parser->overwritten( expSequence->getCurrentOverwritten() );
		parsingSuccess = !parser->parseSequenceTimingFile();

		if( !parsingSuccess )
			break;

		runSingleExperiment(false);	//don't document it yet
		expSequence->setCurrentExperimentToDone();

		if(documented)
			sequence.addExperiment(sti_Server->getRegisteredDevices());

		runsRemaining = expSequence->getNextExperiment();	//sets up overwritten variables in parser
		experimentNumber++;

	}

	if(documented)
	{
		sequence.writeSequenceXML();
	}
}


void ServerTimingSeqControl_i::_cxx_continue()
{
}


void ServerTimingSeqControl_i::stop()
{

	runContinuous = false;
	sti_Server->stopServer();
	sti_Server->stopAllDevices();
}

void ServerTimingSeqControl_i::pause()
{
	sti_Server->pauseServer(false);
}

void ServerTimingSeqControl_i::resume()
{
//	if( !sti_Server->isPausedByDevice() )
//		sti_Server->playEvents();

	sti_Server->unpauseServer(false);

}


STI::Client_Server::ExpSequence_ptr ServerTimingSeqControl_i::expSeq()
{
	STI::Client_Server::ExpSequence_ptr dummy = 0;
	return dummy;
}

char* ServerTimingSeqControl_i::errMsg()
{
	const char* dummy = "dummy";
	return CORBA::string_dup(dummy);
}

char* ServerTimingSeqControl_i::transferErr(const char* deviceID)
{
	CORBA::String_var error( sti_Server->getTransferErrLog(deviceID).c_str() );
	return error._retn();
}

STI::Types::TExpRunInfo* ServerTimingSeqControl_i::getDefaultRunInfo()
{
	std::string defaultSingleRunPath = "c:/code";
	std::string defaultSingleRunFilename = "trial";


	using STI::Types::TExpRunInfo;
	using STI::Types::TExpRunInfo_var;
	
	TExpRunInfo_var tRunInfo( new TExpRunInfo() );

	tRunInfo->serverBaseDirectory = defaultSingleRunPath.c_str();
	tRunInfo->filename = defaultSingleRunFilename.c_str();
	tRunInfo->description = (parser->getParsedDescription()).c_str();

	return tRunInfo._retn();
}

STI::Types::TExpSequenceInfo* ServerTimingSeqControl_i::getDefaultSequenceInfo()
{
	std::string defaultSequenceFilename = "timingSeq.xml";
	std::string defaultSequencePath = "c:/code";
	std::string defaultSequenceFilenameBase = "timingSeq";


	using STI::Types::TExpSequenceInfo;
	using STI::Types::TExpSequenceInfo_var;
	
	TExpSequenceInfo_var tSeqInfo( new TExpSequenceInfo() );

	tSeqInfo->sequenceDescription = "";
	tSeqInfo->filename = defaultSequenceFilename.c_str();
	tSeqInfo->serverBaseDirectory = defaultSequencePath.c_str();
	tSeqInfo->trialFilenameBase = defaultSequenceFilenameBase.c_str();

	return tSeqInfo._retn();

}

