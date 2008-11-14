/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_AD_FAST
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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

#ifndef STF_DDS_DEVICE_H
#define STF_DDS_DEVICE_H

#include "stf_dds.h"
#include "STI_Device.h"

namespace STF_DDS {

class dds_Device : public dds, public STI_Device
{
public:

	dds_Device(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string IPAddress,
		unsigned short ModuleNumber,
		unsigned int EtraxMemoryAddress);
	~dds_Device();

	//STI_Device functions

	// Device setup
	bool deviceMain(int argc, char **argv);

	// Device Attributes
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();
	bool writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event);
	bool readChannel(STI_Server_Device::TMeasurement & Measurement);

	// Device Command line interface setup
	std::string executeArgs(std::string args);
	std::string commandLineDeviceName() {return "stf_ad_fast";}
	void definePartnerDevices() {}; // requires none


};

}

#endif
