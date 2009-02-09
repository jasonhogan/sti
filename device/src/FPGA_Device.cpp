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

#include <FPGA_Device.h>

#include <sstream>
#include <string>

using std::stringstream;
using std::string;


FPGA_Device::FPGA_Device(ORBManager* orb_manager,  std::string    DeviceName, 
						 std::string IPAddress, unsigned short ModuleNumber) :
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber),
ramBlock(ModuleNumber),
ramBus( ramBlock.getStartAddress(), ramBlock.getSizeInWords() )
{
	addPartnerDevice("RAM Controller", IPAddress, 9, "RAMController");

	RamStartAttribute = "RAM_Start_Addr";
	RamEndAttribute   = "RAM_End_Addr";
	AutoRamAttribute  = "Auto RAM Allocation";
	autoRAM_Allocation = true;

	addAttribute(RamStartAttribute, ramBlock.getStartAddress() );
	addAttribute(RamEndAttribute, ramBlock.getEndAddress() );
	addAttribute(AutoRamAttribute, "On", "On, Off");

	addAttributeUpdater( new FPGA_AttributeUpdater(this) );
	
	//Registers for storing RAM parameters for this device
	//Each module uses 3 address that are 4 bytes each ==> 12 bytes
	RAM_Parameters_Base_Address = 0x90000008 + 12 * ModuleNumber;
	startRegisterOffset         = 0;
	endRegisterOffset           = 4;
	eventNumberRegisterOffset   = 8;

	registerBus = new EtraxBus(RAM_Parameters_Base_Address, 3);	//3 words wide
}

FPGA_Device::~FPGA_Device()
{
}


void FPGA_Device::autoAllocateRAM()
{
	string result;
	stringstream commandStream;
	stringstream minBufWriteAttribute;
	minBufWriteAttribute << "MinBufWriteTime_Module_" << getTDevice().moduleNum;

	bool doneAllocating = false;
	uInt32 address = 0;
	uInt32 minimumWriteTime = 0;

	//Buffer Allocation
	while( !doneAllocating && getDeviceStatus() == EventsLoading && autoRAM_Allocation)
	{
		minimumWriteTime = getMinimumWriteTime( ramBlock.getSizeInWords() );

		//Send minimum write time to the RAM controller
		partnerDevice("RAM Controller").setAttribute(minBufWriteAttribute.str(), valueToString(minimumWriteTime));

		//Ask the RAM controller to recalculate the buffer size; return true if it's acceptable
		result = partnerDevice("RAM Controller").execute("calculateBufferSize");
		stringToValue(result, doneAllocating);

		commandStream.str("getStartAddress "); 
		commandStream << getTDevice().moduleNum;
		result = partnerDevice("RAM Controller").execute( commandStream.str() );
		stringToValue(result, address);
		setAttribute(RamStartAttribute, address);

		commandStream.str("getEndAddress ");
		commandStream << getTDevice().moduleNum;
		result = partnerDevice("RAM Controller").execute( commandStream.str() );
		stringToValue(result, address);
		setAttribute(RamEndAttribute, address);
	}
}


void FPGA_Device::loadDeviceEvents()
{

	autoAllocateRAM();

	//Setup the RAM bus so that events can be written to RAM
	ramBus.setMemoryAddress( ramBlock.getStartAddress(), ramBlock.getSizeInWords() );
	
	SynchronousEventVector& events = getSynchronousEvents();
	numberOfEvents = static_cast<uInt32>(events.size());

	//Setup Event Addresses
	unsigned i,j;
	for(i = 0; i < events.size(); i++)
	{
		events.at(i).setupEvent();
	}

	//use j to count up to the RAM size
	//Fill the allocated RAM with events
	for(i = 0, j = 0; (j < ramBlock.getSizeInWords() && i < events.size()); j += 2, i++)	//step by 2 addresses for each event
	{
		events.at(i).loadEvent();
	}

	//RAM is full (or all events are loaded)
	//Tell the FPGA that the events are ready to load into the timing core FIFO
	writeRAM_Parameters();

	if( !changeStatus(EventsLoaded) )
	{
		//something is wrong; this shouldn't happen
		reportMessage(LoadingError, "FPGA_Device::loadDeviceEvents() Failed to change device status to 'Loaded' after filling the RAM.");
		return;
	}

	//Double Buffering
	bool bufferUnderflowed = false;

	uInt32 bufferSize = ramBlock.getSizeInWords() / 2;	//size in terms of events
	uInt32 currentEvent;
	
	while( !bufferUnderflowed && i < numberOfEvents)
	{
		currentEvent = getCurrentEventNumber();
		
		if(currentEvent > i + bufferSize)
		{
			reportMessage(LoadingError, "Buffer Underflow Error");
			bufferUnderflowed = true;
		}
		if(currentEvent > i + (bufferSize / 2))
		{
			//Write to half the buffer
			for(i, j = 0; ( j <  (bufferSize / 2) 
				&& i <  numberOfEvents && i <= staleEventNumber ); i++, j += 2)
			{
				events.at(i).loadEvent();
			}
		}
	}
}

