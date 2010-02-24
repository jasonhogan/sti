/*! \file agilentE4411bSpectrumAnalyzerDevice.h
 *  \author David M.S. Johnson
 *  \brief header file for agilentE4411bSpectrumAnalyzerDevice class
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


#ifndef agilentE4411bSpectrumAnalyzerDEVICE_H
#define agilentE4411bSpectrumAnalyzerDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "GPIB_Device.h"
#include <map>
#include <string>


class agilentE4411bSpectrumAnalyzerDevice : public GPIB_Device
{
public:
	
	agilentE4411bSpectrumAnalyzerDevice(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory = "//atomsrv1/EP/Data/deviceLogFiles",
							std::string GCipAddress = "eplittletable.stanford.edu",
							unsigned short GCmoduleNumber = 0);
	~agilentE4411bSpectrumAnalyzerDevice() {};

private:

// Device main()
	bool deviceMain(int argc, char** argv) {return false;};    //called in a loop while it returns true

    // Device Attributes
	void defineGpibAttributes();
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value) {return false;}


	// Device Command line interface setup
	std::string execute(int argc, char** argv);


	//functions for generating commands
	//bool saveData(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector); //saves a trace to two vectors, one for frequency, one for data


	unsigned short primaryAddress;
	unsigned short secondaryAddress;
	std::string gpibID;

	int numberPoints;
	double referenceLevel;
	double normalizedReferenceLevel;
	double referenceLevelPosition;
	double powerPerDivision;
	double centerFrequency;
	double frequencySpan;
	double startFrequency;
	double stopFrequency;
	double freqIncrement;
	double peakLocation;
	bool enableAveraging;
	//bool initialized;

};

#endif

