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

FPGA_Device::FPGA_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename) :
STI_Device(orb_manager, DeviceName, configFilename)
{
	FPGA_init();
}

FPGA_Device::FPGA_Device(ORBManager* orb_manager, std::string DeviceName, 
						 std::string IPAddress, unsigned short ModuleNumber) :
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	FPGA_init();
}

void FPGA_Device::FPGA_init()
{
	std::string IPAddress = getIP();
	unsigned short ModuleNumber = getModule();

	ramBlock.setModuleNumber(ModuleNumber);
	
	addMutualPartnerDevice("Trigger", IPAddress, 8, "FPGA_Trigger");
	addPartnerDevice("RAM Controller", IPAddress, 9, "RAM_Controller");

	RamStartAttribute = "RAM_Start_Addr";
	RamEndAttribute   = "RAM_End_Addr";
	AutoRamAttribute  = "Auto RAM Allocation";
	autoRAM_Allocation = false;

	addAttributeUpdater( new FPGA_AttributeUpdater(this) );
	
	//Registers for storing RAM parameters for this device
	//Each module uses 3 address that are 4 bytes each ==> 12 bytes
	RAM_Parameters_Base_Address = 0x90000008 + 12 * ModuleNumber;

	startRegisterOffset         = 0;
	endRegisterOffset           = 4;
	eventNumberRegisterOffset   = 8;

	registerBus = new EtraxBus(RAM_Parameters_Base_Address, 3);	//3 words wide
	ramBus      = new EtraxBus( ramBlock.getStartAddress(), ramBlock.getSizeInWords() );
}

FPGA_Device::~FPGA_Device()
{
	delete ramBus;
	delete registerBus;
}


bool FPGA_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return readChannelDefault(channel, valueIn, dataOut, getMinimumEventStartTime());
}



bool FPGA_Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	return writeChannelDefault(channel, value, getMinimumEventStartTime());
}


bool FPGA_Device::playSingleEventDefault(const RawEvent& event)
{
	return playSingleEventFPGA(event);
}


