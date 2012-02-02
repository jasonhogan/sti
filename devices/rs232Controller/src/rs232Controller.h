/*! \file rs232Controller.h
 *  \author David M.S. Johnson
 *  \brief header file
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  This serial communications is based on code from: Ramon de Klein (Ramon.de.Klein@ict.nl)
 *  Code was downloaded from: http://www.codeproject.com/KB/system/serial.aspx
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


#ifndef rs232Controller_H
#define rs232Controller_H

#if defined(_MSC_VER)
	//#include "serialport.h"
#endif

#include <string>
//#define STRICT
#include <tchar.h>
#include <windows.h>
#include "Serial.h"
#include <vector>

class rs232Controller 
	{ 
	public:
		
		rs232Controller(std::string comportString = "COM0", unsigned int baudRate = 9600, unsigned int dataBits = 8, std::string parity = "None", unsigned int stopBits = 1); //constructor; ADDED DEFAULTS 12/20/11
		~rs232Controller(); //constructor
		std::string queryDevice(std::string commandString, int sleepTimeMS = 100, int readLength = 30);
		//std::vector <int> binaryQueryDevice(std::string commandString);
		std::vector <int> continuousBinaryQueryDevice();
		void commandDevice(std::string commandString);

		std::vector <int> binaryQueryDevice(std::string commandString, int sleepTimeMS = 100);

		int ShowError (int error, std::string errorMessage);
		bool initialized;
		
	protected:
		char buffer[256];   // Make the buffer long enough for longest expected read. 
		

	private:

		//CSerialPort * port;

		CSerial * serial;
		int    lastErrorCode;

		CSerial::EBaudrate getBaudRate(unsigned int baudRate);
		CSerial::EDataBits getDataBits(unsigned int dataBits);
		CSerial::EParity   getParity(std::string parity);
		CSerial::EStopBits getStopBits(unsigned int stopBits);


	};


#endif
