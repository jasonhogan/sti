/*************************************************************************
 *
 * Name:   GPIB_device.h
 *
 * C++ Windows header for Stanford Timing Interface to control a generic GPIB device connected via the NI ENET-100 hub 
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef GPIB_device_H
#define GPIB_device_H

// Includes

#include <iostream>
#include <cstring>

#include "ni4882.h"

// Class Definitions
class GPIB_device 
	{ 
	public:
		
		GPIB_device(std::string gpibHost, int controllerNumber); //constructor

		void GPIB_Error(char *source);

		int Initialize_GPIB_Controller(const char* interface_name, int padd);

		void Query_Device(int padd, int sadd, char *command_string,
            std::string& strBuffer, int read_size);

		void Command_Device (int padd, int sadd, char *command_string, 
			std::string& strBuffer, int read_size);

		void Close_Handles(void);

		bool readUntilTerminationCharacter (int padd, int sadd, char *command_string, std::string & result);
		bool readUntilNewLine (int padd, int sadd, char *command_string, std::string & result);

		char * bigBuffer;

	protected:
	
		char buffer[101];   // Make the buffer 1 char longer than longest expected read.

		int GPIB_interface;

	private:

		int GPIB_controller[100];
		int num_controllers;
		int BD_PAD;

	};


#endif
