// RemoteFileServer.cpp : Defines the entry point for the console application.
//
#include <omniORB4/omniURI.h>
#include <string>
#include <iostream>
#include <string>

#include "ORBManager.h"
#include "FileServer_i.h"

using namespace std;

ORBManager* orbManager;
FileServer_i* fileServer;

int main(int argc, char* argv[])
{
	string homeDirectory = ".";

	orbManager = new ORBManager(argc, argv);

	if(argc > 0)
	{
		homeDirectory = string( argv[1] );
	}

	fileServer = new FileServer_i( homeDirectory );

	orbManager->registerServant(fileServer,"FileServer.obj");

	orbManager->run();

	return 0;
}

