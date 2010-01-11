/*! \file SerialData.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Analog_Devices_VCO::SerialData
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

#include "SerialData.h"

Analog_Devices_VCO::SerialData::SerialData() :
CLOCK(0),
DATA(0),
LE(0)
{
}

Analog_Devices_VCO::SerialData::SerialData(bool clock, bool data, bool le) :
CLOCK(clock),
DATA(data),
LE(le)
{
}

Analog_Devices_VCO::SerialData::~SerialData()
{
}

unsigned int Analog_Devices_VCO::SerialData::getParallelData() const
{
	unsigned int data;

	// PC parallel port data (DB25)
	// See ADF4360 evaluation board data sheet.
	data = 0x01 * CLOCK +		// DB25 pin 2
		   0x02 * DATA  +		// DB25 pin 3
		   0x04 * LE;			// DB25 pin 4

	// Note: DB25 pin 2 corresponds to parallel port Bit Data 0
	// and is addressible at the parallel port base address (usually 0x378).

	return data;
}


unsigned int Analog_Devices_VCO::SerialData::getData(unsigned int vcoAddress) const
{
	unsigned int data;

	if(vcoAddress < 6)
	{
		data = 0x01 * CLOCK +					// DB15 pin 0
			   0x02 * DATA  +					// DB15 pin 1
			   (0x04 << vcoAddress) * LE;		// DB15 pin (2 + vcoAddress)
	}
	else
	{
		//invalid address
		data = 0;
	}

	return data;

}

bool Analog_Devices_VCO::SerialData::getPin(unsigned pin, unsigned int vcoAddress)
{
	if(vcoAddress < 6)
	{
		if(pin == 0)                      // DB15 pin 0
			return CLOCK;
		else if(pin == 1)                 // DB15 pin 1
			return DATA;
		else if(pin == (2 + vcoAddress))  // DB15 pin (2 + vcoAddress)
			return LE;
		else
			return false;
	}
	else
	{
		//invalid address
		return false;
	}
}