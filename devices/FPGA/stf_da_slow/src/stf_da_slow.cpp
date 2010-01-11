/*! \file stf_da_slow.cpp
 *  \author Alexander Franz Sugarbaker
 *  \brief Source-file for the class STF_DA_SLOW::STF_DA_SLOW
 *  \section license License
 *
 *  Copyright (C) 2008 Alex Sugarbaker <sugarbak@stanford.edu>\n
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "stf_da_slow.h"

EtraxBus* STF_DA_SLOW::da_slow::bus = NULL;

STF_DA_SLOW::da_slow::da_slow(unsigned int address) 
{
	// Etrax Memory Address
	if(bus == NULL) {
		bus = new EtraxBus(address);
	}
}

STF_DA_SLOW::da_slow::~da_slow() 
{
	// should probably delete the object???
}

//set_value sets channel (0-39) to voltage (-10V to 10V)
//value has 14 bits to cover -10V to +10V, all 1s = -10V
//bits has 10 bits (0 = least significant bit in "bits"):
//		0 active
//		1-6 channel
//		7 register 0 (see AD5380 datasheet)
//		8 register 1
//		9 reset
//combined concatenates bits and value with
//		bits in the more significant bits
//combined should be written to etrax bus address 0x90000054
//the board seems mis-wired - in groups of 4, the 2nd and 3rd
//		channels are always flipped (ie 1<->2, 5<->6, etc.)
bool STF_DA_SLOW::da_slow::set_value(unsigned int channel, double voltage) 
{
	value = static_cast<int>((-voltage + 10.)*16383./20.);

	//corrects channel switchup
	if (channel%4 == 1) channel++;
	else if (channel%4 == 2) channel--;
	//std::cout << "Channel " << channel << std::endl;

	bits = (channel << 1) + 1 + (1 << 7) + (1 << 8);
	combined = (bits << 14) + value;

	//prints out the full bit string to be written to the bus
	std::bitset<24> combined_bitset (combined);
	std::cout << "Writing " << combined_bitset << std::endl;


	#ifdef HAVE_LIBBUS
		bus->writeData(combined);
		return true;
	#else
	std::cerr << "libbus-error: writeData." << std::endl;
		return false;
	#endif
}

//reset sets all of the outputs to 0V simultaneously
//it uses the CLR special function of the AD5380
bool STF_DA_SLOW::da_slow::reset() 
{
	//write the CLR code for 0V
	value = 16383/2;
	bits = (1 << 2) + 1;
	combined = (bits << 14) + value;
	#ifdef HAVE_LIBBUS
		bus->writeData(combined);
	#else
		std::cerr << "libbus-error: writeData." << std::endl;
		return false;
	#endif

	//send the CLR command
	bits = (1 << 3) + 1;
	combined = bits << 14;
	#ifdef HAVE_LIBBUS
		bus->writeData(combined);
		return true;
	#else
		std::cerr << "libbus-error: writeData." << std::endl;
		return false;
	#endif
}

