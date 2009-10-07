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
ramBlock(ModuleNumber)
{
	addMutualPartnerDevice("Trigger", IPAddress, 8, "FPGA_Trigger");
	addPartnerDevice("RAM Controller", IPAddress, 9, "RAM_Controller");

	RamStartAttribute = "RAM_Start_Addr";
	RamEndAttribute   = "RAM_End_Addr";
	AutoRamAttribute  = "Auto RAM Allocation";
	autoRAM_Allocation = true;

	addAttributeUpdater( new FPGA_AttributeUpdater(this) );
	
	//Registers for storing RAM parameters for this device
	//Each module uses 3 address that are 4 bytes each ==> 12 bytes
	RAM_Parameters_Base_Address = 0x90000008 + 12 * ModuleNumber;

	startRegisterOffset         = 0;
	endRegisterOffset           = 4;
	eventNumberRegisterOffset   = 8;

//cout << "(FPGA_Device) Memory Address MODIFIED2 = " << RAM_Parameters_Base_Address << endl;

	registerBus = new EtraxBus(RAM_Parameters_Base_Address, 3);	//3 words wide
	ramBus      = new EtraxBus( ramBlock.getStartAddress(), ramBlock.getSizeInWords() );
}

FPGA_Device::~FPGA_Device()
{
	delete ramBus;
	delete registerBus;
}

bool FPGA_Device::readChannel(ParsedMeasurement& Measurement)
{
	RawEvent rawEvent(Measurement);

	return writeChannel(rawEvent);
}

bool FPGA_Device::writeChannel(const RawEvent& Event)
{
	//implementation based on "single-line timing file" scheme

	unsigned i;
	
	getSynchronousEvents().clear();

	changeStatus(EventsEmpty);

	RawEventMap rawEventsIn;
	rawEventsIn[Event.time()].push_back( Event );	//at time 0

	try {
		parseDeviceEvents(rawEventsIn, getSynchronousEvents() );	//pure virtual
	}
	catch(...)	//generic conflict or error
	{
std::cerr << "writeChannel exception caught!!" << std::endl;
		return false;
	}

	//sort in time order
	getSynchronousEvents().sort();

	//Assign event numbers
	for(i = 0; i < getSynchronousEvents().size(); i++)
	{
		getSynchronousEvents().at(i).setEventNumber( i );
	}


	bool autoOld = autoRAM_Allocation;
	autoRAM_Allocation = false;
	ramBlock.setRAM_Block_Size(wordsPerEvent() * getSynchronousEvents().size() );

	loadEvents();
	
	autoRAM_Allocation = autoOld;
	
	while( !eventsLoaded() ) {}

std::cerr << "About to play # " << getSynchronousEvents().size() << std::endl;
	playEvents();

//	cerr << "Measurement Check time: " << getSynchronousEvents().at(0).getMeasurement()->time() << endl;
//	cerr << "Measurement Check numberValue: " << getSynchronousEvents().at(0).getMeasurement()->numberValue() << endl;

	bool success = true;
	stringstream commandStream;
	string result;

	commandStream.str(""); 
	commandStream << "trigger " << getTDevice().moduleNum;
	result = partnerDevice("Trigger").execute( commandStream.str() );
//	stringToValue(result, success);

	while(getDeviceStatus() == Running) {};

	return success;
}


void FPGA_Device::autoAllocateRAM()
{
	string result;
	stringstream minBufWriteAttribute;
	minBufWriteAttribute << "MinWriteTime Mod_" << getTDevice().moduleNum;

	bool doneAllocating = false;
	uInt32 minimumWriteTime = 0;

	//Buffer Allocation
	while( !doneAllocating && getDeviceStatus() == EventsLoading && autoRAM_Allocation)
	{
		autoRAM_Allocation = getAddressesFromController();

		minimumWriteTime = getMinimumWriteTime( ramBlock.getSizeInWords() );

		//Send minimum write time to the RAM controller
		partnerDevice("RAM Controller").setAttribute(minBufWriteAttribute.str(), valueToString(minimumWriteTime));

		//Ask the RAM controller to recalculate the buffer size; return true if it's acceptable
		result = partnerDevice("RAM Controller").execute("calculateBufferSize");
		doneAllocating |= !stringToValue(result, doneAllocating);	//done if conversion fails too
	}
}

