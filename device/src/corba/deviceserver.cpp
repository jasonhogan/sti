/*! \file deviceserver.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the deviceserver test program
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


#include <omniORB4/omniURI.h>
#include <string>
#include <iostream>

#include "ORBManager.h"
#include "Configure_i.h"

using namespace std;

Configure_i* configureServant;

int main(int argc, char **argv)
{
	ORBManager orbManager(argc, argv);

	configureServant = new Configure_i();
	
	//Inter-servant communication
//	parserServant->add_ModeHandler(modeHandlerServant);
	
	//Register Servants
	orbManager.registerServant(configureServant, 
		"STI.Context/Device.Context/TestDevice.Context/Configure.Object");


	cerr << orbManager.errMsg() << endl;

	orbManager.run();
	
	return 0;
}