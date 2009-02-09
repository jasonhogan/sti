/*! \file FPGA_RAM_Block.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class FPGA_RAM_Block
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

#ifndef FPGA_RAM_BLOCK_H
#define FPGA_RAM_BLOCK_H

#include <types.h>

class FPGA_RAM_Block
{
public:

	FPGA_RAM_Block(unsigned ModuleNumber);
	~FPGA_RAM_Block();

	void setDefaultAddresses();
	bool setStartAddress(uInt32 address);
	bool setEndAddress(uInt32 address);
	
	uInt32 getStartAddress() const;
	uInt32 getEndAddress() const;
	uInt32 getSizeInWords() const;
	
	uInt32 getWrappedAddress(uInt32 wordNumber) const;
	uInt32 getAddress(uInt32 wordNumber) const;

private:

	uInt32 moduleNumber;

	//FPGA External RAM constants
	uInt32 RAM_Start_Addr_Base;
	uInt32 RAM_End_Addr_Base;
	uInt32 RAM_Size_Base_Bytes;
	uInt32 RAM_Word_Size;
	uInt32 RAM_Size_Base_Words;

	//Default addresses for this module
	uInt32 RAM_Size;
	uInt32 RAM_Start_Addr;
	uInt32 RAM_End_Addr;
};


#endif
