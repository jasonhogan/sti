/*! \file NovatechChannelPair.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class NovatechChannelPair
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

#ifndef NOVATECH_CHANNEL_PAIR_H
#define NOVATECH_CHANNEL_PAIR_H

#include <STI_Device.h>

class NovatechChannelPair : public STI_Device
{
public:

    //***************BEGIN STI_Device functions***************//

    NovatechChannelPair(ORBManager* orb_manager,  std::string    DeviceName, 
		std::string Address,    unsigned short ModuleNumber, unsigned short lowFreqChannel_, unsigned short highFreqChannel_, std::string partnerName_);
    ~NovatechChannelPair();
    
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

    //****************END STI_Device functions***************//

	bool initialized;

private:

	double centerFreq;  //in MHz
	double deltaFreq;   //in MHz
	double freqRampRate; //in MHz/ms
	double maxResolution;  //in MHz
	unsigned short lowFreqChannel;
	unsigned short highFreqChannel;
	std::string partnerName;

    // Additional NovatechChannelPair class content...

};

#endif
