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
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
//RAM chip is addresssed by a 26 bit wide bus from the etrax.
//2^26 = 0x3ffffff bytes in RAM; we want 32 bit words
//uInt32 FPGA_RAM_Block::RAM_First_Memory_Address = 0x00001000;		//reserve the first 0x1000 addresses 
//uInt32 FPGA_RAM_Block::RAM_Last_Memory_Address = 0x03ffffff;
uInt32 FPGA_RAM_Block::RAM_First_Memory_Address = 0x90030000;		//reserve the first 0x1000 addresses 
uInt32 FPGA_RAM_Block::RAM_Last_Memory_Address  = 0x90037fff;
//uInt32 FPGA_RAM_Block::RAM_First_Memory_Address = 0;
//uInt32 FPGA_RAM_Block::RAM_Last_Memory_Address = 80;

uInt32 FPGA_RAM_Block::RAM_Word_Size       = 4;		//4 bytes per 32 bit word
uInt32 FPGA_RAM_Block::RAM_Size_Base_Bytes = (RAM_Last_Memory_Address - RAM_First_Memory_Address + 1);	//inclusive
uInt32 FPGA_RAM_Block::RAM_Size_Base_Words = RAM_Size_Base_Bytes / RAM_Word_Size;

uInt32 FPGA_RAM_Block::RAM_Start_Addr_Base = RAM_First_Memory_Address;
uInt32 FPGA_RAM_Block::RAM_End_Addr_Base   = RAM_Last_Memory_Address;


//uInt32 FPGA_RAM_Block::RAM_Size_Base_Bytes = 4*80;

FPGA_RAM_Block::FPGA_RAM_Block()
{
}


FPGA_RAM_Block::FPGA_RAM_Block(unsigned ModuleNumber)
{
	setModuleNumber(ModuleNumber);
}


FPGA_RAM_Block::~FPGA_RAM_Block()
{
}

void FPGA_RAM_Block::setModuleNumber(unsigned ModuleNumber)
{
	moduleNumber = ModuleNumber;

	uInt32 sizeBytes = RAM_Size_Base_Words * RAM_Word_Size;	//takes care of rounding
	RAM_End_Addr_Base = RAM_Start_Addr_Base + sizeBytes - RAM_Word_Size;	//largest valid word address

	setDefaultAddresses();
}


void FPGA_RAM_Block::setDefaultAddresses()
{
	//Default addresses for this module
	uInt32 RAM_Size_Words = RAM_Size_Base_Words / 8;	//size for each module in words

//	cout << "RAM_Size_Words MODIFIED: " << RAM_Size_Words << endl;

	startWord = moduleNumber * RAM_Size_Words;
	endWord   = startWord + (RAM_Size_Words - 1);

//	RAM_Start_Addr      = RAM_Start_Addr_Base + (moduleNumber * RAM_Size_Words * RAM_Word_Size);
//	RAM_End_Addr        = RAM_Start_Addr + (RAM_Size_Words - 1) * RAM_Word_Size;
}

uInt32 FPGA_RAM_Block::numberOfBytes(uInt32 startAddress, uInt32 endAddress)
{
	return ( (startAddress < endAddress) ? (endAddress - startAddress + 1) : 0 );
}

bool FPGA_RAM_Block::isValidWordAddress(uInt32 address) const
{
	//check that the address points to the beginning a word

	bool valid = false;

	if(RAM_Start_Addr_Base <= address && address <= RAM_End_Addr_Base)
	{
		valid = ( address - RAM_Start_Addr_Base) % RAM_Word_Size == 0;
	}
	return valid;

	////lowest valid address is module dependent.  Allow at least a start and end address for all modules
	//if(RAM_Start_Addr_Base + (moduleNumber * 2 * RAM_Word_Size) <= address && 
	//	address <= (RAM_End_Addr_Base - ((7 - moduleNumber) * 2 * RAM_Word_Size) + RAM_Word_Size) )
	//{
	//	//address must be a mulitple of the word size away from the start
	//	valid = ( address - RAM_Start_Addr_Base) % RAM_Word_Size == 0;
	//}
	//return valid;
}

uInt32 FPGA_RAM_Block::getNearestValidWordAddress(uInt32 address) const
{
	if(isValidWordAddress(address))
		return address;
	if(address < RAM_Start_Addr_Base)
		return RAM_Start_Addr_Base;
	if(address > RAM_End_Addr_Base)
		return RAM_End_Addr_Base;

	return address - ( (address - RAM_Start_Addr_Base) % RAM_Word_Size );
}


