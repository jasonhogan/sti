

#include "ENET_GPIB_device.h"
#include "Vortex6000.h"
#include "USB1408FS.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <windows.h>

#define PI 3.14159265

int main(int argc, char* argv[])

{
	double pv; // current piezo voltage
	double feedback_signal; // signal applied to current input on vortex controller from CsLock board
	double set_point_voltage = 6; // voltage level for abs(feedback_signal) above which increment piezo_voltage by piezo_adjustment
	double piezo_adjustment = 0.1; // amount to adjust piezo voltage when feedback_signal above threshold_voltage 

	double store_val1;
	double store_val2;
	double avg_signal;

	//Scan over Rb87 cooling line (1.12 GHz red of 85 cooling)
	// assumes laser is locked ~400 MHz offset from Rb85 cooling

	//initialize frequency generator
	Vortex6000 vortex6000;

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 6;

	vortex6000.what_is_my_name();
	while(1) {
		pv = vortex6000.get_piezo_voltage();
		std::cerr << "piezo voltage: " << pv << std::endl;
		feedback_signal = usb1408fs.read_input_channel(usb_channel);
		store_val1 = feedback_signal;

		std::cerr << "feedback_signal: " << feedback_signal << std::endl;
		Sleep(300); // wait 300 msec to make sure it isn't a transient
		
		
		feedback_signal = usb1408fs.read_input_channel(usb_channel); // avg over 2 measurements
		std::cerr << "feedback_signal: " << feedback_signal << std::endl;
		store_val2 = feedback_signal;

		avg_signal = (0.5)*(store_val1 + store_val2);
		std::cerr << avg_signal << std::endl;
		
		if(avg_signal > set_point_voltage) {
			pv = pv - piezo_adjustment;
			std::cerr << "**piezo voltage: " << pv << std::endl;
			vortex6000.set_piezo_voltage(pv);
		}

		if(avg_signal < (-1.0*set_point_voltage)) {
			pv = pv + piezo_adjustment;
			std::cerr << "**piezo voltage: " << pv << std::endl;
			vortex6000.set_piezo_voltage(pv);
		}
		Sleep(10000);
	}
	
	return 0;
};