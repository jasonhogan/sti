/*! \file RAM_Controller_Device.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class RAM_Controller_Device
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

#include "RAM_Controller_Device.h"
#include <sstream>
#include <vector>
#include <string>

using std::stringstream;
using std::vector;
using std::string;


RAM_Controller_Device::RAM_Controller_Device(
	ORBManager* orb_manager, 
	std::string DeviceName, 
	std::string IPAddress, 
	unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, IPAddress, ModuleNumber)
{
	for(unsigned i = 0; i < 8; i++)
	{
		RAM_blocks.push_back( FPGA_RAM_Block(i) );
		writeTimes.push_back( 0 );
	}

	calculateBufferSizeMutex = new omni_mutex();
	calculateBufferSizeCondition = new omni_condition(calculateBufferSizeMutex);

	calculatingBufferSize = false;
	numberOfRegisteredModules = 0;
	allocationCycles = 0;
}

RAM_Controller_Device::~RAM_Controller_Device()
{
}

bool RAM_Controller_Device::deviceMain(int argc, char** argv)
{
	return false;
}
void RAM_Controller_Device::defineAttributes()
{
	stringstream writeTimeKey;
	stringstream bufferSizeKey;

	for(unsigned module = 0; module < 8; module++)
	{
		writeTimeKey.str("");
		writeTimeKey << "MinWriteTime Mod_" << module;
		addAttribute(writeTimeKey.str(), 0);
		
		bufferSizeKey.str("");
		bufferSizeKey << "BufferSize Mod_" << module;
		addAttribute(bufferSizeKey.str(), 0);
	}
}

void RAM_Controller_Device::refreshAttributes()
{
	stringstream writeTimeKey;
	stringstream bufferSizeKey;

	for(unsigned module = 0; module < 8; module++)
	{
		writeTimeKey.str("MinWriteTime Mod_");
		writeTimeKey << module;
		setAttribute(writeTimeKey.str(), getWriteTime(module));
		
		bufferSizeKey.str("BufferSize Mod_");
		bufferSizeKey << module;
		setAttribute(bufferSizeKey.str(), getBufferSize(module));
	}
}

bool RAM_Controller_Device::updateAttribute(std::string key, std::string value)
{
	bool convertSuccess = false;
	unsigned module;
	uInt32 intValue;
	string::size_type moduleBegin = key.find_last_of("_");
	
	//pick off the module number at the end of the attribute
	if( moduleBegin != string::npos && (moduleBegin + 1) < key.length() )
	{
		convertSuccess = stringToValue( key.substr(moduleBegin + 1), module );
		convertSuccess &= stringToValue(value, intValue);
	}


	bool success = false;

	if(key.find("MinWriteTime") != string::npos && convertSuccess && module < 8)	{		writeTimes.at(module) = intValue;		success = true;	}	if(key.find("BufferSize") != string::npos && convertSuccess && module < 8)	{		success = setBufferSize(module, intValue);
		success = true;
	}
	return success;
}

void RAM_Controller_Device::defineChannels()
{
}

bool RAM_Controller_Device::readChannel(ParsedMeasurement& Measurement)
{
	return false;
}

bool RAM_Controller_Device::writeChannel(const RawEvent& Event)
{
	return false;
}

void RAM_Controller_Device::definePartnerDevices()
{
}

std::string RAM_Controller_Device::execute(int argc, char** argv)
{
	bool convertSuccess = false;
	unsigned module;
	string result = "";
	vector<string> args;
	convertArgs(argc, argv, args);

	//the first arguement is the device's name

	if(args.size() == 2 && args.at(1).compare("calculateBufferSize") == 0)
	{
		result = valueToString( calculateBufferSize() );
	}

	if(args.size() == 3)
	{
		convertSuccess = stringToValue(args.at(2), module);

		if(convertSuccess && args.at(1).compare("getStartAddress") == 0)
		{
			result = valueToString( RAM_blocks.at(module).getStartAddress() );
		}
		if(convertSuccess && args.at(1).compare("getEndAddress") == 0)
		{
			result = valueToString( RAM_blocks.at(module).getEndAddress() );
		}
	}

	return result;
}

void RAM_Controller_Device::parseDeviceEvents(const RawEventMap& eventsIn, 
					   SynchronousEventVector& eventsOut) throw(std::exception)
{
}

void RAM_Controller_Device::stopEventPlayback()
{
}

uInt32 RAM_Controller_Device::getWriteTime(unsigned module)
{
	return writeTimes.at(module);
}

uInt32 RAM_Controller_Device::getStartAddress(unsigned module)
{
	return RAM_blocks.at(module).getStartAddress();
}

uInt32 RAM_Controller_Device::getEndAddress(unsigned module)
{
	return RAM_blocks.at(module).getEndAddress();
}

uInt32 RAM_Controller_Device::getBufferSize(unsigned module)
{
	return RAM_blocks.at(module).getSizeInWords();
}


bool RAM_Controller_Device::setBufferSize(unsigned module, uInt32 value)
{
	bool success = false;
	if(module < RAM_blocks.size())
	{
		success = RAM_blocks.at(module).setEndAddress(
			RAM_blocks.at(module).getStartAddress() 
			+ value 
			- RAM_blocks.at(module).getRAM_Word_Size());
		
		for(unsigned i = module + 1; i < RAM_blocks.size(); i++)
		{
			success &= RAM_blocks.at(i).setStartAddress( 
					RAM_blocks.at(i - 1).getEndAddress() 
					+ RAM_blocks.at(i - 1).getRAM_Word_Size() );
		}
		//should resize all others too...
	}

	return success;	//not allowed currently
}

bool RAM_Controller_Device::calculateBufferSize()
{

	//all registered devices call this function; the controller must wait
	//until all have called before returning since it needs to know all minimum wait times

	const PartnerDeviceMap& partners = getRegisteredPartners();

	calculateBufferSizeMutex->lock();
	{
		if( !calculatingBufferSize )
		{
			calculatingBufferSize = true;
			numberOfRegisteredModules = partners.size();
			numberOfModulesCalculatingBufferSize = 0;
			allocationCycles++;
		}
		numberOfModulesCalculatingBufferSize++;
	}
	calculateBufferSizeMutex->unlock();


	//Timed wait for synchronization; wake up when the last partner gets here
	if(numberOfModulesCalculatingBufferSize == numberOfRegisteredModules)
	{
		//all modules have been here; wake up them all
		calculateBufferSizeCondition->broadcast();
	}
	else
	{
		unsigned long wait_s;
		unsigned long wait_ns;
	
		//calculate absolute time to wake up
		omni_thread::get_time(&wait_s, &wait_ns, 10, 0);	//10 second timeout

		calculateBufferSizeCondition->timedwait(wait_s, wait_ns);
	}
	calculatingBufferSize = false;

	//Compute the new buffer size and determine if it is acceptable

	return calculateNewRAMSizes();
}

bool RAM_Controller_Device::calculateNewRAMSizes()
{
	bool acceptable = false;
	uInt32 minValue = 0xffffffff;
	uInt32 maxValue = 0;
	uInt32 totalTime = 0;

	unsigned i;

	//Find min and max wait times
	for(i = 0; i < writeTimes.size(); i++)
	{
		if(writeTimes.at(i) < minValue)
			minValue = writeTimes.at(i);
		if(writeTimes.at(i) > maxValue)
			maxValue = writeTimes.at(i);
		totalTime += writeTimes.at(i);
	}

	double fractionalDif = 0;
	if(maxValue != 0) 
		fractionalDif = 1.0 * (maxValue - minValue) / maxValue;

	if( fractionalDif > 0.1 )
	{
		if(allocationCycles > 5)
		{
			reportMessage(LoadingError, "Warning: RAM write times still differ by more than 10% after 5 iterations. Aborting automatic allocation.");
			acceptable = true;
		}
		else
		{
			acceptable = false;
			uInt32 newSize;
			//adjust RAM block sizes
			for(i = 0; i < writeTimes.size(); i++)
			{
				newSize = ( (writeTimes.at(i) * RAM_blocks.at(i).getSizeInWords()) / totalTime);

				RAM_blocks.at(i).setEndAddress( 
					RAM_blocks.at(i).getStartAddress() + newSize 
					- RAM_blocks.at(i).getRAM_Word_Size());
				
				if((i + 1) < writeTimes.size())
				{
					RAM_blocks.at(i + 1).setStartAddress( 
						RAM_blocks.at(i).getStartAddress() + newSize );
				}
			}
		}
	}

	if (acceptable)
	{
		allocationCycles = 0;	//reset for next time;
	}

	return acceptable;
}