/*! \file FPGA_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class FPGA_Device
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

#include "FPGA_Device.h"
//#include "PartnerDevice.h"
#include <STI_Exception.h>

#include "FPGADeviceEventEngine.h"
#include "FPGALoadAccessPolicy.h"
#include "EtraxBus.h"

#include <ConfigFile.h>

#include <sstream>
#include <string>

using std::stringstream;
using std::string;
using std::endl;

using STI::FPGA::FPGA_Device;

//FPGA_Device::FPGA_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename) :
//STI_Device(orb_manager, DeviceName, configFilename)
//FPGA_Device(std::string DeviceName, std::string IPAddress, unsigned short ModuleNumber)
//: STI_Device(DeviceName, IPAddress, ModuleNumber)
//{
//	FPGA_init();
//
//	ConfigFile config(configFilename);
//	unsigned int time;
//	bool parseSuccess = config.getParameter("Minimum Polling Interval (ms)", time);	//optional parameter
//
//	if(parseSuccess)
//	{
//		pollTime_ms = time;		//overrides the default 100 ms defined in FPGA_init()
//	}
//
//}
FPGA_Device::FPGA_Device(const std::string& deviceName, const std::string& configFilename) :
STI_Device(deviceName, configFilename)
{
	FPGA_init();
}

FPGA_Device::FPGA_Device(const std::string& deviceName, 
						 const std::string& IPAddress, unsigned short ModuleNumber) :
STI_Device(deviceName, IPAddress, ModuleNumber)
{
	FPGA_init();
}

void FPGA_Device::makeNewDeviceEventEngine(const STI::TimingEngine::EngineID& engineID, STI::TimingEngine::EventEngine_ptr& newEngine)
{
	using STI::TimingEngine::EventEngine_ptr;

	STI::FPGA::FPGADeviceEventEngineSetup engineSetup;
	engineSetup.wordsPerEvent = wordsPerEvent();

	STI::FPGA::FPGA_RAM_Block_ptr newRAMBlock = STI::FPGA::FPGA_RAM_Block_ptr(new FPGA_RAM_Block(getDeviceID().getModule()));

	newEngine = EventEngine_ptr(new FPGADeviceEventEngine(engineID, *this, engineSetup, newRAMBlock));
	fpgaLoadPolicy->addEngine(engineID, newRAMBlock);
}

void FPGA_Device::FPGA_init()
{
	using STI::FPGA::FPGA_RAM_Block_ptr;
	using STI::FPGA::FPGALoadAccessPolicy;
	using STI::FPGA::EtraxBus_ptr;

	moduleRamBlock = FPGA_RAM_Block_ptr(new FPGA_RAM_Block());
	moduleRamBlock->setModuleNumber(getDeviceID().getModule());

	fpgaLoadPolicy = STI::FPGA::FPGALoadAccessPolicy_ptr( new FPGALoadAccessPolicy(moduleRamBlock) );
	
//	STI::TimingEngine::LoadAccessPolicy_ptr newLoadPolicy = fpgaLoadPolicy;
	eventEngineManager->setLoadPolicy(fpgaLoadPolicy);

	std::string IPAddress = getDeviceID().getAddress();
	unsigned short ModuleNumber = getDeviceID().getModule();

	addPartnerDevice("Trigger", IPAddress, 8, "FPGA_Trigger");
	partnerDevice("Trigger").enablePartnerEvents();

	RamStartAttribute = "RAM_Start_Addr";
	RamEndAttribute   = "RAM_End_Addr";

	STI::Device::AttributeUpdater_ptr updater(new FPGA_AttributeUpdater(this));
	addAttributeUpdater(updater);
	
	//Registers for storing RAM parameters for this device
	//Each module uses 3 address that are 4 bytes each ==> 12 bytes
	RAM_Parameters_Base_Address = 0x90000008 + 12 * ModuleNumber;

	startRegisterOffset         = 0;
	endRegisterOffset           = 4;
	eventNumberRegisterOffset   = 8;

	registerBus = EtraxBus_ptr(new EtraxBus(RAM_Parameters_Base_Address, 3));	//3 words wide
	ramBus      = EtraxBus_ptr(new EtraxBus( moduleRamBlock->getStartAddress(), moduleRamBlock->getSizeInWords() ));

//	setSaveAttributesToFile(true);
}

FPGA_Device::~FPGA_Device()
{
}

void FPGA_Device::writeRAM_Parameters(uInt32 startAddress, uInt32 endAddress, uInt32 numberOfEvents)
{
	//Setup the RAM parameters so the timing core know where in RAM the events are stored
	registerBus->writeData( startAddress, startRegisterOffset );
	registerBus->writeData( endAddress, endRegisterOffset );
	registerBus->writeData( wordsPerEvent() * numberOfEvents, eventNumberRegisterOffset );	//write number of RAM entries
}


void FPGA_Device::parseDeviceEvents(const STI::TimingEngine::TimingEventGroupMap& eventsIn, 
									STI::TimingEngine::SynchronousEventVector& eventsOut)
throw(std::exception)
{
	if(eventsIn.size() > 0)
	{
		//Submit an arm request to the trigger for this device.  This configures the trigger's armBits so that
		//trigger events will properly start and stop this FPGA device.
		partnerDevice("Trigger").event(0, getDeviceID().getModule(), "Stop", eventsIn.begin()->second->at(0));
		partnerDevice("Trigger").event(1, getDeviceID().getModule(), "Play", eventsIn.begin()->second->at(0));
	}

	parseDeviceEventsFPGA(eventsIn, eventsOut);

	//Attempt to resize the sub block's memory to fit the requested event list.
	fpgaLoadPolicy->setSubBlockSize(*parsingEngineID, wordsPerEvent() * eventsOut.size());
	
	//First check if the module has enough memory if the entire RAM is used for these events.
	if(wordsPerEvent() * eventsOut.size() > moduleRamBlock->getSizeInWords()) {
		int bytesRequired = wordsPerEvent() * eventsOut.size() * moduleRamBlock->getRAM_Word_Size();
		int bytesAvailable = moduleRamBlock->getSizeInBytes();

		std::stringstream memErr;
		memErr << "Not enough memory in FPGA module " 
			<< getDeviceID().getModule() << " ("  << getDeviceID().getName() << ")."<< std::endl
			<< "       Required: " << bytesRequired << " bytes." << endl
			<< "       Available: " << bytesAvailable << " bytes.";

		throw STI_Exception( memErr.str() );
	}
	
	//Now check to see if this sub block was able to resize to fit the events.
	if(wordsPerEvent() * eventsOut.size() > parsingEngineRAMBlock->getSizeInWords()) {
		//Resizing failed!
		int bytesRequired = wordsPerEvent() * eventsOut.size() * parsingEngineRAMBlock->getRAM_Word_Size();
		int bytesAvailable = parsingEngineRAMBlock->getSizeInBytes();
		int bytesAvailableInModule = moduleRamBlock->getSizeInBytes();

		std::stringstream memErr;
		memErr << "Not enough memory available for engine " << parsingEngineID->print() << "in FPGA module"
			<< getDeviceID().getModule() << " ("  << getDeviceID().getName() << ")."<< std::endl
			<< "       Required: " << bytesRequired << " bytes." << endl
			<< "       Available to engine: " << bytesAvailable << " bytes." << endl
			<< "       Available to module: " << bytesAvailableInModule << " bytes.";
		if(bytesAvailableInModule >= bytesRequired && bytesAvailableInModule > bytesAvailable) {
			memErr << "The engine's RAM block may have failed to resize for one of the following reasons:" << std::endl
				   << "  (1) Adjacent engine RAM blocks are not overlappable." << std::endl
				   << "  (2) The engine's RAM block is not resizeable." << std::endl;
		}

		throw STI_Exception( memErr.str() );
	}
}

bool FPGA_Device::readChannel(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut)
{
	return readChannelDefault(channel, commandIn, measurementOut, getMinimumEventStartTime());
}

bool FPGA_Device::writeChannel(unsigned short channel, const STI::Utils::MixedValue& commandIn)
{
	return writeChannelDefault(channel, commandIn, getMinimumEventStartTime());
}






short FPGA_Device::wordsPerEvent() const
{
	return 2;
}

void FPGA_Device::FPGA_AttributeUpdater::defineAttributes()
{
	addAttribute(device_->RamStartAttribute, 
		STI::Utils::valueToString(device_->moduleRamBlock->getDefaultStartAddress(), "", std::ios::hex) );
	addAttribute(device_->RamEndAttribute, 
		STI::Utils::valueToString(device_->moduleRamBlock->getDefaultEndAddress(), "", std::ios::hex) );
}

bool FPGA_Device::FPGA_AttributeUpdater::updateAttributes(string key, string value)
{
	uInt32 tempInt;
	bool successInt = STI::Utils::stringToValue(value, tempInt, std::ios::hex);	//comming from a hex string

	bool success = false;
	
	if(key.compare(device_->RamStartAttribute) == 0 && successInt)
	{
		device_->moduleRamBlock->setStartAddress(tempInt);
		success = true;
	}
	else if(key.compare(device_->RamEndAttribute) == 0 && successInt)
	{
		device_->moduleRamBlock->setEndAddress(tempInt);
		success = true;
	}

	if(success) {
		//Update the memory handle to point to the newly defined RAM addresses.
		device_->ramBus->setMemoryAddress( 
			device_->moduleRamBlock->getStartAddress(), 
			device_->moduleRamBlock->getSizeInWords() );
	}

	return success;
}
void FPGA_Device::FPGA_AttributeUpdater::refreshAttributes()
{
	setAttribute( device_->RamStartAttribute, 
		STI::Utils::valueToString(device_->moduleRamBlock->getStartAddress(), "", std::ios::hex) );
	setAttribute( device_->RamEndAttribute, 
		STI::Utils::valueToString(device_->moduleRamBlock->getEndAddress(), "", std::ios::hex) );
}


uInt32 FPGA_Device::queryEventsRemainingRegister()
{
	return registerBus->readData(eventNumberRegisterOffset) / wordsPerEvent();  //events remaining to load from RAM FIFO to the timing core
}




