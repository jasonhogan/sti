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

#include <string>

class FPGA_RAM_Block
{
public:

	FPGA_RAM_Block();
	FPGA_RAM_Block(unsigned ModuleNumber);
	~FPGA_RAM_Block();

	void setModuleNumber(unsigned ModuleNumber);

	void setDefaultAddresses();
	void setStartAddress(uInt32 address);
	void setEndAddress(uInt32 address);

	void increaseRAM_Block_SizeTo(uInt32 words);
	void setRAM_Block_Size(uInt32 words);

	bool isValidWordAddress(uInt32 address) const;
	uInt32 getNearestValidWordAddress(uInt32 address) const;
	
	uInt32 getStartAddress() const;
	uInt32 getEndAddress() const;
	uInt32 getSizeInWords() const;
	
	uInt32 getWrappedAddress(uInt32 wordNumber) const;
	uInt32 getAddress(uInt32 wordNumber) const;

	static uInt32 getRAM_Word_Size();
	static uInt32 getTotal_RAM_Size_Words();

	uInt32 startWord;
	uInt32 endWord;

	uInt32 addressToWord(uInt32 address) const;
	uInt32 wordToAddress(uInt32 word) const;
	void setStartWord(uInt32 word);
	void setEndWord(uInt32 word);

private:

	
	uInt32 moduleNumber;

	//FPGA External RAM constants
	static uInt32 RAM_Start_Addr_Base;
	static uInt32 RAM_End_Addr_Base;
	static uInt32 RAM_Size_Base_Bytes;
	static uInt32 RAM_Word_Size;
	static uInt32 RAM_Size_Base_Words;
	static uInt32 numberOfBytes(uInt32 startAddress, uInt32 endAddress);
	static uInt32 RAM_First_Memory_Address;
	static uInt32 RAM_Last_Memory_Address;

	//Addresses for this module
//	uInt32 RAM_Start_Addr;
//	uInt32 RAM_End_Addr;
};


#endif
