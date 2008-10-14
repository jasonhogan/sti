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
	orbManager = new ORBManager(argc, argv);
	fileServer = new FileServer_i();

	orbManager->registerServant(fileServer,"FileServer.obj");

	string file = "ReadMe.txt";
//	fileServer->fileExists(file.c_str());

	cerr << orbManager->errMsg() << endl;

	orbManager->run();

	return 0;
}

