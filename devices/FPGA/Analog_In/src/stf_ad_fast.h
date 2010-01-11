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

#ifndef STF_AD_FAST_H
#define STF_AD_FAST_H

#include <EtraxBus.h>
#include <omnithread.h>
#include <vector>
#include <string>

namespace STF_AD_FAST {

class ad_fast {

public:

	ad_fast(unsigned int EtraxMemoryAddress);
	~ad_fast();

	double read_data();

private:

	int data;

	//For writing data directly to the Etrax memory bus
	static EtraxBus *bus;	//only one EtraxBus allowed per memory address

};

}

#endif

