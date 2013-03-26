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
	expSequence = NULL;
	parser = NULL;
	stopSequence = false;
}


ServerTimingSeqControl_i::~ServerTimingSeqControl_i()
{
}



void ServerTimingSeqControl_i::add_Parser(Parser_i* var)
{
	assert(var != NULL);
	parser = var;
}


void ServerTimingSeqControl_i::add_ExpSequence(ExpSequence_i* var)
{
	assert(var != NULL);
	expSequence = var;
	expSequence->resetExpNumber();
}


void ServerTimingSeqControl_i::runSingle(::CORBA::Boolean documented)
{
	parser->clearOverwritten();

	runSingleExperiment(documented);
}


void ServerTimingSeqControl_i::runSingleExperiment(bool documented)
{
	//if( !sti_Server->requestPlay() )
	//	return;

	sti_Server->playEvents();
	
	//put everything below in sti_Server->document(EngineInstance);
	if (documented)
	{
		//Make directory structure
		std::string baseDirectory = sti_Server->getDocumentationSettings()->getTodaysBaseAbsDir();
		fs::create_directories(fs::path(baseDirectory));



		ExperimentDocumenter documenter(baseDirectory, sti_Server->getDocumentationSettings(), 
			parser->getParsedDescription());

		documenter.addTimingFiles( parser->getTimingFiles() );
		documenter.addVariables( parser->getParsedVars() );
		
		documenter.addParsedEventsTable(parser->getParsedEvents(), parser->getAllChannels(), parser->getTimingFiles() );

		const std::vector<std::string>& devicesWithEvents = sti_Server->getDevicesWithEvents();
		RemoteDeviceMap& registeredDevices = sti_Server->getRegisteredDevices();

		//for(unsigned i = 0; i < devicesWithEvents.size(); i++)
		//{
		//	documenter.addDeviceData( *registeredDevices.find(devicesWithEvents.at(i))->second );
		//}
		
		RemoteDeviceMap::iterator it;
		for(it = registeredDevices.begin(); it != registeredDevices.end(); it++)
		{
			documenter.addDeviceData( *it->second );
		}
		
		documenter.writeToDisk();
		documenter.copyTimingFiles();
	}
}

void ServerTimingSeqControl_i::runSingleContinuous()
{
	parser->clearOverwritten();

	sti_Server->playEvents(true);


	//runContinuous = true;
	//
	//while(runContinuous)
	//{
	//	runSingleExperiment(false);
	//}
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
	}

	bool parsingSuccess;
	expSequence->resetExpNumber();
	bool runsRemaining = expSequence->getNextExperiment();

	unsigned experimentNumber = 0;
	stopSequence = false;

	while(runsRemaining && !stopSequence)
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

		if( !parsingSuccess || stopSequence )
			break;

		runSingleExperiment(false);	//don't document it yet

		if(stopSequence)
			break;

		expSequence->setCurrentExperimentToDone();


		if(documented)
		{
			//copy overwritten var list for documentation in series file
			const STI::Types::TOverwrittenSeq& varSeq = expSequence->getCurrentOverwritten();
			std::map<std::string, std::string> overwrittenVars;

			for(unsigned i = 0; i < varSeq.length(); i++)
			{
				overwrittenVars.insert( std::pair<std::string, std::string>(
					CORBA::string_dup(varSeq[i].name), 
					CORBA::string_dup(varSeq[i].value)
					) );
			}

			sequence.addExperiment(sti_Server->getRegisteredDevices(), overwrittenVars);
			sequence.writeToDisk();		//save the current series file to disk
		}


		runsRemaining = expSequence->getNextExperiment();	//sets up overwritten variables in parser
		experimentNumber++;
	}

	sti_Server->updateState();

	if(documented)
	{
		sequence.writeToDisk();
	}
}


void ServerTimingSeqControl_i::stop()
{
	stopSequence = true;
	sti_Server->stopServer();
	sti_Server->stopAllDevices();
}

void ServerTimingSeqControl_i::pause()
{
	sti_Server->pauseServer(false);
}

void ServerTimingSeqControl_i::resume()
{
	sti_Server->unpauseServer(false);
}


