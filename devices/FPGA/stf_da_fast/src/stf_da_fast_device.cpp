/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_DA_FAST
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

#include "stf_da_fast_device.h"


STF_DA_FAST::STF_DA_FAST_Device::STF_DA_FAST_Device(
		ORBManager*		orb_manager, 
		std::string		DeviceName, 
		std::string		IPAddress,
		unsigned short	ModuleNumber,
		unsigned int EtraxMemoryAddress) :
da_fast(EtraxMemoryAddress),
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
}

STF_DA_FAST::STF_DA_FAST_Device::~STF_DA_FAST_Device()
{
}

bool STF_DA_FAST::STF_DA_FAST_Device::deviceMain(int argc, char **argv)
{
	return false;
}
	
void STF_DA_FAST::STF_DA_FAST_Device::defineAttributes()
{

	addAttribute("Output_Voltage", write_data(double output_voltage));
}

void STF_AD_FAST::STF_AD_FAST_Device::refreshAttributes()
{

	setAttribute("Output_Voltage", write_data(double output_voltage));

}

bool STF_DA_FAST::
STF_DA_FAST_Device::updateAttribute(std::string key, std::string value)
{
	double tempDouble;
	bool successDouble = stringToValue(value, tempDouble);

	bool success = false;

	if(key.compare("Output_Voltage") == 0 && successDouble)		success = write_data(tempDouble);

	return success;
}

void STF_DA_FAST::STF_DA_FAST_Device::defineChannels()
{
}

bool STF_DA_FAST::STF_DA_FAST_Device::
readChannel(STI_Server_Device::TMeasurement & Measurement)
{	
	return false;
}

bool STF_DA_FAST::STF_DA_FAST_Device::
writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event)
{
	//Event.data.number( write_data(something) ); //not sure how to implement this
	return true;
}

std::string STF_DA_FAST::STF_DA_FAST_Device::execute(int argc, char **argv)
{
	return "";
}

