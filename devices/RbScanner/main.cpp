

//#include "ENET_GPIB_device.h" //functions to used the ENET GPIB interface
//#include "HP83711B.h" //definition of the HP83711B function generator class
#include "USB1408FS.h" //definition of the USB1408FS class
#include "RbScanner.h"
//#include "WhichLock.h"
//#include "Matlab.h"

#include <iostream> //cin & cout commands


#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage


int main(int argc, char* argv[])

{
	//define initial state
	bool notLocked;

	std::vector <double> DAQ_vector;
	std::vector <double> FREQ_vector;
	std::vector <double> FITDAQ_vector;
	std::vector <double> FITFREQ_vector;

	//define scan variables

    double start_freq = 1.4; // start point in GHz
    double freq_incr = .003; //increment frequency in GHz
    double end_freq = 2.4; // endpoint in GHz   
    double rf_power = 0.8; // output power in dBm
    bool change_vals = true; // have user defined values
	int usb_channel = 7;
	double offsetGHz = 0.7;
	bool save_data = true;


	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_channel << std::endl;
    std::cout << "Start Frequency: " << start_freq << " GHz" << std::endl;
    std::cout << "End Frequency: " << end_freq << " GHz" << std::endl;
    std::cout << "Frequency Increment: " << freq_incr << " GHz" << std::endl;
    std::cout << "Output Power: " << rf_power << " dBm" << std::endl;
	std::cout << "Offset Lock frequency: " << offsetGHz << " GHz" << std::endl;
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

		std::cout << "Enter desired offset lock frequency in GHz. Default value is: " << offsetGHz << " GHz" << std::endl << "frequency = ";
        std::cin >> offsetGHz;
   
    }
   

	RBSCANNER rbscanner;

	USB1408FS usb1408fs;

//	WHICHLOCK whichlock;

//	MATLABPLOTTER matlabplotter;

//	rbscanner.scan_rb(FREQ_vector, DAQ_vector, usb_channel, start_freq, end_freq, freq_incr, rf_power);

//	notLocked = whichlock.LockedTo(DAQ_vector, FREQ_vector, FITDAQ_vector, FITFREQ_vector, offsetGHz);
/*
	if (notLocked) {
		std::cout << "Eeeek! Error!" << std::endl;
	}


	matlabplotter.plotfreqscan(FREQ_vector, DAQ_vector);

	matlabplotter.plotlockpoints(FITFREQ_vector, FITDAQ_vector);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);

	FREQ_vector.clear();
	DAQ_vector.clear();
	FITFREQ_vector.clear();
	FITDAQ_vector.clear();

	*/

	return 0;
};