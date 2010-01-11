

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
	double pv; // program piezo voltage
	double measured_pv; //piezo voltage from controller
	double feedback_signal = 0; // signal applied to current input on vortex controller from CsLock board
	double set_point_voltage = 3.5; // voltage level for abs(feedback_signal) above which increment piezo_voltage by piezo_adjustment
	double piezo_adjustment = 0.1; // amount to adjust piezo voltage when feedback_signal above threshold_voltage 

	double avg_signal;

	//Scan over Rb87 cooling line (1.12 GHz red of 85 cooling)
	// assumes laser is locked ~400 MHz offset from Rb85 cooling

	//initialize frequency generator
	Vortex6000 vortex6000;

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 6;

	int i;

	vortex6000.what_is_my_name(); // for the heck of it

	measured_pv = vortex6000.get_piezo_voltage();
	pv = measured_pv;

	//	int x; // for cin break

	while(1) {
		measured_pv = vortex6000.get_piezo_voltage();
		if (pv != measured_pv) {
			std::cerr << "Discrepancy between measured & expected piezo voltage." << std::endl;
			std::cerr << "Measured PV: " << measured_pv << std::endl;
		}

		std::cerr << "piezo voltage: " << pv << std::endl;
		avg_signal = 0;

		while( fabs(avg_signal) < set_point_voltage ) {
			
			for(i=1; i < 10; i++) {
				feedback_signal = feedback_signal + usb1408fs.read_input_channel(usb_channel);
				Sleep(100);
			}

			avg_signal = 0.1 * feedback_signal;
			feedback_signal = 0;

			std::cerr << "avg_signal: " << avg_signal << std::endl;
		}
		
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

	}
	
	return 0;
};
