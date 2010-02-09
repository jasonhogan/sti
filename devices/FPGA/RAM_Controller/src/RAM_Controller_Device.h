/*! \file RAM_Controller.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class RAM_Controller
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

#ifndef RAM_CONTROLLER
#define RAM_CONTROLLER

#include <STI_Device.h>
#include "FPGA_RAM_Block.h"

#include <vector>


class RAM_Controller_Device : public STI_Device
{
public:

	RAM_Controller_Device(ORBManager* orb_manager,  std::string    DeviceName, 
             std::string IPAddress,    unsigned short ModuleNumber);
	~RAM_Controller_Device();

private:
	// Device main()
    bool deviceMain(int argc, char** argv);

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
    bool readChannel(DataMeasurement& Measurement);
    bool writeChannel(const RawEvent& Event);

    // Device Command line interface setup
    void definePartnerDevices();
    std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback();
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

	//****************END STI_Device functions***************//

	uInt32 getWriteTime(unsigned module);
	uInt32 getStartAddress(unsigned module);
	uInt32 getEndAddress(unsigned module);
	uInt32 getBufferSize(unsigned module);
	bool setBufferSize(unsigned module, uInt32 value);
	bool calculateBufferSize();
	bool calculateNewRAMSizes();


	std::vector<FPGA_RAM_Block> RAM_blocks;
	std::vector<uInt32> writeTimes;
	
	omni_mutex* calculateBufferSizeMutex;
	omni_condition* calculateBufferSizeCondition;

	bool calculatingBufferSize;
	unsigned numberOfRegisteredModules;
	unsigned numberOfModulesCalculatingBufferSize;
	unsigned allocationCycles;

};

#endif

