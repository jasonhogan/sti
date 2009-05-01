/*! \file CsLock.cpp
 *  \author David M.S. Johnson (from GradEmbed base code)
 *  \brief Source-file for the class CsLock
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

#include "CsLock.h"

#include <iostream>
using namespace std;


//*************************************************************************
// Public Functions

CsLock::CsLock(std::string deviceName, int Address) : SerialDevice(deviceName, Address)
{

	HC594_numTotalBits = 8;

	HC594_bEnablePolarity = false;  // Data latched on rising edge of CS# ("EN")


	// 13-Bit Serial DACs with Force/Sense Connection, 10 ppm/C Internal Reference
	// Used in:  Cs Lock Board;

	MAX5132_bEnablePolarity = false;  // Data latched on rising edge of /CS ("EN")
	MAX5132_numTotalBits = 16;
	MAX5132_13bitMax = 8191;


	//	The following Control Codes are ADDED to the requested DAC voltage to
	//	create the proper command bitstream 

	MAX5132_NOP 	= 0X0000;		//		No op
	MAX5132_LDVdac 	= 0X2000;		//		Load DAC input register only
	MAX5132_LDAll 	= 0X4000;		//		Load DAC input register and DAC current register
	MAX5132_UPOHi 	= 0XC000;		//		Set User Prog Output Hi
	MAX5132_UPOLo	= 0X8000;		//		Set User Prog Output Lo
	
	//MAX509 chip control stuff

	MAX509_numTotalBits = 12;
	MAX509_bEnablePolarity = false;  // Data latched on rising edge of /CS ("EN")


	//CsLock constants

//	LaserTypeF3 = 0;
//	LaserTypeF4 = 1;
	TotalRegLength = (2*(MAX509_numTotalBits+MAX5132_numTotalBits)+HC594_numTotalBits);
	F4RampVoltageLocation = 0;
	F3RampVoltageLocation = (F4RampVoltageLocation + MAX5132_numTotalBits);
	F4GainsLocation = (F3RampVoltageLocation + MAX5132_numTotalBits);
	F3GainsLocation = (F4GainsLocation + MAX509_numTotalBits);
	CtrlBitsLocation = (F3GainsLocation + MAX509_numTotalBits);

	SerBuf.resize(TotalRegLength, SerialData(0,0,0,0));	//default serial buffer size is 32 elements - laser lock board may a longer buffer

	//Intialize control bits: enable the output, bypass the integrators for F3;
	//		disable the output, bypass the integrators for F4.
	CsCtrl.F3Int1 =			1;
	CsCtrl.F3Int2 =			0;
	CsCtrl.F3OutEnable =		1;
	CsCtrl.Unused1 =		0;
	CsCtrl.F4Int1 =			0;
	CsCtrl.F4Int2 =			0;
	CsCtrl.F4OutEnable =		1;
	CsCtrl.Unused2 =		0;

	//Determines which lock circuit the user is currently able to adjust. See main.
	whichCircuit = LaserTypeF3;
}


std::string CsLock::deviceType() const
{
	return "Cesium Lock";
}


//-------------------------------------------------------------------------
// getControlValue - converts the control bits of CsCtrl into binary.
//-------------------------------------------------------------------------

int CsLock::getControlValue()
{
	int value;

	value = CsCtrl.F3Int1 + 2*CsCtrl.F3Int2 + 4*CsCtrl.F3OutEnable + 
		8*CsCtrl.Unused1 + 16*CsCtrl.F4Int1 + 32*CsCtrl.F4Int2 + 
		64*CsCtrl.F4OutEnable + 128*CsCtrl.Unused2;

	return value;
}

//-------------------------------------------------------------------------
// setControlBits - opens (0 output, 1 integrators) or closes (1 output,
// 0 integrators) the F3 and F4 switches according to getControlValue(),
// which can be set by the user.
//-------------------------------------------------------------------------
void CsLock::setControlBits()
{
	MAX5132_CmdNOP(F4RampVoltageLocation);
	MAX5132_CmdNOP(F3RampVoltageLocation);
	MAX509_NOP(F4GainsLocation);
	MAX509_NOP(F3GainsLocation);

	HC594_SetAll(getControlValue(), CtrlBitsLocation); 
	
	DSerialSendData(address(), TotalRegLength);
}

double CsLock::gainRangeCheck(double newGain, double minGain, double maxGain)
{
	double gain = newGain;

	if(newGain < minGain)
	{
		gain = minGain;
	}
	else if(newGain > maxGain)
	{
		gain = maxGain;
	}

	return gain;
}


//-------------------------------------------------------------------------
// setDiffGain - sets the differential gain.  fracGain is the fraction of
// the maximum gain to apply. 0 <= fracGain <= 1.
//-------------------------------------------------------------------------

void CsLock::setGain(int circuit, int gainType, double fracGain)
{

	double newGain = gainRangeCheck(fracGain, 0, 1);

	int VDAC;

	switch(circuit)
	{
	case LaserTypeF3:
	case LaserTypeF4:
		//Convert gain to a binary number.
		VDAC = int (255 * newGain);
		switch(gainType)
		{
		case DiffGain: 
			gains[circuit].diffGain = newGain;
			break;
		case PropGain: 
			gains[circuit].propGain = newGain;
			break;
		case Int1Gain: 
			gains[circuit].int1Gain = newGain;
			break;
		case Int2Gain: 
			gains[circuit].int2Gain = newGain;
			break;
		default:
			cerr << "Invalid gainType = " << gainType << endl;
			break;
		}

		CsLockSetGain(gainType, VDAC, circuit, address());
		break;
	default:
		cerr << "Invalid circuit number: " << circuit << endl;
		break;
	}

}

void CsLock::setDiffGain(double fracGain)
{
	setGain(whichCircuit, DiffGain, fracGain);
}


//-------------------------------------------------------------------------
// setPropGain - sets the proportional gain.  fracGain is the fraction of
// the maximum gain to apply. 0 <= fracGain <= 1.
//-------------------------------------------------------------------------

void CsLock::setPropGain(double fracGain)
{
	setGain(whichCircuit, PropGain, fracGain);
}

//-------------------------------------------------------------------------
// setInt1Gain - sets the integral 1 gain.  fracGain is the fraction of
// the maximum gain to apply. 0 <= fracGain <= 1.
//-------------------------------------------------------------------------

void CsLock::setInt1Gain(double fracGain)
{
	setGain(whichCircuit, Int1Gain, fracGain);
}


//-------------------------------------------------------------------------
// setInt2Gain - sets the integral 2 gain.  fracGain is the fraction of
// the maximum gain to apply. 0 <= fracGain <= 1.
//-------------------------------------------------------------------------

void CsLock::setInt2Gain(double fracGain)
{
	setGain(whichCircuit, Int2Gain, fracGain);
}



//-------------------------------------------------------------------------
// setOffset - sets the offset.  fracGain is the fraction of
// the gain to apply. -1 <= fracGain <= 1. fracGain = 0 means 0 offset.
//-------------------------------------------------------------------------

void CsLock::setOffset(double fracGain)
{
	double newGain = gainRangeCheck(fracGain,-1, 1);

	gains[whichCircuit].offset = newGain;

	//Convert gain to a binary number.
	int VDAC = int(MAX5132_13bitMax * (1 + newGain) / 2);
	
	CsLockSetRampVoltage(VDAC, whichCircuit, address());
}

//-------------------------------------------------------------------------
// setOffsetFast - sets the offset.  fracGain is the fraction of
// the maximum gain to apply. -1 <= fracGain <= 1. fracGain = 0.5 means 0 
// offset. Leaves the rest of the buffer untouched.
//-------------------------------------------------------------------------

void CsLock::setOffsetFast(double fracGain)
{
	//Convert gain to a binary number.
	int VDAC = int(MAX5132_13bitMax * (1 + fracGain) / 2);

	CsLockSetRampVoltageFast(VDAC, whichCircuit, address());
}


double CsLock::getGain(int circuit, int gainType)
{
	double gain = 0;

	switch(gainType)
	{
	case DiffGain: 
		gain = gains[circuit].diffGain;
		break;
	case PropGain: 
		gain = gains[circuit].propGain;
		break;
	case Int1Gain: 
		gain = gains[circuit].int1Gain;
		break;
	case Int2Gain: 
		gain = gains[circuit].int2Gain;
		break;
	case OffsetGain: 
		gain = gains[circuit].offset;
		break;
	default:
		cerr << "Invalid gainType = " << gainType << endl;
		break;
	}

	return gain;

}

double CsLock::getDiffGain()   {return getDiffGain(whichCircuit);}
double CsLock::getPropGain()   {return getPropGain(whichCircuit);}
double CsLock::getInt1Gain()   {return getInt1Gain(whichCircuit);}
double CsLock::getInt2Gain()   {return getInt2Gain(whichCircuit);}
double CsLock::getOffset()     {return getOffset(whichCircuit);}
bool CsLock::getOutputEnable() {return getOutputEnable(whichCircuit);}
bool CsLock::getInt1Enable()   {return getInt1Enable(whichCircuit);}
bool CsLock::getInt2Enable()   {return getInt2Enable(whichCircuit);}



double CsLock::getDiffGain(int circuit)	{return getGain(circuit, DiffGain);}
double CsLock::getPropGain(int circuit) {return getGain(circuit, PropGain);}
double CsLock::getInt1Gain(int circuit) {return getGain(circuit, Int1Gain);}
double CsLock::getInt2Gain(int circuit) {return getGain(circuit, Int2Gain);}
double CsLock::getOffset(int circuit)   {return getGain(circuit, OffsetGain);}

bool CsLock::getOutputEnable(int circuit)
{
	bool enable;

	if(circuit == LaserTypeF3)
	{
		enable = CsCtrl.F3OutEnable;
	}
	else
	{
		enable = CsCtrl.F4OutEnable;
	}

	return enable;
}

bool CsLock::getInt1Enable(int circuit)
{
	bool enable;

	if(circuit == LaserTypeF3)
	{
		enable = CsCtrl.F3Int1;
	}
	else
	{
		enable = CsCtrl.F4Int1;
	}

	return enable;
}

bool CsLock::getInt2Enable(int circuit)
{
	bool enable;

	if(circuit == LaserTypeF3)
	{
		enable = CsCtrl.F3Int2;
	}
	else
	{
		enable = CsCtrl.F4Int2;
	}

	return enable;
}



void CsLock::setOutputEnable(bool enable)
{
	if(whichCircuit == LaserTypeF3)
	{
		CsCtrl.F3OutEnable = enable;
	}
	else
	{
		CsCtrl.F4OutEnable = enable;
	}
	
	setControlBits();
}

void CsLock::setInt1Enable(bool enable)
{
	if(whichCircuit == LaserTypeF3)
	{
		CsCtrl.F3Int1 = enable;
	}
	else
	{
		CsCtrl.F4Int1 = enable;
	}
	
	setControlBits();
}

void CsLock::setInt2Enable(bool enable)
{
	if(whichCircuit == LaserTypeF3)
	{
		CsCtrl.F3Int2 = enable;
	}
	else
	{
		CsCtrl.F4Int2 = enable;
	}
	
	setControlBits();
}


//*************************************************************************
// Private Functions
//*************************************************************************


//-------------------------------------------------------------------------
// HC594_SetAll() - specify all 8 bits
//-------------------------------------------------------------------------

int CsLock::HC594_SetAll(int bits, int location)
{	
	return DSerialBufferBuild(bits, HC594_numTotalBits, location, HC594_bEnablePolarity);
	
}


//-------------------------------------------------------------------------
// HC594_OutEnable()
//-------------------------------------------------------------------------

int CsLock::HC594_OutEnable(int location)
{
	return DSerialBufferBuild(0X2, HC594_numTotalBits,location, HC594_bEnablePolarity);
}


//-------------------------------------------------------------------------
// HC594_OffsetLock()
//-------------------------------------------------------------------------

int CsLock::HC594_OffsetLock(int location)
{	
	return DSerialBufferBuild(0X3, HC594_numTotalBits, location, HC594_bEnablePolarity);
}


//-------------------------------------------------------------------------
// HC594_OffsetUnLock()
//-------------------------------------------------------------------------

int CsLock::HC594_OffsetUnLock(int location)
{
	return DSerialBufferBuild(0X0, HC594_numTotalBits, location, HC594_bEnablePolarity);
}


//-------------------------------------------------------------------------
// HC594_SwitchChannel()
//-------------------------------------------------------------------------
// This function switchs channel 0,1,2,3 independently between 180 Mhz RF 
// source and DDS RF source. 
//
// when switchMode=0, the corresponding channel was set to 180Mhz
// when switchMode=1, the corresponding channel was set to DDS
//
// The parameter 'location' denotes the first bit position for HC594.
//
void CsLock::HC594_SwitchChannel(int channelNumber, bool switchMode, int location)
{
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
	

//-------------------------------------------------------------------------
// HC594_SetChannel()
//-------------------------------------------------------------------------
// This function sets the control mode for channel 0,1,2,3,4 
// independently.
//
// when controlMode=0, Attenuation mode for the corresponding channel
// when controlMode=1, Modulation mode for the corresponding channel 
//
// The parameter 'location' denotes the first bit position for HC594.
//
void CsLock::HC594_SetChannel(int channelNumber, bool controlMode, int location)
{
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


//*************************************************************************
// Board driver functions follow

//-------------------------------------------------------------------------
// CsLockSetControlBits - initialize CsCtrl before first use.  Sets relevant bits, writes serial
// Addr is the CsLock board address.

void CsLock::CsLockSetControlBits(int integrator1,int integrator2,int outEn,int LaserLine,int Addr)
{
	MAX5132_CmdNOP(F4RampVoltageLocation);
	MAX5132_CmdNOP(F3RampVoltageLocation);
	MAX509_NOP(F4GainsLocation);
	MAX509_NOP(F3GainsLocation);
	
	if (LaserLine == LaserTypeF3){	// F=3
		CsCtrl.F3Int1 = integrator1;
		CsCtrl.F3Int2 = integrator2;	
		CsCtrl.F3OutEnable = outEn;
	}
	else {	// F=4
		CsCtrl.F4Int1 = integrator1;
		CsCtrl.F4Int2 = integrator2;		
		CsCtrl.F4OutEnable = outEn;
	}
	

	HC594_SetAll(getControlValue(), CtrlBitsLocation); 
	
	DSerialSendData(Addr, TotalRegLength);
}


//-------------------------------------------------------------------------
// CsLockSetGain - Sets specified gain value, writes serial
// use enumCsLockGain defined in header to specify whichGain.  data is gain value (8 bit)
// Addr is the CsLock board address.  VDAC = 0 implies gain is 0. VDAC = 255
// implies gain is max.

void CsLock::CsLockSetGain(int whichGain, int VDAC, int LaserLine, int Addr)
{
	MAX5132_CmdNOP(F4RampVoltageLocation);
	MAX5132_CmdNOP(F3RampVoltageLocation);

	if (LaserLine == LaserTypeF3){	// F=3
		MAX509_NOP(F4GainsLocation);
		MAX509_LoadInputSetDAC(whichGain, VDAC, F3GainsLocation);
	}
	else {	// F=4
		MAX509_LoadInputSetDAC(whichGain, VDAC, F4GainsLocation);
		MAX509_NOP(F3GainsLocation);
	}
	
	HC594_SetAll(getControlValue(), CtrlBitsLocation); 

	DSerialSendData(Addr, TotalRegLength);
}


//-------------------------------------------------------------------------
// CsLockSetRampVoltage - Sets specified ramp voltage value, writes serial
// Addr is the CsLock board address.

void CsLock::CsLockSetRampVoltage(int VDAC, int LaserLine, int Addr)
{
	if (LaserLine == LaserTypeF3){	// F=3
		MAX5132_CmdNOP(F4RampVoltageLocation);
		MAX5132_CmdSetVDAC(VDAC, F3RampVoltageLocation);
	}
	else {	// F=4
		MAX5132_CmdSetVDAC(VDAC, F4RampVoltageLocation);
		MAX5132_CmdNOP(F3RampVoltageLocation);
	}
	
	MAX509_NOP(F4GainsLocation);
	MAX509_NOP(F3GainsLocation);
	
	HC594_SetAll(getControlValue(), CtrlBitsLocation); 

	DSerialSendData(Addr, TotalRegLength);
}


//-------------------------------------------------------------------------
// CsLockSetRampVoltageFast - Sets specified ramp voltage value (rest of CsSerBuf is untouched), writes serial
// Addr is the CsLock board address.

void CsLock::CsLockSetRampVoltageFast(int VDAC, int LaserLine, int Addr)
{
	if (LaserLine == LaserTypeF3)	// F=3
		MAX5132_CmdSetVDAC(VDAC, F3RampVoltageLocation);
	else	// F=4
		MAX5132_CmdSetVDAC(VDAC, F4RampVoltageLocation);
		
	DSerialSendData(Addr, TotalRegLength);
}



/********************************************************************

Bit Programming Instructions:

See MAX5132 Datasheet.  Control code constants listed
above should be sent alone or added to DAC data as appropriate.


*********************************************************************/

