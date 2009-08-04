/*************************************************************************
 *
 * Name:   HP83711B.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the HP83711B signal generator via the ENET_GPIB_device interface
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
#include "HP83711B.h"



//===========================================================================

HP83711B::HP83711B(int primaryAddress, int secondaryAddress, double userMaximumPower)
{
	//stuff	
	primary_address = primaryAddress; //usually 16
	secondary_address = secondaryAddress; //usually 0
	maximumOutputPower = userMaximumPower;

	output_off();

}

//===========================================================================

HP83711B::~HP83711B()
{
	//stuff	
	ENET_GPIB_device::Close_Handles();

}

//===========================================================================

void HP83711B::set_frequency(double frequencyInGHz)
{

	std::ostringstream convert_freq;
	convert_freq << frequencyInGHz;
	std::string freq_str = convert_freq.str();

	std::string command_str = "FREQ:CW " + freq_str + " GHZ";
	
	//char * command_char = new char[command_str.size()+1];

	//strcpy_s(command_char, strlen(command_char), command_str.c_str());

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
}

//===========================================================================

void HP83711B::what_is_my_name()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*idn?", buffer, 100);
	printf ("%s\n\n", buffer);
	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "*opt?", buffer, 100);
	printf ("%s\n\n", buffer);


	
}

//===========================================================================

double HP83711B::get_frequency()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "FREQ:CW?", buffer, 100);
	printf ("%s\n\n", buffer);	

	return atof(buffer);
	
}

//===========================================================================

void HP83711B::get_freq_increment() 
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "FREQ:CW:STEP:INCR?", buffer, 100);
	printf ("%s\n\n", buffer);

	
}

//===========================================================================

void HP83711B::set_freq_increment(double freq_increment) 
{

	std::ostringstream convert_freq;
	convert_freq << freq_increment;
	std::string freq_str = convert_freq.str();

	std::string command_str = "FREQ:STEP:INCR " + freq_str + " GHZ";
	
	//char * command_char = new char[command_str.size()+1];

	//strcpy_s(command_char, strlen(command_char), command_str.c_str());

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	
}

//===========================================================================

void HP83711B::increment_frequency_up() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "FREQ:CW UP", buffer, 100);
	
}

//===========================================================================

void HP83711B::increment_frequency_down()
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, "FREQ:CW DOWN", buffer, 100);
	
	
}

//===========================================================================

void HP83711B::set_power(double power)
{

	if(power <= maximumOutputPower)
	{
		std::ostringstream convert_power;
		convert_power << power;
		std::string power_str = convert_power.str();

		std::string command_str = ":POW:LEV " + power_str + " dBm";

		ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, const_cast<char*>(command_str.c_str()), buffer, 100);
	}
	else
		std::cerr << "Command power," << power << " dBm, is too high. Please choose a value less than " << maximumOutputPower << " dBm." << std::endl;
	
}

//===========================================================================

double HP83711B::get_power()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "POW:LEV?", buffer, 100);
	printf ("%s\n\n", buffer);	

	return atof(buffer);
	
}

//===========================================================================

void HP83711B::output_on() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, ":POW:STAT ON", buffer, 100);
	
}

//===========================================================================

void HP83711B::output_off() 
{

	ENET_GPIB_device::Command_Device (GPIBinterface, primary_address, secondary_address, ":POW:STAT OFF", buffer, 100);
	
}

//===========================================================================
void HP83711B::get_output_state()
{

	ENET_GPIB_device::Query_Device (GPIBinterface, primary_address, secondary_address, "POW:STAT?", buffer, 100);
	printf ("%s\n\n", buffer);	
	
}