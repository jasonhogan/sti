/*************************************************************************
 *
 * Name:   agilentE4411B.h
 *
 * C++ Windows header for Stanford Timing Interface to control the agilentE4411B spectrum analyzer via the ENET_GPIB_device interface
 *
 * David Johnson 8/4/2009
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef agilentE4411B_H
#define agilentE4411B_H

// Includes

#include "ENET_GPIB_device.h"
#include <vector> //needed to be able to use vectors for data storage
#include <iostream>
#include <string>
#include <sstream>

// Class Definitions
class agilentE4411B : public ENET_GPIB_device
	{ 
	public:
		
		agilentE4411B(int primaryAddress); //constructor

		~agilentE4411B(); //destructor
		
		
		void what_is_my_name(); //returns the device ID and the device options
		bool setWaveformPoints(int numberPoints); //set how much data the scope is going to store
		bool setupTrigger(std::string source, std::string mode, std::string slope); //options are: "Channel 1", "Channel 2", "External" //options are: "Edge" //options: "Pos", "Neg"
		bool setupAcquisition(); //applies all standard options
		bool queryScalingInformation(); //get all info about how the data will be scaled
		std::string saveData(); //save the current waveform to data

		bool parseData(std::string &data, std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector);
		
		std::string quickQuery(std::string query);
		bool quickCommand(std::string command);

	protected:
		
	
	private:

		int primary_address; //primary GPIB address for HP83711B signal generator

		int secondary_address; //secondary GPIB address for HP83711B signal generator

		double referenceLevel;
		double normalizedReferenceLevel;
		double referenceLevelPosition;
		double powerPerDivision;
		double centerFrequency;
		double frequencySpan;
		double startFrequency;
		double stopFrequency;
		double freqIncrement;

     
	};


#endif
