/*! \file dummyserver.cpp
 *  \author Jason Hogan and Jon Harvey
 *  \brief A dummy server which registers Objects with the NameService.
 */

//#include "stdafx.h"

#include <omniORB4/omniURI.h>
#include <string>
#include <iostream>

#include "ORBManager.h"
#include "Control_i.h"
#include "ExpSequence_i.h"
#include "ModeHandler_i.h"
#include "Parser_i.h"

using namespace std;

Control_i* controlServant;
ExpSequence_i* expSequenceServant;
ModeHandler_i* modeHandlerServant;
Parser_i* parserServant;

int main(int argc, char **argv)
{
	ORBManager orbManager(argc, argv);

	controlServant = new Control_i();
	expSequenceServant = new ExpSequence_i();
	modeHandlerServant = new ModeHandler_i();
	parserServant = new Parser_i();
	
	//Inter-servant communication
	parserServant->add_ModeHandler(modeHandlerServant);
	controlServant->add_Parser(parserServant);
	controlServant->add_ModeHandler(modeHandlerServant);
	controlServant->add_ExpSequence(expSequenceServant);
	
	//Register Servants
	orbManager.registerServant(controlServant, 
		"STI_Client_Server.Context/Control.Object");

	orbManager.registerServant(expSequenceServant, 
		"STI_Client_Server.Context/ExpSequence.Object");

	orbManager.registerServant(modeHandlerServant, 
		"STI_Client_Server.Context/ModeHandler.Object");

	orbManager.registerServant(parserServant, 
		"STI_Client_Server.Context/Parser.Object");



	cerr << orbManager.errMsg() << endl;

	orbManager.run();
	
	return 0;
}