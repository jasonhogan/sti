/*! \file STI_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class STI_Device
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
#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif

#include "STI_Device.h"
#include "ORBManager.h"
#include "Configure_i.h"
#include "DataTransfer_i.h"
#include "Attribute.h"
#include "device.h"

#include <string>
#include <map>
using std::string;
using std::map;


#include <iostream>
using namespace std;


STI_Device::STI_Device(ORBManager* orb_manager, std::string DeviceName, 
					   std::string DeviceType, std::string Address, 
					   unsigned short ModuleNumber)
: orbManager(orb_manager), deviceName(DeviceName)
{
	attributes_ptr = &attributes;

	//TDevice
	tDevice.deviceType = CORBA::string_dup(DeviceType.c_str());
	tDevice.address = CORBA::string_dup(Address.c_str());
	tDevice.moduleNum = ModuleNumber;

	configureServant = new Configure_i(this);
	dataTransferServant = new DataTransfer_i(this);

	// Aquire a reference to configureServant from the NameService.
	// When found, register this STI_Device with the server and acquire 
	// a unique deviceID.
	serverConfigureFound = false;
	registedWithServer = false;
	omni_thread::create(acquireServerReferenceWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);

	// Hold until serverConfigureFound and registedWithServer.
	// Register servants with the Name Service, then mount the STI_Device
	// using DeviceServer::mount(deviceID) 
	omni_thread::create(initServerWrapper, (void*)this, 
		omni_thread::PRIORITY_LOW);
}

//reaquireServerReference()

STI_Device::~STI_Device()
{
}

void STI_Device::initServerWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->initServer();
}

void STI_Device::initServer()
{
	// Wait until the ServerConfigure Object is found and the DeviceID
	// has been acquired.
	while(!serverConfigureFound) {}
	while(!registedWithServer) {}	// Have DeviceID

	cerr << "Registered!!" << endl;

	STI_Server_Device::Configure_var ConfigureRef;

	string contextName = CORBA::string_dup(tDeviceID->deviceContext);
	contextName.insert(0,"STI/Device/");

	string configureObjectName = "Configure.Object";
	string dataTransferObjectName = "DataTransfer.Object";


	// Loop until this STI_Device succesfully registers its 
	// servants with the Name Service
	do {

		cerr << "Trying to register servants: ." << (contextName + configureObjectName)  << "."<< endl;

		orbManager->registerServant(configureServant, 
			contextName + configureObjectName);
		orbManager->registerServant(dataTransferServant, 
			contextName + dataTransferObjectName);

		// Try to resolve one of the servants as a test
		CORBA::Object_var obj = orbManager->getObjectReference(
			contextName + configureObjectName);
		ConfigureRef = STI_Server_Device::Configure::_narrow(obj);

	} while(CORBA::is_nil(ConfigureRef));  
	// CAREFULL: This doesn't mean the servants are live, just that their 
	// is a resolvable reference on the Name Service. Add another check for this.
		
	cerr << "Servants registered!!" << endl;

	//setChannels()
	//mountDevice()
	try {
		ServerConfigureRef->mountDevice(CORBA::string_dup(tDeviceID->deviceID));
	}
	catch(CORBA::TRANSIENT& ex) {
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}
}


void STI_Device::acquireServerReferenceWrapper(void* object)
{
	STI_Device* thisObject = (STI_Device*) object;
	thisObject->acquireServerReference();
}

// getDeviceID() should only be called from inside a try{} block
void STI_Device::getDeviceID()
{
	serverName = ServerConfigureRef->serverName();

	// registerDevice() and get the unique DeviceID from the server;
	tDeviceID = ServerConfigureRef->registerDevice(
		CORBA::string_dup(getDeviceName().c_str()), tDevice);

	registedWithServer = tDeviceID->registered;
}

void STI_Device::acquireServerReference()
{

	CORBA::Object_var obj;

	// Try to acquire ServerConfigure Object
	while(!serverConfigureFound || !registedWithServer)
	{
		obj = orbManager->getObjectReference(
			"STI/Device/ServerConfigure.Object");
		
		ServerConfigureRef = STI_Server_Device::ServerConfigure::_narrow(obj);
		
		if( !CORBA::is_nil(ServerConfigureRef) )
		{
			// Object reference was found on the NameService
			serverConfigureFound = true;
			try {
	cerr << "gets to here?" << endl;
				getDeviceID();
			}
			catch(CORBA::TRANSIENT& ex) {
				cerr << "Caught system exception CORBA::" 
					<< ex._name() << " -- unable to contact the "
					<< "STI Server." << endl
					<< "Make sure the server is running and that omniORB is "
					<< "configured correctly." << endl;
			}
			catch(CORBA::SystemException& ex) {
				cerr << "Caught a CORBA::" << ex._name()
					<< " while trying to contact the STI Server." << endl;
			}
		}
		else
		{
			// Reference is nil
			cerr << "ServerConfigure Object was not found." << endl;
		}
	}
}


std::string STI_Device::getDeviceName()
{
	return deviceName;
}

std::string STI_Device::getServerName()
{
	if(serverConfigureFound)
		return serverName;
	else
		return "NOT FOUND";
}

std::string STI_Device::dataTransferErrorMsg()
{
	return dataTransferError.str();
}

attributeMap const * STI_Device::getAttributes()
{
	// Initialize to defaults the first time this is called
	if(attributes.empty())
	{
		// Call to pure virtual
		defineAttributes();
	}

	return attributes_ptr;
}

bool STI_Device::setAttribute(string key, string value)
{
	// Initialize to defaults the first time this is called
	if(attributes.empty())
		defineAttributes();	// Call to pure virtual

	if(attributes.empty())
		return false;	//There are no defined attributes

	attributeMap::iterator attrib = attributes.find(key);

	if(attrib == attributes.end())
	{
		return false;	// Attribute not found
	}
	else
	{
		// set the attribute
		attrib->second.setValue(value);
		return true;
	}
}

