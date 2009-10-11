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

#include <types.h>

class STI_Device;

class StreamingBuffer
{
public:

	StreamingBuffer();
	StreamingBuffer(STI_Device* device, unsigned short channel, 
		bool status, double period = 1, unsigned int depth = 2);
	~StreamingBuffer();

	void setStreamingStatus(bool status);
	bool setSamplePeriod(double period);
	bool setBufferDepth(unsigned int depth);

	Int64 getSamplePeriod();
	Int64 getCurrentTime();
	unsigned int getBufferDepth();

	omni_mutex *bufferMutex;
	omni_thread * thread;

private:

	Int64 sleepPID(Int64 timeToWait);
	void resetSleepServo();

	static void measurementLoopWrapper(void* object);
	bool measurementLoop();

	std::deque<STI::Types::TMeasurement> buffer;
	
	bool streamingStatus;
	Int64 samplePeriod;
	unsigned int bufferDepth;

	STI_Device* sti_Device;
	unsigned short Channel;

	STI::Types::TMeasurement tMeasurement;	//temp measurement data


	Int64 t_goal;
	Int64 t_sleep;
	Int64 t_error;

	Int64 lastError;
	Int64 errorDerivative;
	Int64 errorIntegral;

//	_LARGE_INTEGER time;
//	_LARGE_INTEGER InitialTime;
//	_LARGE_INTEGER frequency;

};

#endif

