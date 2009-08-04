/*************************************************************************
 *
 * Name:   HP83711B.h
 *
 * C++ Windows header for Stanford Timing Interface to control the HP83711B signal generator via the ENET_GPIB_device interface
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef HP83711B_H
#define HP83711B_H

// Includes

#include "ENET_GPIB_device.h"

// Class Definitions
class HP83711B : public ENET_GPIB_device
	{ 
	public:
		
		HP83711B(int primaryAddress, int secondaryAddress, double maximumPower); //constructor

		~HP83711B(); //destructor
		
		void set_frequency(double frequencyInGHz); //change the cw frequency of the generator

		void what_is_my_name(); //returns the device ID and the device options

		double get_frequency(); //returns the current cw frequency

		void get_freq_increment(); //returns the frequency increment step

		void set_freq_increment(double freq_increment); //set the frequency step increment size

		void increment_frequency_up(); //increments frequency by the step amount

		void increment_frequency_down(); //increments frequency by the step amount

		void set_power(double power); //sets the output power in dBm

		double get_power(); //returns the output power in dBm

		void output_on(); //turns the RF output on

		void output_off(); //turns the RF output off

		void get_output_state(); //determines if the output is on or off

	protected:
	
	private:

		int primary_address; //primary GPIB address for HP83711B signal generator

		int secondary_address; //secondary GPIB address for HP83711B signal generator

		double maximumOutputPower; //maximum allowed output power according to the user's application
     
	};


#endif
