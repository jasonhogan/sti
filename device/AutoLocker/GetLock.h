/*************************************************************************
 *
 * Name:   GetLock.h
 *
 * C++ Windows header for scanning and locking the Vortex
 *
 * Susannah Dickerson 7/8/2008
 * Dave Johnson
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef GETLOCK_H
#define GETLOCK_H

// Includes
#include <iostream>
#include "Matlab.h"
#include "cbw.h"
#include "USB1408FS.h"
#include "AGILENT8648A.h"
#include "math.h"


// Class Definitions
class GETLOCK 
	{ 
	public:
		// Constants
		float lockVoltage;
		double GHzToV;

		// Constructor
		GETLOCK();
		// Destructor
		~GETLOCK();

		// Functions
		bool lock(double* offsetGHz_p, MATLABPLOTTER &matlabplotter, USB1408FS &usb1408fs, AGILENT8648A &agilent8648a);
		void setLockVoltage (double voltage, USB1408FS &usb1408fs);

	protected:
	
	private:
		// Constants
		double start_voltage; // start point in GHz
		double voltage_incr; //increment frequency in GHz
		double end_voltage; // endpoint in GHz 
		int usb_input_channel;
		int usb_output_channel;

		// Functions
		void getParameters ();
		void plot(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, MATLABPLOTTER &matlabplotter);
		void plot(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, int start, int end, MATLABPLOTTER &matlabplotter);
		bool scan (std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, USB1408FS &usb1408fs);
		int findGlobalMin(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, int start, int end);
		double findCoolingPeak(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector);
		double findGlobalMax(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, int start, int end);
		int position(std::vector <double>& myVector, double element);

		bool save_data;

	};


#endif
