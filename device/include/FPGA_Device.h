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

#ifndef FPGA_DEVICE_H
#define FPGA_DEVICE_H

#include <STI_Device.h>
#include <FPGA_RAM_Block.h>
#include <EtraxBus.h>


// add header files required for using linux interrupts
#ifdef USE_INTERRUPTS
/*
 * asynctest.c: use async notification to read stdin
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 */

	#include <signal.h>
	#include <fcntl.h>
	#include <unistd.h> 
#endif

class FPGA_Device : public STI_Device
{
public:
	FPGA_Device(ORBManager* orb_manager, std::string DeviceName, std::string configFilename);

	FPGA_Device(ORBManager* orb_manager, std::string DeviceName, 
			std::string IPAddress, unsigned short ModuleNumber);
	virtual ~FPGA_Device();

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
	virtual void parseDeviceEventsFPGA(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) 
		throw(std::exception) = 0;
	virtual double getMinimumEventStartTime() = 0;

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	virtual void pauseEventPlayback() = 0;	//for devices that require non-generic pause commands


private:
	void FPGA_init();

	void loadDeviceEvents();

	bool playSingleEventFPGA(const RawEvent& rawEvent);
	bool playSingleEventDefault(const RawEvent& event);

	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	void parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) 
		throw(std::exception);

private:

	unsigned long pollTime_ms;

	uInt32 RAM_Parameters_Base_Address;
	uInt32 startRegisterOffset;
	uInt32 endRegisterOffset;
	uInt32 eventNumberRegisterOffset;

	uInt32 numberOfEvents;
	bool autoRAM_Allocation;

#ifdef USE_INTERRUPTS
	// functions to handle asyncronous notifications
	bool ayncNotification; //this is the flag which alerts the waitForEvent thread
	int fd; //file descriptor for /dev/fpga_interrupt
	char * device; //name of file to open
	
	// everything is based around this struct, also the name of the function that starts the listener
    struct sigaction action; //create the sigaction memory structure. this is defined in signal.h 

	void sighandler(int signo) //the function that handles a SIGIO. must be passed signo
	{
		printf("We entered the sighandler.\n");	
		if (signo==SIGIO)
		{
			asyncNotification = true;
			printf("We got an interrupt from the FPGA!\n");
			waitForEventTimer->signal(); //this will wake up any sleeping threads
		}
    	return;
	}

#endif

public:
	FPGA_RAM_Block ramBlock;
	EtraxBus* registerBus;
	EtraxBus* ramBus;
	
	uInt32 getCurrentEventNumber();
	virtual short wordsPerEvent() const;
	void waitForEvent(unsigned eventNumber);

private:
	std::string RamStartAttribute;
	std::string RamEndAttribute;
	std::string AutoRamAttribute;

	void autoAllocateRAM();
	bool getAddressesFromController();
	void sendAddressesToController();

	void writeRAM_Parameters();
	uInt32 getMinimumWriteTime(uInt32 bufferSize);

	void sleepwait(unsigned long secs, unsigned long nanosecs = 0);

	omni_mutex* waitForEventMutex;
	omni_condition* waitForEventTimer;

	class FPGA_AttributeUpdater : public AttributeUpdater
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

	//template<int N=32>
	//class FPGA_BitLineEvent : public BitLineEvent<N>
	template<int N=32, class E=SynchronousEvent>
	class FPGA_BitLineEvent : public BitLineEvent<N,E>	{
	public:
		FPGA_BitLineEvent(double time, FPGA_Device* device) : BitLineEvent<N,E>(time, device), device_f(device) { }
		FPGA_BitLineEvent(const FPGA_BitLineEvent &copy) : BitLineEvent<N,E>(copy) { }

		//Read the contents of the time register for this event from the FPGA
		uInt32 readBackTime()
		{
			return device_f->ramBus->readDataFromAddress( timeAddress );
		}
		//Read the contents of the value register for this event from the FPGA
		uInt32 readBackValue()
		{
			return device_f->ramBus->readDataFromAddress( valueAddress );
		}

		virtual void waitBeforePlay()
		{
			//Have the cpu sleep until the event is almost ready.  As a result, the cpu may (theoretically)
			//get slightly behind the FPGA.  Of course, the FPGA will always follow hard timing.  The slight 
			//asynchronicity between the cpu and FPGA is not important, and the benefit is reduced polling 
			//of the event counter register.

			sleepUntil( getTime() );

			//Now check the event counter until this event actually plays.
			device_f->waitForEvent( getEventNumber() );
//			cerr << "waitBeforePlay() is finished " << getEventNumber() << endl;
		}

		virtual void sleepUntil(uInt64 time)
		{
			unsigned long wait_s;
			unsigned long wait_ns;

			statusMutex->lock();
			{
				Int64 wait = static_cast<Int64>(time) - device_f->getCurrentTime() ;

//cout << "FPGA_Device::sleepUntil::wait = " << wait << endl;
				if(wait > 0 && !played)
				{
					//calculate absolute time to wake up
					omni_thread::get_time(&wait_s, &wait_ns, 
						Clock::get_s(wait), Clock::get_ns(wait));

					playCondition->timedwait(wait_s, wait_ns);
				}
			}
			statusMutex->unlock();
		}

	private:
		virtual void setupEvent()
		{
			time32 = static_cast<uInt32>( getTime() / 10 );	//in clock cycles! (1 cycle = 10 ns)
			timeAddress  = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() );
			valueAddress = device_f->ramBlock.getWrappedAddress( 2*getEventNumber() + 1 );
		}
		virtual void loadEvent()
		{
			//write the event to RAM
			device_f->ramBus->writeDataToAddress( time32, timeAddress );
			device_f->ramBus->writeDataToAddress( getValue(), valueAddress );
		}
		virtual void playEvent(){
//			cerr << "playEvent() " << getEventNumber() << endl;
		}
		virtual void collectMeasurementData() = 0;

	private:
		uInt32 timeAddress;
		uInt32 valueAddress;
		uInt32 time32;

		FPGA_Device* device_f;
	};


	typedef FPGA_BitLineEvent<> FPGA_Event;	//shortcut for a 32 bit FPGA event
	typedef FPGA_BitLineEvent<32, DynamicSynchronousEvent> FPGA_DynamicEvent;

};

#endif
