/*! \file visa32Controller.h
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


#ifndef visa32Controller_H
#define visa32Controller_H

#if defined(_MSC_VER)
	//#include "serialport.h"
#endif


#include "visa.h"


#include <string>
//#define STRICT
#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>


class visa32Controller 
	{ 
	public:
		
		visa32Controller(std::string comportString); //constructor
		~visa32Controller(); //constructor
		std::string queryDevice(std::string commandString, int bufferLength = 100);
		std::string readData(std::string commandString);
		//std::vector <int> binaryQueryDevice(std::string commandString);
		//std::vector <int> continuousBinaryQueryDevice();
		void commandDevice(std::string commandString);

		//std::vector <int> binaryQueryDevice(std::string commandString, int sleepTimeMS = 100);

		int ShowError (int error, std::string errorMessage);
		bool initialized;
		
	protected:
		//ViPBuf buffer;   // Make the buffer long enough for longest expected read. 
		

	private:

		ViSession defaultRM;     
		ViSession instr;
		ViUInt32 numInstrs;
		ViFindList findList;   
		ViUInt32 retCount;
		ViUInt32 writeCount;
		ViStatus status;
		char instrResourceString[VI_FIND_BUFLEN];

		
		char stringinput[512];
		ViChar curve[20000];

		


	};


#endif
