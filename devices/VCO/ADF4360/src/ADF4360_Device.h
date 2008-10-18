/*! \file ADF4360_Device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Analog_Devices_VCO::ADF4360_Device
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

#ifndef ANALOG_DEVICES_ADF4360_DEVICE
#define ANALOG_DEVICES_ADF4360_DEVICE

#include "ADF4360.h"
#include "STI_Device.h"

namespace Analog_Devices_VCO {

class ADF4360_Device : public ADF4360, public STI_Device
{
public:

	ADF4360_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string IPAddress, 
		unsigned short ModuleNumber,
		unsigned int VCO_Address,
		unsigned int EtraxMemoryAddress,
		unsigned short ADF4360_model);
	~ADF4360_Device();

	//STI_Device functions
	bool deviceMain();
	
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	void defineChannels();
	bool writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event);
	bool readChannel(STI_Server_Device::TMeasurement & Measurement);

	std::string executeArgs(std::string args);
	std::string commandLineDeviceName() {return "adf4360";}
	void definePartnerDevices() {}; // requires none


};

}

#endif
