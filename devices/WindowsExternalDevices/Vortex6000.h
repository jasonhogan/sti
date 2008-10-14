/*************************************************************************
 *
 * Name:   HP83711B.h
 *
 * C++ Windows header for Stanford Timing Interface to control the Vortex 6000 Laser Controller via the ENET_GPIB_device interface
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef VORTEX6000_H
#define VORTEX6000_H

// Includes

#include "ENET_GPIB_device.h"
#include <string>
#include <sstream>

// Class Definitions
class Vortex6000 : public ENET_GPIB_device
	{ 
	public:
		
		Vortex6000(); //constructor

		~Vortex6000(); //destructor

		void what_is_my_name(); //returns the device ID and the device options

		double get_piezo_voltage(); //returns the current piezo voltage set point

		void set_piezo_voltage(double piezo_voltage); //sets the piezo voltage in volts

	protected:
	
	private:

		int primary_address; //primary GPIB address for HP83711B signal generator

		int secondary_address; //secondary GPIB address for HP83711B signal generator
     
	};


#endif
