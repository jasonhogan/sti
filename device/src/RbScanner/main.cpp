

#include "ENET_GPIB_device.h"
#include "HP83711B.h"
#include "USB1408FS.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>

#define PI 3.14159265

int main(int argc, char* argv[])

{
	
	//Scan over Rb87 cooling line (1.12 GHz red of 85 cooling)
	// assumes laser is locked ~400 MHz offset from Rb85 cooling

	//initialize frequency generator
	HP83711B hp83711b;

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 6;

	//define loop variables
	double freq = 1; //frequency in GHz
	double freq_incr = .01; //increment frequency in GHz


	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(2);

	//define vectors for storing the data

	std::vector <double> DAQ_vector;
	std::vector <double> FREQ_vector;

	//take the modulation off data point
	FREQ_vector.push_back(0);
	DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel));

	//turn on the function generator
	hp83711b.output_on();

	//loop from 1 GHz to 2GHz
	while(freq <= 2) {

		FREQ_vector.push_back(freq); //record function generator frequency

		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel)); //take data

		// change the frequency
		hp83711b.increment_frequency_up();
		freq = freq + freq_incr;

		//std::cout << freq << std::endl;

		Sleep(20); //wait for the function generator to settle. spec'd time is 20ms

	}

for(int i=0;i < DAQ_vector.size(); i++)
{
	double stra = FREQ_vector.at(i);
	double strb = DAQ_vector.at(i);

	std::cout << stra << " , " << strb << std::endl;

}

	return 0;
};