/*! \file Trigger_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Trigger_Device
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

#include "Trigger_Device.h"

#include <iostream>

Trigger_Device::Trigger_Device(ORBManager* orb_manager, std::string configFilename, uInt32 EtraxMemoryAddress) : 
STI_Device(orb_manager, "FPGA_Trigger", configFilename),
etraxMemoryAddress(EtraxMemoryAddress)
{
	bus = new EtraxBus(EtraxMemoryAddress);
	busSleepMutex      = new omni_mutex();
	busSleepCondition  = new omni_condition(busSleepMutex);
	sleepTime = 10000000; // in nanoseconds

	serverPauseMutex = new omni_mutex();

	triggerPauseMutex = new omni_mutex();
	triggerPauseCondition = new omni_condition(triggerPauseMutex);


	initialGlobalWaitCutoff = 10;		//ns

	play  = (1 << 0);
	stop  = (1 << 1);
	pause = (1 << 2);
	waitForExternal = 0;
	armBits = 0;

	waitingForExternalTrigger = false;
	triggerPaused = false;
}

Trigger_Device::~Trigger_Device()
{
	delete bus;
}

bool Trigger_Device::deviceMain(int argc, char** argv)
{
	return false;
}

void Trigger_Device::defineAttributes()
{
}

void Trigger_Device::refreshAttributes()
{
}

bool Trigger_Device::updateAttribute(std::string key, std::string value)
{
	return false;
}

void Trigger_Device::defineChannels()
{
	for(unsigned i = 0; i < 8; i++)
	{
		addOutputChannel(i, ValueString);	//one channel for each FPGA module (for establishing the trigger's armbits)
	}
	addOutputChannel(8, ValueString);	//global trigger channel (affects all modules)
}

bool Trigger_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return readChannelDefault(channel, valueIn, dataOut);
}

bool Trigger_Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	return writeChannelDefault(channel, value);
}


void Trigger_Device::definePartnerDevices()
{
}

std::string Trigger_Device::execute(int argc, char** argv)
{
	unsigned long wait_s;
	unsigned long wait_ns;

	bool convertSuccess = false;
	unsigned module;
	string result = "";
	vector<string> args;
	STI::Utils::convertArgs(argc, argv, args);

	//the first arguement is the device's name ("FPGA Trigger" in this case)

	if(args.size() == 3 && args.at(1).compare("trigger") == 0)
	{
		convertSuccess = stringToValue(args.at(2), module);
	
		if(convertSuccess)
		{
			//trigger ini
			TriggerEvent triggerStop(0, stop, this);
//std::cerr << "TriggerStop: " << triggerStop.getBits() << std::endl;
			triggerStop.playEvent();
//int x;
//std::cerr << "type something. paused." << std::endl;
//std::cin >> x;

			// go to sleep in order to wait for bus activity from last playEvent() to finish
			busSleepMutex->lock();
			{
				//Find the absolute time for waking up (sleepTime nanoseconds from now)
				omni_thread::get_time(&wait_s, &wait_ns, 0, sleepTime);

				busSleepCondition->timedwait(wait_s, wait_ns);
			}
			busSleepMutex->unlock();

			//trigger a single module
			TriggerEvent triggerSingle(1, play, this);
//std::cerr << "ini triggerSingle: " << triggerSingle.getBits() << " , module: " << module << std::endl;
			triggerSingle.setBits(true, 4 + module, 4 + module);	//"arm" bits run from 4 to 11
//std::cerr << "triggerSingle: " << triggerSingle.getBits() << std::endl;
			triggerSingle.playEvent();
		}
		else
		{
			result = "Trigger command conversion error.";
		}
	}

	return result;
}

void Trigger_Device::parseDeviceEvents(const RawEventMap& eventsIn,
					   SynchronousEventVector& eventsOut) throw(std::exception)
{
	//Add an "play" event with time = 0
	// for now, we have added a hack work around to arm the fpga. this asserts stop (0b010) then 0 on command bits,
	//followed by pause (0b100) followed by play (0b001) executed as fast as the computer can go. 
	//FPGA clock does not start until play is asserted
	//other non-fpga devices will be ahead by time delay it takes to get to fpga play event
//	eventsOut.push_back( new TriggerEvent(0, stop, this) );
//	eventsOut.push_back( new TriggerEvent(1, 0, this) );
//	eventsOut.push_back( new TriggerEvent(2, pause, this) );
//	eventsOut.push_back( new TriggerEvent(3, play, this) );

	//eventsIn is typically empty, but there can be user defined events
	RawEventMap::const_iterator events;
	uInt32 value = 0;
	uInt32 lastvalue = 0;
	armBits = 0;		//determines which FPGA cores to arm based on registered FPGA devices

	TriggerEvent* newEvent = NULL;

	unsigned short module;
	bool earlyWaitDetected = false;
	double earlyWaitTime = 0;

	//look for early waits (a global wait called in first 10 ns)
	//All FPGA devices automatically submit a Stop and Play event to the trigger within the first 10 ns.
	//If a global Wait is submitted before 10 ns then the automatically generated Play is replaced with a Wait
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->first < initialGlobalWaitCutoff &&
				events->second.at(i).channel() == 8 && 
				getTriggerEventValue(events->second.at(i).stringValue()) == waitForExternal)
			{
				earlyWaitDetected = true;
				earlyWaitTime = events->first;
			}
		}
	}

	//initial scan on module channels 0-7
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		//all these events happen at the same time and should be consolidated into one event:
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).channel() < 8)
			{
				value = getTriggerEventValue(events->second.at(i).stringValue());

				//check for multiple events
				if(i > 0 && lastvalue != value)
					throw EventConflictException(events->second.at(i-1), 
						events->second.at(i), 
						"The trigger cannot have multiple events at the same time." );
			
				lastvalue = value;

				//early wait detection: replace play with wait
				if(earlyWaitDetected && events->first < initialGlobalWaitCutoff && value == play)
					value = waitForExternal;	//override the play with the global wait

				if(i == 0 && newEvent == NULL)
					newEvent = new TriggerEvent(events->second.at(i).time(), value, this);

				//set the arm bits for the new event
				module = events->second.at(i).channel();	//module number is encoded by the trigger channel

				newEvent->armModule(module);
			}
		}
		
		if(newEvent != NULL)
		{
			if(armBits < newEvent->getArmBits())	//For "global" trigger events: keep track of the armbits needed for the largest number of modules with any events
				armBits = newEvent->getArmBits();

			eventsOut.push_back(newEvent);	
			newEvent = NULL;	//eventsOut ptr_vector has the reference; no need to call delete.
		}
	}

	//now look for global events (affect all armed modules) on channel 8
	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		for(unsigned i = 0; i < events->second.size(); i++)
		{
			if(events->second.at(i).channel() == 8)
			{
				if(newEvent == NULL)	//this should always be true
				{
					if(earlyWaitDetected && events->first == earlyWaitTime)
					{
						//this is the early wait event; don't add it again.
					}
					else
					{
						newEvent = new TriggerEvent(
							events->second.at(i).time(), 
							getTriggerEventValue(events->second.at(i).stringValue()), 
							this);
						newEvent->setBits(armBits, 4, 11);	//affects all modules with events
					}
				}
			}
		}
		if(newEvent != NULL)
		{
			eventsOut.push_back(newEvent);	
			newEvent = NULL;	//eventsOut ptr_vector has the reference; no need to call delete.
		}
	}

}

uInt32 Trigger_Device::getTriggerEventValue(std::string eventValue)
{
	uInt32 value = 0;

	if(eventValue.compare("play") == 0 ||
		eventValue.compare("Play") == 0 ||
		eventValue.compare("PLAY") == 0)
	{
		value = play;
	}
	if(eventValue.compare("stop") == 0 ||
		eventValue.compare("Stop") == 0 ||
		eventValue.compare("STOP") == 0)
	{
		value = stop;
	}
	if(eventValue.compare("pause") == 0 ||
		eventValue.compare("Pause") == 0 ||
		eventValue.compare("PAUSE") == 0)
	{
		value = pause;
	}
	if(
		eventValue.compare("wait for external trigger") == 0 ||
		eventValue.compare("Wait for external trigger") == 0 ||
		eventValue.compare("WAIT FOR EXTERNAL TRIGGER") == 0 ||
		eventValue.compare("Wait For External Trigger") == 0 ||
		eventValue.compare("external trigger")          == 0 ||
		eventValue.compare("EXTERNAL TRIGGER")          == 0 ||
		eventValue.compare("External Trigger")          == 0 ||
		eventValue.compare("wait")                      == 0 ||
		eventValue.compare("Wait")                      == 0 ||
		eventValue.compare("WAIT")                      == 0)
	{
		value = waitForExternal;
	}

	return value;
}


void Trigger_Device::stopEventPlayback()
{
	serverPauseMutex->lock();
	{
		waitingForExternalTrigger = false;
		triggerPaused = false;
		triggerPauseCondition->broadcast();		//in case it's waiting for some strange reason
	}
	serverPauseMutex->unlock();
	
	writeData(stop + getOffsetArmBits());
}

uInt32 Trigger_Device::getOffsetArmBits()
{
	return (armBits << 4);	//arm bits run from 4 to 11
}

void Trigger_Device::pauseEventPlayback()
{
	triggerPauseMutex->lock();
	{
		triggerPauseCondition->broadcast();		//in case it's waiting for some strange reason
		triggerPaused = true;	
//		if(!waitingForExternalTrigger)
	}
	triggerPauseMutex->unlock();

	writeData(pause + getOffsetArmBits());
}
void Trigger_Device::resumeEventPlayback() 
{
	if(waitingForExternalTrigger)
	{
		pauseServer();
		writeData(waitForExternal + getOffsetArmBits());
	}
	else
	{
		writeData(play + getOffsetArmBits());
	}

	triggerPauseMutex->lock();
	{
		triggerPaused = false;
		triggerPauseCondition->broadcast();
	}
	triggerPauseMutex->unlock();
}

void Trigger_Device::writeData(uInt32 data)
{
	bus->writeData(data);
//std::cout << "Trigger_Device::writeData(" << data << ")" << std::endl;
//int x;
//std::cin >> x;
//	if(bus == NULL)
//		std::cout << "bus is NULL!" << std::endl;
//	else
//		bus->writeData(data, 0);
}


void Trigger_Device::TriggerEvent::playEvent()
{
	trigger->writeData( getValue() );

//	cout << "trigger playEvent() " << getValue() << " : " << getBits(0,3) << endl;	

	if( getBits(0,3) == trigger->waitForExternal )	//wait for external trigger event
	{
		trigger->waitForExternalTrigger();
//		cout << "trigger->waitForExternalTrigger()" << endl;
	}
}
void Trigger_Device::TriggerEvent::armModule(unsigned short module)
{
	if(module < 8)
		setBits(true, 4 + module, 4 + module);
}

void Trigger_Device::waitForExternalTrigger()
{
	serverPauseMutex->lock();
	{
		waitingForExternalTrigger = true;
		pauseServer();
	}
	serverPauseMutex->unlock();

	bool externalTriggerOccurred = false;
	bool keepWaiting = true;
	while(keepWaiting && !externalTriggerOccurred)
	{
		externalTriggerOccurred = ( (bus->readData() & 0x1) == 1);	//check if FPGA is in "play" state (0b0001)

		cout << "Trigger bus->readData() " << bus->readData() << " external? " << externalTriggerOccurred << endl;

		serverPauseMutex->lock();
		{
			keepWaiting = waitingForExternalTrigger;
		}
		serverPauseMutex->unlock();


		triggerPauseMutex->lock();
		{
			if(triggerPaused) {
cout << "Trigger paused!" << endl;
				triggerPauseCondition->wait();
			}
cout << "Trigger UNpaused!" << endl;
		}
		triggerPauseMutex->unlock();

		omni_thread::yield();

	}

	cout << "Trigger left while. " << endl;

	unpauseServer();

	serverPauseMutex->lock();
	{
		waitingForExternalTrigger = false;
	}
	serverPauseMutex->unlock();
}


