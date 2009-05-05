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

#include <string>
#include <iostream>

#include <ORBManager.h>
#include "stf_dds_device.h"

using namespace std;


ORBManager* orbManager;

int main(int argc, char* argv[])
{
//	orbManager = new ORBManager(argc, argv);    

//	unsigned short module = 3;

//	unsigned int memAddress = 0x90000038;
//	string ipAddress = "ep-timing1.stanford.edu";

	//"analog_in_ch3" on timing board ch3
//	STF_DDS::dds_Device DDS_ch0(
//		orbManager, "DDS_ch0", ipAddress, module, memAddress);

//	orbManager->run();

//	STF_DDS::dds dds_ch0(0x90000066);

//	dds_ch0.setFrequency(STF_DDS::one, 100e6, 100);
//	dds_ch0.setAmplitude(STF_DDS::one, 300, 200);
//	dds_ch0.setPhase(STF_DDS::one, 100,300);

	return 0;
}

