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

#ifndef STF_DDS_H
#define STF_DDS_H

#include <EtraxBus.h>
#include <omnithread.h>
#include <vector>
#include <string>
#include <types.h>
#include <utils.h>

namespace STF_DDS {

	enum sweep_type{amplitude, phase, frequency};
	enum which_channel {one, two, three, four, all};
	enum which_mode {one_bit, four_bit};

class dds {



public:

	dds(unsigned int EtraxMemoryAddress);
	~dds();

	double read_data();
	bool write_data(unsigned int data);

	bool setChannel(which_channel channel, Int64 time);
	bool setFrequency(which_channel channel, uInt32 frequency, Int64 time);
	bool setAmplitude(which_channel channel, uInt32 amplitude, Int64 time);
	bool setPhase(which_channel channel, uInt32 phase, Int64 time);
	bool setSweep(which_channel channel, sweep_type sweep, uInt32 startPoint, uInt32 endPoint, int delta, Int64 time);
	bool masterReset();

private:

	//For writing data directly to the Etrax memory bus
	static EtraxBus *bus;	//only one EtraxBus allowed per memory address

	int operationTime; //given the current mode, what is the max time for an operation to complete

	std::vector<uInt8> addressList;
	std::vector<uInt32> commandList;
	std::vector<Int64> timeList;

	which_channel currentChannel; //stores which channel is currently set in the DDS. Don't change if you don't have to.
	which_mode currentMode; //stores the current serial interface mode

	bool setTime(Int64 time);

};

}

#endif