bool FPGA_Device::playSingleEventFPGA(const RawEvent& rawEvent)
{
	//implementation based on "single-line timing file" scheme
	
	getSynchronousEvents().clear();
	getMeasurements().clear();

	changeStatus(EventsEmpty);

	RawEventMap rawEventsIn;
	rawEventsIn[rawEvent.time()].push_back( rawEvent );

	if(rawEvent.isMeasurementEvent())	//measurement event
		getMeasurements().push_back( rawEvent.getMeasurement() );

	if(!parseEvents(rawEventsIn))
		return false;

//	try {
//		parseDeviceEvents(rawEventsIn, getSynchronousEvents() );	//pure virtual
//	}
//	catch(...)	//generic conflict or error
//	{
//std::cerr << "writeChannel exception caught!!" << std::endl;
//		return false;
//	}
//
//	//sort in time order
//	getSynchronousEvents().sort();
//
//	//Assign event numbers
//	for(i = 0; i < getSynchronousEvents().size(); i++)
//	{
//		getSynchronousEvents().at(i).setEventNumber( i );
//	}


	bool autoOld = autoRAM_Allocation;
	autoRAM_Allocation = false;
	ramBlock.increaseRAM_Block_SizeTo( wordsPerEvent() * getSynchronousEvents().size() );
//	ramBlock.setRAM_Block_Size(wordsPerEvent() * getSynchronousEvents().size() );

	loadEvents();

	waitForStatus(EventsLoaded);
	
	autoRAM_Allocation = autoOld;

//	if( !prepareToPlay() )
	if(!changeStatus(PreparingToPlay))
		return false;

	playEvents();

	waitForStatus(Playing);


		
//	if( deviceStatusIs(EventsLoading) )
//	{
//		deviceLoadingMutex->lock();
//		{
//cout << "deviceLoadingCondition->wait()" << endl;
//			deviceLoadingCondition->wait();
//		}
//		deviceLoadingMutex->unlock();
//	}
//cout << "***deviceLoadingCondition is unlocked!" << endl;


//	while( !eventsLoaded() ) {}




//cout << "FPGA_Device::writeChannel::eventsLoaded() time = " << eventsLoadedClock.getCurrentTime()/1000000 << endl;


//	cerr << "Measurement Check time: " << getSynchronousEvents().at(0).getMeasurement()->time() << endl;
//	cerr << "Measurement Check numberValue: " << getSynchronousEvents().at(0).getMeasurement()->numberValue() << endl;




	bool success = deviceStatusIs(Playing);
	stringstream commandStream;
	string result;

	commandStream.str(""); 
	commandStream << "trigger " << getTDevice().moduleNum;
	
	if(success)
	{
		result = partnerDevice("Trigger").execute( commandStream.str() );
		success &= STI::Utils::stringToValue(result, success);
	}
//cout << "FPGA_Device::writeChannel::trigger time = " << triggerClock.getCurrentTime()/1000000 << endl;

	waitForStatus(EventsLoaded);


//	if( deviceStatusIs(Playing) )
//	{
//		deviceRunningMutex->lock();
//		{
//cout << "deviceRunningCondition->wait()" << endl;
//			deviceRunningCondition->wait();
//		}
//		deviceRunningMutex->unlock();
//	}
//cout << "***deviceRunningCondition is unlocked!" << endl;
//
//
////	while(getDeviceStatus() == Playing && !stopPlayback) {};
//
//
//cout << "FPGA_Device::writeChannel time = " << writeChannelClock.getCurrentTime() << endl;

	success = deviceStatusIs(EventsLoaded);

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
	while( !doneAllocating && deviceStatusIs(EventsLoading) && autoRAM_Allocation)
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
	if( !autoRAM_Allocation || !partnerDevice("RAM Controller").isAlive() )
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
	if( !autoRAM_Allocation || !partnerDevice("RAM Controller").isAlive() )
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

	cerr << "autoRAM_Allocation = " << autoRAM_Allocation << endl;

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
		events.at(i).setup();
	}

	uInt32 bufferSize = ramBlock.getSizeInWords() / wordsPerEvent();	//size in terms of events

	//use j to count up to the RAM size in events
	//Fill the allocated RAM with events
	
	uInt32 nextToLoad, j;
	for(nextToLoad = 0, j = 0; 
		( j < bufferSize && nextToLoad < events.size() ); nextToLoad++, j++)
	{
		events.at(nextToLoad).load();
	}

	//RAM is full (or all events are loaded)
	//Tell the FPGA that the events are ready to load into the timing core FIFO
	writeRAM_Parameters();

	if( !changeStatus(EventsLoaded) )
	{
		//something is wrong; this shouldn't happen
		reportMessage(LoadingError, "FPGA_Device::loadDeviceEvents() Failed to change device status to 'Loaded' after filling the RAM.");
		changeStatus(EventsEmpty);
		return;
	}

	//Double Buffering
	bool doubleBufferRequired = false;
	bool bufferUnderflowed = false;
	uInt32 measuredEventIndex;	
	uInt32 nextToPlay;

	// nextToLoad is the next event to load
	// nextToPlay is the next event to play

	while( nextToLoad < numberOfEvents && !bufferUnderflowed )
	{
		doubleBufferRequired = true;
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
				events.at(nextToLoad).load();
			}
		}
	}

	if(doubleBufferRequired)
		changeStatus(EventsEmpty);
	else
		changeStatus(EventsLoaded);

}
void FPGA_Device::waitForEvent(unsigned eventNumber)
{
	//wait until the event has been played
	

	//N events loaded in FPGA memory:
	//1st event has loaded; getCurrentEventNumber() = 0
	//1st event has played; getCurrentEventNumber() = 1   *
	//2nd event has loaded; getCurrentEventNumber() = 1
	//2nd event has played; getCurrentEventNumber() = 2   *
	//Nth event has loaded; getCurrentEventNumber() = N-1
	//Nth event has played; getCurrentEventNumber() = N   *
	

	//NOTE: eventNumber is the index of an array that begins at zero.

	// event #1 (i.e., 0 + 1) has played when getCurrentEventNumber() == 1

	while( (getCurrentEventNumber() < (eventNumber + 1) ) && !stopPlayback && !pausePlayback) {};
//	cout << "FPGA_Device '" << getDeviceName() << "' stopped while waiting for event #" << eventNumber << endl;


	// while loop exit conditions:
	//waitForEvent(0);   when 1st event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(0);   when 1st event has played, getCurrentEventNumber() < (eventNumber + 1)  == False
	//waitForEvent(1);   when 2nd event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(1);   when 2nd event has played, getCurrentEventNumber() < (eventNumber + 1)  == False
	//waitForEvent(N-1); when Nth event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(N-1); when Nth event has played, getCurrentEventNumber() < (eventNumber + 1)  == False

}

