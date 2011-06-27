
#include "STI_Shell.h"
#include <utils.h>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;
using std::stringstream;
using std::endl;
using std::cout;

STI_Shell::STI_Shell()
{
	orbManager = NULL;
	disconnect();
	running = true;
}

STI_Shell::~STI_Shell()
{
	disconnect();
}


bool STI_Shell::execute(std::string cmd)
{
	bool success = true;
	ParseCode code;
	vector<string> parameters;

	code = parseCommand(cmd, parameters);

	switch(code)
	{
	case Empty:
		break;
	case Connect:
		success = connect( parameters.at(0) );
		break;
	case Disconnect:
		disconnect();
		break;
	case Parse:
		success = STI_parse( parameters.at(0) );
		break;
	case Play:
		STI_play();
		break;
	case Stop:
		STI_stop();
		break;
	case Status:
		cout << "Server state: " << STI_status() << endl;
		break;
	case Exit:
		running = false;
		break;
	case Invalid:
	case Help:
		printHelp();
	default:
		success = false;
		break;
	}

	return success;
}

void STI_Shell::printHelp()
{
	cout << endl 
		<< "STI Shell command list:" << endl << endl
//		<<  "----------------------------------------------------------------------------" << endl
		<<  "   connect [IPaddress:port]     Connect to an STI Server" << endl 
		<<  "                                (e.g., connect localhost:2809)." << endl << endl
		<<  "   disconnect                   Disconnect from the STI Server." << endl << endl
		<<  "   parse [fileName]             Parse timing file at absolute server" << endl
		<<  "                                path \"fileName\" (e.g., parse c:\\test.py)." << endl << endl
		<<  "   play                         Play the currently parsed timing file." << endl << endl
		<<  "   stop                         Stop the currently playing timing file." << endl << endl
		<<  "   exit                         Close the STI Shell." << endl << endl
		<<  "   status                       Display STI Server's current state." << endl << endl
		<<  "   help                         Display this information." << endl << endl;
//		<<  "----------------------------------------------------------------------------" << endl;
}

string STI_Shell::serverName() 
{
	return server;
}

STI_Shell::ParseCode STI_Shell::parseCommand(std::string cmd, std::vector<std::string>& parameters)
{
	if(cmd.compare("")==0)
		return Empty;

	ParseCode code = Invalid;
	parameters.clear();

	STI::Utils::splitString(cmd, " ", parameters);

	if(parameters.front().compare("connect")==0)
	{
		if(parameters.size() == 2)
			code = Connect;
	}
	if(parameters.front().compare("disconnect")==0)
	{
			code = Disconnect;
	}
	if(parameters.front().compare("parse")==0)
	{
		if(parameters.size() == 2)
			code = Parse;
	}
	if(parameters.front().compare("play")==0)
	{
		code = Play;
	}
	if(parameters.front().compare("stop")==0)
	{
		code = Stop;
	}
	if(parameters.front().compare("status")==0)
	{
		code = Status;
	}
	if(parameters.front().compare("exit")==0)
	{
		code = Exit;
	}
	if(parameters.front().compare("help")==0 || parameters.front().compare("?")==0 
		|| parameters.front().compare("-h")==0 || parameters.front().compare("--h")==0)
	{
		code = Help;
	}

	if(code != Invalid && parameters.size() > 0)
	{
		parameters.erase( parameters.begin() );
	}

	return code;
}
void STI_Shell::disconnect()
{
	connected = false;
	server = "~";
	
	if( orbManager != NULL )
	{
		delete orbManager;
		orbManager = NULL;
	}
}

