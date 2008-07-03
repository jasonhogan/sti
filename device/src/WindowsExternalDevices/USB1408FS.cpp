/*************************************************************************
 *
 * Name:   USB1408FS.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control the Measurement Computing USB-1408FS DAQ
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "USB1408FS.h"

//===========================================================================

USB1408FS::USB1408FS()
{
	//stuff	
	
	BoardNum = 0;
    UDStat = 0;
    channel = 6;
    inputGain = BIP10VOLTS;
	outputGain = UNI4VOLTS;
    DataValue = 0;
	Options = DEFAULTOPTION;
        
    RevLevel = (float)CURRENTREVNUM;


//	std::cerr << "What is the BoardNumber?" << BoardNum << std::endl;

	// Declare UL Revision Level 
	UDStat = cbDeclareRevision(&RevLevel);

	/* Initiate error handling
       Parameters:
           PRINTALL :all warnings and errors encountered will be printed
           DONTSTOP :program will continue even if error occurs.
                     Note that STOPALL and STOPFATAL are only effective in 
                     Windows applications, not Console applications. 
   */
    cbErrHandling (PRINTALL, DONTSTOP);


}

//===========================================================================

double USB1408FS::read_input_channel(int channel) 
{

	
	UDStat = cbVIn (BoardNum, channel, inputGain, &DataValue, Options);

	/*
	if(UDStat == NOERRORS)
		{
		std::cout << "\nThe voltage on Channel" << channel << "is: " << DataValue << std::endl;
		}
	*/

	return DataValue;
}

//===========================================================================

void USB1408FS::set_output_voltage(int channel, float output_voltage)
{

	
	UDStat = cbVOut (BoardNum, channel, outputGain, output_voltage, Options);


}
