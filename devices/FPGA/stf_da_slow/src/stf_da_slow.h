/*! \file stf_da_slow.h
 *  \author Alexander Franz Sugarbaker
 *  \brief Include-file for the class STF_DA_SLOW::STF_DA_SLOW
 *  \section license License
 *
 *  Copyright (C) 2008 Alex Sugarbaker <sugarbak@stanford.edu>
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

#ifndef STF_DA_SLOW_H
#define STF_DA_SLOW_H

#include <EtraxBus.h>
#include <bitset>
#include <iostream>
//#include <omnithread.h>
//#include <vector>
//#include <string>

namespace STF_DA_SLOW {

	class da_slow {

	public:
		da_slow(unsigned int address);
		~da_slow();

		bool set_value(unsigned int channel, double voltage);
		//bool initialize();
		bool reset();

	private:
		unsigned int value;
		unsigned int bits;
		unsigned int combined;

		//For writing data directly to the Etrax memory bus
		static EtraxBus* bus;	//only one EtraxBus allowed per memory address

	};

}

#endif