bool STI_Shell::connect(std::string serverAddressAndPort)
{
	if(connected)
		return false;

	vector<string> serverAddress;
	vector<string> argsORB;

	STI::Utils::splitString(serverAddressAndPort, ":", serverAddress);

	if(serverAddress.size() != 2)
	{
		connected = false;
		return false;
	}

	stringstream address;
	address << "-ORBInitRef NameService=corbaname::" << serverAddressAndPort;
	STI::Utils::splitString(address.str(), " ", argsORB);

	int argcORB = argsORB.size();
	char** argvORB = new char*[ argcORB ];
	for(int i = 0; i < argcORB; i++)
	{
		argvORB[i] = new char[argsORB[i].size() + 1];
		strcpy(argvORB[i], argsORB[i].c_str());
	}

	if( orbManager != NULL)
	{
		delete orbManager;
		orbManager = NULL;
		connected = false;
	}


	orbManager = new ORBManager(argcORB, argvORB);

	CORBA::Object_var obj = orbManager->getObjectReference("STI/Client/ClientBootstrap.Object");
		
	bootstrap = STI::Client_Server::ClientBootstrap::_narrow(obj);

	bool success = false;

	if( !CORBA::is_nil(bootstrap) )
	{
		// Object reference was found on the NameService
		try {
			bootstrap->getServerTimingSeqControl()->status();
//			bootstrap->getParser()->parseFile("C:\\Users\\Jason\\Code\\sti\\timing\\vcoTest.py");
			success = true;
		}
		catch(CORBA::TRANSIENT& ex) {
				//cerr << "Caught system exception CORBA::" 
				//	<< ex._name() << " -- unable to contact the "
				//	<< "STI Server." << endl
				//	<< "Make sure the server is running and that omniORB is "
				//	<< "configured correctly." << endl;
		}
		catch(CORBA::SystemException& ex) {
				//cerr << "Caught a CORBA::" << ex._name()
				//	<< " while trying to contact the STI Server." << endl;
		}
		catch(...) {
			//cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
		}
	}

	if(success)
	{
		server = serverAddressAndPort;
		connected = true;
	}

	return success;
}

bool STI_Shell::STI_parse(std::string file)
{
	if(!connected)
		return false;

	bool error = true;

	try {
		error = bootstrap->getParser()->parseFile( file.c_str() );
	}
	catch(CORBA::TRANSIENT& ex) {
			//cerr << "Caught system exception CORBA::" 
			//	<< ex._name() << " -- unable to contact the "
			//	<< "STI Server." << endl
			//	<< "Make sure the server is running and that omniORB is "
			//	<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
			//cerr << "Caught a CORBA::" << ex._name()
			//	<< " while trying to contact the STI Server." << endl;
	}
	catch(...) {
		//cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
	}
	return !error;
}

void STI_Shell::STI_play()
{
	if(!connected)
		return;

	try {
		bootstrap->getServerTimingSeqControl()->runSingle(false);
	}
	catch(CORBA::TRANSIENT& ex) {
			//cerr << "Caught system exception CORBA::" 
			//	<< ex._name() << " -- unable to contact the "
			//	<< "STI Server." << endl
			//	<< "Make sure the server is running and that omniORB is "
			//	<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
			//cerr << "Caught a CORBA::" << ex._name()
			//	<< " while trying to contact the STI Server." << endl;
	}
	catch(...) {
		//cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
	}
}

void STI_Shell::STI_stop()
{
	if(!connected)
		return;

	try {
		bootstrap->getServerTimingSeqControl()->stop();
	}
	catch(CORBA::TRANSIENT& ex) {
			//cerr << "Caught system exception CORBA::" 
			//	<< ex._name() << " -- unable to contact the "
			//	<< "STI Server." << endl
			//	<< "Make sure the server is running and that omniORB is "
			//	<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
			//cerr << "Caught a CORBA::" << ex._name()
			//	<< " while trying to contact the STI Server." << endl;
	}
	catch(...) {
		//cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
	}
}

std::string STI_Shell::STI_status()
{
	string state = "Disconnected";

	if(!connected)
		return state;

	STI::Types::TStatusLevel statusLevel = STI::Types::LevelError;
	
	try {
		statusLevel = bootstrap->getServerTimingSeqControl()->status().level;
	}
	catch(CORBA::TRANSIENT& ex) {
			//cerr << "Caught system exception CORBA::" 
			//	<< ex._name() << " -- unable to contact the "
			//	<< "STI Server." << endl
			//	<< "Make sure the server is running and that omniORB is "
			//	<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
			//cerr << "Caught a CORBA::" << ex._name()
			//	<< " while trying to contact the STI Server." << endl;
	}
	catch(...) {
		//cerr << "Caught unknown exception in aquireServerConfigure()." << endl;
	}

	switch(statusLevel)
	{
	case STI::Types::LevelParsed:
		state = "Parsed";
		break;
	case STI::Types::LevelParsing:
		state = "Parsing";
		break;
	case STI::Types::LevelPaused:
		state = "Paused";
		break;
	case STI::Types::LevelRunning:
		state = "Playing";
		break;
	case STI::Types::LevelStopped:
		state = "Stopped";
		break;
	case STI::Types::LevelUnparsed:
		state = "Unparsed";
		break;
	case STI::Types::LevelError:
	default:
		state = "Error: Unknown server state.";
		break;
	}

	return state;
}
