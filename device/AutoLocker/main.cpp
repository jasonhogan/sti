

#include "USB1408FS.h" //definition of the USB1408FS class
#include "Matlab.h"

#include <iostream> //cin & cout commands


#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage


int main(int argc, char* argv[])

{
	//define initial state


	std::vector <double> DAQ_vector;
	std::vector <double> voltage_vector;


	//define scan variables

    double start_voltage = 0; // start point in GHz
    double voltage_incr = .003; //increment frequency in GHz
    double end_voltage = 2; // endpoint in GHz   
    bool change_vals = true; // have user defined values
	int usb_input_channel = 5;
	int usb_output_channel = 4;
	bool save_data = true;


	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_input_channel << std::endl;
	std::cout << "USB output channel: " << usb_output_channel << std::endl;
    std::cout << "Start Voltage: " << start_voltage << " GHz" << std::endl;
    std::cout << "End Voltage: " << end_voltage << " GHz" << std::endl;
    std::cout << "Voltage Increment: " << voltage_incr << " GHz" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined start frequency
        std::cout << "Enter desired start voltage in volts. Default value is: " << start_voltage << " V" << std::endl << "start voltage = ";
        std::cin >> start_voltage;
   
        std::cout << "Enter desired end voltage in volts. Default value is: " << end_voltage << " V" << std::endl << "end voltage = ";
        std::cin >> end_voltage;
   
        std::cout << "Enter desired voltage increment in volts. Default value is: " << voltage_incr << " V" << std::endl << "voltage increment = ";
        std::cin >> voltage_incr;
   
    }
   
	USB1408FS usb1408fs;

	double voltage = start_voltage;

	while(voltage <= end_voltage) {

		usb1408fs.set_output_voltage(usb_output_channel, voltage); //set the output voltage to the back of the vortex

		voltage_vector.push_back(voltage); //record DAQ output voltage

		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_input_channel)); //take data

		// change the frequency
		voltage = voltage + voltage_incr;

		Sleep(1); //wait for the DAQ to settle. spec'd rate is 10 KS/s

	}

	MATLABPLOTTER matlabplotter;



	matlabplotter.plotfreqscan(voltage_vector, DAQ_vector);


	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);

	voltage_vector.clear();
	DAQ_vector.clear();

	return 0;
};