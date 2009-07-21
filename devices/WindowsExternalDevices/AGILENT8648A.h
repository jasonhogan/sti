/*************************************************************************
 *
 * Name:   AGILENT8648A.h
 *
 * C++ Windows header for Stanford Timing Interface to control the HP83711B signal generator via the ENET_GPIB_device interface
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef AGILENT8648A_H
#define AGILENT8648A_H

// Includes

#include "ENET_GPIB_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Class Definitions
class AGILENT8648A : public ENET_GPIB_device
	{ 
	public:
		
		AGILENT8648A(double maxPower); //constructor

		~AGILENT8648A(); //destructor
		
		void set_frequency(double frequency); //change the cw frequency of the generator

		void what_is_my_name(); //returns the device ID and the device options

		double get_frequency(); //returns the current cw frequency

		void set_power(double power); //sets the output power in dBm

		double get_power(); //returns the output power in dBm

		void output_on(); //turns the RF output on

		void output_off(); //turns the RF output off

		void get_output_state(); // checks to see if the RF output is off or on

	protected:
	
	private:

		int primary_address; //primary GPIB address for HP83711B signal generator

		int secondary_address; //secondary GPIB address for HP83711B signal generator

		double maximumAllowedPower;
     
	};


#endif
