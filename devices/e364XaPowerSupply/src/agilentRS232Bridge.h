/*! \file agilentRS232Bridge.h
 *  \author David M.S. Johnson
 *  \brief header file
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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


#ifndef AGILENTRS232BRIDGE_H
#define AGILENTRS232BRIDGE_H

#include "visa.h"
// also requires visa32.lib to be appropriately linked. May have to change project settings in order for the linker to find this file
//#include <STI_Device.h>

#include <string>


class agilentRS232Bridge 
	{ 
	public:
		
		agilentRS232Bridge(unsigned short comPort); //constructor
		~agilentRS232Bridge(); //constructor

		
		bool queryDevice(std::string commandString, std::string& strBuffer);
		bool commandDevice(std::string commandString);
		
	protected:
		char buffer[256];   // Make the buffer long enough for longest expected read. 
		

	private:

		void openPort(unsigned short comPort);
		void closePort();
		bool checkError(std::string errorMessage);

		ViSession defaultRM; // Resource manager id - this is just a type-deffed unsigned 32 bit number
		ViSession power_supply; // Identifies power supply - this is just a type-deffed unsigned 32 bit number

		long ErrorStatus; /* VISA Error code */
		bool portOpen;


	};


#endif
