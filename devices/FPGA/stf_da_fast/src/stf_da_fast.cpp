/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Source-file for the class FPGA_daughter_board::STF_da_fast
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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "stf_da_fast.h"

#include <iostream>

EtraxBus* STF_DA_FAST::da_fast::bus = NULL;

STF_DA_FAST::da_fast::da_fast(unsigned int EtraxMemoryAddress) 

{
	// Etrax Memory Address
	if(bus == NULL)
	{
		bus = new EtraxBus(EtraxMemoryAddress);
	}

}

STF_DA_FAST::da_fast::~da_fast()
{
}


bool STF_DA_FAST::da_fast::write_data(double output_voltage)
{
	if (output_voltage <= 10 && output_voltage >= -10)
	{
		double tempDouble = ((output_voltage+10.0)/20.0)*(65535.0);
		#ifdef HAVE_LIBBUS
			data = bus->writeData(tempDouble);
		#endif
		return true;
	}
	else
	{
		std::cerr << "Output values must be between -10 and 10 Volts" << std::endl;
		return false;
	}

}















