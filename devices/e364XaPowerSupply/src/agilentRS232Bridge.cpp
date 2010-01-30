/*! \file agilentRS232Bridge.cpp
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

#include "agilentRS232Bridge.h"

agilentRS232Bridge::agilentRS232Bridge(unsigned short comPort)
{
	//open ports, set up names, etc...
	portOpen = false;
	openPort(comPort);
	

}
agilentRS232Bridge::~agilentRS232Bridge()
{
	//delete anything?
	closePort();
}
void agilentRS232Bridge::openPort(unsigned short comPort)
{
	//ASRL1::INSTR to use Com Port 1
	
	
	stringstream tempStream;
	tempStream << comPort;
	std::string comPortString = tempStream.str();
	
	std::string VISA_address = "ASRL" + comPortString + "::INSTR";
	std::cerr << "VISA Address is: " << VISA_address << std::endl;
	/* Open communication session with the power supply */
	ErrorStatus = viOpenDefaultRM(&defaultRM);
	ErrorStatus = viOpen(defaultRM, const_cast<char*>( VISA_address.c_str() ), 0, 0, &power_supply);
	bool openSuccess = checkError("Unable to open port");
	if(openSuccess)
	{
		portOpen = true;
	}
}
void agilentRS232Bridge::closePort()
{
	//dunno if this takes a comPort #
	/* Close the communication port */
	viClose(power_supply);
	viClose(defaultRM);
	portOpen = false;
}

bool agilentRS232Bridge::checkError(std::string errorMessage)
{
	//error handling function
	bool success = true;
	if (ErrorStatus < VI_SUCCESS)
	{
		success = false;
		std::cerr << errorMessage << std::endl;
		closePort();
	}
	return success;
}
bool agilentRS232Bridge::queryDevice(std::string commandString, std::string& strBuffer)
{
	char * buffer = new char[256];
	bool success = false;
	bool writeSuccess = false;
	bool readSuccess = false;
	
	if(portOpen)
	{
		commandString.append("\n"); // append an endline to the end of the command for the RS-232 to behave properly
		std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
		ErrorStatus = viPrintf(power_supply, const_cast<char*>( commandString.c_str() ) );
		writeSuccess = checkError("Can't Write to Driver");

		if(writeSuccess)
		{
			Sleep(200); /* Unit is milliseconds */
			ErrorStatus = viScanf(power_supply,"%T", buffer); //takes a pointer to memory - "%T" terminates the read on a termination character, "%s" terminates on whitespace
			readSuccess = checkError("Can't Read From Driver");
	
			if(readSuccess)
				strBuffer = std::string(buffer); // easier to work with strings. returns &strBuffer which is the stored data in memory
			
		}
	}

	delete[] buffer; //no memory leaks! hopefully...
	success = (writeSuccess & readSuccess);
	return success;
}
bool agilentRS232Bridge::commandDevice(std::string commandString)
{
	bool writeSuccess = false;
	if(portOpen)
	{
		commandString.append("\n"); // append an endline to the end of the command for the RS-232 to behave properly
		std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
		ErrorStatus = viPrintf(power_supply, const_cast<char*>( commandString.c_str() ) );
		writeSuccess = checkError("Can’t Write to Driver");
		Sleep(200);
	}
	return writeSuccess;
}