uInt32 FPGA_RAM_Block::addressToWord(uInt32 address) const
{
	return (getNearestValidWordAddress(address) - RAM_Start_Addr_Base) / RAM_Word_Size;
}

uInt32 FPGA_RAM_Block::wordToAddress(uInt32 word) const
{
	return getNearestValidWordAddress( 
		RAM_Start_Addr_Base + (word * RAM_Word_Size) );
}




void FPGA_RAM_Block::setStartWord(uInt32 word)
{
	if(word < moduleNumber)
		startWord = moduleNumber;
	else if(word > (RAM_Size_Base_Words - 1) - (7 - moduleNumber))
		startWord = (RAM_Size_Base_Words - 1) - (7 - moduleNumber);
	else
		startWord = word;

	if(startWord > endWord)
		endWord = startWord;
}

void FPGA_RAM_Block::setEndWord(uInt32 word)
{
	if(word < moduleNumber)
		endWord = moduleNumber;
	else if(word > (RAM_Size_Base_Words - 1) - (7 - moduleNumber))
		endWord = (RAM_Size_Base_Words - 1) - (7 - moduleNumber);
	else
		endWord = word;

	if(startWord > endWord)
		startWord = endWord;
}

void FPGA_RAM_Block::setStartAddress(uInt32 address)
{
	setStartWord( addressToWord( address ) );

	//if(address < RAM_Word_Size * moduleNumber)
	//RAM_Start_Addr



	////valid start address is up to one away from the end
	//if(isValidWordAddress(address) && address < RAM_End_Addr_Base)
	//{
	//	RAM_Start_Addr = address;

	//	if(RAM_End_Addr <= RAM_Start_Addr)
	//		RAM_End_Addr = RAM_Start_Addr + RAM_Word_Size;
	//	return true;
	//}
	//return false;
}

//trys to increase the size of the RAM block to words if it's not already larger than this.
void FPGA_RAM_Block::increaseRAM_Block_SizeTo(uInt32 words)
{
	if(getSizeInWords() < words)
		setRAM_Block_Size(words);
}

void FPGA_RAM_Block::setRAM_Block_Size(uInt32 words)
{
	if(words > 0)
		setEndWord(startWord + words - 1);
	else
		setEndWord(startWord);



	//uInt32 tempEnd;
	//tempEnd = RAM_Start_Addr + ((words - 1) * RAM_Word_Size);

	//if( words > 1 && isValidWordAddress(tempEnd) && words <= getMaxBlockSizeWords() )
	//	return setEndAddress(tempEnd);
	//else
	//	return false;
}


void FPGA_RAM_Block::setEndAddress(uInt32 address)
{
	setEndWord(	addressToWord( address ) );

	//if( isValidWordAddress(address) && address > RAM_Start_Addr_Base)
	//{
	//	RAM_End_Addr = address;
	//	
	//	if(RAM_End_Addr <= RAM_Start_Addr)
	//		return setStartAddress(RAM_End_Addr - RAM_Word_Size);
	//	else
	//		return true;
	//}
	//return false;
}

uInt32 FPGA_RAM_Block::getStartAddress() const
{

	return wordToAddress(startWord);
	//	return RAM_Start_Addr;
}

uInt32 FPGA_RAM_Block::getEndAddress() const
{
	return wordToAddress(endWord);
//	return RAM_End_Addr;
}

uInt32 FPGA_RAM_Block::getSizeInWords() const
{

	return (endWord - startWord) + 1;
	//	return (numberOfBytes(RAM_Start_Addr, RAM_End_Addr) / RAM_Word_Size) + 1;
}

uInt32 FPGA_RAM_Block::getAddress(uInt32 wordNumber) const
{
	//gets the address of wordNumber words away from the starting word of this block
	return wordToAddress( startWord + wordNumber );

	////wordNumber can range from zero to the size of the block in words
	//if( wordNumber <= getSizeInWords() )
	//	return (RAM_Start_Addr + wordNumber * RAM_Word_Size);
	//else
	//	return RAM_End_Addr;
}

uInt32 FPGA_RAM_Block::getWrappedAddress(uInt32 wordNumber) const
{
	return getAddress( wordNumber % getSizeInWords() );		//wraps around
}

uInt32 FPGA_RAM_Block::getRAM_Word_Size()
{
	return RAM_Word_Size;
}

uInt32 FPGA_RAM_Block::getTotal_RAM_Size_Words()
{
	return RAM_Size_Base_Words;
}


