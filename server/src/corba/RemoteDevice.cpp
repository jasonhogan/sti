/*! \file RemoteDevice.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class RemoteDevice
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


#include "RemoteDevice.h"
#include "ORBManager.h"

#include <string>
using std::string;


#include <iostream>
using namespace std;


RemoteDevice::RemoteDevice(ORBManager* orb_manager, string name, const STI_Server_Device::TDevice& device, STI_Server_Device::TDeviceID& device_id) 
: name_l(name), orbManager(orb_manager)
{
	mounted = false;

	tDevice.deviceType = CORBA::string_dup(device.deviceType);
	tDevice.address = CORBA::string_dup(device.address);
	tDevice.moduleNum = device.moduleNum;

	tDeviceID.deviceID = CORBA::string_dup(device_id.deviceID);
	tDeviceID.deviceContext = CORBA::string_dup(device_id.deviceContext);
	tDeviceID.registered = device_id.registered;

	// Make Object Reference names
	string context = CORBA::string_dup(tDeviceID.deviceID);
	context.insert(0,"STI/Device/");
	
	configureObjectName = context + "Configure.Object";
	dataTransferObjectName = context + "DataTransfer.Object";

}

RemoteDevice::~RemoteDevice()
{
}

bool RemoteDevice::isMounted()
{
	return mounted;
}

void RemoteDevice::mount()
{
	// mount in a separate thread to avoid hanging 
	// the server during a failed mount
	omni_thread::create(acquireObjectReferencesWrapper, (void*)this, omni_thread::PRIORITY_LOW);

}

void RemoteDevice::unmount()
{
	// _release() references???
	mounted = false;
}

void RemoteDevice::acquireObjectReferencesWrapper(void* object)
{
	RemoteDevice* thisObject = (RemoteDevice*) object;
	thisObject->acquireObjectReferences();
}


void RemoteDevice::acquireObjectReferences()
{
	CORBA::Object_var obj;

	bool configureFound = false;
	bool dataTransferFound = false;

	while(!configureFound || !dataTransferFound)
	{
		obj = orbManager->getObjectReference(configureObjectName);
		ConfigureRef = STI_Server_Device::Configure::_narrow(obj);
		if( !CORBA::is_nil(ConfigureRef) )
			configureFound = true;

		obj = orbManager->getObjectReference(dataTransferObjectName);
		DataTransferRef = STI_Server_Device::DataTransfer::_narrow(obj);
		if( !CORBA::is_nil(DataTransferRef) )
			dataTransferFound = true;
	}

	// Check that the mount is good
	try{
		ConfigureRef->deviceType();
		cerr << "Mounted!!!" << endl;
		mounted = true;
	}
	catch(CORBA::TRANSIENT& ex) {		
		mounted = false;
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << " -- unable to contact the "
			<< "STI Server." << endl
			<< "Make sure the server is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		mounted = false;
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact the STI Server." << endl;
	}
}

std::string RemoteDevice::deviceName()
{
	return name_l;
}

STI_Server_Device::TDevice const * RemoteDevice::device() const
{
	return &tDevice;
}

STI_Server_Device::TDeviceID const * RemoteDevice::deviceID() const
{
	return &tDeviceID;
}