/*************************************************************************
 *
 * Name:   WHICHLOCK.h
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

#define KEYLENGTH    3
#define LABELLENGTH 24

// Class Definitions
class WHICHLOCK 
	{ 
	public:
		WHICHLOCK();  // constructor
		~WHICHLOCK(); // destructor

		// functions
		bool LockedTo(std::vector<double> &DAQ_vector, std::vector<double> &FREQ_vector, std::vector<double> &FITDAQ_vector, std::vector<double> &FITFREQ_vector, double offsetGHz);

	protected:
	
	private:

		// constants
		double windowGHz;
		double* freqsGHz;
		std::string* labels;

		// functions
		bool isLocked(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double lockpointGHz, double* error_p, std::vector<double> &FITDAQ_vector, std::vector<double> &FITFREQ_vector);

		bool buildKeyFreq(double* keyFreqGHz, double lockpointGHz, double* range);
		bool isInRange(double* freqList,int length, double* range);

		bool testForPeaks(std::vector <double>& DAQ_vector, std::vector <double>& FREQ_vector, double* keyFreq, int* trueMax);
		int  position(std::vector <double>& myVector, double element);
		int findMax(std::vector <double>& myVector, unsigned int start, unsigned int end);
		double findErr (double* diffs, int length);
		double leastSquaresSum(double* diffs, int length, double step);

	};

#endif
