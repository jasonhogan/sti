/*! \file STI_Device_Template.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the example class PDMonitorDevice
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

#ifndef PDMONITOR_DEVICE_H
#define PDMONITOR_DEVICE_H

#include <STI_Application.h>
#include <TaggedConfigFile.h>
#include <vector>

class PDMonitorDevice : public STI_Application
{
public:

    //***************BEGIN STI_Device functions***************//

    PDMonitorDevice(ORBManager* orb_manager,  std::string    DeviceName, 
		std::string configFile, std::string appConfigFilePath);
    ~PDMonitorDevice();
    

	void defineFunctionCalls() {};
	MixedData handleFunctionCall(std::string functionName, std::vector<MixedValue> args);

    // Application main()
    bool appMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineAppChannels();
    bool readAppChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
    bool writeAppChannel(unsigned short channel, const MixedValue& value);

    // Device Command line interface setup
    void definePartnerDevices();
    std::string Execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};


	bool initialized;

    //****************END STI_Device functions***************//


    // Additional PDMonitorDevice class content...
private:

	bool enableDataLogging;
	bool enableCalibration;
	double testDeviceValue;

	TaggedConfigFile appConfigFile;

	bool getChannels(std::map<unsigned short, unsigned short> &channels);
	bool getLabels(std::map<unsigned short, std::string> &channelLabels);
	bool getSetpoints(std::map<unsigned short, double > &channelSetpoints);
	bool getLayout(std::map<unsigned short, std::vector<int> > &channelLayout);
	bool getPartnerDevices();

	std::vector<std::vector<std::string> > partnerSettings; //contains name, module, and ip

	class PhotoDetector {
	public:

		PhotoDetector():value(0),setpoint(0),slope(0),
			offset(0),deviceChannel(0),usbDAQChannel(0),label("") {};
		~PhotoDetector() {};

		double value;
		double setpoint;
		double slope;
		double offset;
		unsigned short deviceChannel;
		unsigned short usbDAQChannel;
		std::string label;
		std::vector <int> layout;
	};

	std::map<unsigned short, PhotoDetector> photoDetectorMap;
	
	void getCommaColonMap(std::vector<std::string> &field, std::map<std::string, std::string> &fieldMap);
	void getCommaColonVector(std::vector<std::string> &field, std::vector<std::vector<std::string> > &fieldVector);
	bool getCalibration(PhotoDetector &pd);
	bool writeSetpoints();
		
};

#endif
