/*! \file ADF4360.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Analog_Devices_VCO::ADF4360
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "ADF4360.h"

#include <iostream>

omni_mutex* Analog_Devices_VCO::ADF4360::serialBufferMutex = new omni_mutex();
omni_condition* Analog_Devices_VCO::ADF4360::serialBufferCondition = new omni_condition(serialBufferMutex);

EtraxBus* Analog_Devices_VCO::ADF4360::bus = NULL;

Analog_Devices_VCO::ADF4360::ADF4360(unsigned int VCO_Address, unsigned int EtraxMemoryAddress, 
									unsigned short ADF4360_model) :
adf4560_model(ADF4360_model),
controlLatch(0),
nCounterLatch(0),
rCounterLatch(0),
vcoLatches(controlLatch, nCounterLatch, rCounterLatch)
{
	// Addresses
	vcoAddress = VCO_Address;
	setPCParallelAddress(0x378);

	if(bus == NULL)
	{
		bus = new EtraxBus(EtraxMemoryAddress);
	}

	init(ADF4360_model);
}

Analog_Devices_VCO::ADF4360::ADF4360(unsigned int VCO_Address, unsigned short ADF4360_model) :
adf4560_model(ADF4360_model),
controlLatch(0),
nCounterLatch(0),
rCounterLatch(0),
vcoLatches(controlLatch, nCounterLatch, rCounterLatch)
{
	// Addresses
	vcoAddress = VCO_Address;
	setPCParallelAddress(0x378);

	init(ADF4360_model);
}

void Analog_Devices_VCO::ADF4360::init(unsigned short ADF4360_model)
{
// Set all latches to zero
//	controlLatch.assign(24, false);
//	nCounterLatch.assign(24, false);
//	rCounterLatch.assign(24, false);

	// Set control bits
	// controlLatch			// (C2, C1) = (0, 0)
	nCounterLatch.at(1) = true;	// (C2, C1) = (1, 0)
	rCounterLatch.at(0) = true;	// (C2, C1) = (0, 1)

	// ADF4360 shift register time constants
	t1 = 10*1;
	t4 = 25*1;
	t5 = 25*1;
	t6 = 10*1;
	t7 = 20*1;
	deltaT = 5;

	initialized = false;
	serialBuffer.clear();
	initialize(ADF4360_model);

//enableDivideBy2();
}

Analog_Devices_VCO::ADF4360::~ADF4360()
{
	if(serialBufferMutex != 0)
	{
		serialBufferMutex->lock();
		
		if(bus != 0)
		{
			delete bus;
			bus = 0;
		}

		if(serialBufferMutex != 0)
			serialBufferMutex->unlock();

		delete serialBufferMutex;
		serialBufferMutex = 0;
	}
}




Analog_Devices_VCO::ADF4360::VCOLatches& Analog_Devices_VCO::ADF4360::getVCOLatches()
{
	return vcoLatches;
}


void Analog_Devices_VCO::ADF4360::initialize(unsigned short ADF4360_model)
{
	setADF4360_Parameters(ADF4360_model);
	initialized = true;

	powerEnabled = true;

	setCorePowerLevel(1);	//10mA (recommended)
	setFref(10.0);			//10MHz
	setChargePumpCurrent(7 ,7);
	setOutputPower(3);

	//Phase Detector Polarity from ADF4360 datasheet
	//"The positive setting enabled by programming a 1 is used when using the 
	//on-chip VCO with a passive loop filter or with an active noninverting 
	//filter. It can also be set to 0, which is required if an active 
	//inverting loop filter is used."
	setPhaseDetectorPolarity(true);

	setPreScalerValue(32);

	//counter initial values
	set_PFD_Freq(0.2);	//200 kHz
	setFvco(modelParams.fvco_Ini);

	sendRLatch();
	sendControlLatch();
	//delay 5 ms
	sendNLatch();


}

void Analog_Devices_VCO::ADF4360::sendLatches()
{
	sendRLatch();
	sendControlLatch();
	//delay here at startup...
	sendNLatch();
}
void Analog_Devices_VCO::ADF4360::sendControlLatch()
{
	BuildSerialBuffer(controlLatch);
	sendSerialData();
}

void Analog_Devices_VCO::ADF4360::sendNLatch()
{
	BuildSerialBuffer(nCounterLatch);
	sendSerialData();
}

void Analog_Devices_VCO::ADF4360::sendRLatch()
{
	BuildSerialBuffer(rCounterLatch);
	sendSerialData();
}


void Analog_Devices_VCO::ADF4360::setPCParallelAddress(const int address)
{
	parallelAddress = address;
}

int Analog_Devices_VCO::ADF4360::PCParallelAddress() const
{
	return parallelAddress;
}

void Analog_Devices_VCO::ADF4360::sendSerialData()
{
	unsigned long wait_s, wait_ns;

	if(initialized)
	{
		serialBufferMutex->lock();
		{
			for(unsigned i=0; i < serialBuffer.size(); i++)
			{
				#ifdef _MSC_VER
				omni_thread::get_time(&wait_s, &wait_ns, 0, 1000000);	//1 ms
				serialBufferCondition->timedwait(wait_s, wait_ns);	//put thread to sleep
				#endif

				writeData(serialBuffer[i]);
			}
			serialBuffer.clear();
		}
		serialBufferMutex->unlock();
	}
}

void Analog_Devices_VCO::ADF4360::writeData(const SerialData& data)
{
#ifdef _MSC_VER
//	Out32(PCParallelAddress(), data.getParallelData());
	Out32(PCParallelAddress(), data.getData(vcoAddress));
#endif

#ifdef HAVE_LIBBUS
	bus->writeData(data.getData(vcoAddress));
#endif
}

unsigned int Analog_Devices_VCO::ADF4360::getVCOAddress()
{
	return vcoAddress;
}

void Analog_Devices_VCO::ADF4360::BuildSerialBufferLean(std::bitset<24>& latch)
{
	serialBuffer.clear();

	// Setup
	serialBuffer.push_back(SerialData(0, 0, 0));	//LE=0

	// DATA
	for(int i=latch.size()-1; i >= 0; i--)
	{
		serialBuffer.push_back(SerialData(0, latch.at(i), 0));	//CLOCK=0
		serialBuffer.push_back(SerialData(1, latch.at(i),0));	//CLOCK=1
	}

	// LE pulse
	serialBuffer.push_back(SerialData(0, 0, 0));	//LE=0
	serialBuffer.push_back(SerialData(0, 0, 1));	//LE=1
}

void Analog_Devices_VCO::ADF4360::BuildSerialBuffer(std::bitset<24>& latch)
{
	// * serialBuffer entries are correctly time ordered (earliest entries
	//   in time are at the beginning of the buffer).  This means that the MSB
	//   of the latch must be put at the beginning of the buffer, since the 
	//   ADF4360 shift register expects the MSB first.
	//
	// * DATA is latched on rising edge of CLOCK.
	//
	// * DATA in shift register is loaded into latch when LE goes high.

	serialBuffer.clear();

	// Setup
	for(unsigned t = 0; t < t1; t += deltaT) //LE Setup Time
		serialBuffer.push_back(SerialData(0, 0, 0));	//LE=0

	// DATA
	for(int i=latch.size()-1; i >= 0; i--)
	{
		for(unsigned t = 0; t < t5; t += deltaT) //CLOCK Low Duration
			serialBuffer.push_back(SerialData(0, latch.at(i), 0));	//CLOCK=0

		for(unsigned t = 0; t < t4; t += deltaT) //CLOCK High Duration
			serialBuffer.push_back(SerialData(1, latch.at(i),0));	//CLOCK=1
	}

	// LE pulse
	for(unsigned t = 0; t < t6; t += deltaT) //CLOCK to LE Setup Time
		serialBuffer.push_back(SerialData(0, 0, 0));	//LE=0
	for(unsigned t = 0; t < t7; t += deltaT) //LE Pulse Width
		serialBuffer.push_back(SerialData(0, 0, 1));	//LE=1
}


void Analog_Devices_VCO::ADF4360::setPreScalerValue(bool P2, bool P1)
{
	if(!P2 && !P1)		//(P2,P1)=(0,0)  -->  8/9
		Prescalar = 8;
	else if(!P2 && P1)	//(P2,P1)=(0,1)  -->  16/17
		Prescalar = 16;
	else				//(P2,P1)=(1,x)  -->  32/33
		Prescalar = 32;

	controlLatch.at(22) = P1;	//P1
	controlLatch.at(23) = P2;	//P2
	
	sendControlLatch();
}
void Analog_Devices_VCO::ADF4360::setPreScalerValue(unsigned short P)
{
	switch(P)
	{
	case 8:
		setPreScalerValue(false, false);
		break;
	case 16:
		setPreScalerValue(false, true);
		break;
	case 32:
		setPreScalerValue(true, false);
		break;
	default:
		break;
	}
}
unsigned int Analog_Devices_VCO::ADF4360::getPrescalar() const
{
	return Prescalar;
}

void Analog_Devices_VCO::ADF4360::enableDivideBy2()
{
	nCounterLatch.at(22) = true;
	sendNLatch();
}

void Analog_Devices_VCO::ADF4360::disableDivideBy2()
{
	nCounterLatch.at(22) = false;
	sendNLatch();
}

void Analog_Devices_VCO::ADF4360::SynchronousPowerDownPrepare()
{
	controlLatch.at(20) = 1;	//PD1
	controlLatch.at(21) = 1;	//PD2
	powerEnabled = false;
}


void Analog_Devices_VCO::ADF4360::SynchronousPowerDown()
{
	SynchronousPowerDownPrepare();
	sendControlLatch();
}
void Analog_Devices_VCO::ADF4360::PowerUpPrepare()
{
	//reset controlLatch
	controlLatch.at(20) = 0;	//PD1
	controlLatch.at(21) = 0;	//PD2
	powerEnabled = true;
}

void Analog_Devices_VCO::ADF4360::PowerUp()
{
	PowerUpPrepare();
	sendLatches();
}

bool Analog_Devices_VCO::ADF4360::getPowerStatus()
{
	return powerEnabled;
}

bool Analog_Devices_VCO::ADF4360::setChargePumpCurrent(unsigned short I1, unsigned short I2)
{
	bool status = false;

	if(I1 < 8)
	{
		//Current setting 1
		controlLatch.at(14) = ((I1 & 0x1) >> 0) == 1 ? true : false;	//CPI1
		controlLatch.at(15) = ((I1 & 0x2) >> 1) == 1 ? true : false;	//CPI2
		controlLatch.at(16) = ((I1 & 0x4) >> 2) == 1 ? true : false;	//CPI3
		status = true;
	}

	if(I2 < 8)
	{
		//Current setting 2
		controlLatch.at(17) = ((I2 & 0x1) >> 0) == 1 ? true : false;	//CPI4
		controlLatch.at(18) = ((I2 & 0x2) >> 1) == 1 ? true : false;	//CPI5
		controlLatch.at(19) = ((I2 & 0x4) >> 2) == 1 ? true : false;	//CPI6
		status = true;
	}

	return status;
}


unsigned short Analog_Devices_VCO::ADF4360::getOutputPower()
{
	return powerLevel;
}

bool Analog_Devices_VCO::ADF4360::setOutputPower(unsigned short level) 
{
	if(level < 4)
	{
		powerLevel = level;

		controlLatch.at(12) = ((level & 0x1) >> 0) == 1 ? true : false;	//PL1
		controlLatch.at(13) = ((level & 0x2) >> 1) == 1 ? true : false;	//PL2

		//reset controlLatch
		controlLatch.at(20) = 0;	//PD1
		controlLatch.at(21) = 0;	//PD2

		powerEnabled = true;

		return true;
	}
	return false;
}
bool Analog_Devices_VCO::ADF4360::setCorePowerLevel(unsigned short level)
{	
	//level=0: (PC2,PC1) = (0,0)  -->  5mA
	//level=1: (PC2,PC1) = (0,1)  -->  10mA  (recommended)
	//level=2: (PC2,PC1) = (1,0)  -->  15mA
	//level=3: (PC2,PC1) = (1,1)  -->  20mA

	if(level < 4)
	{
		controlLatch.at(2) = ((level & 0x1) >> 0) == 1 ? true : false;	//PC1
		controlLatch.at(3) = ((level & 0x2) >> 1) == 1 ? true : false;	//PC2
		return true;
	}
	return false;
}
void Analog_Devices_VCO::ADF4360::setPhaseDetectorPolarity(bool pdp)
{
	controlLatch.at(8) = pdp;
}

void Analog_Devices_VCO::ADF4360::setMuteTillLockDetect(bool mute)
{
	controlLatch.at(11) = mute;
}
std::string Analog_Devices_VCO::ADF4360::printControlLatch() const
{
	const std::bitset<24>& latch = controlLatch;

	std::string result;
	std::string one =  "1";
	std::string zero = "0";
	std::string divider = "|";

	result += divider;
	result += (latch.at(23)? one : zero);
	result += (latch.at(22)? one : zero);
	result += divider;
	result += (latch.at(21)? one : zero);
	result += (latch.at(20)? one : zero);
	result += divider;
	result += (latch.at(19)? one : zero);
	result += (latch.at(18)? one : zero);
	result += (latch.at(17)? one : zero);
	result += divider;
	result += (latch.at(16)? one : zero);
	result += (latch.at(15)? one : zero);
	result += (latch.at(14)? one : zero);
	result += divider;
	result += (latch.at(13)? one : zero);
	result += (latch.at(12)? one : zero);
	result += divider;
	result += (latch.at(11)? one : zero);
	result += divider;
	result += (latch.at(10)? one : zero);
	result += divider;
	result += (latch.at(9)? one : zero);
	result += divider;
	result += (latch.at(8)? one : zero);
	result += divider;
	result += (latch.at(7)? one : zero);
	result += (latch.at(6)? one : zero);
	result += (latch.at(5)? one : zero);
	result += divider;
	result += (latch.at(4)? one : zero);
	result += divider;
	result += (latch.at(3)? one : zero);
	result += (latch.at(2)? one : zero);
	result += divider;
	result += (latch.at(1)? one : zero);
	result += (latch.at(0)? one : zero);
	result += divider;

	return result;
}

std::string Analog_Devices_VCO::ADF4360::printNLatch() const
{
	const std::bitset<24>& latch = nCounterLatch;

	std::string result;
	std::string one =  "1";
	std::string zero = "0";
	std::string divider = "|";

	result += divider;
	result += (latch.at(23)? one : zero);
	result += divider;
	result += (latch.at(22)? one : zero);
	result += divider;
	result += (latch.at(21)? one : zero);
	result += divider;
	result += (latch.at(20)? one : zero);
	result += (latch.at(19)? one : zero);
	result += (latch.at(18)? one : zero);
	result += (latch.at(17)? one : zero);
	result += (latch.at(16)? one : zero);
	result += (latch.at(15)? one : zero);
	result += (latch.at(14)? one : zero);
	result += (latch.at(13)? one : zero);
	result += (latch.at(12)? one : zero);
	result += (latch.at(11)? one : zero);
	result += (latch.at(10)? one : zero);
	result += (latch.at(9)? one : zero);
	result += (latch.at(8)? one : zero);
	result += divider;
	result += (latch.at(7)? one : zero);
	result += divider;
	result += (latch.at(6)? one : zero);
	result += (latch.at(5)? one : zero);
	result += (latch.at(4)? one : zero);
	result += (latch.at(3)? one : zero);
	result += (latch.at(2)? one : zero);
	result += divider;
	result += (latch.at(1)? one : zero);
	result += (latch.at(0)? one : zero);
	result += divider;

	return result;
}

std::string Analog_Devices_VCO::ADF4360::printRLatch() const
{
	const std::bitset<24>& latch = rCounterLatch;

	std::string result;
	std::string one =  "1";
	std::string zero = "0";
	std::string divider = "|";

	result += divider;
	result += (latch.at(23)? one : zero);
	result += (latch.at(22)? one : zero);
	result += divider;
	result += (latch.at(21)? one : zero);
	result += (latch.at(20)? one : zero);
	result += divider;
	result += (latch.at(19)? one : zero);
	result += divider;
	result += (latch.at(18)? one : zero);
	result += divider;
	result += (latch.at(17)? one : zero);
	result += (latch.at(16)? one : zero);
	result += divider;
	result += (latch.at(15)? one : zero);
	result += (latch.at(14)? one : zero);
	result += (latch.at(13)? one : zero);
	result += (latch.at(12)? one : zero);
	result += (latch.at(11)? one : zero);
	result += (latch.at(10)? one : zero);
	result += (latch.at(9)? one : zero);
	result += (latch.at(8)? one : zero);
	result += (latch.at(7)? one : zero);
	result += (latch.at(6)? one : zero);
	result += (latch.at(5)? one : zero);
	result += (latch.at(4)? one : zero);
	result += (latch.at(3)? one : zero);
	result += (latch.at(2)? one : zero);
	result += divider;
	result += (latch.at(1)? one : zero);
	result += (latch.at(0)? one : zero);
	result += divider;

	return result;
}

bool Analog_Devices_VCO::ADF4360::setACounter(unsigned int A)
{
	if(A <= 31)
	{
		ACounter = A;
		for(unsigned i=0; i < 5; i++)	//5 bit latch
		{
			nCounterLatch.at(2 + i)  = (A >> i) & 0x1;	//A1 -> A5
		}
		return true;
	}
	return false;
}
bool Analog_Devices_VCO::ADF4360::setBCounter(unsigned int B)
{
	if(B <= 8191 && B >= 3)	// B < 3 is not allowed
	{
		BCounter = B;
		for(unsigned i=0; i < 13; i++)	//13 bit latch
		{
			nCounterLatch.at(8 + i)  = (B >> i) & 0x1;	//B1 -> B13
		}
		return true;
	}

	return false;
}

bool Analog_Devices_VCO::ADF4360::setRCounter(unsigned int R)
{
	if(R <= 16383)
	{
		RCounter = R;
		for(unsigned i=0; i < 14; i++)	//14 bit latch
		{
			rCounterLatch.at(2 + i)  = (R >> i) & 0x1;	//R1 -> R14
		}
		return true;
	}

	return false;
}

unsigned int Analog_Devices_VCO::ADF4360::getACounter() const
{
	return ACounter;
}

unsigned int Analog_Devices_VCO::ADF4360::getBCounter() const
{
	return BCounter;
}

unsigned int Analog_Devices_VCO::ADF4360::getRCounter() const
{
	return RCounter;
}

unsigned int Analog_Devices_VCO::ADF4360::getN() const
{
	return ( (getPrescalar() * getBCounter()) + getACounter() );
}

bool Analog_Devices_VCO::ADF4360::setN(unsigned int N)
{
	if(getPrescalar() > 0)
	{
		if( setBCounter( N / getPrescalar() ) )
		{
			if( setACounter( N - getPrescalar()*getBCounter() ) )
			{
				return true;
			}
		}
	}
	return false;
}

double Analog_Devices_VCO::ADF4360::getFvco() const
{
	return (double)( getN() * getFref() / getRCounter() );
}


bool Analog_Devices_VCO::ADF4360::setFvco(double Fvco)
{
	double fPFD = get_PFD_Freq();

	if(Fvco > 0 && fPFD > 0)
	{
		if(Fvco < modelParams.fvco_Min && false)			// too small
			return setN( static_cast<unsigned int>(modelParams.fvco_Min / fPFD) );
		else if(Fvco > modelParams.fvco_Max && false)	// too big
			return setN( static_cast<unsigned int>(modelParams.fvco_Max / fPFD) );
		else
			return setN( static_cast<unsigned int>(Fvco / fPFD) );
	}
	return false;
}
double Analog_Devices_VCO::ADF4360::getFref() const
{
	return F_ref;
}

bool Analog_Devices_VCO::ADF4360::setFref(double f_ref)
{
	if(f_ref > 0)
	{
		F_ref = f_ref;
		return true;
	}
	return false;
}


bool Analog_Devices_VCO::ADF4360::set_PFD_Freq(double PFD_freq)
{
	double R;
	
	if(PFD_freq > 0)
	{
		R = getFref() / PFD_freq;
		
		return setRCounter( static_cast<unsigned int>(R) );
	}
	return false;
}

double Analog_Devices_VCO::ADF4360::get_PFD_Freq()
{
	return static_cast<double>( getFref() / getRCounter() );
}

void Analog_Devices_VCO::ADF4360::setADF4360_Parameters(unsigned short ADF4360_model)
{
	// All frequencies in MHz

	switch(ADF4360_model)
	{
	case 0:				//ADF4360-0
		modelParams.fvco_Min = 2400;
		modelParams.fvco_Max = 2725;
		modelParams.fvco_Ini = 2500;
		break;
	case 1:				//ADF4360-1
		modelParams.fvco_Min = 2050;
		modelParams.fvco_Max = 2450;
		modelParams.fvco_Ini = 2200;
		break;
	case 2:				//ADF4360-2
		modelParams.fvco_Min = 1850;
		modelParams.fvco_Max = 2150;
		modelParams.fvco_Ini = 2000;
		break;
	case 3:				//ADF4360-3
		modelParams.fvco_Min = 1600;
		modelParams.fvco_Max = 1950;
		modelParams.fvco_Ini = 1800;
		break;
	case 4:				//ADF4360-4
		modelParams.fvco_Min = 1450;
		modelParams.fvco_Max = 1750;
		modelParams.fvco_Ini = 1600;
		break;
	case 5:				//ADF4360-5
		modelParams.fvco_Min = 1200;
		modelParams.fvco_Max = 1400;
		modelParams.fvco_Ini = 1300;
		break;
	case 6:				//ADF4360-6
		modelParams.fvco_Min = 1050 ;
		modelParams.fvco_Max = 1250;
		modelParams.fvco_Ini = 1200;
		break;
	case 7:				//ADF4360-7
		modelParams.fvco_Min = 350;
		modelParams.fvco_Max = 1800;
		modelParams.fvco_Ini = 1600;
		break;
	case 8:				//ADF4360-8
		modelParams.fvco_Min = 65;
		modelParams.fvco_Max = 400;
		modelParams.fvco_Ini = 200;
		break;
	case 9:				//ADF4360-9
		modelParams.fvco_Min = 1.1;
		modelParams.fvco_Max = 200;
		modelParams.fvco_Ini = 100;
		break;
	}
}

