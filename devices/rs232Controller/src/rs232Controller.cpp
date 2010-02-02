/*! \file rs232Controller.cpp
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

#include "rs232Controller.h"
#include <iostream>

#define STRICT
#include <tchar.h>

#include <windows.h>

#include "Serial.h"


rs232Controller::rs232Controller(int comPort)
{
	serial = new CSerial;
	lLastError = ERROR_SUCCESS;
	int errorCode = 0;

    // Attempt to open the serial port
	//comportString = "COM" + valueToString(comPort);
    lLastError = serial->Open(_T("COM4"),0,0,false);
	if (lLastError != ERROR_SUCCESS)
		errorCode = ShowError(serial->GetLastError(), _T("Unable to open COM-port"));

    // Setup the serial port (9600,N81) using hardware handshaking
    lLastError = serial->Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		errorCode = ShowError(serial->GetLastError(), _T("Unable to set COM-port setting"));

	// Setup handshaking
	
    lLastError = serial->SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS)
		errorCode = ShowError(serial->GetLastError(), _T("Unable to set COM-port handshaking"));

    // The serial port is now ready and we can send/receive data. If
	// the following call blocks, then the other side doesn't support
	// hardware handshaking.
	
	//open ports, set up names, etc...
//	port = new CSerialPort; 
//	port->Open(comPort, 9600, CSerialPort::NoParity, 8, CSerialPort::OneStopBit, CSerialPort::NoFlowControl);
//	port->Set0WriteTimeout();
  //  port->Set0ReadTimeout();
//	port->ClearDTR();
 //   port->ClearRTS();
}
rs232Controller::~rs232Controller()
{
	//delete anything?
//	port->ClearWriteBuffer();
 //   port->ClearReadBuffer();
 //   port->Flush();
  //  port->Close();
//	delete port;

	// Close the port again
    serial->Close();
	delete serial;
}

std::string rs232Controller::queryDevice(std::string commandString)
{
	int readLength = 30;
	char * buffer = new char[readLength + 1];
	for(int i = 0; i<readLength; i++)
		buffer[i] = ' ';

	//port->TerminateOutstandingWrites();
	//port->ClearWriteBuffer();
   // port->ClearReadBuffer();
	
	commandString.append("\x0D"); // append an endline to the end of the command for the RS-232 to behave properly
	std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
	lLastError = serial->Write(commandString.c_str());
	//port->Write(commandString.c_str(), commandString.size());

	Sleep(50); /* Unit is milliseconds */
	
//	DWORD dwRead = port->Read(buffer, readLength);

	lLastError = serial->Read(buffer, readLength);

	buffer[readLength] = '\0';

	std::string readOutput = std::string(buffer);

	size_t length = readOutput.size();
	size_t found;

	// different member versions of find in the same order as above:
	found=readOutput.find("Ü");
	readOutput.erase(found, length);

	std::cout << "Serial Port: " << readOutput << std::endl;

  //  port->TerminateOutstandingReads();
			
	delete[] buffer; //no memory leaks! hopefully...

	return readOutput;
}
void rs232Controller::commandDevice(std::string commandString)
{
	//port->TerminateOutstandingWrites();
	//port->ClearWriteBuffer();
  //  port->ClearReadBuffer();
	
	commandString.append("\x0D"); // append an endline to the end of the command for the RS-232 to behave properly
	std::cerr << "Write Command String: ********" << commandString << "*******" << std::endl;
	lLastError = serial->Write(commandString.c_str());
//	port->Write(commandString.c_str(), commandString.size());
}
int rs232Controller::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	//::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}


