/*! \file StreamingBuffer.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class StreamingBuffer
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

#include "StreamingBuffer.h"
#include "Attribute.h"
#include "STI_Device.h"

#include <ctime>


#include <iostream>
using namespace std;

StreamingBuffer::StreamingBuffer()
{
	cerr << "default constructor" << endl;
}
/*
StreamingBuffer::StreamingBuffer(STI_Device* device, unsigned short channel, bool status)
{
	StreamingBuffer(device, channel, status, 1000000000, 2);
}
*/

StreamingBuffer::StreamingBuffer(STI_Device*    device, 
								 unsigned short channel,
								 bool           status, 
								 double         period, 
								 unsigned int   depth) :
sti_Device(device),
Channel(channel)
{
cerr << "Constructing ch: " << channel << endl;


	thread = new omni_thread(measurementLoopWrapper, (void*)this, omni_thread::PRIORITY_LOW);
	cerr << "thread made: " << thread->id() << endl;

	setStreamingStatus(status);
	cerr << "c period: " << period << endl;
	setSamplePeriod(period);
	setBufferDepth(depth);

	resetSleepServo();

	tMeasurement.channel = Channel;


	bufferMutex = new omni_mutex();
}

StreamingBuffer::StreamingBuffer(const StreamingBuffer& sb)
{
	sti_Device = sb.sti_Device;
	Channel = sb.Channel;
	thread = sb.thread;
	resetSleepServo();
	tMeasurement.channel = Channel;
	bufferMutex = sb.bufferMutex;
	
}

StreamingBuffer::~StreamingBuffer()
{
	cerr << "destructor!!!" << endl;
}

void StreamingBuffer::measurementLoopWrapper(void* object)
{
	StreamingBuffer* thisObject = (StreamingBuffer*) object;
	
	while(thisObject->measurementLoop()) { cerr << ".";};

	thisObject->setStreamingStatus(false);
}

Int64 StreamingBuffer::sleepPID(long timeToWait)
{
	double pGain = 0.9;		//proportional gain
	double iGain = 0.5;		//integral gain
	double dGain = 0.5;		//derivative gain

	//I
	errorIntegral += t_error;

	//D
	errorDerivative = t_error - lastError;
	lastError = t_error;


	if(timeToWait < 0)
		return 0;

	return timeToWait - 
		(pGain * t_error + iGain * errorIntegral + dGain * errorDerivative);
}

void StreamingBuffer::resetSleepServo()
{
	t_goal = getCurrentTime();
	t_error = 0;
	t_sleep = 0;

	lastError = 0;
	errorDerivative = 0;
	errorIntegral = 0;
}

#include "utils.h"

bool StreamingBuffer::measurementLoop()
{
//	cerr << "measurementLoop() " << int64_to_str(t_goal)  << " : " << int64_to_str(getCurrentTime()) << endl;

	while(getCurrentTime() < t_goal) {};		//busy wait if there is still time left

	t_goal += samplePeriod;
//	cerr << "samplePeriod: " << int64_to_str(samplePeriod)  << endl;

	// Feedback
//	t_sleep = sleepPID(t_goal - getCurrentTime());
	t_sleep = (t_goal - getCurrentTime());

	if(t_sleep > 0)
	{
		//minimize busy waiting
		thread->sleep(
		static_cast<long>(  ( t_sleep - (t_sleep % 1000000000) ) / 1000000000  ), 
		static_cast<long>(t_sleep % 1000000000)
		);	
	}

	// Measurement
	sti_Device->readChannel(tMeasurement);
	tMeasurement.time = getCurrentTime();
	buffer.push_back(tMeasurement);

	cerr << "measurement: " << (buffer.back().time/1e9) << endl;

	t_error = tMeasurement.time - t_goal;	//positive means it waited too long

	// Trim buffer to correct depth
	bufferMutex->lock();
	{
		if(buffer.size() > 0)
		{
			buffer.pop_front();
		}
		if(buffer.size() > bufferDepth)
		{
			buffer.erase( buffer.begin(), buffer.begin() + (buffer.size() - bufferDepth) );
		}

	}
	bufferMutex->unlock();

	return streamingStatus;	// || error

	

//	thread->start();
//	thread->yield();
//	thread->sleep(0,11);
////	thread->timedwait(22,10);
//	thread->exit();

}


void StreamingBuffer::setStreamingStatus(bool status)
{
	cerr << "setStreamingStatus: " << thread->id() << endl;
	streamingStatus = status;

	if(streamingStatus)
	{
		//start thread
		resetSleepServo();


		thread->start();
	}
	else
	{
		//sleep thread
//		thread->exit();
	}
}


bool StreamingBuffer::setSamplePeriod(double period)
{
	cerr << "period: " << period << endl;

	samplePeriod = static_cast<Int64>(period * 1e9);
	return true;
}


bool StreamingBuffer::setBufferDepth(unsigned int depth)
{
	bufferDepth = depth;
	return true;
}

//getData(double t_initial, double t_duration, double delta_t);


unsigned int StreamingBuffer::getBufferDepth()
{
	return bufferDepth;
}


Int64 StreamingBuffer::getSamplePeriod()
{
	return samplePeriod;
}

Int64 StreamingBuffer::getCurrentTime()
{
	return static_cast<Int64>(clock() * 1e6);
}

