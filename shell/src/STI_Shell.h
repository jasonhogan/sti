/*! \file STI_Shell.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_Shell
 *  \section license License
 *
 *  Copyright (C) 2011 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef STI_SHELL_H
#define STI_SHELL_H

#include <string>
#include <vector>

#include <ORBManager.h>
#include <client.h>

using STI::Client_Server::ClientBootstrap_var;


class STI_Shell
{
public:
	STI_Shell();
	~STI_Shell();

	bool execute(std::string cmd);

	bool connectedToServer() { return connected; }
	
	std::string serverName();
	bool isRunning() { return running; }

	void printHelp();

private:

	enum ParseCode {Empty, Invalid, Exit, Connect, Disconnect, Parse, Play, Stop, Help, Status};

	ParseCode parseCommand(std::string cmd, std::vector<std::string>& parameters);
	bool connect(std::string serverAddressAndPort);
	void disconnect();

	bool STI_parse(std::string file);
	void STI_play();
	void STI_stop();
	std::string STI_status();

	bool connected;
	bool running;
	std::string server;
	ORBManager* orbManager;

	ClientBootstrap_var bootstrap;

};

#endif