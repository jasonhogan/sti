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


RemoteDevice::RemoteDevice(ORBManager* orb_manager, 
						   string		name, 
						   const STI_Server_Device::TDevice &	device, 
						   const STI_Server_Device::TDeviceID & device_id) 
: name_l(name), orbManager(orb_manager)
{
	active = false;

	tDevice.deviceType = CORBA::string_dup(device.deviceType);
	tDevice.address = CORBA::string_dup(device.address);
	tDevice.moduleNum = device.moduleNum;

	cerr << "**** RemoteDevice Context: " << CORBA::string_dup(device_id.deviceContext) << endl;

	tDeviceID.deviceID = CORBA::string_dup(device_id.deviceID);
	tDeviceID.deviceContext = CORBA::string_dup(device_id.deviceContext);
	tDeviceID.registered = device_id.registered;

	// Make Object Reference names
	string context = tDeviceID.deviceContext;
	context.insert(0,"STI/Device/");
	
	configureObjectName = context + "Configure.Object";
	timeCriticalObjectName = context + "timeCriticalData.Object";
	streamingObjectName = context + "streamingData.Object";
	
	cerr << "RemoteDevice: " << configureObjectName << endl;

}

RemoteDevice::~RemoteDevice()
{
}

bool RemoteDevice::isActive()
{
	bool servantsAlive = false;

	try{
		// Just look for one servant
		ConfigureRef->deviceType();
		cerr << "Active!!!" << endl;
		servantsAlive = true;
	}
	catch(CORBA::TRANSIENT& ex) {		
		servantsAlive = false;
		cerr << "Caught system exception CORBA::" 
			<< ex._name() << endl << "Unable to contact the "
			<< "Device '" << deviceName() << "'." << endl
			<< "Make sure the device is running and that omniORB is "
			<< "configured correctly." << endl;
	}
	catch(CORBA::SystemException& ex) {
		servantsAlive = false;
		cerr << "Caught a CORBA::" << ex._name()
			<< " while trying to contact Device '" << deviceName() << "'." << endl;
	}

	return servantsAlive;
}

void RemoteDevice::activate()
{
	// mount in a separate thread to avoid hanging 
	// the server during a failed mount
	omni_thread::create(acquireObjectReferencesWrapper, (void*)this, omni_thread::PRIORITY_LOW);
}

void RemoteDevice::deactivate()
{
	// _release() references???
	active = false;
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
	bool timeCriticalDataFound = false;
	bool streamingDataFound = false;
	int timeout = 10;	// try 10 times

	while( (!configureFound || !timeCriticalDataFound || !streamingDataFound)
		&& (--timeout > 0) )
	{
		obj = orbManager->getObjectReference(configureObjectName);
		ConfigureRef = STI_Server_Device::Configure::_narrow(obj);
		if( !CORBA::is_nil(ConfigureRef) )
			configureFound = true;

		obj = orbManager->getObjectReference(timeCriticalObjectName);
		timeCriticalDataRef = STI_Server_Device::DataTransfer::_narrow(obj);
		if( !CORBA::is_nil(timeCriticalDataRef) )
			timeCriticalDataFound = true;
		
		obj = orbManager->getObjectReference(streamingObjectName);
		streamingDataRef = STI_Server_Device::DataTransfer::_narrow(obj);
		if( !CORBA::is_nil(streamingDataRef) )
			streamingDataFound = true;
	}

	active = isActive();

	cerr << "Done with RemoteDevice::acquireObjectReferences()" << endl;
}

bool RemoteDevice::addChannel(const STI_Server_Device::TDeviceChannel & tChannel)
{
	if(isUnique(tChannel))
	{
		channels.push_back(tChannel);
		return true;
	}
	else
	{
		cerr << "Error: Duplicate channel in device '" 
			<< deviceName() << "'." << endl;
		return false;
	}
}

bool RemoteDevice::isUnique(const STI_Server_Device::TDeviceChannel & tChannel)
{
	bool unique = true;
	int i;
	for(i = 0; i < channels.size(); i++)
	{
		if(channels[i].channel == tChannel.channel)	//same channel not allowed
		{
			unique = false;
		}
	}
	return unique;
}

std::string RemoteDevice::deviceName()
{
	return name_l;
}

STI_Server_Device::TDevice const * RemoteDevice::device() const
{
	return &tDevice;
}

STI_Server_Device::TDeviceID * RemoteDevice::deviceID()
{
	return &tDeviceID;
}


void RemoteDevice::printChannels()
{
	for(int i=0; i < channels.size(); i++)
	{
		cerr << "Channel " << i << ": " << channels[i].channel << endl;
	}
}
