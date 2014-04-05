/*! \file STI_Device_Template.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class Novatech409B
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

#ifndef NOVATECH409B_H
#define NOVATECH409B_H

#include <STI_Device.h>
#include "rs232Controller.h"

class Novatech409B : public STI_Device
{
public:

    //***************BEGIN STI_Device functions***************//

	Novatech409B(ORBManager* orb_manager, const ConfigFile& configFile);
    //Novatech409B(ORBManager* orb_manager,  std::string    DeviceName, 
    //         std::string Address,    unsigned short ModuleNumber, unsigned short comPort);
    ~Novatech409B();
    
    // Device main()
    bool deviceMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
    bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
    bool writeChannel(unsigned short channel, const MixedValue& value);

    // Device Command line interface setup
    void definePartnerDevices();
    std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

	std::string getDeviceHelp();

    //****************END STI_Device functions***************//

	bool initialized;

private:

	void intializeSerial(unsigned short comPort);

	rs232Controller * serialController;

	class FrequencyChannel {
	public:
		double frequency;			//0 to 171.1276031 (MHz)
		double amplitude;			//0 to 100 (percent)
		double phase;				//0 to 360 (degrees)
	};

	int amplitudeMaxVal;
	int phaseMaxVal;

	std::vector <FrequencyChannel> frequencyChannels;

	void parseQUE(std::string queOutput);
	bool checkRanges(FrequencyChannel& triplet, std::string& error);
	bool refreshLocallyStoredFrequencyChannels();

	void getTriplet(const MixedValue& eventValue, FrequencyChannel& triplet);


	void addTablePoint(FrequencyChannel& triplet0, FrequencyChannel& triplet1, unsigned address, double dwell_ns, bool isHoldEvent);
	void makeTableCommand(FrequencyChannel& triplet, int channel, unsigned address, double dwell_ns, bool isHoldEvent, std::string& command);
	bool isHoldEvent(const RawEventMap::const_iterator& events);

	void preTableCommands();
	void postTableCommands();

	friend class NovatechTableEvent;

	class NovatechTableEvent : public SynchronousEventAdapter
	{
	public:
		NovatechTableEvent(double time, Novatech409B* device) : SynchronousEventAdapter(time, device), _this(device)
		{
		}
		void playEvent()
		{
			//put Novatech in Table Mode
			_this->serialController->queryDevice("M t", 50, 30);
		}
	private:
		Novatech409B* _this;
	};

	//class NovatechTableEvent : public SynchronousEventAdapter
	//{
	//public:
	//	NovatechTableEvent(
	//		double time, 
	//		FrequencyChannel& tripletChannel0,
	//		unsigned address, double dwell,
	//		STI_Device* device) :
	//	  SynchronousEventAdapter(time, device) 
	//	  {
	//		  makeTableCommand(tripletChannel0, 0, address, dwell, channelCommand_0);
	//		  bothChannelsChange = false;
	//	  }
	//	  
	//	  NovatechTableEvent(
	//		  double time, 
	//		  FrequencyChannel& tripletChannel0, 
	//		  FrequencyChannel& tripletChannel1, 
	//		  unsigned address, double dwell,
	//		  STI_Device* device) :
	//	  SynchronousEventAdapter(time, device) 
	//	  {
	//		  makeTableCommand(tripletChannel0, 0, address, dwell, channelCommand_0);
	//		  makeTableCommand(tripletChannel1, 1, address, dwell, channelCommand_1);
	//		  bothChannelsChange = true;
	//	  }
	//	
	//private:

	//	void makeTableCommand(FrequencyChannel& triplet, int channel, unsigned address, double dwell, std::string& command);

	//	std::string channelCommand_0;
	//	std::string channelCommand_1;
	//	bool bothChannelsChange;
	//};


};


#endif
