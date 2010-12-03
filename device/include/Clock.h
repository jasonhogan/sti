/*! \file Clock.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class Clock
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

#ifndef STI_CLOCK_H
#define STI_CLOCK_H

#include "types.h"

class Clock
{
public:
	Clock();
	~Clock();

	template<typename T>
	bool operator< (const T &rhs) const
	{
		return (getCurrentTime() < static_cast<Int64>(rhs));
	}
	template<typename T>
	bool operator> (const T &rhs) const
	{
		return (getCurrentTime() > static_cast<Int64>(rhs));
	}

	void reset();
	Int64 getCurrentTime() const;

	void pause();
	void unpause();

	static uInt32 get_s(Int64 time);
	static uInt32 get_ns(Int64 time);

	template<typename T>
	void preset(T ns) 
	{
		preset(static_cast<Int64>(ns));
	}
	void preset(Int64 ns);

private:
	Int64 initialTime;

	Int64 timeOfPause;
	bool paused;

	int clockTicksPerSec;
	Int64 clockMultiplier;

};

#endif


