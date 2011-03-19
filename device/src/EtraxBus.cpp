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
#include <utils.h>
#include <iostream>
using namespace std;

EtraxBus::EtraxBus(uInt32 MemoryAddress, uInt32 NumberOfWords)
{
	setMemoryAddress(MemoryAddress, NumberOfWords);

	readMutex = new omni_mutex();
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
		  << STI::Utils::valueToString(ret) << std::endl;
		}

		//Change the number of wait cycles to 4 (modified 4/29/2009 David Johnson from 2 to 4 - for the modern FPGA wishbone code)
		old_speed=bus_space_read_4(tag, ioh1, 0);
		bus_space_write_4(tag, ioh1, 0, 4);
	}

//cout <<  "Mapping bus space.  Number Of Words = " << numberOfWords << endl;
//cin >> numberOfWords;
//cout <<  "Address != 0x90000008 " << memoryAddress << endl;
//cin >> memoryAddress;


	if ( (ret = bus_space_map(tag, memoryAddress, 4 * numberOfWords, 0, &ioh)) )
	{
	//		errx(1, "could not map bus space, error %d", ret);
		std::cerr << "Could not map bus space, error " << ret << std::endl;
	}

//cout << "success!" << endl;
#endif

}

void EtraxBus::writeDataToAddress(uInt32 data, uInt32 address)
{
	uInt32 offset = 0;
	if(address >= memoryAddress)
	{
		offset = address - memoryAddress;
	}
	else
		std::cout << "Error in EtraxBus::writeDataToAddress().  Address '" 
		<< address << "' is below range '" << memoryAddress << "'." << endl;

	if(offset < 4 * numberOfWords)
	{
		writeData(data, offset);
	}
	else
		std::cout << "Error in EtraxBus::writeDataToAddress().  Address '" 
		<< address << "' is above range '" << (memoryAddress + 4 * numberOfWords) << "'." << endl;
}

void EtraxBus::writeData(uInt32 data, uInt32 addressOffset)
{
#ifdef HAVE_LIBBUS

//cout << "Writing to offset: " << addressOffset << ". Mapped size = " << numberOfWords << endl;
//cin >> addressOffset;
	bus_space_write_4(tag, ioh, addressOffset, data);
//	bus_space_barrier(tag, ioh, addressOffset, 4, BUS_SPACE_BARRIER_WRITE_BEFORE_WRITE);
#endif
}

uInt32 EtraxBus::readDataFromAddress(uInt32 address)
{
	uInt32 offset = 0;
	if(address >= memoryAddress)
	{
		offset = address - memoryAddress;
	}
	else
		std::cout << "Error in EtraxBus::readDataFromAddress().  Address '" 
		<< address << "' is below range '" << memoryAddress << "'." << endl;

	if(offset < 4 * numberOfWords)
	{
		return readData(offset);
	}
	else
		std::cout << "Error in EtraxBus::readDataFromAddress().  Address '" 
		<< address << "' is above range '" << (memoryAddress + 4 * numberOfWords) << "'." << endl;
	return 0;
}

uInt32 EtraxBus::readData(uInt32 addressOffset)
{
	uInt32 value = 0;

	#ifdef HAVE_LIBBUS
//cout << "Reading from offset: " << addressOffset << ". Mapped size = " << numberOfWords << endl;
//cin >> addressOffset;

	readMutex->lock();
	{
		value = bus_space_read_4(tag, ioh, addressOffset);
	}
	readMutex->unlock();

		//bus_space_barrier(space, handle, offset, length, flags);
//		bus_space_barrier(tag, ioh, addressOffset, 4, BUS_SPACE_BARRIER_READ_BEFORE_READ);
	#endif
	
	return value;
}


void EtraxBus::setMemoryAddress(uInt32 MemoryAddress, uInt32 NumberOfWords)
{
//cout << "Memory address = " << MemoryAddress << endl;
	memoryAddress = MemoryAddress;
	numberOfWords = NumberOfWords;
	setupMemoryBus();
}


uInt32 EtraxBus::getMemoryAddress() const
{
	return memoryAddress;
}

