/*! \file StreamingBuffer.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class StreamingBuffer
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

#ifndef STREAMINGBUFFER_H
#define STREAMINGBUFFER_H

#include <deque>
#include "device.h"

class STI_Device;

class StreamingBuffer
{
public:

	StreamingBuffer() {};
	StreamingBuffer(STI_Device* device, unsigned short channel, bool status);
	StreamingBuffer(STI_Device* device, unsigned short channel, 
		bool status, double period, unsigned int depth);
	~StreamingBuffer();

	void setStreamingStatus(bool status);
	bool setSamplePeriod(double period);
	bool setBufferDepth(unsigned int depth);

	double getSamplePeriod();
	double getCurrentTime();

	omni_mutex *bufferMutex;


private:

	long sleepPID(long timeToWait);
	void resetSleepServo();

	static void measurementLoopWrapper(void* object);
	bool measurementLoop();

	std::deque<STI_Server_Device::TMeasurement> buffer;
	omni_thread * thread;
	
	bool streamingStatus;
	double samplePeriod;
	unsigned int bufferDepth;

	STI_Device* sti_Device;
	unsigned short Channel;

	STI_Server_Device::TMeasurement tMeasurement;	//temp measurement data


//these must be signed Int64
long t_goal;
long t_error;
long t_sleep;

long lastError;
long errorDerivative;
long errorIntegral;



};

#endif