uInt32 FPGA_Device::getCurrentEventNumber()
{
	// Returns the most recent event number that has already played on the FPGA, starting at zero.
	// (i.e., goes to one when the first event has played.)

	//This Gets the event number that is currently _loaded_ in the FPGA register.  This is
	//the _next_ event to play.  This value is updated on the FPGA as soon as the previous event plays.
	uInt32 eventsRemaining = registerBus->readData(eventNumberRegisterOffset) / wordsPerEvent();  //events remaining to load
	
	//N events loaded in FPGA memory:
	//1st event loaded; eventsRemaining = wordsPerEvent() * N       / wordsPerEvent()   = N
	//1st event played; eventsRemaining = wordsPerEvent() * (N-1)   / wordsPerEvent()   = N-1
	//2nd event loaded; eventsRemaining = wordsPerEvent() * (N-1)   / wordsPerEvent()   = N-1 
	//2nd event played; eventsRemaining = wordsPerEvent() * (N-2)   / wordsPerEvent()   = N-2
	//Nth event loaded; eventsRemaining = wordsPerEvent() * N-(N-1) / wordsPerEvent()   = 1
	//Nth event played; eventsRemaining = wordsPerEvent() * N-N     / wordsPerEvent()   = 0


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
	addAttribute(device_->AutoRamAttribute, (device_->autoRAM_Allocation ? "On" : "Off"), "On, Off");
}

bool FPGA_Device::FPGA_AttributeUpdater::updateAttributes(string key, string value)
{
	uInt32 tempInt;
	bool successInt = device_->stringToValue(value, tempInt, ios::hex);	//comming from a hex string

	bool success = false;
	
	if(key.compare(device_->RamStartAttribute) == 0 && successInt)
	{
		device_->ramBlock.setStartAddress(tempInt);
		success = true;
	}
	else if(key.compare(device_->RamEndAttribute) == 0 && successInt)
	{
		device_->ramBlock.setEndAddress(tempInt);
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
//	setAttribute( device_->AutoRamAttribute, 
//		(device_->autoRAM_Allocation ? "On" : "Off") );

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
	cerr << "playEvent() " << getEventNumber() << endl;
}

//Read the contents of the time register for this event from the FPGA
uInt32 FPGA_Device::FPGA_Event::readBackTime()
{
	return device_f->ramBus->readDataFromAddress( timeAddress );
}

//Read the contents of the value register for this event from the FPGA
uInt32 FPGA_Device::FPGA_Event::readBackValue()
{
	return device_f->ramBus->readDataFromAddress( valueAddress );
}


void FPGA_Device::FPGA_Event::waitBeforePlay()
{
	device_f->waitForEvent( getEventNumber() );
	cerr << "waitBeforePlay() is finished " << getEventNumber() << endl;
}
