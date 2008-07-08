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
#include "RbScanner.h"

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
		bool LockedTo(double offsetGHz, MATLABPLOTTER matlabplotter);


	protected:
	
	private:

		// constants
		double windowGHz;
		int lockedTo;

		// functions
		bool isLocked(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double lockpointGHz, double* error_p, std::vector<double> &FITDAQ_vector, std::vector<double> &FITFREQ_vector);
		void plot(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, std::vector <double>& FITDAQ_vector, std::vector <double>& FITFREQ_vector, MATLABPLOTTER &matlabplotter);

		bool buildKeyFreq(double* keyFreqGHz, double lockpointGHz, double* range);
		bool isInRange(double* freqList,int length, double* range);

		bool testForPeaks(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double* keyFreq, int* trueMax);
		int  position(std::vector <double>& myVector, double element);
		int  findMax(std::vector <double>& myVector, unsigned int start, unsigned int end);
		double findErr (double* diffs, int length);
		double leastSquaresSum(double* diffs, int length, double step);

	};

#endif
