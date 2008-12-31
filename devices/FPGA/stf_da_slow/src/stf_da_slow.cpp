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

STF_DA_SLOW::da_slow::da_slow(unsigned int address) {
	// Etrax Memory Address
	if(bus == NULL) {
		bus = new EtraxBus(address);
	}
}

STF_DA_SLOW::da_slow::~da_slow() {
	// should probably delete the object???
}

// sets "channel" (0-39) to "voltage" (-10V to 10V)
bool STF_DA_SLOW::da_slow::set_value(unsigned int channel, double voltage) {
	value = int((voltage + 10.)*16383./20.); //14 bits from -10V to +10V
	bits = (channel << 1) + 1;	//10 bits total
								//bit 0 active (TRUE)
								//bits 1-6 channel
								//bits 7-8 register 0-1 (both FALSE), 
								//bit 9 reset (FALSE)
	combined = (bits << 14) + value;

	std::bitset<10> bits_bitset (bits);
	std::bitset<14> value_bitset (value);
	std::bitset<24> combined_bitset (combined);
	std::cout << combined_bitset << std::endl;

	#ifdef HAVE_LIBBUS
		bus->writeData(combined);
		return true;
	#else
	std::cerr << "libbus-error: writeData." << std::endl;
		return false;
	#endif
}