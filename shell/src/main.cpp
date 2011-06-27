/*! \file main.cpp
 *  \author Jason Hogan
 *  \brief main()
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

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <utils.h>
#include "STI_Shell.h"

using namespace std;

void displayHelp();

int main(int argc, char **argv)
{
	STI_Shell shell;

	bool runScript = false;
	string scriptFilename = "";
	size_t posScript, posColon, posSwitch;

	vector<string> args;
	STI::Utils::convertArgs(argc, argv, args);


	//command line argument parsing
	if(args.size() > 3)
	{
		displayHelp();
	}
	else if(args.size() > 1)
	{
		for(unsigned i = 1; i < args.size(); i++)
		{
			posScript = args.at(i).find("-s=");
			posColon = args.at(i).find(":");
			posSwitch = args.at(i).find("-");

			if(posScript != std::string::npos)
			{
				//this is a script
				runScript = true;
				scriptFilename = args.at(i).substr(3);
			}
			else if(posScript == std::string::npos && posColon != std::string::npos)
			{
				//this is a server address
				shell.execute("connect " + args.at(i));
			}
			else if(args.at(i).compare("-h")==0 || args.at(i).compare("--h")==0)
			{
				//show command line help and exit
				displayHelp();
				shell.execute("exit");
			}
		}
	}


	//Open and run script (if specified)
	if(runScript)
	{
		std::fstream script(scriptFilename.c_str(), std::fstream::in);

		if( !script.is_open() )
		{
			std::cout << "Error opening script file '" << scriptFilename << "'." << std::endl;
		}
		else
		{
			std::string line;
			bool success = true;

			while( success && getline(script, line) && shell.isRunning() )
			{
				cout << "STI:" << shell.serverName() << ">" << line << endl;
				success = shell.execute(line);
			}
		}
	}


	//Enter command loop
	string cmd;
	while( shell.isRunning() )
	{
		cout << "STI:" << shell.serverName() << ">";
		getline(cin, cmd);
		shell.execute(cmd);
	}

	return 0;
}


void displayHelp()
{
	cout << endl 
		<< "STI Shell -- Command Line Interface" << endl << endl;
	cout << "Usage:" << endl << endl;
	cout << "stishell [serverIP:Port] [Options]" << endl << endl;
	cout << "[serverIP:Port] is the valid STI Server IP address and port number." << endl 
		<< "For example, localhost:2809 would connect to IP localhost and port 2809." << endl << endl;
	cout << "[Options] may include:" << endl;
	cout << "    -h               Shows this help page." << endl;
	cout << "    -s=\"filename\"    Runs the script in file \"filename\"." << endl << endl;
}

