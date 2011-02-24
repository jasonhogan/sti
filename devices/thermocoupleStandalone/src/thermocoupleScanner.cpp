/*************************************************************************
 *
 * Name:   thermocoupleScanner.cpp
 *
 * C++ Windows code for reading in thermocouples from measurement computing mux
 *
 * David Johnson 2/23/2011
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "thermocoupleScanner.h" 

//===========================================================================

thermocoupleScanner::thermocoupleScanner()
{
	//initialize USB-1408FS DAQ

	#ifndef USB_DAQ
	#define USB_DAQ
		usb1408fs0 = new USB1408FS(0);
		usb1408fs1 = new USB1408FS(1);
	#endif

}

thermocoupleScanner::~thermocoupleScanner() {}

//===========================================================================

void thermocoupleScanner::scanThermocouples(std::vector <double> &thermocoupleVoltages)
{
	// read CJC voltage
	double cjcVoltage0 = usb1408fs0->read_input_channel(2);
	
	double gain = 0.7; // g = 700, 1000 mV in a V; converts measurement into mV

	// define number of channels to scan over
	int startChannel0 = 0;
	int endChannel0 = 31;
	int channelIterator = startChannel0;

	//define fix for channels 0-19 wired backwards
	double voltageModifier = -1;

	//loop from start freq to start + interval
	while(channelIterator <= endChannel0) 
	{
		if(channelIterator <= 19)
			voltageModifier = -1;
		else
			voltageModifier = 1;

		thermocoupleVoltages.push_back( voltageModifier*(usb1408fs0->read_MUX_input_channel(channelIterator)) / gain ); //take data
		//change the channel
		channelIterator++;
		Sleep(50); //wait for the usb daq to settle
	}	

	//read board 1 cjc
	double cjcVoltage1 = usb1408fs1->read_input_channel(2);
	int startChannel1 = 0;
	int endChannel1 = 22;
	channelIterator = startChannel1;

	//loop from start freq to start + interval
	while(channelIterator <= endChannel1) 
	{
		if(channelIterator <= 7)
			voltageModifier = -1;
		else
			voltageModifier = 1;
		thermocoupleVoltages.push_back(voltageModifier*(usb1408fs1->read_MUX_input_channel(channelIterator)) / gain); //take data
		// change the channel
		channelIterator++;
		Sleep(50); //wait for the usb daq to settle
	}	
}