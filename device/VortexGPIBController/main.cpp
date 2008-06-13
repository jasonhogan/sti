

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
	
	//Scan over Rb87 cooling line (1.12 GHz red of 85 cooling)
	// assumes laser is locked ~400 MHz offset from Rb85 cooling

	//initialize frequency generator
	Vortex6000 vortex6000;

	//initialize USB-1408FS DAQ
	//USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 6;

	vortex6000.what_is_my_name();

	vortex6000.get_piezo_voltage();

	//vortex6000.set_piezo_voltage(61.8);

	//vortex6000.get_piezo_voltage();

	return 0;
};