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
#include <EtraxBus.h>
#include <omnithread.h>
#include <vector>
#include <string>
#include <bitset>

namespace Analog_Devices_VCO {

class ADF4360 {

public:

	ADF4360(unsigned int VCO_Address, unsigned int EtraxMemoryAddress, 
		unsigned short ADF4360_model);

	ADF4360(unsigned int VCO_Address, unsigned short ADF4360_model);

	~ADF4360();

	void initialize(unsigned short ADF4360_model);
	
	unsigned int getVCOAddress();
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

	bool getPowerStatus();
	void PowerUp();
	void PowerUpPrepare();

	// Control Latch
	void setPreScalerValue(unsigned short P);
	void SynchronousPowerDown();
	void SynchronousPowerDownPrepare();
	bool setChargePumpCurrent(unsigned short I1, unsigned short I2);
	bool setCorePowerLevel(unsigned short level);
	void setMuteTillLockDetect(bool mute);
	void setPhaseDetectorPolarity(bool pdp);
	
	bool setOutputPower(unsigned short level);
	unsigned short getOutputPower();

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


	struct VCOLatches
	{
	public:
		VCOLatches(std::bitset<24>& c, std::bitset<24>& n, std::bitset<24>& r) :
		  controlLatch(c),
		  nCounterLatch(n),
		  rCounterLatch(r) 
		  {};

		void setLatches(VCOLatches& latches)
		{
			controlLatch  = latches.controlLatch;
			nCounterLatch = latches.nCounterLatch;
			rCounterLatch = latches.rCounterLatch;
		}

		std::bitset<24>& controlLatch;
		std::bitset<24>& nCounterLatch;
		std::bitset<24>& rCounterLatch;
	};

	VCOLatches& getVCOLatches();
	std::vector<SerialData>& getSerialBuffer() { return serialBuffer; };

protected:

	void BuildSerialBuffer(std::bitset<24>& latch);

	void BuildSerialBufferLean(std::bitset<24>& latch);
private:

	void init(unsigned short ADF4360_model);

	void enableDivideBy2();
	void disableDivideBy2();

	void setPreScalerValue(bool P2, bool P1);
	
	void sendSerialData();
	void writeData(const SerialData& data);
	
	std::bitset<24> controlLatch;
	std::bitset<24> nCounterLatch;
	std::bitset<24> rCounterLatch;

	VCOLatches vcoLatches;

//	std::vector<bool> controlLatch;
//	std::vector<bool> nCounterLatch;
//	std::vector<bool> rCounterLatch;

	unsigned int ACounter;
	unsigned int BCounter;
	unsigned int RCounter;
	unsigned int Prescalar;

	double F_ref;
	unsigned short powerLevel;

	void setADF4360_Parameters(unsigned short ADF4360_model);
	
	struct ADF4360_Model_Parameters
	{
		double fvco_Min;
		double fvco_Max;
		double fvco_Ini;
	};

	unsigned short adf4560_model;
	ADF4360_Model_Parameters modelParams;



	std::vector<SerialData> serialBuffer;
	int parallelAddress;
	unsigned int vcoAddress;

	//static class SharedMemory {
	//public:

	//	SharedMemory(uInt32 memAddress)
	//	{
	//		bus = new EtraxBus(memAddress);
	//		serialBufferMutex = new omni_mutex();
	//	}
	//	~SharedMemory()
	//	{
	//		delete bus;
	//		delete serialBufferMutex;
	//	}

	//	//only one instance can send its buffer at a time
	//	omni_mutex* serialBufferMutex;

	//	//For writing data directly to the Etrax memory bus
	//	EtraxBus* bus;	//only one EtraxBus allowed per memory address

	//} etraxMemory;

	//only one instance can send its buffer at a time
	static omni_mutex* serialBufferMutex;
	static omni_condition* serialBufferCondition;

	//For writing data directly to the Etrax memory bus
	static EtraxBus* bus;	//only one EtraxBus allowed per memory address

	unsigned t1;
	unsigned t4;
	unsigned t5;
	unsigned t6;
	unsigned t7;
	unsigned deltaT;

	bool initialized;
	bool powerEnabled;
};

}

#endif

