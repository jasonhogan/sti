

#include "ENET_GPIB_device.h" //functions to used the ENET GPIB interface
#include "HP83711B.h" //definition of the HP83711B function generator class
#include "USB1408FS.h" //definition of the USB1408FS class
#include "RbScanner.h"

#include <iostream> //cin & cout commands


#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage

int main(int argc, char* argv[])

{
	//define scan variables
	

    double start_freq = 1.4; // start point in GHz
    double freq_incr = .003; //increment frequency in GHz
    double end_freq = 2.4; // endpoint in GHz   
    double rf_power = 0.8; // output power in dBm
    bool change_vals = true; // have user defined values
	int usb_channel = 7;

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
   

	RBSCANNER rbscanner;

	rbscanner.scan_rb(usb_channel, start_freq, end_freq, freq_incr, rf_power, true);

	return 0;
};