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

		void plotData(std::vector <double> &timeVector, std::vector <double> &signalVector, bool new_figure); //plot data
	
		void savedata(unsigned int number, double pushFrequency,double pullFrequency, double pushPower, double pullPower, std::vector <double> &timeVectorOff, std::vector <double> &signalVectorOff,
							 std::vector <double> &timeVectorSerrodyne, std::vector <double> &signalVectorSerrodyne);

		void savedata(std::vector <double> &timeVector, std::vector <double> &signalVector);

		std::string generateDate();

		void sendmail(std::string message, std::string subject, std::vector <std::string>& recipients);

	protected:
	
		

	private:

		Engine *ep;

	};


#endif