void playDeviceEvents()
{
	staleEventNumber = 0;

	SynchronousEventVector& events = getSynchronousEvents();

	for(unsigned i = 0; i < events.size(); i++)
	{
		events.at(i).playEvent();
		events.at(i).collectMeasurementData();
		staleEventNumber = i;
	}
}

uInt32 FPGA_Device::getCurrentEventNumber()
{
	uInt32 eventsRemaining = registerBus->readData(eventNumberRegisterOffset) / 2;
	
	if(numberOfEvents >= eventsRemaining)
		return (numberOfEvents - eventsRemaining);
	else
		return numberOfEvents;
}

void FPGA_Device::writeRAM_Parameters()
{
	//Setup the RAM parameters so the timing core know where in RAM the events are stored
	registerBus->writeData( ramBlock.getStartAddress(), startRegisterOffset );
	registerBus->writeData( ramBlock.getEndAddress(), endRegisterOffset );
	registerBus->writeData( numberOfEvents, eventNumberRegisterOffset);
}

uInt32 FPGA_Device::getMinimumWriteTime(uInt32 bufferSize)
{
	SynchronousEventVector& events = getSynchronousEvents();
	
	//bufferSize is in words; 
	//evtBufferSize is the size of half the buffer as measured in Events (2 words each)
	unsigned evtBufferSize = (bufferSize / 4);

	uInt64 newTime = 0;
	uInt64 minimumWriteTime = 0;

	unsigned i = 0;
	while( i < events.size() )
	{
		i += evtBufferSize;

		if( i < events.size() )
			newTime = events.at(i).getTime() - events.at(i - evtBufferSize).getTime();
		else
			newTime = events.back().getTime() - events.at(i - evtBufferSize).getTime();
	
		if(newTime < minimumWriteTime || i == evtBufferSize)
			minimumWriteTime = newTime;
	}

	if(minimumWriteTime > 0xffffffff)
		return 0xffffffff;
	else
		return static_cast<uInt32>(minimumWriteTime);
}

bool FPGA_Device::FPGA_AttributeUpdater::updateAttributes(string key, string value)
{
	uInt32 tempInt;
	bool successInt = device_->stringToValue(value, tempInt);

	bool success = true;
	
	if(key.compare(device_->RamStartAttribute) == 0 && successInt)	{		success = device_->ramBlock.setStartAddress(tempInt);
	}
	else if(key.compare(device_->RamEndAttribute) == 0 && successInt)	{		success = device_->ramBlock.setEndAddress(tempInt);
	}
	else if(key.compare(device_->AutoRamAttribute) == 0)
	{
		success = true;

		if(value.compare("Off") == 0)
			device_->autoRAM_Allocation = false;
		else if(value.compare("On") == 0)
			device_->autoRAM_Allocation = true;
		else
			success = false;
	}

	return success;
}
void FPGA_Device::FPGA_AttributeUpdater::refreshAttributes()
{
	device_->setAttribute( device_->RamStartAttribute, device_->ramBlock.getStartAddress() );
	device_->setAttribute( device_->RamEndAttribute, device_->ramBlock.getEndAddress() );
	device_->setAttribute( device_->AutoRamAttribute, (device_->autoRAM_Allocation ? "On" : "Off") );
}


FPGA_Device::FPGA_Event::FPGA_Event(double time, FPGA_Device* device,
									unsigned eventNumber) : 
BitLineEvent(time, device), 
device_f(device)
{
	setEventAddress(eventNumber);
}

void FPGA_Device::FPGA_Event::setEventAddress(unsigned eventNumber)
{
	eventNumber_ = eventNumber;
}

void FPGA_Device::FPGA_Event::setupEvent()
{
	time32 = static_cast<uInt32>( getTime() );
	timeAddress  = device_f->ramBlock.getWrappedAddress( 2*eventNumber_ );
	valueAddress = device_f->ramBlock.getWrappedAddress( 2*eventNumber_ + 1 );
}

void FPGA_Device::FPGA_Event::loadEvent()
{
	//write the event to RAM
	device_f->ramBus.writeData( time32, timeAddress );
	device_f->ramBus.writeData( getValue(), valueAddress );
}

void FPGA_Device::FPGA_Event::playEvent()
{
	//Wait until this event has been loaded into the device's FIFO before returning.
	//(Ideally wait until the event has been played...)
	while(device_f->getCurrentEventNumber() < eventNumber_) {}
}
