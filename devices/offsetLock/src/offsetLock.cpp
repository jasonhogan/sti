/*! \file offsetLock.cpp
 *  \author David M.S. Johnson (from GradEmbed base code)
 *  \brief Source-file for the class offsetLock
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "offsetLock.h"

#include <iostream>
using namespace std;


offsetLock::offsetLock(std::string deviceName, int Address) : SerialDevice(deviceName, Address)
{
	//LMX2326
	set_R_Counter = 0x0;
	set_N_Counter = 0x1;
	set_Function = 0x2;
	LMX2326location = 0;
	LMX2326_bEnablePolarity = false;
	LMX2326_numTotalBits = 21; //correct from datasheet 12/21/2009 DJ
	
	//MAX509
	MAX509_numTotalBits = 12;
	MAX509_bEnablePolarity = false;  // Data latched on rising edge of /CS ("EN")
	MAX509location = LMX2326_numTotalBits;

	//HC594
	HC594_numTotalBits = 8;
	HC594_bEnablePolarity = false;  // Data latched on rising edge of CS# ("EN")
	HC594location = (MAX509location + MAX509_numTotalBits);

	//for communication
	OffsetLockMod_numTotalBits = LMX2326_numTotalBits + MAX509_numTotalBits + HC594_numTotalBits;

	//Initialize PLL parameters
	P = 32;
	intGain = 0;
	proGain = 0;
	OLAddress = Address;
	compareFreq_MHz = 0.001;
	bLockEnable = false;

	SerBuf.resize(OffsetLockMod_numTotalBits, SerialData(0,0,0,0));	//default serial buffer size is 32 elements - laser lock board may a longer buffer

}
std::string offsetLock::deviceType() const
{
	return "Offset Lock";
}

bool offsetLock::enableOffsetLock(bool enable, bool iEnable, double servoFrequency, double iGain, double pGain)
{
	//are we going to be enabled?
	bLockEnable = enable;

	//check that gains are okay
	int iGainInternal = (int)(iGain * 255);
	int pGainInternal = (int)(pGain * 255);
	if( (iGainInternal > 255) || (iGainInternal < 0) || (pGainInternal > 255) || (pGainInternal < 0) )
		return false;

	//set internal variable to external desired servo frequency
	lockFreq_MHz = servoFrequency;

	if (lockFreq_MHz < 0.0){
		lockFreq_MHz = -1 * (lockFreq_MHz);
		positiveDetuning = false;	// negative detuning
	}
	else
		positiveDetuning = true;	// positive detuning

	RCounter = (int) (10.0/compareFreq_MHz);	// 10 MHz oscillator
	NCounter = (int)(lockFreq_MHz/compareFreq_MHz);	// cast to int
	BCounter = (int) (NCounter / P);
	ACounter = NCounter - BCounter * P;

	if (ACounter > 31 || BCounter < 3 || BCounter > 16383 || RCounter < 3 || RCounter > 16383 || (ACounter > BCounter) )
	{
		std::cerr << "Invalid scaler values" << std::endl;
		std::cerr << "A Counter: " << ACounter << std::endl;
		std::cerr << "B Counter: " << BCounter << std::endl;
		std::cerr << "R Counter: " << RCounter << std::endl;

		return false;
	}

	if (bLockEnable){
		LMX2326_FunctionLatch( ((positiveDetuning ? 1 : 0)<<5) + 1, LMX2326location); //apply reset latch
		MAX509_SetOutputMode(0, MAX509location);
		HC594_OffsetUnLock(HC594location);
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);
		
		LMX2326_SetNCounter(ACounter, BCounter, LMX2326location);
		MAX509_SetPropGain(0, MAX509location);		
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);

		std::cerr << "A Counter: " << ACounter << std::endl;
		std::cerr << "B Counter: " << BCounter << std::endl;

				
		MAX509_SetIntGain(0, MAX509location);
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);	
		
		LMX2326_SetRCounter(RCounter, LMX2326location);
		HC594_OutEnable(HC594location);	
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);

		std::cerr << "R Counter: " << RCounter << std::endl;

		LMX2326_FunctionLatch( ((positiveDetuning ? 1 : 0)<<5) , LMX2326location);	// set the reset latch to 0
				
		for (proGain=0; proGain<255; proGain++){	
			// increase the proportional gain to max
			MAX509_SetPropGain(proGain, MAX509location);
			DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);
 		}
		
		HC594_OffsetLock(HC594location);
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);
		   	
   		for (intGain=0; intGain <= iGainInternal; intGain++){
			// increase the integral gain to final value
			//std::cerr << "I Gain: " << intGain << std::endl;
			MAX509_SetIntGain(intGain, MAX509location);
			DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);           				
 		}
		
		for (proGain=255; proGain >= pGainInternal; proGain--){	
			// decrease the proportional gain to final value
			//std::cerr << "P Gain: " << proGain << std::endl;
			MAX509_SetPropGain(proGain, MAX509location);
			DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);		
 		}		
		std::cerr << "Raman Offset lock set" << std::endl;
		
		
		
	}
	else 
	{	// unlock
		LMX2326_FunctionLatch(0X10, LMX2326location);
		MAX509_SetPropGain(255, MAX509location);
		HC594_OffsetUnLock(HC594location);
		DSerialSendData(OLAddress, OffsetLockMod_numTotalBits);
	}			

	return true;
}
int offsetLock::LMX2326_FunctionLatch(int function, int location)
{
	return DSerialBufferBuild( (function<<2)+set_Function, LMX2326_numTotalBits, location, LMX2326_bEnablePolarity);
}
int offsetLock::LMX2326_SetRCounter(int RCounter, int location)
{
	return DSerialBufferBuild( (RCounter<<2)+set_R_Counter, LMX2326_numTotalBits, location, LMX2326_bEnablePolarity);
}
int offsetLock::LMX2326_SetNCounter(int ACounter, int BCounter, int location)
{
	int NCounterBits;
	NCounterBits = (BCounter<<5) + ACounter;
	return DSerialBufferBuild( (NCounterBits<<2)+set_N_Counter, LMX2326_numTotalBits, location, LMX2326_bEnablePolarity);
}
int offsetLock::HC594_SetAll(int bits, int location) // HC594_SetAll() - specify all 8 bits
{	
	return DSerialBufferBuild(bits, HC594_numTotalBits, location, HC594_bEnablePolarity);
}
int offsetLock::HC594_OutEnable(int location)
{
	return DSerialBufferBuild(0X2, HC594_numTotalBits,location, HC594_bEnablePolarity);
}

int offsetLock::HC594_OffsetLock(int location)
{	
	return DSerialBufferBuild(0X3, HC594_numTotalBits, location, HC594_bEnablePolarity);
}
int offsetLock::HC594_OffsetUnLock(int location)
{
	return DSerialBufferBuild(0X0, HC594_numTotalBits, location, HC594_bEnablePolarity);
}
void offsetLock::HC594_SwitchChannel(int channelNumber, bool switchMode, int location)
{
	// This function switchs channel 0,1,2,3 independently between 180 Mhz RF 
	// source and DDS RF source. 
	//
	// when switchMode=0, the corresponding channel was set to 180Mhz
	// when switchMode=1, the corresponding channel was set to DDS
	//
	// The parameter 'location' denotes the first bit position for HC594.
	//
   int onchipLocation;
   
   switch(channelNumber)
   {
   		case 0:
   		case 1:
   			onchipLocation=location;
   			break;
   		case 2:
   			onchipLocation=location+3;
   			break;
   		case 3:
   			onchipLocation=location+4;
   			break;
   		default:
   			cout << endl << "DSP:HC594_SwitchChannel() - invalid channel number" << endl;
   			return;
   	}
	
	DSerialBufferBuild(switchMode, 1, onchipLocation, HC594_bEnablePolarity);
}	
void offsetLock::HC594_SetChannel(int channelNumber, bool controlMode, int location)
{
	// This function sets the control mode for channel 0,1,2,3,4 
	// independently.
	//
	// when controlMode=0, Attenuation mode for the corresponding channel
	// when controlMode=1, Modulation mode for the corresponding channel 
	//
	// The parameter 'location' denotes the first bit position for HC594.
	//
   int onchipLocation;
   
   switch(channelNumber)
   {
   		case 0:
   			onchipLocation=location+1;
   			break;
   		case 1:
   			onchipLocation=location+2;
   			break;
   		case 2:
   			onchipLocation=location+5;
   			break;
   		case 3:
   			onchipLocation=location+6;
   			break;
   		case 4:
   			onchipLocation=location+7;
   			break;
   		default:
   			
			cout << endl << "DSP:HC594_SwitchChannel() - invalid channel number" << endl;

			//PrintHost("DSP:HC594_SwitchChannel() - invalid channel number");
   			return;
   	}
	
	DSerialBufferBuild(controlMode, 1, onchipLocation, HC594_bEnablePolarity);
}	
int offsetLock::MAX509_LoadDACInput(int chan, int data, int location)
{
	// MAX509_LoadDACInput() - load input but do not set DAC
	// chan: 0,1,2,3 for A,B,C,D, or enum MAX509_A etc. [A1 A0 0 1 D7..D0]
	return DSerialBufferBuild((chan<<10)+0X100+data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_LDAC(int location)
{
	// MAX509_LDAC() - update all DACs from respective input registers
	return DSerialBufferBuild(0X200, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_LoadInputSetDAC(int chan, int data, int location)
{
	// MAX509_LoadInputSetDAC() - load input and set DAC
	// chan: 0,1,2,3 for A,B,C,D, or enum MAX509_A etc. [A1 A0 1 1 D7..D0]

	return DSerialBufferBuild((chan<<10)+0X300+data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}

int offsetLock::MAX509_SetAllDAC(int data, int location) // MAX509_SetAllDAC() - update all DACs from shift register
{
	return DSerialBufferBuild(data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_NOP(int location) // MAX509_NOP() - no operation (NOP), shifts data in shift register
{
	return DSerialBufferBuild(0X400, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_SetPropGain(int propGain, int location)
{
	return DSerialBufferBuild(0X300+propGain, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_SetIntGain(int intGain, int location)
{
	return DSerialBufferBuild(0X700+intGain, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}
int offsetLock::MAX509_SetOutputMode(int mode, int location)
{
	return DSerialBufferBuild( (mode==0)? 0XA00 : 0XE00, MAX509_numTotalBits, location, MAX509_bEnablePolarity);;
}
