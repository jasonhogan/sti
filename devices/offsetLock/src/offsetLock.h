#ifndef OFFSETLOCK_H
#define OFFSETLOCK_H

/*! \file offsetLock.h
 *  \author David M.S. Johnson (from GradEmbed base code)
 *  \brief Include-file for the class offsetLock
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

#include "SerialDevice.h"

#include <string>

class offsetLock : public SerialDevice
{
public:

	offsetLock(std::string deviceName, int Address);
	virtual ~offsetLock() {};

	//Access functions
//	virtual DeviceID deviceID() const;
	virtual std::string deviceType() const;

	bool enableOffsetLock(bool enable, bool iEnable, double servoFrequency, double iGain, double pGain);


private:

	// PLL parameters
	int P;
	int ACounter;
	int BCounter;
	int RCounter;
	int NCounter;
	int intGain;
	int proGain;
	bool positiveDetuning;
	int OLAddress;
	double lockFreq_MHz;
	double compareFreq_MHz;
	bool bLockEnable;

	//communication variables
	int OffsetLockMod_numTotalBits;

	//LMX2326
	int set_R_Counter;
	int set_N_Counter;
	int set_Function;
	bool LMX2326_bEnablePolarity;
	int LMX2326_numTotalBits;
	int LMX2326location;
	//functions
	int LMX2326_FunctionLatch(int function, int location);
	int LMX2326_SetRCounter(int RCounter, int location);
	int LMX2326_SetNCounter(int ACounter, int BCounter, int location);
	

	//HC594
	//variables
	int HC594_numTotalBits; // value = 8
	bool HC594_bEnablePolarity;
	int HC594location;
	//functions
	int HC594_SetAll(int bits, int location);
	int HC594_OffsetLock(int location);
	int HC594_OffsetUnLock(int location);
	int HC594_OutEnable(int location);
	void HC594_SwitchChannel(int channelNumber, bool switchMode, int location);
	void HC594_SetChannel(int channelNumber, bool controlMode, int location);

	// MAX509
	// variables
	int MAX509_numTotalBits;
	bool MAX509_bEnablePolarity;
	int MAX509location;

	enum {MAX509_A = 0, MAX509_B, MAX509_C, MAX509_D};
	// functions
	int MAX509_SetOutputMode(int mode, int location);
	int MAX509_NOP(int location);
	int MAX509_LoadDACInput(int chan, int data, int location);
	int MAX509_LDAC(int location);
	int MAX509_LoadInputSetDAC(int chan, int data, int location);
	int MAX509_SetAllDAC(int data, int location);

	int MAX509_SetPropGain(int propGain, int location);
	int MAX509_SetIntGain(int intGain, int location);

};

#endif
