/*! \file EtraxBus.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class EtraxBus
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "EtraxBus.h"

#include <iostream>

EtraxBus::EtraxBus(uInt32 MemoryAddress)
{
	setMemoryAddress(MemoryAddress);
}


EtraxBus::~EtraxBus()
{
}


void EtraxBus::setupMemoryBus()
{
#ifdef HAVE_LIBBUS
	int ret;

	/* Grab bus space. */
	
	if(tag == NULL)
	{							
		if ((ret = bus_space_tag(&tag, "memio")) != 0)
		//	errx(1, "could not access physical memory, error %d", ret);
		std::cerr << "Could not access physical memory, error " << ret << std::endl;

		if((ret=bus_space_map(tag, 0xb0014008, 0x00000004, 0, &ioh1)) != 0)
		{
		std::cerr << "Couldn't map bus space for speed register, error: "
		  << u32_to_str(ret) << std::endl;
		}

		//Change the number of wait cycles to 2
		old_speed=bus_space_read_4(tag, ioh1, 0);
		bus_space_write_4(tag, ioh1, 0, 2);
	}

	if ((ret = bus_space_map(tag, memoryAddress, 4, 0, &ioh)))
	//		errx(1, "could not map bus space, error %d", ret);
		std::cerr << "Could not map bus space, error " << ret << std::endl;
#endif
}

void EtraxBus::writeData(uInt32 data)
{
#ifdef HAVE_LIBBUS
	bus_space_write_4(tag, ioh, 0, data);
#endif
}

uInt32 EtraxBus::readData()
{
	uInt32 value = 0;

	#ifdef HAVE_LIBBUS
		value = bus_space_read_4(tag, ioh, 0);
	#endif
	
	return value;
}


void EtraxBus::setMemoryAddress(uInt32 MemoryAddress)
{
	memoryAddress = MemoryAddress;
	setupMemoryBus();
}


uInt32 EtraxBus::getMemoryAddress() const
{
	return memoryAddress;
}

