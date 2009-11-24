#ifndef CSLOCK_H
#define CSLOCK_H

/*! \file CsLock.h
 *  \author David M.S. Johnson (from GradEmbed base code)
 *  \brief Include-file for the class CsLock
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

class CsLock : public SerialDevice
{
public:

	CsLock(std::string deviceName, int Address);
	virtual ~CsLock() {};

	//Access functions
//	virtual DeviceID deviceID() const;
	virtual std::string deviceType() const;


	// HC594 control bits
	struct CsControl {
		int F3Int1;		//F=3 integrator 1 switch	(QA)					
		int F3Int2;		//F=3 integrator 2 switch	(QB)
		int F3OutEnable;	//F=3 Output enable		(QC)
		int Unused1;		//				(QD)

		int F4Int1;		//F=3 integrator 1 switch	(QE)
		int F4Int2;		//F=3 integrator 2 switch	(QF)
		int F4OutEnable;	//F=3 Output enable		(QG)
		int Unused2;		//				(QH)
	};

	CsControl CsCtrl;
	
	int getControlValue();
	void setControlBits();

	enum laserType {LaserTypeF3 = 0, LaserTypeF4 = 1};
//	int LaserTypeF3;
//	int LaserTypeF4;


	void setPropGain(double fracGain);
	void setDiffGain(double fracGain);
	void setInt1Gain(double fracGain);
	void setInt2Gain(double fracGain);
	void setOutputEnable(bool enable);
	void setInt1Enable(bool enable);
	void setInt2Enable(bool enable);
	void setOffset(double fracGain);
	void setOffsetFast(double fracGain);
	
	double gainRangeCheck(double newGain, double minGain, double maxGain);

	void setGain(int circuit, int gainType, double fracGain);

	double getGain(int circuit, int gainType);

	double getPropGain();
	double getDiffGain();
	double getInt1Gain();
	double getInt2Gain();
	double getOffset();
	bool   getOutputEnable();
	bool   getInt1Enable();
	bool   getInt2Enable();

	double getPropGain(int circuit);
	double getDiffGain(int circuit);
	double getInt1Gain(int circuit);
	double getInt2Gain(int circuit);
	double getOffset(int circuit);
	bool   getOutputEnable(int circuit);
	bool   getInt1Enable(int circuit);
	bool   getInt2Enable(int circuit);

	void setWhichCircuit(int circuit) { whichCircuit = circuit; }
	int getWhichCircuit() {return whichCircuit;}

private:
	
	int whichCircuit;

	struct CsGains {
		double offset;
		double diffGain;
		double propGain;
		double int1Gain;
		double int2Gain;
	};

	CsGains gains[2];	

	int HC594_numTotalBits; // value = 8

	bool HC594_bEnablePolarity;

	int HC594_SetAll(int bits, int location);
	int HC594_OffsetLock(int location);
	int HC594_OffsetUnLock(int location);
	int HC594_OutEnable(int location);
	void HC594_SwitchChannel(int channelNumber, bool switchMode, int location);
	void HC594_SetChannel(int channelNumber, bool controlMode, int location);


	enum CsLockGainType {DiffGain = 0, PropGain = 1, Int1Gain = 2, Int2Gain = 3, OffsetGain = 4};

	// CsLock board constants
	int TotalRegLength;
	int F4RampVoltageLocation;
	int F3RampVoltageLocation;
	int F4GainsLocation;
	int F3GainsLocation;
	int CtrlBitsLocation;
		

	
	// CsLock board driver routines
	void CsLockSetControlBits(int integrator1,int integrator2,int outEn,int LaserLine,int Addr);
	void CsLockSetGain(int whichGain, int VDAC, int LaserLine, int Addr);
	void CsLockSetRampVoltage(int VDAC, int LaserLine, int Addr);
	void CsLockSetRampVoltageFast(int VDAC, int LaserLine, int Addr);
	void CsLockPresetScan(int PresetScanMode);

	//MAX5132 constant variables
	int MAX5132_numTotalBits;
	bool MAX5132_bEnablePolarity;
	int MAX5132_13bitMax;

	int MAX5132_NOP; 	
	int MAX5132_LDVdac; 
	int MAX5132_LDAll; 
	int MAX5132_UPOHi; 
	int MAX5132_UPOLo;



	//MAX5132 Function declarations

	int MAX5132_CmdSetVDAC(int VDAC, int location);
	int MAX5132_CmdSetUPOHi(int location);
	int MAX5132_CmdSetUPOLo(int location);
	int MAX5132_CmdNOP(int location);

	// MAX509 constant variables
	int MAX509_numTotalBits;
	bool MAX509_bEnablePolarity;

	enum {MAX509_A = 0, MAX509_B, MAX509_C, MAX509_D};

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
