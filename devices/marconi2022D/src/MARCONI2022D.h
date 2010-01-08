/*************************************************************************
 *
 * Name:   MARCONI2022D.h
 *
 * C++ Windows header for Stanford Timing Interface to control the HP83711B signal generator via the ENET_GPIB_device interface
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef MARCONI2022D_H
#define MARCONI2022D_H

// Includes

#include "ENET_GPIB_device.h"

// Class Definitions
class MARCONI2022D : public ENET_GPIB_device
	{ 
	public:
		
		MARCONI2022D(); //constructor

		~MARCONI2022D(); //destructor
		
		void set_frequency(double freqGHz); //change the cw frequency of the generator

		void what_is_my_name(); //returns the device ID and the device options

		void get_frequency(); //returns the current cw frequency

		void get_freq_increment(); //returns the frequency increment step

		void set_freq_increment(double freq_incrGHz); //set the frequency step increment size

		void increment_frequency_up(); //increments frequency by the step amount

		void increment_frequency_down(); //increments frequency by the step amount

		void set_power(double power); //sets the output power in dBm

		void get_power(); //returns the output power in dBm

		void output_on(); //turns the RF output on

		void output_off(); //turns the RF output off

	protected:
	
	private:

		int primary_address; //primary GPIB address for MARCONI2022D signal generator

		int secondary_address; //secondary GPIB address for MARCONI2022D signal generator
     
	};


#endif
