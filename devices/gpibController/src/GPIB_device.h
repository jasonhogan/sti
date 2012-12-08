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
#include <vector>

#include "ni4882.h"


// Class Definitions
class GPIB_device 
	{ 
	public:
		
		GPIB_device(std::string gpibHost); //constructor

		void GPIB_Error(char *source);

		int Initialize_GPIB_Controller(const char* interface_name, int padd);

		void Query_Device(int padd, int sadd, char *command_string,
            std::string& strBuffer, int read_size);

		void Command_Device (int padd, int sadd, char *command_string, 
			std::string& strBuffer, int read_size);

		void Close_Handles(void);

		bool readUntilTerminationCharacter (int padd, int sadd, char *command_string, std::string & result);
		bool readUntilNewLine (int padd, int sadd, char *command_string, std::string & result);

		void GPIB_device::QueryAndReadChars(int padd, int sadd, char *command_string, 
									 std::vector <unsigned char> & resultVector, int read_size);

		char * bigBuffer;

	protected:
	
		char buffer[101];   // Make the buffer 1 char longer than longest expected read.

		int GPIB_interface;

	private:

		int GPIB_controller[100];
		int num_controllers;

		template<typename T> std::string valueToString(T inValue, std::string Default="", std::ios::fmtflags numBase=std::ios::dec, std::streamsize precision=9)
		{
			std::string outString;
			std::stringstream tempStream;
			tempStream.precision(precision);
			tempStream.setf( numBase, std::ios::basefield );

			tempStream << inValue;
			outString = tempStream.str();

			if( !tempStream.fail() )
				return outString;
			else
				return Default;
		}

	};


#endif