void FPGA_Device::sendAddressesToController()
{
	if( !partnerDevice("RAM Controller").isAlive() )
		return;

	bool success = true;
	stringstream commandStream;
	string result;

	commandStream.str(""); 
	commandStream << "setStartAddress " << getTDevice().moduleNum << " " << ramBlock.getStartAddress();
	result = partnerDevice("RAM Controller").execute( commandStream.str() );
	stringToValue(result, success);

	commandStream.str("");
	commandStream << "setEndAddress " << getTDevice().moduleNum << " " << ramBlock.getEndAddress();
	result = partnerDevice("RAM Controller").execute( commandStream.str() );
	stringToValue(result, success);
}

bool FPGA_Device::getAddressesFromController()
{
	if( !partnerDevice("RAM Controller").isAlive() || !autoRAM_Allocation)
		return false;

	bool success = true;
	stringstream commandStream;
	string result;
	uInt32 address = 0;

	commandStream.str(""); 
	commandStream << "getStartAddress " << getTDevice().moduleNum;
	result = partnerDevice("RAM Controller").execute( commandStream.str() );
	if(stringToValue(result, address))
		success &= setAttribute(RamStartAttribute, valueToString(address, "", ios::hex));
	else
		success = false;

	commandStream.str("");
	commandStream << "getEndAddress " << getTDevice().moduleNum;
	result = partnerDevice("RAM Controller").execute( commandStream.str() );
	if(stringToValue(result, address))
		success &= setAttribute(RamEndAttribute, valueToString(address, "", ios::hex));
	else
		success = false;

	return success;
}

