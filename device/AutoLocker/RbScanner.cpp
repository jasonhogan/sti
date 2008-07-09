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
	start_freq = 1.4; // start point in GHz
    freq_incr = .003; //increment frequency in GHz
    end_freq = 2.4; // endpoint in GHz   
    rf_power = 0.8; // output power in dBm
	usb_channel = 7;

	//set the output power in dBm
	hp83711b.set_power(0);
}

RBSCANNER::~RBSCANNER() {

	


}

//===========================================================================

void RBSCANNER::getParameters ()
{
    bool change_vals = true; // have user defined values

	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_channel << std::endl;
    std::cout << "Start Frequency: " << start_freq << " GHz" << std::endl;
    std::cout << "End Frequency: " << end_freq << " GHz" << std::endl;
    std::cout << "Frequency Increment: " << freq_incr << " GHz" << std::endl;
    std::cout << "Output Power: " << rf_power << " dBm" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined start frequency
        std::cout << "Enter desired start frequency in GHz. Default value is: " << start_freq << " GHz" << std::endl << "start freq = ";
        std::cin >> start_freq;
   
        std::cout << "Enter desired end frequency in GHz. Default value is: " << end_freq << " GHz" << std::endl << "end freq = ";
        std::cin >> end_freq;
   
        std::cout << "Enter desired frequency increment in GHz. Default value is: " << freq_incr << " GHz" << std::endl << "freq increment = ";
        std::cin >> freq_incr;
   
        std::cout << "Enter desired output power in dBm. Default value is: " << rf_power << " dBm" << std::endl << "power = ";
        std::cin >> rf_power;
    }


}


void RBSCANNER::scan_rb(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector, USB1408FS &usb1408fs)
{
	//Scan laser

	double freq; //frequency in GHz
	
	getParameters();
	freq = start_freq; //frequency in GHz

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

//===========================================================================
