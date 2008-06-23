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


StreamingBuffer::StreamingBuffer()
{
	StreamingBuffer(false);
}


StreamingBuffer::StreamingBuffer(bool status)
{
	setStreamingStatus(status);
}


StreamingBuffer::StreamingBuffer(bool status, double period, unsigned int depth)
{
	setStreamingStatus(status);
	setSamplePeriod(period);
	setBufferDepth(depth);
}


StreamingBuffer::~StreamingBuffer()
{
}


void StreamingBuffer::setStreamingStatus(bool status)
{
	streamingStatus = status;

	if(streamingStatus)
	{
		//start thread
	}
	else
	{
		//sleep thread
	}
}


bool StreamingBuffer::setSamplePeriod(double period)
{
	samplePeriod = period;
	return true;
}


bool StreamingBuffer::setBufferDepth(unsigned int depth)
{
	bufferDepth = depth;
	return true;
}