/*************************************************************************
 *
 * Name:   Matlab.h
 *
 * C++ Windows header for easily using Matlab api functions
 *
 * David Johnson 7/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef MATLAB_H
#define MATLAB_H

// Includes

#include <math.h> //used for the 'floor' command
#include <time.h> //need to provide date time stamp for files

#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage


#include <iostream> //cin & cout commands

#include <windows.h> //needed for ni488.2 library

#include "engine.h" //load matlab engine

// Class Definitions
class MATLABPLOTTER 
	{ 
	public:
		
		MATLABPLOTTER(); //constructor

		void plotfreqscan(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector); //plot data

		void plotlockpoints(std::vector <double> &FITFREQ_vector, std::vector <double> &FITDAQ_vector); //plot the over-laid lock points
		
		void savedata(bool save_data); //save the output

	protected:
	
		

	private:

		Engine *ep;

		//mxArray *data_freq;
		//mxArray *data_DAQ;

	};


#endif
