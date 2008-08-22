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
	// see ADF4360 evaluation board data sheet
	data = 0x01 * 0 +			// pin 1 (unused)
		   0x02 * CLOCK +		// pin 2
		   0x04 * DATA  +		// pin 3
		   0x08 * LE;			// pin 4

	return data;
}
