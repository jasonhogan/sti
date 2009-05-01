#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "SerialDevice.h"
#include "utils.h"

#include "stdlib.h"
#include <iostream>
#include <sstream>

using namespace std;

#ifdef HAVE_BUS_SPACE_H

bus_space_tag_t SerialDevice::tag = NULL;

#endif


SerialDevice::SerialDevice(string deviceName)
{
	setDeviceName(deviceName);
	setLPT1Address(0x378);
	setupAddress();
}

SerialDevice::SerialDevice(std::string deviceName, int address)
{
	setDeviceName(deviceName);
	setLPT1Address(0x378);
	setAddress(address);	
	setupAddress();
}

string SerialDevice::deviceName() const
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
	bus_space_write_4(tag, ioh, 0, data.getValue());
#else
	#ifndef _MSC_VER
		cerr << "libbus-error: writeData." << endl;
	#endif
#endif
#ifdef _MSC_VER
	Out32(LPT1Address(), data.getValue_ECDA());
#endif
//		cerr << deviceName() << ":wrote to bus" << endl;
}

void SerialDevice::setupAddress()
{
#ifdef HAVE_LIBBUS
	int ret;


	addr = 	0x9000006c; //changed from 0x90000028 to 0x9000006c modified 4/29/2009 David Johnson

	/* Grab bus space. */
	
	if(tag == NULL)
	{							
		if ((ret = bus_space_tag(&tag, "memio")) != 0)
			errx(1, "could not access physical memory, error %d", ret);

		if((ret=bus_space_map(tag, 0xb0014008, 0x00000004, 0, &ioh1)) != 0)
		{
		cerr << "Couldn't map bus space for speed register, error: "
		  << u32_to_str(ret) << endl;
		}

		//Change the number of wait cycles to 4 (modified 4/29/2009 David Johnson from 2 to 4 - for the modern FPGA wishbone code)
		old_speed=bus_space_read_4(tag, ioh1, 0);
		bus_space_write_4(tag, ioh1, 0, 4); //Changed the value to be written from 2 to 4 DJ 4/29/2009
	}

	if ((ret = bus_space_map(tag, addr, 4, 0, &ioh)))
			errx(1, "could not map bus space, error %d", ret);



#else

	cerr << "libbus-error: Initializing address." << endl;
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
