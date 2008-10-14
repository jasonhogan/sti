/*! \file ADF4360.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Analog_Devices_VCO::ADF4360
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef ANALOG_DEVICES_VCO_ADF4360_H
#define ANALOG_DEVICES_VCO_ADF4360_H

#ifdef _MSC_VER
// PC parallel port output in Windows
void _stdcall Out32(short PortAddress, short data);
#endif

#include "SerialData.h"

#include <vector>
#include <string>

namespace Analog_Devices_VCO {

class ADF4360 {

public:

	ADF4360();
	~ADF4360();

	void initialize();
	
	void setPCParallelAddress(int address);
	int PCParallelAddress() const;

	std::string printControlLatch() const;
	std::string printNLatch() const;
	std::string printRLatch() const;

	unsigned int getACounter() const;
	unsigned int getBCounter() const;
	unsigned int getRCounter() const;
	unsigned int getPrescalar() const;

	// N = (P*B + A)
	// F_vco = N * F_PFD
	unsigned int getN() const;
	bool setN(unsigned int N);

	void sendControlLatch();
	void sendNLatch();
	void sendRLatch();
	void sendLatches();

	// Control Latch
	void setPreScalerValue(unsigned short P);
	void SynchronousPowerDown();
	bool setChargePumpCurrent(unsigned short I1, unsigned short I2);
	bool setOutputPower(unsigned short level);
	bool setCorePowerLevel(unsigned short level);
	void setMuteTillLockDetect(bool mute);
	void setPhaseDetectorPolarity(bool pdp);

	// N counter latch
	bool setACounter(unsigned int A);
	bool setBCounter(unsigned int B);

	// R counter latch
	bool setRCounter(unsigned int R);

	// VCO Output Frequency
	double getFvco() const;
	bool setFvco(double Fvco);

	// Reference Frequency -- Evaluation board provides 10 MHz by default
	double getFref() const;
	bool setFref(double f_ref);

	// PFD frequency (phase-frequency detector frequency)
	// F_PFD = F_ref / R
	bool set_PFD_Freq(double PFD_freq);
	double get_PFD_Freq();

private:

	void setPreScalerValue(bool P2, bool P1);
	
	void sendSerialData();
	void writeData(const SerialData & data);
	void BuildSerialBuffer(std::vector<bool> & latch);
	
	std::vector<bool> controlLatch;
	std::vector<bool> nCounterLatch;
	std::vector<bool> rCounterLatch;

	unsigned int ACounter;
	unsigned int BCounter;
	unsigned int RCounter;
	unsigned int Prescalar;

	double F_ref;

	std::vector<SerialData> serialBuffer;
	int parallelAddress;

	unsigned t1;
	unsigned t4;
	unsigned t5;
	unsigned t6;
	unsigned t7;
	unsigned deltaT;

};

}

#endif