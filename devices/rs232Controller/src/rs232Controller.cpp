/*! \file rs232Controller.cpp
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

#include "rs232Controller.h"
#include <iostream>

// uses the CSerial class from Serial.h & Serial.cpp
//#define STRICT
#include <tchar.h>
#include <windows.h>
#include "Serial.h"


rs232Controller::rs232Controller(std::string comportString)
{
	serial = new CSerial;
	lastErrorCode = ERROR_SUCCESS;
	int errorCode = 0;

	initialized = true;

    // Attempt to open the serial port
	lastErrorCode = serial->Open(_T(comportString.c_str()),0,0,false);
	if (lastErrorCode != ERROR_SUCCESS) {
		errorCode = ShowError(serial->GetLastError(), "Unable to open COM-port");
		initialized = false;
	}

    // Setup the serial port (19200,N81) using hardware handshaking
    lastErrorCode = serial->Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lastErrorCode != ERROR_SUCCESS) {
		errorCode = ShowError(serial->GetLastError(), "Unable to set COM-port setting");
		initialized = false;
	}

	// Setup handshaking
	
    lastErrorCode = serial->SetupHandshaking(CSerial::EHandshakeOff);
	if (lastErrorCode != ERROR_SUCCESS) {
		errorCode = ShowError(serial->GetLastError(), "Unable to set COM-port handshaking");
		initialized = false;
	}

    // The serial port is now ready and we can send/receive data. If
	// the following call blocks, then the other side doesn't support
	// hardware handshaking.
}
rs232Controller::~rs232Controller()
{
	// Close the port again
    serial->Close();
	delete serial;
}

std::string rs232Controller::queryDevice(std::string commandString, int sleepTimeMS /* = 100 */)
{
	int readLength = 30;
	char * buffer = new char[readLength + 1];
	for(int i = 0; i<readLength; i++)
		buffer[i] = ' ';

	commandString.append("\x0D"); // append an endline to the end of the command for the RS-232 to behave properly
//	std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
	lastErrorCode = serial->Write(commandString.c_str());

	Sleep(sleepTimeMS); /* Unit is milliseconds */

	lastErrorCode = serial->Read(buffer, readLength);
	buffer[readLength] = '\0';
	std::string readOutput = std::string(buffer);
	size_t length = readOutput.size();
	size_t found;
	found=readOutput.find("Ü");
	if (found != std::string::npos)
		readOutput.erase(found, length);
//	std::cout << "Serial Port: " << readOutput << std::endl;
			
	delete[] buffer; //no memory leaks! hopefully...

	return readOutput;
}
void rs232Controller::commandDevice(std::string commandString)
{
	commandString.append("\x0D"); // append an endline to the end of the command for the RS-232 to behave properly
//	std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
	lastErrorCode = serial->Write(commandString.c_str());
}
int rs232Controller::ShowError (int error, std::string errorMessage)
{
	std::cerr << "Error Code: " << error << " with error message: " << errorMessage << std::endl;
	return 1;
}

std::vector <int> rs232Controller::binaryQueryDevice(std::string commandString, int sleepTimeMS /* = 100 */)
{
	std::vector <int> bufferInt;
	int readLength = 7;	

	char * buffer = new char[readLength + 1];
	for(int i = 0; i < readLength; i++)
		buffer[i] = ' ';

	commandString.append("\x0D"); // append an endline to the end of the command for the RS-232 to behave properly

	lastErrorCode = serial->Write(commandString.c_str());

	Sleep(sleepTimeMS); /* Unit is milliseconds */	
	
	lastErrorCode = serial->Read(buffer, readLength);

	for(int j = 0; j < readLength; j++) {
		bufferInt.push_back((int) buffer[j]);
	}

	delete[] buffer; //no memory leaks! hopefully...

	//return readOutput;
	return bufferInt;
}