void FPGA_Device::loadDeviceEvents()
{
	if( autoRAM_Allocation && partnerDevice("RAM Controller").isAlive() )
	{
		autoAllocateRAM();
	}

	//Setup the RAM bus so that events can be written to RAM
	ramBus->setMemoryAddress( ramBlock.getStartAddress(), ramBlock.getSizeInWords() );
	
	SynchronousEventVector& events = getSynchronousEvents();
	numberOfEvents = static_cast<uInt32>(events.size());

	//Setup Event Addresses
	for(unsigned i = 0; i < events.size(); i++)
	{
		events.at(i).setupEvent();
	}

	uInt32 bufferSize = ramBlock.getSizeInWords() / wordsPerEvent();	//size in terms of events

	//use j to count up to the RAM size in events
	//Fill the allocated RAM with events
	
	uInt32 nextToLoad, j;
	for(nextToLoad = 0, j = 0; 
		( j < bufferSize && nextToLoad < events.size() ); nextToLoad++, j++)
	{
		events.at(nextToLoad).loadEvent();
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
	uInt32 measuredEventIndex;	
	uInt32 nextToPlay;

	// nextToLoad is the next event to load
	// nextToPlay is the next event to play

	while( nextToLoad < numberOfEvents && !bufferUnderflowed )
	{
		nextToPlay = getCurrentEventNumber();
		measuredEventIndex = getMeasuredEventNumber() + bufferSize;	//the last event to be measured in the old buffer
		
		if(nextToLoad < nextToPlay)
		{
			reportMessage(LoadingError, "Buffer Underflow Error");
			bufferUnderflowed = true;
		}
		if(nextToLoad < nextToPlay + (bufferSize / 2))
		{
			//Write to half the buffer
			for(j = 0; ( j < (bufferSize / 2) 
				&& nextToLoad < numberOfEvents 
				&& nextToLoad <= measuredEventIndex ); nextToLoad++, j++)
			{
				events.at(nextToLoad).loadEvent();
			}
		}
	}
}
void FPGA_Device::waitForEvent(unsigned eventNumber)
{
	//wait until the event has been played
cerr << "FPGA_Device::waitForEvent() " << getCurrentEventNumber() << "-> ";
	while(getCurrentEventNumber() < eventNumber) {cerr << eventNumber << ".";} cerr << endl;
}

uInt32 FPGA_Device::getCurrentEventNumber()
{
	//starts at zero.  Goes to one when the first event has played
	uInt32 eventsRemaining = registerBus->readData(eventNumberRegisterOffset) / wordsPerEvent();
	
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
	registerBus->writeData( wordsPerEvent() * numberOfEvents, eventNumberRegisterOffset );	//write number of RAM entries
}

uInt32 FPGA_Device::getMinimumWriteTime(uInt32 bufferSize)
{
	SynchronousEventVector& events = getSynchronousEvents();
	
	//evtBufferSize is the size of half the full buffer since we're double buffering.
	//Also bufferSize is in words and evtBufferSize is in events. Each event is wordsPerEvent() words.
	unsigned evtBufferSize = (bufferSize / (2 * wordsPerEvent()));

	uInt64 newTime = 0;
	uInt64 minimumWriteTime = 0xffffffff;	//start out with max value

	unsigned i = 0;
	while( i < events.size() )
	{
		i += evtBufferSize;

		if( i < events.size() )
			newTime = events.at(i).getTime() - events.at(i - evtBufferSize).getTime();
		else
			newTime = events.back().getTime() - events.at(i - evtBufferSize).getTime();
	
		if(newTime < minimumWriteTime)
			minimumWriteTime = newTime;
	}

	if(minimumWriteTime > 0xffffffff)
		return 0xffffffff;
	else
		return static_cast<uInt32>(minimumWriteTime);
}

short FPGA_Device::wordsPerEvent()
{
	return 2;
}

void FPGA_Device::FPGA_AttributeUpdater::defineAttributes()
{
	addAttribute(device_->RamStartAttribute, 
		device_->valueToString(device_->ramBlock.getStartAddress(), "", ios::hex) );
	addAttribute(device_->RamEndAttribute, 
		device_->valueToString(device_->ramBlock.getEndAddress(), "", ios::hex) );
	addAttribute(device_->AutoRamAttribute, "On", "On, Off");
}

bool FPGA_Device::FPGA_AttributeUpdater::updateAttributes(string key, string value)
{
	uInt32 tempInt;
	bool successInt = device_->stringToValue(value, tempInt, ios::hex);	//comming from a hex string

	bool success = true;
	
	if(key.compare(device_->RamStartAttribute) == 0 && successInt)	{		device_->ramBlock.setStartAddress(tempInt);
		success = true;
	}
	else if(key.compare(device_->RamEndAttribute) == 0 && successInt)	{		device_->ramBlock.setEndAddress(tempInt);
		success = true;
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

	if(success)
	{
		device_->writeRAM_Parameters();
		//send start and end addresses to controller
		//device_
	}

	return success;
}
void FPGA_Device::FPGA_AttributeUpdater::refreshAttributes()
{
	if( !device_->getAddressesFromController() )
	{
		setAttribute( device_->RamStartAttribute, 
			device_->valueToString(device_->ramBlock.getStartAddress(), "", ios::hex) );
		setAttribute( device_->RamEndAttribute, 
			device_->valueToString(device_->ramBlock.getEndAddress(), "", ios::hex) );
	}
	setAttribute( device_->AutoRamAttribute, 
		(device_->autoRAM_Allocation ? "On" : "Off") );

	device_->sendAddressesToController();
}


FPGA_Device::FPGA_Event::FPGA_Event(double time, FPGA_Device* device) : 
BitLineEvent<32>(time, device), 
device_f(device)
{
}

void FPGA_Device::FPGA_Event::setupEvent()
{
	time32 = static_cast<uInt32>( getTime() / 10 );	//in clock cycles! (1 cycle = 10 ns)
	timeAddress  = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() );
	valueAddress = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() + 1 );
}

void FPGA_Device::FPGA_Event::loadEvent()
{
	//write the event to RAM
	device_f->ramBus->writeDataToAddress( time32, timeAddress );
	device_f->ramBus->writeDataToAddress( getValue(), valueAddress );
}

void FPGA_Device::FPGA_Event::playEvent()
{
}

//Read the contents of the time register for this event from the FPGA
uInt32 FPGA_Device::FPGA_Event::readBackTime()
{
	return device_f->ramBus->readDataFromAddress( timeAddress );
}

//Read the contents of the time register for this event from the FPGA
uInt32 FPGA_Device::FPGA_Event::readBackValue()
{
	return device_f->ramBus->readDataFromAddress( valueAddress );
}
