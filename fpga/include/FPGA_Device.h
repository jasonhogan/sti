/*! \file FPGA_Device.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class FPGA_Device
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

#ifndef STI_FPGA_FPGA_DEVICE_H
#define STI_FPGA_FPGA_DEVICE_H

#include "FPGATypes.h"
#include "STI_Device.h"
#include "FPGA_RAM_Block.h"

#include "AttributeUpdater.h"
#include "FPGA_BitLineEvent.h"

//#include <boost/thread/locks.hpp>
//#include <boost/thread.hpp>

namespace STI
{
namespace FPGA
{

class FPGA_Device : public STI::Device::STI_Device
{
public:
//	FPGA_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);
	FPGA_Device(const std::string& deviceName, const std::string& configFilename);
	FPGA_Device(const std::string& deviceName, const std::string& IPAddress, unsigned short ModuleNumber);

//	FPGA_Device(ORBManager* orb_manager, std::string DeviceName, 
//			std::string IPAddress, unsigned short ModuleNumber);
	virtual ~FPGA_Device();

	void makeNewDeviceEventEngine(const STI::TimingEngine::EngineID& engineID, STI::TimingEngine::EventEngine_ptr& newEngine);

private:

	// Device main()
	virtual bool deviceMain(int argc, char* argv[]) = 0;	//called in a loop while it returns true

	// Device Attributes
	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

	// Device Channels
	virtual void defineChannels() = 0;

	// Device Command line interface setup
	virtual void definePartnerDevices() = 0;
	virtual std::string execute(int argc, char** argv) = 0;


	// Device-specific event parsing
	virtual void parseDeviceEventsFPGA(const STI::TimingEngine::TimingEventGroupMap& eventsIn, STI::TimingEngine::SynchronousEventVector& eventsOut) 
		throw(std::exception) = 0;
	virtual double getMinimumEventStartTime() = 0;

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	virtual void pauseEventPlayback() = 0;	//for devices that require non-generic pause commands


private:
	void FPGA_init();

	void loadDeviceEvents();

	bool readChannel(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut);
	bool writeChannel(unsigned short channel, const STI::Utils::MixedValue& commandIn);

	void parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, STI::TimingEngine::SynchronousEventVector& eventsOut) 
		throw(std::exception);
public:
	
	virtual short wordsPerEvent() const;
	uInt32 queryEventsRemainingRegister();

private:

	void writeRAM_Parameters(uInt32 startAddress, uInt32 endAddress, uInt32 numberOfEvents);

	STI::FPGA::FPGALoadAccessPolicy_ptr fpgaLoadPolicy;
	
	FPGA_RAM_Block_ptr moduleRamBlock;			//RAM allocation helper for the entire device.
	FPGA_RAM_Block_ptr parsingEngineRAMBlock;	//RAM allocation for the engine parsing, loading, etc.
	STI::TimingEngine::EngineID* parsingEngineID;

	EtraxBus_ptr registerBus;	//For specifying start and end registers and the event counter.
	EtraxBus_ptr ramBus;		//Handle to the event FIFO memory space on the FPGA.

	uInt32 RAM_Parameters_Base_Address;
	uInt32 startRegisterOffset;
	uInt32 endRegisterOffset;
	uInt32 eventNumberRegisterOffset;

	std::string RamStartAttribute;
	std::string RamEndAttribute;

	class FPGA_AttributeUpdater : public STI::Device::AttributeUpdater
	{
	public:
		FPGA_AttributeUpdater(FPGA_Device* device) : 
		  AttributeUpdater(device), device_(device) {}
		void defineAttributes();
		bool updateAttributes(std::string key, std::string value);
		void refreshAttributes();
	private:
		FPGA_Device* device_;
	};


protected:
	
	friend class FPGADeviceEventEngine;
	friend class FPGA_BitLineEvent_DeviceWrapper;

//	template<int N> friend class FPGA_BitLineEvent;

};

}
}

#endif
