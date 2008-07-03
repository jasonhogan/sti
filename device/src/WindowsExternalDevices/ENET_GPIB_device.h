/*************************************************************************
 *
 * Name:   ENET_GPIB_device.h
 *
 * C++ Windows header for Stanford Timing Interface to control a generic GPIB device connected via the NI ENET-100 hub 
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef ENET_GPIB_DEVICE_H
#define ENET_GPIB_DEVICE_H

// Includes

#include <iostream>
#include <cstring>

#include "ni4882.h"

// Class Definitions
class ENET_GPIB_device 
	{ 
	public:
		
		ENET_GPIB_device(); //constructor

		void GPIB_Error(char *source);

		int Initialize_GPIBENET_Controller(char *interface_name, int padd);

		void Query_Device(int bdhandle, int padd, int sadd, char *command_string,
                  char *buffer, int read_size);

		void Command_Device (int bdhandle, int padd, int sadd, char *command_string, 
		   char *buffer, int read_size);

		void Close_Handles(void);

		
		
	protected:
	
		char buffer[101];   // Make the buffer 1 char longer than longest expected read.

		int GPIBinterface;

	private:

		int enet_controller[100];
		int num_controllers;

	};


#endif
