/*************************************************************************
 *
 * Name:   thermocoupleScanner.h
 *
 * C++ Windows header for reading in thermocouples from measurement computing mux board
 *
 * David Johnson 2/23/2011
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef THERMOCOUPLESCANNER_H
#define THERMOCOUPLESCANNER_H

// Includes

#include "USB1408FS.h" //definition of the USB1408FS class
//#include "Matlab.h" //load matlab engine

//#include <math.h> //used for the 'floor' command
//#include <time.h> //need to provide date time stamp for files
#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage
#include <iostream> //cin & cout commands

// Class Definitions
class thermocoupleScanner 
	{ 
	public:
		
		thermocoupleScanner(); 

		~thermocoupleScanner(); //destructor

		void scanThermocouples(std::vector <double> &thermocoupleVoltages); //scan over all pre-defined thermocouples

	protected:
	
	private:

		USB1408FS* usb1408fs0; //usb-daq object
		USB1408FS* usb1408fs1; //usb-daq object

		//Engine *ep; //Matlab engine
		//mxArray *thermocoupleVoltages; //array to transfer data imported into c++ into Matlab

	};


#endif
