/*************************************************************************
 *
 * Name:   AGILENT8648A.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the AGILENT8648A signal generator via the ENET_GPIB_device interface
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include <iostream>
#include <string>
#include <sstream>

#include "ENET_GPIB_device.h"
#include "AGILENT8648A.h"



//===========================================================================

AGILENT8648A::AGILENT8648A(double maxPower)
{
	//stuff	
	primary_address = 19;
	secondary_address = 0;
	maximumAllowedPower = maxPower;

	//output_off();

}

//===========================================================================

AGILENT8648A::~AGILENT8648A()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}

//===========================================================================

void AGILENT8648A::set_frequency(double frequency)
{
// frequency in GHZ!!
	std::ostringstream convert_freq;
	convert_freq << (frequency);
	std::string freq_str = convert_freq.str();

	std::string command_str = "FREQ:CW " + freq_str + " MHZ";

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
}

//===========================================================================

void AGILENT8648A::what_is_my_name()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*idn?", buffer, 100);
	printf ("%s\n\n", buffer);
	//ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*opt?", buffer, 100);
	//printf ("%s\n\n", buffer);


	
}

//===========================================================================

double AGILENT8648A::get_frequency()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "FREQ:CW?", buffer, 100);
	printf ("%s\n\n", buffer);

	double frequency = (atof(buffer)) / 1e6;

	return frequency;
	
}

//===========================================================================

void AGILENT8648A::set_power(double power)
{

	if(power <= maximumAllowedPower)
	{
		std::ostringstream convert_power;
		convert_power << power;
		std::string power_str = convert_power.str();

		std::string command_str = "POW:AMPL " + power_str + " DBM";
	
		//char * command_char = new char[command_str.size()+1];

		//strcpy_s(command_char, strlen(command_char), command_str.c_str());


		ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	}
	else
		std::cerr << "Command power," << power << " dBm, is too high. Please choose a value less than " << maximumAllowedPower << " dBm." << std::endl;
	
	
}

//===========================================================================

double AGILENT8648A::get_power()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "POW:AMPL?", buffer, 100);
	printf ("%s\n\n", buffer);

	return atof(buffer);
	
}

//===========================================================================

void AGILENT8648A::get_output_state()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "OUTP:STAT?", buffer, 100);
	
	
	printf ("%s\n\n", buffer);	
	
}

//===========================================================================

void AGILENT8648A::output_on() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "OUTP:STAT ON", buffer, 100);
	
}

//===========================================================================

void AGILENT8648A::output_off() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "OUTP:STAT OFF", buffer, 100);
	
}

//===========================================================================