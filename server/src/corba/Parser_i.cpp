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
#include "ModeHandler_i.h"
#include "Parser_i.h"

#include <iostream>


Parser_i::Parser_i()
{
	modeHandler = NULL;
}


Parser_i::~Parser_i()
{
	remove_ModeHandler();
}


void Parser_i::add_ModeHandler(ModeHandler_i* var)
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

void Parser_i::remove_ModeHandler()
{
	if(modeHandler != NULL)
	{
		modeHandler->_remove_ref();
	}

	modeHandler = NULL;
}



::CORBA::Boolean Parser_i::parseFile(const char* filename)
{
	return true;
}


::CORBA::Boolean Parser_i::parseString(const char* code)
{
	return true;
}


STI_Client_Server::TOverwrittenSeq* Parser_i::overwritten()
{
	STI_Client_Server::TOverwrittenSeq* dummy = 0;
	return dummy;
}


void Parser_i::overwritten(const STI_Client_Server::TOverwrittenSeq& _v)
{
}


::CORBA::Boolean Parser_i::lockOnParse()
{
	return true;
}


void Parser_i::lockOnParse(::CORBA::Boolean _v)
{
}


char* Parser_i::outMsg()
{
	char* dummy = "Some dummy output message";
	return CORBA::string_dup(dummy);
}


char* Parser_i::errMsg()
{
	char* msg = "Error: An error has occurred in Parser_i and errMsg() has been called";
	return CORBA::string_dup(msg);
}


char* Parser_i::mainFile()
{
	char* dummy = "main file";
	return CORBA::string_dup(dummy);
}

/* Parser_i::channels()
 *
 * Eventually should send back legitimate information about the control devices
 * and instrumentation channels with which they are associated.
 *
 * At present sends back a dummy array of 2 channels hard-coded in here 
 * --Jonathan Harvey (harv@stanford.edu)
 *
 * @return STI_Client_Server::TChannelSeq* channelList  The output list of channels
 */
STI_Client_Server::TChannelSeq* Parser_i::channels()
{
    STI_Client_Server::TChannelSeq* channelList = new STI_Client_Server::TChannelSeq();

    /* Set up the list which will hold all of the TChannels */
    STI_Client_Server::TChannel* terminatorChannelArray = new STI_Client_Server::TChannel[2];

    /* The first way of defining a channel: Set up the Device component first, 
     * then build a TChannel */     
    short newChannel = 5;

    char* newDeviceType = "some device type";
    char* newAddress = "AB CD EF GH";
    short newModuleType = 32;

    STI_Client_Server::TDevice* terminatorDevice = new STI_Client_Server::TDevice();
    terminatorDevice->address = newAddress;
    terminatorDevice->deviceType = newDeviceType;
    terminatorDevice->moduleNum = newModuleType;

    STI_Client_Server::TChannel* terminatorChannel = new STI_Client_Server::TChannel();
    terminatorChannel->device = *terminatorDevice;
    terminatorChannel->channel = newChannel;

    terminatorChannelArray[0] = *terminatorChannel;

    /* The other way: Directly defining the components of the list*/
    terminatorChannelArray[1].channel = 2;
    terminatorChannelArray[1].device.deviceType = "A Second Device Type";
    terminatorChannelArray[1].device.address = "A1 B2 C3 D4";
    terminatorChannelArray[1].device.moduleNum = 16;

    /* Finally, replace the TChannelSeq with the list */
    channelList->replace(2, 2, terminatorChannelArray);

    return channelList;
}

/* Parser_i::files()
 * 
 * Eventually should send back a list of file locations where all the source files
 * going into the parser live. 
 *
 * Currently sends back a hard-coded dummy sequence of 4 strings, tests truncation and
 * manipulation of individual elements. 
 * -- Jason Hogan (hogan@stanford.edu) and Jonathan Harvey (harv@stanford.edu)
 *
 * @return testSeq  The hard-coded sequence of test strings
 */
STI_Client_Server::TStringSeq* Parser_i::files()
{

	char* test = "mode Handler overwritten!!";
	modeHandler->localDummy = test;

	STI_Client_Server::TStringSeq* testSeq= 0;
        
        testSeq = new STI_Client_Server::TStringSeq();
        
        char** data = new char*[5];
        data[0] = "test0";
        data[1] = "test433";
        data[2] = "test2";
        data[4] = "test4";
        
        testSeq->replace(10,4,data); // gracefully truncates
           
        (*testSeq)[3] = "hello"; // and this works
        
	return testSeq;
}


STI_Client_Server::TVariableSeq* Parser_i::variables()
{
	STI_Client_Server::TVariableSeq* dummy = 0;
	return dummy;
}

/* Parser_i::files()
 * 
 * Eventually should send back all the event data of the experiment.
 *
 * Currently sends back a hard-coded dummy sequence of two events.
 * -- Jonathan Harvey (harv@stanford.edu)
 *
 * @return testSeq  The hard-coded sequence of test strings
 */
STI_Client_Server::TEventSeq* Parser_i::events()
{
    //STI_Client_Server::TEventSeq* dummy = 0;
    STI_Client_Server::TEventSeq* eventList = new STI_Client_Server::TEventSeq();

    /* Set up the list which will hold all of the TEvents */
    STI_Client_Server::TEvent* terminatorEventArray = new STI_Client_Server::TEvent[2];
    terminatorEventArray[0].channel = 32;
    terminatorEventArray[0].time = 123.4567;
    terminatorEventArray[0].pos.file = 1;
    terminatorEventArray[0].pos.line = 23456;
    terminatorEventArray[0].value.number(1001);
    
    terminatorEventArray[1].channel = 16;
    terminatorEventArray[1].time = 765.4321;
    terminatorEventArray[1].pos.file = 2;
    terminatorEventArray[1].pos.line = 65432;
    terminatorEventArray[1].value.stringVal("Some String Event Happened!");
    
    eventList->replace(2,2,terminatorEventArray);
    
    return eventList;	//Warning, eventList is currently local!
}

