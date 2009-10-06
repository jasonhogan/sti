#ifndef SERIALDATA_H
#define SERIALDATA_H

// Filename: SerialData.h
//
// Purpose: 

#include "types.h"

class SerialData
{
public:

	SerialData();
	SerialData(bool ADDR, bool CLK, bool DIN, bool EN);
	~SerialData();

	struct SerialBits
	{
		bool ADDR;			// Address line control bit	-- LSB
		bool CLK;			// Clock bit
		bool DIN;			// Serial data bit
		bool EN;			// Enable bit				-- MSB
	} bits;

	uInt32 getValue();		// DIO port serial communication value
	uInt32 getValue_ECDA();	//Alternate bit convention
	uInt32 getValue_EDCA();	//Alternate bit convention

private:


};


/*
#if DIOLatch16	// 4 or 5 serial bits depending on DIO Latch board
		int S_P		:1;					// Switch of serial communication direction
#endif
		int ADDR	:1;					// Address line control signal
		int CLK		:1;					// Clock signal 
		int	DIN		:1;					// Serial data input 
		int EN		:1;					// Enable byte
		int unused	:(32-kSerialLines);
	} bits;					
*/


#endif
