/*! \file Clock.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class Clock
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

#include <Clock.h>
#include <ctime>
#include <iostream>

#include <omniORB4/CORBA.h>

Clock::Clock()
{
	clockTicksPerSec = CLOCKS_PER_SEC;	// =1000 typically in windows
						// =1000000 on Etrax

	paused = false;
	timeOfPause = 0;

	//convert time to nanoseconds

#if defined(_MSC_VER)
	clockMultiplier = static_cast<Int64>(1000000000 / clockTicksPerSec);
#else
	clockMultiplier = static_cast<Int64>( (1000000000 / clockTicksPerSec) * 1000 );

#endif

//	std::cout << "clockMultiplier = " << clockMultiplier << std::endl;	

	reset();

//	uInt64 maxDouble = 9007199254740992;
}

Clock::~Clock()
{
}

void Clock::reset()
{
	preset(0);
}

//returns the current time in nanoseconds
Int64 Clock::getCurrentTime() const
{
	unsigned long time_s;
	unsigned long time_ns;
	
	omni_thread::get_time(&time_s, &time_ns, 0, 0);
	Int64 omniTime = (1000000000 * static_cast<Int64>(time_s)) + static_cast<Int64>(time_ns);
//	Int64 omniTime = static_cast<Int64>(1000000000 * time_s + time_ns);
//	std::cout << "omniTime = " << omniTime << ", " << (omniTime - initialTime) << std::endl;


//	clock_t temp = clock();
//	std::cout << "Clock::getCurrentTime():  " << temp << ", " << (static_cast<Int64>(temp))
//		<< ", " << (static_cast<Int64>(temp) * clockMultiplier) << ", "
//		<< ( (static_cast<Int64>(temp) * clockMultiplier) - initialTime ) << std::endl;

//	return ( (static_cast<Int64>(temp) * clockMultiplier) - initialTime );
	//return ( (static_cast<Int64>(clock()) * clockMultiplier) - initialTime );

//	return ( (static_cast<Int64>(clock()) * clockMultiplier) - initialTime );

	return (omniTime - initialTime);


}



void Clock::preset(Int64 ns)
{
	// clock() returns a clock_t (usually 32 bits) which rolls over after
	// 49.7 days assuming CLOCKS_PER_SEC=1000
//	initialTime = (static_cast<Int64>( clock() ) * clockMultiplier) - ns;
        

	unsigned long time_s;
        unsigned long time_ns;

        omni_thread::get_time(&time_s, &time_ns, 0, 0);
        initialTime = (1000000000 * static_cast<Int64>(time_s)) + static_cast<Int64>(time_ns);

}

void Clock::pause()
{
	if(!paused)
	{
		paused = true;
		timeOfPause = getCurrentTime();
	}
}

void Clock::unpause()
{
	if(paused)
	{
		paused = false;
		preset(timeOfPause);
	}
}


uInt32 Clock::get_s(Int64 time)
{
	if( time < 0)
		return static_cast<uInt32>( -1*time / 1000000000 );
	else
		return static_cast<uInt32>( time / 1000000000 );
}

uInt32 Clock::get_ns(Int64 time)
{
	
	if( time < 0)
		return static_cast<uInt32>( -1*time - (get_s(time) * 1000000000) );
	else
		return static_cast<uInt32>( time - (get_s(time) * 1000000000) );
}



