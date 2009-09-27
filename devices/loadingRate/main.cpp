

#include "ENET_GPIB_device.h" //functions to used the ENET GPIB interface
#include "HP83711B.h" //definition of the HP83711B function generator class
#include "USB1408FS.h" //definition of the USB1408FS class
#include "Matlab.h"

#include <iostream> //cin & cout commands
#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage
#include <time.h>


int main(int argc, char* argv[])

{
	MATLABPLOTTER matlabplotter;
	USB1408FS usb1408fs;
	HP83711B hp83711b(16, 0, 5);

	clock_t startTime;
	clock_t endTime;

	std::vector <double> DAQ_vector;
	std::vector <double> time_vector;

	while(1)
	{
		// this goes forever
		// step 1. switch repump on & off
		hp83711b.output_off();
		hp83711b.output_on();
		// step 2. scan for 1 second
		startTime = clock();\
		endTime = startTime + 1 * CLOCKS_PER_SEC; 
		while( difftime (endTime, clock()) > 0 )
		{
			time_vector.push_back( difftime(clock(), startTime) );
			DAQ_vector.push_back( usb1408fs.read_input_channel(6) );
			// we take data as quick as we can for 1 second
		}

		matlabplotter.plotfreqscan(time_vector, DAQ_vector);
		time_vector.clear();
		DAQ_vector.clear();

	}


	/*
	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);
	*/

	return 0;
};