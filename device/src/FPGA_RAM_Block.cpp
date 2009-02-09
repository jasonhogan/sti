/*! \file FPGA_RAM_Block.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class FPGA_RAM_Block
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

#include <FPGA_RAM_Block.h>

FPGA_RAM_Block::FPGA_RAM_Block(unsigned ModuleNumber) :
moduleNumber(ModuleNumber)
{
	//RAM chip is addresssed by a 26 bit wide bus from the etrax.
	//2^26 = 0x3ffffff bytes in RAM; we want 32 bit words
	RAM_Start_Addr_Base = 0x90001000;		//reserve the first 0x1000 addresses 
	RAM_End_Addr_Base   = 0x93ffffff;
	RAM_Size_Base_Bytes = (RAM_End_Addr_Base - RAM_Start_Addr_Base);
	RAM_Word_Size       = 4;		//4 bytes per 32 bit word
	RAM_Size_Base_Words = RAM_Size_Base_Bytes / RAM_Word_Size;

	setDefaultAddresses();
}

void FPGA_RAM_Block::setDefaultAddresses()
{
	//Default addresses for this module
	RAM_Size            = RAM_Size_Base_Words / 8;	//size for each module
	RAM_Start_Addr      = RAM_Start_Addr_Base + (moduleNumber * RAM_Size);
	RAM_End_Addr        = RAM_Start_Addr + RAM_Size - RAM_Word_Size;
}

bool FPGA_RAM_Block::setStartAddress(uInt32 address)
{
	if(RAM_Start_Addr_Base < address && 
		address < (RAM_End_Addr_Base - RAM_Word_Size))
	{
		RAM_Start_Addr = address;

		if(RAM_Start_Addr > (RAM_End_Addr - RAM_Word_Size))
			RAM_End_Addr = RAM_Start_Addr + RAM_Word_Size;
		return true;
	}
	return false;
}

bool FPGA_RAM_Block::setEndAddress(uInt32 address)
{
	if(RAM_Start_Addr_Base < address && address < RAM_End_Addr_Base)
	{
		RAM_End_Addr = address;

		if(RAM_End_Addr <= RAM_Start_Addr)
			RAM_Start_Addr = RAM_End_Addr - RAM_Word_Size;
		return true;
	}
	return false;
}

uInt32 FPGA_RAM_Block::getStartAddress() const
{
	return RAM_Start_Addr;
}

uInt32 FPGA_RAM_Block::getEndAddress() const
{
	return RAM_End_Addr;
}

uInt32 FPGA_RAM_Block::getSizeInWords() const
{
	return (RAM_End_Addr - RAM_Start_Addr + RAM_Word_Size);
}

uInt32 FPGA_RAM_Block::getAddress(uInt32 wordNumber) const
{
	//wordNumber can range from zero to the size of the block in words

	if( wordNumber <= getSizeInWords() )
		return (RAM_Start_Addr + wordNumber * RAM_Word_Size);
	else
		return RAM_End_Addr;
}

uInt32 FPGA_RAM_Block::getWrappedAddress(uInt32 wordNumber) const
{
	return getAddress( wordNumber % getSizeInWords() );		//wraps around
}

