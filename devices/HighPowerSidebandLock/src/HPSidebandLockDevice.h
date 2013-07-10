/*! \file HPSidebandLockDevice.h
 *  \author Jason Hogan
 *  \brief header file for HPSidebandLockDevice
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef HPSIDEBANDLOCKDEVICE_H
#define HPSIDEBANDLOCKDEVICE_H

#include <STI_Device_Adapter.h>

class HPSidebandLockDevice : public STI_Device_Adapter
{
public:

	HPSidebandLockDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber);
	~HPSidebandLockDevice();

	void defineChannels();
	void definePartnerDevices();

	void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);


private:
	
	class HPLockCallback;
	friend class HPLockCallback;
//	typedef boost::shared_ptr<HPLockCallback> HPLockCallback_ptr;


	class HPLockCallback : public MeasurementCallback
	{
	public:
		HPLockCallback(HPSidebandLockDevice* thisDevice) : _this(thisDevice) {}
		void handleResult(const STI::Types::TMeasurement& measurement);
		
	private:
		HPSidebandLockDevice* _this;
	};

	MeasurementCallback_ptr sensorCallback;

};

#endif
