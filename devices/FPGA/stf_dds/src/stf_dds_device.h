/*! \file ADF4360.h
 *  \author David M.S. Johnson 
 *  \brief Include-file for the class FPGA_daughter_board::STF_AD_FAST
 *  \section license License
 *
 *  Copyright (C) 2008 David M.S. Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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


#ifndef STF_DDS_DEVICE_H
#define STF_DDS_DEVICE_H

#include "FPGA_Device.h"
#include "ParsedDDSValue.h"
#include <math.h>

class STF_DDS_Device : public FPGA_Device
{
	class DDS_Event;
	class DDS_Parameters;
public:

	STF_DDS_Device(ORBManager* orb_manager, std::string configFilename);
	~STF_DDS_Device() {};

private:
	//STI_Device functions

	// Device main()
	bool deviceMain(int argc, char **argv) {return false;};

	// Device Attributes
	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	// Device Channels
	void defineChannels();

	// Device Command line interface setup
	std::string execute(int argc, char **argv) {return "";};
	void definePartnerDevices() {}; // requires none

	// Device-specific event parsing
	void parseDeviceEventsFPGA(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);
	double getMinimumEventStartTime() { return minimumAbsoluteStartTime; }
	
	// Event Playback control
	void stopEventPlayback() {};
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

	short wordsPerEvent() const {return 3;}; //DDS is special in that it requires 3 words per event

	DDS_Event* generateDDScommand(double time, uInt32 addr);
	uInt32 generateDDSphase(double doublePhase);
	uInt32 generateDDSamplitude(double doubleAmplitude);
	
	uInt32 generateDDSfrequency(double doubleFrequency);
	double generateDDSfrequencyInMHz(uInt32 hexFrequency);

	uInt32 generateRampRate(double rampRateInPercent);

	bool parseVectorType( RawEvent eventVector, vector<int> * commandList);
	bool parseStringType( RawEvent eventString, vector<int> * commandList);
	bool parseFrequencySweep(double startVal, double endVal, double rampTime);
	bool checkSettings();
	void restoreDefaults();
	void setSweepMode(unsigned k);
	void setNormalMode(unsigned k);

	std::string errorMessage;

	bool initialized;
	bool sweepMode;

	bool updateDDS; //allows multiple attributes to be changed before running a timing sequence to update
	bool notInitialized; //determines if DDS has been setup with correct VCO freq, etc.. If it has, don't need to re-run every time
	bool IOUpdate;
	bool sweepOnLastCommand;

	bool ExternalClock;
	double extClkFreq; // 25-500 MHz
	double crystalFreq; //25 MHz
	double sampleFreq; // internal sampling rate of DDS chip. Should be 500 MSPS
	double SYNC_CLK;  // 0.25*sampleFreq.  In MHz, even though sampleFreq is in MSPS.
	uInt32 PLLmultiplier; // valid values are 4-20. Multiplier for the input clock. 10*25 MHz crystal = 250 MHz -> 0x80000000 = 250 MHz
	uInt32 activeChannel;
	bool VCOEnable;
	uInt32 ModulationLevel; // set to 0 for now

	double eventSpacing; //minimum time between events
	double holdOff; //calibrated time to sync DDS with digital line

	double minimumAbsoluteStartTime; //needed for getMinimumEventStartTime();

	vector<DDS_Parameters> dds_parameters;

	class DDS_Parameters {
	public:
		
		DDS_Parameters();

		bool sweepMode;

		uInt32 ChargePumpControl; // higher values increase the charge pump current
		uInt32 ProfilePinConfig; // Determines how the profile pins are configured
		uInt32 RuRd; // Ramp Up / Ramp Down control
		
		uInt32 AFPSelect;
		bool LSnoDwell;
		bool LinearSweepEnable;
		bool LoadSRR;
		bool AutoclearSweep;
		bool ClearSweep;
		bool AutoclearPhase;
		bool ClearPhase;
		bool SinCos;
		uInt32 DACCurrentControl;
		uInt32 Phase;
		double PhaseInDegrees;
		uInt32 Frequency;
		double FrequencyInMHz;
		bool AmplitudeEnable;
		bool LoadARR;
		uInt32 Amplitude;
		double AmplitudeInPercent;
		uInt32 risingDeltaWord;
		uInt32 fallingDeltaWord;
		uInt32 risingSweepRampRate;
		uInt32 fallingSweepRampRate;
		uInt32 sweepEndPoint;
		double risingDeltaWordInMHz;
		double fallingDeltaWordInMHz;
		double sweepEndPointInMHz;
		double risingSweepRampRateInPercent;
		double fallingSweepRampRateInPercent;
		bool profilePin;
		bool sweepOnLastCommand;
		bool sweepUpFast;

	};


	enum DDSEventType {Change, Sweep, None};
	
	
	class DDS_Event : public FPGA_BitLineEvent<64>
	{
	public:
		DDS_Event(double time, uInt32 command, uInt32 value, FPGA_Device* device);
		DDS_Event(const DDS_Event &copy) : FPGA_BitLineEvent<64>(copy) { }

		void setupEvent();
		void loadEvent();
		void playEvent(){}; //no implementation for DDS
		void collectMeasurementData(){}; //no implementation for DDS

	private:
		uInt32 timeAddress;
		uInt32 valueAddress;
		uInt32 commandAddress;
		uInt32 time32;

		FPGA_Device* device_f;
	};
	


};


#endif
