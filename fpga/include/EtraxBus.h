/*! \file EtraxBus.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class EtraxBus
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

#ifndef ETRAXBUS_H
#define ETRAXBUS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

//#include <omniORB4/CORBA.h>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>

//#ifndef _MSC_VER
////This should only get loaded when cross compiling (i.e., never in windows).
////However, we should use a better preprocessor flag than this hack...
//#include <err.h>
//#endif

#ifdef HAVE_BUS_SPACE_H
extern "C" {
#  include <bus_space.h>
}
#endif

#include <utils.h>

class EtraxBus
{
public:

	EtraxBus(uInt32 MemoryAddress, uInt32 NumberOfWords=1);
	~EtraxBus();

	uInt32 readDataFromAddress(uInt32 address);
	uInt32 readData(uInt32 addressOffset=0);

	void writeDataToAddress(uInt32 data, uInt32 address);
	void writeData(uInt32 data, uInt32 addressOffset=0);


	void setMemoryAddress(uInt32 MemoryAddress, uInt32 NumberOfWords=1);
	uInt32 getMemoryAddress() const;

	void setupMemoryBus();

private:

	uInt32 memoryAddress;
	uInt32 numberOfWords;

	uInt32 lastMemoryAddress;
	uInt32 lastNumberOfWords;

#ifdef HAVE_BUS_SPACE_H

	// variables for setting the address for writing via CPU addresses using bus_space_write
	bus_space_tag_t  tag;	//static?
	bus_space_handle_t      ioh, ioh1;
	uInt32                  old_speed;

#endif

//	omni_mutex* readMutex;
	mutable boost::shared_mutex readMutex;

};

#endif
