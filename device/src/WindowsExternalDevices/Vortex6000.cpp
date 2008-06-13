/*************************************************************************
 *
 * Name:   HP83711B.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the Vortex 6000 Laser Controller via the ENET_GPIB_device interface
 *
 * David Johnson 6/13/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include <iostream>
#include <string>
#include <sstream>

#include "ENET_GPIB_device.h"
#include "Vortex6000.h"

//===========================================================================

Vortex6000::Vortex6000()
{
	//stuff	
	primary_address = 1;
	secondary_address = 0;

}

//===========================================================================

Vortex6000::~Vortex6000()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}


//===========================================================================


void Vortex6000::what_is_my_name()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*idn?", buffer, 100);
	std::cout << "Identification:" << std::endl;
	printf ("%s\n\n", buffer);
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, ":SYST:INF:SHO?", buffer, 100);
	std::cout << "Instrument Operating Hours:" << std::endl;
	printf ("%s\n\n", buffer);
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, ":SYST:INF:DHO?", buffer, 100);
	std::cout << "Laser Head Operating Hours:" << std::endl;
	printf ("%s\n\n", buffer);
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, ":SYST:INF:HWAV?", buffer, 100);
	std::cout << "Laser Head Wavelength:" << std::endl;
	printf ("%s\n\n", buffer);
	
}

//===========================================================================


void Vortex6000::get_piezo_voltage()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, ":SOUR:VOLT:PIEZ?", buffer, 100);
	printf ("%s\n\n", buffer);	
	
}

//===========================================================================

void Vortex6000::set_piezo_voltage(double piezo_voltage) 
{

	std::ostringstream convert_piezo_voltage;
	convert_piezo_voltage << piezo_voltage;
	std::string piezo_str = convert_piezo_voltage.str();

	std::string command_str = ":SOUR:VOLT:PIEZ " + piezo_str;
	
	char * command_char = new char[command_str.size()+1];

	strcpy(command_char,command_str.c_str());

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, command_char, buffer, 100);
	
}

//===========================================================================

