/*************************************************************************
 *
 * Name:   USB1408FS.h
 *
 * C++ Windows header for Stanford Timing Interface to control the Measurement Computing USB-1408FS DAQ
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef AUTOLOCKER_H
#define AUTOLOCKER_H

// Includes
#include <iostream>



// Class Definitions
class AUTOLOCKER 
	{ 
	public:
		
		AUTOLOCKER(); //constructor

		void enable_lock(); 
		void disable_lock(); 

	protected:
	
	private:

		int channel;


	};


#endif
