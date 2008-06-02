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

//This is a test platform for STI_Device interface.
//Combined with autoserver.cpp, deviceserver.cpp will automatically detect
//a Server and acquire a unique DeviceID from the Server. It then uses 
//this ID to register its servants and mount itself on the Server.


#include <omniORB4/omniURI.h>
#include <string>
#include <iostream>

#include "ORBManager.h"
#include "Configure_i.h"

#include "STI_Device.h"
#include "device.h"

#include "testDevice.h"

using namespace std;

Configure_i* configureServant;
ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);


	// A derived class of STI_Device
	testDevice t1(orbManager,"My Test Device", "testDevice", "192_52_77_1", 0);

	cerr << "t1 type: " << t1.configureServant->deviceType() << endl;

	char* key = "key1";
	cerr << "t1 Attribute: " << t1.configureServant->getAttribute(key) << endl;

	t1.configureServant->setAttribute("key1","new value");
	cerr << "t1 setAttribute: " << t1.configureServant->getAttribute(key)  << endl;

	STI_Server_Device::TAttributeSeq* tAttribSeqTemp = t1.configureServant->attributes();
	cerr << "t1 TAttributeSeq --> " << *((*tAttribSeqTemp)[0].key + 1)  << endl<<endl;

	char *static_arr[] = {"one", "two", "three"};

	int len=sizeof(static_arr);
	int num = sizeof(static_arr)/sizeof(char*);
	int i=0;

	while(len>0)
	{
		cerr <<num<<" len = " << len <<" : "<< static_arr[i] << endl;
		len -= sizeof(static_arr[i]);
		i++;
	}
	cerr<<endl<<endl;



		
	// Look for a ServerDevice Obj and configure server with channel info, etc.
	// Loop until contact is made or timeout or server error

	// Make a new thread that waits for orb->run() and then mounts the device on the server.
	// The thread loops, trying to resolve one of the device's own servants (DeviceControl)
	// from the NameService, calling status(), until timeout or error.

	//Register Device Servants under context provided by deviceID from server

	// class STI_Device "has a" ORBManager
	// class Analog : public Board, public STI_Device	// Makes Analog board devices
	// this way one ORBManager can have multiple Devices (e.g. for the timing main board)

	//Clarify differences between Device, DeviceServer and Server
	
	/* Server is the intermediary between Client and DeviceServer (and DocumentationServer??)
	** DeviceServer auto activates with Server
	** DeviceServer controls program flow (start(), pause(), etc..) for one or more Devices
	** Devices auto activate with Server
	** Each application implements the DeviceServer interface

	STI_Device
	- registerServant(Configure)
	- registerServant(DataTransfer)
	- ServerConfigure*
	- ORBManager*
	*/


//	cerr << orbManager->errMsg() << endl;

	cerr << "ORB about to Run." << endl;
	orbManager->run();
	
	return 0;
}