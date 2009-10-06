/*! \file SerialDevice.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class SerialDevice
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#include "SerialDevice.h"
#include <sstream>
#include <string>
#include <iostream>
using std::string;

SerialDevice::SerialDevice(std::string deviceName, int address)
{
	bus = new EtraxBus(0x9000006c);	//changed from 0x90000028 to 0x9000006c modified 4/29/2009 David Johnson

	setDeviceName(deviceName);
	setLPT1Address(0xEC00); //for the LAVA parallel-pci port //for legacy parallel ports that use the standard 0x378 address
	setAddress(address);	
}

std::string SerialDevice::deviceName() const
{
	return deviceName_l;
}

void SerialDevice::setDeviceName(string deviceName)
{
	deviceName_l=deviceName;
}

void SerialDevice::setLPT1Address(int address)
{
	LPT1Address_l = address;
}

int SerialDevice::LPT1Address()
{
	return LPT1Address_l;
}

void SerialDevice::setAddress(int address)
{
	address_l = address;
}

int SerialDevice::address()
{
	return address_l;
}

/*
string u32_to_str(uint32 num,int width=0)
{
	ostringstream o;
	o.fill('0');
	o.width(width);
	o << num;
	return o.str();
}

*/

void SerialDevice::writeData(SerialData data)
{
// this is the working command on the timing system 3/19/2008 DJ
//		system((std::string("bus -a 0x90000028 -w ")+u32_to_str(data.getValue())).c_str() );

#ifdef HAVE_LIBBUS
	bus->writeData( data.getValue() );
#else
	
#ifdef _MSC_VER
	Out32(LPT1Address(), data.getValue_EDCA());
#endif

#endif

}


//-------------------------------------------------------------------------
// DSerialBufferBuild() -- generate serial data and enable bit pattern
//
// The data parameter bits are converted from parallel to serial, reading 
// numbits worth of bits, MSB first.
// The location parameter allows the binary data to be added to any location 
// within a block of serial data.
// SerBuf is a vector of <SerialData>, a structure defined to access serial line 
// bits.
//
// Returns location+numbits to allow constructs like: 
//		location = DSerialBufferBuild(SBuf,data,numbits,location);
//
// Example: DSerialBuildBuffer(0xFA4, 12, 0);  Here 'data' is interpreted as 
// a 12 bit command word, and is copied into a SerialData array where the bits 
// are stored in sequential vector locations. 
// bEnablePolarity is used because the chip select polarity required differs.
// TRUE -> enable line high while writing.
// SerBuf[i] bits are sent in order of increasing i.  (Lower location number 
// means data is sent earlier.)


int SerialDevice::DSerialBufferBuild(unsigned long data, int numbits, 
									 int location, bool bEnablePolarity)
{
	int loc;
	int bshift;
	int stop = location + numbits;
	
	for (loc=location, bshift=numbits-1; loc < stop; loc++, bshift--)
	{
		//Send most significant bits first
		SerBuf[loc].bits.DIN = ((data & (1 << bshift)) >> bshift)==1 ? 1 : 0;

		SerBuf[loc].bits.EN = (bEnablePolarity) ? 1 : 0;
//		cerr << "bEnablePolarity: " << bEnablePolarity << " " << SerBuf[loc].bits.EN << endl;
	}
	
	return location + numbits;
}


//-------------------------------------------------------------------------
// DSerialSendData() -- send serial data to the specified address, generating 
//						clock signals as needed.
//
// Chips that require an inverted clock have an inverter chip that causes a delay,
// so it is still OK to change data and clock together as done in DSerialSendData().

void SerialDevice::DSerialSendData(int address, int length)
{
	int i; 						// index for serial output

	
	SerialData SData;


	//Raise the address line before address value set
	//Lower clock signal - lock the shift register
	SData.bits.ADDR		=	1;			
	SData.bits.CLK		=	0;			
//	SData.bits.EN		=	(SerBuf[0].bits.EN) ? 0 : 1;	// Enable off
	SData.bits.EN		=	SerBuf[0].bits.EN;				// Enable off
	SData.bits.DIN		=	0;

	writeData(SData);


	//***Send address***//	

	for(i=0; i <= 7; i++)
	{
		SData.bits.DIN = (address >> i) & 1;	// load address bit
		
		SData.bits.CLK = 0;						// lower clock
		writeData(SData);
		
		SData.bits.CLK = 1;						// raise clock
		writeData(SData);
	}

	//lower address line to enable address decode (multiplexer)
	SData.bits.ADDR		=	0;		
	SData.bits.CLK		=	0;	//added 5/28/03 (avoid transition on enable edge)
	SData.bits.DIN		=	0;
	SData.bits.EN		=	SerBuf[0].bits.EN;	// added 11/5/03
//	writeData(SData);  


	//***Send data***//
	for (i=0; i < length; i++)
	{
		SData.bits.DIN = SerBuf[i].bits.DIN;
		SData.bits.EN = SerBuf[i].bits.EN;
		
		SData.bits.CLK = 0;
		writeData(SData);

		SData.bits.CLK = 1;
		writeData(SData);
	}

	SData.bits.DIN = 0; //SMD added
	SData.bits.CLK = 0;
	writeData(SData);
	SData.bits.CLK = 0;
	writeData(SData);		// clock low for 1 Tcyc before chip select
	
	//Invert from value during writing, set by SerialBufferBuild()
	SData.bits.EN = (SData.bits.EN) ? 0 : 1;	//Set chip select lines after sending data
	writeData(SData);
	
	SData.bits.ADDR = 1;		// Address line raised to lock the latch  SMD uncomment
	writeData(SData);
	writeData(SData);			
	
//	cerr << "SerBuf.clear()" << endl;
//	SerBuf.clear();
	// Send Array
	//sendSerialBufferToCPU();
}
