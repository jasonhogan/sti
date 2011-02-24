/*************************************************************************
 *
 * Name:   USB1408FS.h
 *
 * C++ Windows header for Stanford Timing Interface to control the Measurement Computing USB-1408FS DAQ
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/


#ifndef USB1408FS_H
#define USB1408FS_H

// Includes
#include <iostream>
#include "cbw.h"


// Class Definitions
class USB1408FS 
	{ 
	public:
		
		USB1408FS(int inputBoardNum); //constructor

		void set_output_voltage(int channel, float output_voltage); //change output voltage value on a D/A out channel (0-1)

		double read_input_channel(int channel); //read in the voltage on a A/D input channel (3-7)

		double read_MUX_input_channel(int channel); //read in the voltage on a MUXed input channel (1-32)

	protected:
	
	private:

		int BoardNum;
		int UDStat;
		int channel;
		int inputGain;
		int outputGain;
		float DataValue;
		int Options;
        
		float    RevLevel;

	};


#endif
