/*************************************************************************
 *
 * Name:   MARCONI2022D.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the MARCONI2022D signal generator via the ENET_GPIB_device interface
 *
 * Original Code, David Johnson 6/4/2008
 * Adapted, Susannah Dickerson 7/11/08
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include <iostream>
#include <string>
#include <sstream>

#include "ENET_GPIB_device.h"
#include "MARCONI2022D.h"



//===========================================================================

MARCONI2022D::MARCONI2022D()
{
	//stuff	
	primary_address = 18;
	secondary_address = 0;

	output_off();

}

//===========================================================================

MARCONI2022D::~MARCONI2022D()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}

//===========================================================================

void MARCONI2022D::set_frequency(double freqGHz)
{

	std::ostringstream convert_freq;
	convert_freq << freqGHz * 1000;
	std::string freq_str = convert_freq.str();

	std::string command_str = "CF " + freq_str + " MZ";

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
}

//===========================================================================

void MARCONI2022D::what_is_my_name()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "SF 5", buffer, 100);
	printf ("%s\n\n", buffer);
	//ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*opt?", buffer, 100);
	//printf ("%s\n\n", buffer);


	
}

//===========================================================================

void MARCONI2022D::get_frequency()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "QU CF", buffer, 100);
	printf ("%s\n\n", buffer);	
	
}

//===========================================================================

void MARCONI2022D::get_freq_increment() 
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "QU DE CF", buffer, 100);
	printf ("%s\n\n", buffer);

	
}

//===========================================================================

void MARCONI2022D::set_freq_increment(double freq_incrGHz) 
{

	std::ostringstream convert_freq;
	convert_freq << freq_incrGHz * 100;
	std::string freq_str = convert_freq.str();

	std::string command_str = "DE CF " + freq_str + " MZ";
	
	//char * command_char = new char[command_str.size()+1];

	//strcpy_s(command_char, strlen(command_char), command_str.c_str());

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
}

//===========================================================================

void MARCONI2022D::increment_frequency_up() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "DE CF UP", buffer, 100);
	
}

//===========================================================================

void MARCONI2022D::increment_frequency_down()
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "De CF DN", buffer, 100);
	
	
}

//===========================================================================

void MARCONI2022D::set_power(double power)
{

	std::ostringstream convert_power;
	convert_power << power;
	std::string power_str = convert_power.str();

	std::string command_str = "LV " + power_str + " DB";
	
	//char * command_char = new char[command_str.size()+1];

	//strcpy_s(command_char, strlen(command_char), command_str.c_str());


	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
	
}

//===========================================================================

void MARCONI2022D::get_power()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "QU LV", buffer, 100);
	printf ("%s\n\n", buffer);	
	
}

//===========================================================================

void MARCONI2022D::output_on() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "C1", buffer, 100);
	
}

//===========================================================================

void MARCONI2022D::output_off() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "C0", buffer, 100);
	
}

//===========================================================================