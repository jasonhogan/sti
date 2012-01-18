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
	rs232Controller(comportString, 9600, 8, "None", 1);
}

rs232Controller::rs232Controller(std::string comportString, unsigned int baudRate, unsigned int dataBits, std::string parity, unsigned int stopBits)
{
	serial = new CSerial;
	lastErrorCode = ERROR_SUCCESS;
	int errorCode = 0;

	initialized = true;

    // Attempt to open the serial port

//	lastErrorCode = serial->Open( (LPCTSTR) comportString.c_str(),0,0,false);
	lastErrorCode = serial->Open(_T( comportString.c_str() ), 0, 0, false);
	if (lastErrorCode != ERROR_SUCCESS) {
		errorCode = ShowError(serial->GetLastError(), "Unable to open COM-port");
		initialized = false;
	}


	CSerial::EBaudrate sBaudRate = getBaudRate(baudRate);
	CSerial::EDataBits sDataBits = getDataBits(dataBits);
	CSerial::EParity sParity   = getParity(parity);
	CSerial::EStopBits sStopBits = getStopBits(stopBits);

    // Setup the serial port (19200,N81) using hardware handshaking
    //lastErrorCode = serial->Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	lastErrorCode = serial->Setup(sBaudRate,sDataBits,sParity,sStopBits);
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

std::string rs232Controller::queryDevice(std::string commandString, int sleepTimeMS /*= 100*/, int readLength /*= 30*/)
{
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

CSerial::EBaudrate rs232Controller::getBaudRate(unsigned int baudRate)
{
	switch (baudRate)
	{
	case 110:
		return CSerial::EBaud110;
	case 300:
		return CSerial::EBaud300;
	case 600:
		return CSerial::EBaud600;
	case 1200:
		return CSerial::EBaud1200;
	case 2400:
		return CSerial::EBaud2400;
	case 4800:
		return CSerial::EBaud4800;
	case 9600:
		return CSerial::EBaud9600;
	case 14400:
		return CSerial::EBaud14400;
	case 19200:
		return CSerial::EBaud19200;
	case 38400:
		return CSerial::EBaud38400;
	case 56000:
		return CSerial::EBaud56000;
	case 57600:
		return CSerial::EBaud57600;
	case 115200:
		return CSerial::EBaud115200;
	case 128000:
		return CSerial::EBaud128000;
	case 256000:
		return CSerial::EBaud256000;
	default:
		return CSerial::EBaudUnknown;
	}
}

CSerial::EDataBits rs232Controller::getDataBits(unsigned int dataBits)
{
	switch (dataBits)
	{
	case 8:
		return CSerial::EData8;
	case 7:
		return CSerial::EData7;
	case 6:
		return CSerial::EData6;
	case 5:
		return CSerial::EData5;
	default:
		return CSerial::EDataUnknown;
	}
}

CSerial::EParity   rs232Controller::getParity(std::string parity)
{
	if (parity.compare("None")==0)
		return CSerial::EParNone;
	if (parity.compare("Odd")==0)
		return CSerial::EParOdd;
	if (parity.compare("Even")==0)
		return CSerial::EParEven;
	if (parity.compare("Mark")==0)
		return CSerial::EParMark;
	if (parity.compare("Space")==0)
		return CSerial::EParSpace;
	
	return CSerial::EParUnknown;
}
CSerial::EStopBits rs232Controller::getStopBits(unsigned int stopBits)
{
	switch (stopBits)
	{
	case 2:
		return CSerial::EStop2;
	case 1:
		return CSerial::EStop1;
	default:
		return CSerial::EStopUnknown;
	}
}