//-------------------------------------------------------------------------
// MAX5132_CmdSetVDAC()
//

int CsLock::MAX5132_CmdSetVDAC(int VDAC, int location)
{

	int Vout;
	
	//	Enforce 13-bit limits on DAC range
	if(VDAC > MAX5132_13bitMax) 	
		Vout = MAX5132_13bitMax;	
	else if(VDAC < 0)		
		Vout = 0;		
	else
		Vout = VDAC;
		
	Vout = Vout + MAX5132_LDAll;		//	Append the correct control code bits
	
	return DSerialBufferBuild(Vout, MAX5132_numTotalBits, location, MAX5132_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX5132_CmdSetUPOHi()
//

int CsLock::MAX5132_CmdSetUPOHi(int location)
{
	
	return DSerialBufferBuild(MAX5132_UPOHi, MAX5132_numTotalBits, location, MAX5132_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX5132_CmdSetUPOLo()
//

int CsLock::MAX5132_CmdSetUPOLo(int location)
{
	
	return DSerialBufferBuild(MAX5132_UPOLo, MAX5132_numTotalBits, location, MAX5132_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX5132_CmdNOP()
//
int CsLock::MAX5132_CmdNOP(int location)
{
	return DSerialBufferBuild(MAX5132_NOP, MAX5132_numTotalBits, location, MAX5132_bEnablePolarity);
}

//-------------------------------------------------------------------------
// MAX509_SetOutputMode()
// mode=1: chip serial out (DOUT) clocked out on rising edge (default), mode=0: falling edge

int CsLock::MAX509_SetOutputMode(int mode, int location)
{		
	return DSerialBufferBuild((mode==0)?0XA00:0XE00, MAX509_numTotalBits, location,MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_LoadDACInput() - load input but do not set DAC
// chan: 0,1,2,3 for A,B,C,D, or enum MAX509_A etc. [A1 A0 0 1 D7..D0]

int CsLock::MAX509_LoadDACInput(int chan, int data, int location)
{
	return DSerialBufferBuild((chan<<10)+0X100+data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_LDAC() - update all DACs from respective input registers

int CsLock::MAX509_LDAC(int location)
{
	return DSerialBufferBuild(0X200, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_LoadInputSetDAC() - load input and set DAC
// chan: 0,1,2,3 for A,B,C,D, or enum MAX509_A etc. [A1 A0 1 1 D7..D0]

int CsLock::MAX509_LoadInputSetDAC(int chan, int data, int location)
{
	return DSerialBufferBuild((chan<<10)+0X300+data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_SetAllDAC() - update all DACs from shift register

int CsLock::MAX509_SetAllDAC(int data, int location)
{
	return DSerialBufferBuild(data, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_NOP() - no operation (NOP), shifts data in shift register

int CsLock::MAX509_NOP(int location)
{
	return DSerialBufferBuild(0X400, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}



//-------------------------------------------------------------------------
// MAX509_SetPropGain()

int CsLock::MAX509_SetPropGain(int propGain, int location)
{
	return DSerialBufferBuild(0X300+propGain, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


//-------------------------------------------------------------------------
// MAX509_SetIntGain()

int CsLock::MAX509_SetIntGain(int intGain, int location)
{
	return DSerialBufferBuild(0X700+intGain, MAX509_numTotalBits, location, MAX509_bEnablePolarity);
}


