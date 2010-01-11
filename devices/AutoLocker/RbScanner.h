/*************************************************************************
 *
 * Name:   RbScanner.h
 *
 * C++ Windows header for Stanford Timing Interface to scan the Rb Spectrum using a function generator
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef RBSCANNER_H
#define RBSCANNER_H

// Includes

#include "ENET_GPIB_device.h" //functions to used the ENET GPIB interface
#include "HP83711B.h" //definition of the HP83711B function generator class
#include "USB1408FS.h" //definition of the USB1408FS class
#include <math.h> //used for the 'floor' command
#include <time.h> //need to provide date time stamp for files

#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage
#include <windows.h> //needed for ni488.2 library
#include <iostream> //cin & cout commands
#include "engine.h" //load matlab engine

// Class Definitions
class RBSCANNER 
	{ 
	public:
		
		RBSCANNER(); //constructor

		~RBSCANNER(); //destructor

		void scan_rb(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector, USB1408FS &usb1408fs);

	protected:
	
	private:

		double start_freq; // start point in GHz
	    double freq_incr; //increment frequency in GHz
		double end_freq; // endpoint in GHz   
		double rf_power; // output power in dBm
		int usb_channel;


		HP83711B hp83711b;

		void getParameters ();

	};


#endif
