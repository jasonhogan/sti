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
#include "Vortex6000.h"
#include "math.h"

// Class Definitions
class GETLOCK 
	{ 
	public:
		// Constants
		double lockVoltage;
		double GHzToV;

		// Constructor
		GETLOCK(Vortex6000 &vortex6000);
		// Destructor
		~GETLOCK();

		// Functions
		void setVortexVoltage (double voltage, Vortex6000 &vortex6000);
		bool lock(double* offsetGHz_p, MATLABPLOTTER &matlabplotter, 
					USB1408FS &usb1408fs, AGILENT8648A &agilent8648a,
					Vortex6000 &vortex6000);


	protected:
	
	private:
		// Constants
		double start_voltage; // start point in GHz
		double voltage_incr; //increment frequency in GHz
		double end_voltage; // endpoint in GHz 
		double vortexVoltage;
		int usb_input_channel;
		int usb_output_channel;

		// Functions
		void getParameters ();

		bool scan(std::vector <double>& voltage_vector,
			std::vector <double>& DAQ_vector, USB1408FS &usb1408fs);

		void plot(std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, 
				MATLABPLOTTER &matlabplotter);
		void plot(std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, 
				std::vector <double>& voltageSB_vector, 
				std::vector <double>& DAQSB_vector, 
				MATLABPLOTTER &matlabplotter);

		double findCoolingPeak(std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, int start, int end);

		int findGlobalMin(std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, int start, int end);
		void getTwoLowestMinima (std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, 
				std::vector <int>& minPositions, 
				int* minPosSmallest, int* minPosLarger);
		int findGlobalMax(std::vector <double>& voltage_vector, 
				std::vector <double>& DAQ_vector, int start, int end);
		int position(std::vector <double>& myVector, double element);

		double findSidebandPeak(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, int start, int end);	
		int derivativeTest(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, std::vector <int>& minPositions, int bigWindow);


	};


#endif
