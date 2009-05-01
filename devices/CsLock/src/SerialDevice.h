#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

/*! \file SerialDevice.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class SerialDevice
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

//ASSUMES:  #define DIOLatch16 FALSE	
// set TRUE for original DIO Latch board (16 out/16 in), FALSE for new (24 out/8 in)


#include "SerialData.h"
#include <EtraxBus.h>

#include <string>
#include <vector>

#ifdef _MSC_VER
void _stdcall Out32(short PortAddress, short data);
#endif

class SerialDevice
{
public:

	SerialDevice(std::string deviceName, int address);
	virtual ~SerialDevice() {};

	//Access functions
	virtual std::string deviceType() const=0;
	virtual std::string deviceName() const;
	void setDeviceName(std::string deviceName);
	void setLPT1Address(int address);
	int LPT1Address();

	void setAddress(int address);
	int address();

	void writeData(SerialData data);
	
	std::vector<SerialData> SerBuf;

	int DSerialBufferBuild(unsigned long data, int numbits, 
							int location, bool bEnablePolarity);

	void DSerialSendData(int address, int length);


private:

	int address_l;
	std::string deviceName_l;

	int	LPT1Address_l;
	EtraxBus* bus;

};

#endif
