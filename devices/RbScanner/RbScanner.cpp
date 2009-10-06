/*************************************************************************
 *
 * Name:   RbScanner.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to scan the Rb Spectrum using a function generator
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "RbScanner.h" //definition of the USB1408FS class

//===========================================================================

RBSCANNER::RBSCANNER()
{
	//stuff	
	//initialize frequency generator
//	HP83711B hp83711b;

		//set the output power in dBm
//	hp83711b.set_power(0);

	//initialize USB-1408FS DAQ

	#ifndef USB_DAQ
	#define USB_DAQ
		USB1408FS usb1408fs;
	#endif

}

RBSCANNER::~RBSCANNER() {

	


}

//===========================================================================
/*

void RBSCANNER::scan_rb(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector, int usb_channel, double start_freq, double end_freq, double freq_incr, double rf_power)
{

		//Scan laser

	double freq = start_freq; //frequency in GHz
	
	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(rf_power);

	//define vectors for storing the data

	//std::vector <double> DAQ_vector;
	//std::vector <double> FREQ_vector;

	hp83711b.output_on();

	hp83711b.set_frequency(freq);
	
	Sleep(100);

	//loop from start freq to start + interval
	while(freq <= end_freq) {

		FREQ_vector.push_back(freq); //record function generator frequency

		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel)); //take data

		// change the frequency
		hp83711b.increment_frequency_up();
		freq = freq + freq_incr;

		Sleep(50); //wait for the function generator to settle. spec'd time is 20ms

		}	




}
*/
//===========================================================================
