/*! \file agilentL1500aSpectrumAnalyzerDevice.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for the class agilentL1500aSpectrumAnalyzerDevice
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

#include "agilentL1500aSpectrumAnalyzerDevice.h"

agilentL1500aSpectrumAnalyzerDevice::agilentL1500aSpectrumAnalyzerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber) : 
GPIB_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory, GCipAddress, GCmoduleNumber)
{ 
	//primaryAddress = primaryGPIBAddress; //normally 1
	//secondaryAddress = 0;
	//gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	numberPoints = 0;
	startFrequency = 0;
	stopFrequency = 500;
	referenceLevel = -50; //dBm
	peakLocation = 0;
	enableAveraging = false;

}
void agilentL1500aSpectrumAnalyzerDevice::defineGpibAttributes()
{
	addGpibAttribute("Start Frequency (Hz)", "FA", "", true);
	addGpibAttribute("Stop Frequency (Hz)", "FB", "", true);
	addGpibAttribute("Reference Level (dBm)", "RL", "", true);
	addGpibAttribute("Peak Location (Hz)", "MKPK HI; MKA", "", true);
}