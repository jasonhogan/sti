/*! \file vortexFrequencyScannerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class vortexFrequencyScannerDevice
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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



#include "vortexFrequencyScannerDevice.h"

vortexFrequencyScannerDevice::vortexFrequencyScannerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{ 
	//initialize values
	vortexLoopLimit = 3;
	vortexLoopEnabled = false;
	enable = false;

	
	lockSetPointVoltage = 0;
	frequency = centerFrequency;
	isRedDetuning = true; // this determines what the vortex piezo range should be

	daSlowChannel = 38;

	centerFrequency = 250; //measure to check on startup
	voltsPerMHz = 2.6 / 130; //measured - can calibrate??
	lowerFrequencyLimit = 110;
	upperFrequencyLimit = 500;
	lockResolution = 3;

	errorSignal = 0;
	errorSignalLimit = 0.05;

}

void vortexFrequencyScannerDevice::defineAttributes() 
{
	addAttribute("Enable Vortex Loop", "Off", "On, Off"); 
	addAttribute("Frequency (MHz)", frequency); 
	addAttribute("Detuning", "Red", "Red, Blue"); 
}

void vortexFrequencyScannerDevice::refreshAttributes() 
{
	setAttribute("Enable Vortex Loop", (enable ? "On" : "Off")); //response to the IDN? query
	setAttribute("Frequency (MHz)", frequency); //response to the IDN? query
	setAttribute("Detuning", (isRedDetuning ? "Red" : "Blue")); //response to the IDN? query
}

bool vortexFrequencyScannerDevice::updateAttribute(string key, string value)
{

	bool success = false;
	double tempDouble;
	std::string newSetPointString;

	bool successDouble = stringToValue(value, tempDouble);

	if(key.compare("Enable Vortex Loop") == 0)
	{
		if(value.compare("On") == 0)
			enable = true;
		else
			enable = false;

		vortexLoopMutex->lock();
		{
			vortexLoopEnabled = enable;
			if(vortexLoopEnabled)
			vortexLoopCondition->signal();
		}
	vortexLoopMutex->unlock();
		success = true;
	}
	else if(key.compare("Detuning") == 0)
	{
		if(value.compare("Red") == 0)
			isRedDetuning = true;
		else
			isRedDetuning = false;

		success = true;
	}
	else if(key.compare("Frequency (MHz)") == 0)
	{

		bool notSatisfied = true;
		double newFrequency = tempDouble;
		
		string measureString;
		if(newFrequency > lowerFrequencyLimit && newFrequency < upperFrequencyLimit)
		{
			while(notSatisfied)
			{
				if( (newFrequency - frequency) > lockResolution || (frequency - newFrequency) > lockResolution)
				{
					lockSetPointVoltage = (centerFrequency - tempDouble) * (isRedDetuning * 2 - 1) * voltsPerMHz;
					newSetPointString = valueToString(daSlowChannel) + " " + valueToString(lockSetPointVoltage);
					partnerDevice("slow").execute(newSetPointString.c_str()); //usage: partnerDevice("lock").execute("--e1");
					successDouble = stringToValue(partnerDevice("spectrumAnalyzer").getAttribute("Peak Location (MHz)"), frequency);
				}
				else
					notSatisfied = false;

				measureString = partnerDevice("usb_daq").execute("3 1");
				successDouble = stringToValue(measureString, errorSignal);

				if( errorSignal > errorSignalLimit || errorSignal < (-1*errorSignalLimit) )
				{
					notSatisfied = false;
					std::cerr << "The Laser is out of Lock" << std::endl;
					enable = false;
					vortexLoopMutex->lock();
					{
						vortexLoopEnabled = enable;
						if(vortexLoopEnabled)
							vortexLoopCondition->signal();
					}
					success = false;
				}
			}
		}
		else
		{
			std::cerr << "Please choose a frequency between " << lowerFrequencyLimit << " & " << upperFrequencyLimit << " MHz" << std::endl;
			success = false;
		}

		success = true;
	}
	
	return success;
}
void vortexFrequencyScannerDevice::definePartnerDevices()
{
	addPartnerDevice("spectrumAnalyzer", "eplittletable.stanford.edu", 18, "agilentE4411bSpectrumAnalyzer"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("vortex", "eplittletable.stanford.edu", 2, "Scanning Vortex");
	addPartnerDevice("slow", "ep-timing1.stanford.edu", 4, "Slow Analog Out"); //local name (shorthand), IP address, module #, device name as defined in main function
	addPartnerDevice("usb_daq", "eplittletable.stanford.edu", 31, "usb1408fs"); //local name (shorthand), IP address, module #, device name as defined in main function
	//addPartnerDevice("mux", "eplittletable.stanford.edu", 5, "Agilent34970a"); //local name (shorthand), IP address, module #, device name as defined in main function
	//addPartnerDevice("marconi", "eplittletable.stanford.edu", 13, "marconi2022dFunctionGenerator");
	//addPartnerDevice("Digital Board", "ep-timing1.stanford.edu", 2, "Digital Out");
	
}

bool vortexFrequencyScannerDevice::deviceMain(int argc, char **argv)
{
	return false;
}
void vortexFrequencyScannerDevice::vortexLoopWrapper(void* object)
{
	vortexFrequencyScannerDevice* thisObject = static_cast<vortexFrequencyScannerDevice*>(object);
	thisObject->vortexLoop();
}
void vortexFrequencyScannerDevice::vortexLoop()
{
	unsigned long wait_s, wait_ns;
	string measureString;
	double appliedVoltage = 0;
	double appliedVoltageAverage = 0;
	double oldAppliedVoltage = 0;
	bool measureSuccess;
	bool commandSuccess;
	string piezoCommandString;
	double piezoVoltage = 0;
	double feedbackSign = -1;

	while(1) //never return in order to keep the thread alive
	{
		
		//
		omni_thread::yield();
		vortexLoopMutex->lock();
		{
			if(!vortexLoopEnabled)
				vortexLoopCondition->wait();
		}
		vortexLoopMutex->unlock();
		//
		//calculate absolute time to wake up
		omni_thread::get_time(&wait_s, &wait_ns, 1, 0); //only fill in the last 2 - computes the values for the first 2 arguments
		vortexLoopMutex->lock();
		{
			vortexLoopCondition->timedwait(wait_s, wait_ns);	//put thread to sleep
		}
		vortexLoopMutex->unlock();

		//get the actuator signal
		measureString = partnerDevice("usb_daq").execute("4 1");
		std::cerr << "The measured voltage is: " << measureString << std::endl;
		measureSuccess = stringToValue(measureString, appliedVoltage);
		
		if( (appliedVoltage > vortexLoopLimit) || (appliedVoltage < -vortexLoopLimit) )
		{
			measureString = partnerDevice("vortex").getAttribute("Piezo Voltage (V)");
			measureSuccess = stringToValue(measureString, piezoVoltage);
			std::cerr << "The measured piezo voltage is: " << measureString << std::endl;

			if(measureSuccess)
			{
				if( (appliedVoltage - vortexLoopLimit) > 0 )
					piezoVoltage = piezoVoltage - 0.1;
				else
					piezoVoltage = piezoVoltage + 0.1;

				oldAppliedVoltage = appliedVoltage;
			
				commandSuccess = partnerDevice("vortex").setAttribute("Piezo Voltage (V)", valueToString(piezoVoltage));

				//check to see that feedback signal changed & thus laser is still locked
		
				//wait for the laser to settle	
				//calculate absolute time to wake up
				omni_thread::get_time(&wait_s, &wait_ns, 1, 0); //only fill in the last 2 - computes the values for the first 2 arguments
				vortexLoopMutex->lock();
				{
					vortexLoopCondition->timedwait(wait_s, wait_ns);	//put thread to sleep
				}
				vortexLoopMutex->unlock(); 

				//get the actuator signal - average 10 of them together over 1 second
				for(int i=0; i<10; i++)
				{
					measureSuccess = stringToValue(partnerDevice("usb_daq").execute("6 1"), appliedVoltage);
					appliedVoltageAverage = (appliedVoltageAverage * i + appliedVoltage)/(i+1);
					Sleep(100);
				}
			
				//std::cerr << "The averaged voltage is: " << appliedVoltageAverage << std::endl;

				if( (appliedVoltageAverage > oldAppliedVoltage) || (appliedVoltageAverage < -oldAppliedVoltage) )
				{
			// laser has fallen out of lock
				
					vortexLoopMutex->lock();
					{
						// disable both the vortex loop and the lock
						vortexLoopEnabled = false;
						enable = false;

					}
					vortexLoopMutex->unlock();

					refreshAttributes();
					std::cerr << "Laser is out of lock! Fix it!" << std::endl;
				}
			}
		}

	}

}