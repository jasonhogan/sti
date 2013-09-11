

#include "FPGADeviceEventEngine.h"

#include "EtraxBus.h"
#include "FPGA_Device.h"

using STI::FPGA::FPGADeviceEventEngine;
using STI::FPGA::FPGADeviceEventEngineSetup;
using STI::FPGA::EtraxBus_ptr;
using STI::FPGA::FPGA_RAM_Block_ptr;
	
FPGADeviceEventEngine::FPGADeviceEventEngine(const STI::TimingEngine::EngineID& engineID,
											 STI::FPGA::FPGA_Device& fpgaDevice, 
											 const FPGADeviceEventEngineSetup& engineSetup, 
											 const FPGA_RAM_Block_ptr& ramBlock) : 
DeviceEventEngine(fpgaDevice),
localEngineID(engineID), _device(&fpgaDevice), setup(engineSetup), engineRamBlock(ramBlock)
{
//	resetStopFlag();
}

void FPGADeviceEventEngine::postStop()
{
//	stopWaiting();
}

void FPGADeviceEventEngine::preParse()
{
//	_device->setParsingEngineRamBlock(engineRamBlock);
	_device->parsingEngineRAMBlock = engineRamBlock;
//	_device->setParsingEngineID(localEngineID);
	_device->parsingEngineID = &localEngineID;
}

void FPGADeviceEventEngine::preLoad()
{
	//Setup the RAM bus so that events can be written to RAM
	numberOfEvents = static_cast<uInt32>(synchedEvents.size());
}

void FPGADeviceEventEngine::prePlay(
	const STI::TimingEngine::EngineTimestamp& parseTimeStamp, 
	const STI::TimingEngine::EngineTimestamp& playTimeStamp, 
	const STI::TimingEngine::PlayOptions_ptr& playOptions,
	const STI::TimingEngine::DocumentationOptions_ptr& docOptions, const STI::TimingEngine::EngineCallbackHandler_ptr& callBack)
{
//	resetStopFlag();
	_device->writeRAM_Parameters(engineRamBlock->getStartAddress(), engineRamBlock->getEndAddress(), numberOfEvents);
}

void FPGADeviceEventEngine::waitUntilEvent(const STI::TimingEngine::SynchronousEvent& evt, STI::TimingEngine::EventEngineState stateCondition)
{
	//First wait for the time of the event using a sleep. (Default in DeviceEventEngine)
	DeviceEventEngine::waitUntilEvent(evt, stateCondition);
	
	//Check FPGA to see if the event number has played.
	waitForEvent(evt.getEventNumber());
}

void FPGADeviceEventEngine::waitForEvent(unsigned eventNumber)
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

//	bool stop = false;

	
	
	while( inState(STI::TimingEngine::Playing) && (getCurrentEventNumber() < (eventNumber + 1) ) )
	{
//		boost::shared_lock< boost::shared_mutex > readLock(waitForEventMutex);
//		stop = stopWaiting_l;
	}
	
	// while loop exit conditions:
	//waitForEvent(0);   when 1st event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(0);   when 1st event has played, getCurrentEventNumber() < (eventNumber + 1)  == False
	//waitForEvent(1);   when 2nd event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(1);   when 2nd event has played, getCurrentEventNumber() < (eventNumber + 1)  == False
	//waitForEvent(N-1); when Nth event is loaded , getCurrentEventNumber() < (eventNumber + 1)  == True
	//waitForEvent(N-1); when Nth event has played, getCurrentEventNumber() < (eventNumber + 1)  == False
}



uInt32 FPGADeviceEventEngine::getCurrentEventNumber()
{
	// Returns the most recent event number that has already played on the FPGA, starting at zero.
	// (i.e., goes to one when the first event has played.)

	//This Gets the event number that is currently _loaded_ in the FPGA register.  This is
	//the _next_ event to play.  This value is updated on the FPGA as soon as the previous event plays.
	uInt32 eventsRemaining = _device->queryEventsRemainingRegister();

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

//void FPGADeviceEventEngine::stopWaiting()
//{
//	boost::unique_lock< boost::shared_mutex > writeLock(waitForEventMutex);
//	stopWaiting_l = true;
//}
//
//void FPGADeviceEventEngine::resetStopFlag()
//{
//	boost::unique_lock< boost::shared_mutex > writeLock(waitForEventMutex);
//	stopWaiting_l = false;
//}



