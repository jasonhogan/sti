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


	for(int i = 0; i < argc; i++)
	{
		cerr << i << ": " << argv[i] << endl;
	}

	cerr << "-----------" << endl;

	orbManager = new ORBManager(argc, argv);

	for(int i = 0; i < argc; i++)
	{
		cerr << i << ": " << argv[i] << endl;
	}

	if(argc > 0)
	{
		homeDirectory = string( argv[1] );
	}

	fileServer = new FileServer_i( homeDirectory );

	orbManager->registerServant(fileServer,"STI/FileServer/FileServer.obj");

	orbManager->run();

	return 0;
}

