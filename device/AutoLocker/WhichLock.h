/*************************************************************************
 *
 * Name:   WhichLock.h
 *
 * C++ Windows header for determining the locking transition
 *
 * Susannah Dickerson 7/6/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef WHICHLOCK_H
#define WHICHLOCK_H

// Includes
#include <iostream>
#include <vector>
#include <math.h>
#include <new>
#include <string>

#include "Matlab.h"
#include "ENET_GPIB_device.h" //functions to used the ENET GPIB interface
#include "HP83711B.h" //definition of the HP83711B function generator class
#include "USB1408FS.h" //definition of the USB1408FS class

#define KEYLENGTH    3
#define LABELLENGTH 24

// Class Definitions
class WHICHLOCK 
	{ 
	public:
		// constants
		double* freqsGHz;
		std::string* labels;


		WHICHLOCK();  // constructor
		~WHICHLOCK(); // destructor

		// functions
		bool freqDiff(int newTransition, double* freqDiffGHz);
		bool LockedTo(double offsetGHz, MATLABPLOTTER &matlabplotter, USB1408FS &usb1408fs);
		void scan_rb(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector, USB1408FS &usb1408fs);


	protected:
	
	private:

		// constants
		double windowGHz;
		int lockedTo;

		// for Rb scanner
		double start_freq; // start point in GHz
	    double freq_incr; //increment frequency in GHz
		double end_freq; // endpoint in GHz   
		double rf_power; // output power in dBm
		int usb_channel;


		HP83711B hp83711b;


		// functions
		void plot(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, std::vector <double>& FITDAQ_vector, std::vector <double>& FITFREQ_vector, MATLABPLOTTER &matlabplotter);
		bool isLocked(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double lockpointGHz, double* error_p, std::vector<double> &FITDAQ_vector, std::vector<double> &FITFREQ_vector);
		
		bool buildKeyFreq(double* keyFreqGHz, double lockpointGHz, double* range);
		bool isInRange(double* freqList,int length, double* range);

		bool testForPeaks(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double* keyFreq, int* trueMax);
		int  position(std::vector <double>& myVector, double element);
		int  findMax(std::vector <double>& myVector, unsigned int start, unsigned int end);
		double findErr (double* diffs, int length);
		double leastSquaresSum(double* diffs, int length, double step);

		// for Rb scanner
		void getParameters ();


	};

#endif
