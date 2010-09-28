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
		RAM_blocks.push_back( i );
		writeTimes.push_back( 0 );
		cout << "New " << i << ": (" << RAM_blocks.at(i).getStartAddress() << ", " 
				<< RAM_blocks.at(i).getEndAddress() << ")" << endl;
	}
		
	for(unsigned i = 0; i < RAM_blocks.size(); i++)
		{
			cout << "Init Addresses " << i << ": (" 
				<< RAM_blocks.at(i).getStartAddress() << ", " 
				<< RAM_blocks.at(i).getEndAddress() << ")" << endl;
		}

		cout << "------------" << endl;

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
		addAttribute(bufferSizeKey.str(), getBufferSize(module) );
	}
}



void RAM_Controller_Device::refreshAttributes()
{
	stringstream writeTimeKey;
	stringstream bufferSizeKey;

	for(unsigned module = 0; module < 8; module++)
	{
		writeTimeKey.str("");
		writeTimeKey << "MinWriteTime Mod_" << module;
		setAttribute(writeTimeKey.str(), getWriteTime(module));
		
		bufferSizeKey.str("");
		bufferSizeKey << "BufferSize Mod_" << module;
		setAttribute(bufferSizeKey.str(), getBufferSize(module) );
	}
}
//works here


void RAM_Controller_Device::defineChannels()
{
}

bool RAM_Controller_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return false;
}

bool RAM_Controller_Device::writeChannel(unsigned short channel, const MixedValue& value)
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
	uInt32 addressValue;
	string result = "";
	vector<string> args;
	STI::Utils::convertArgs(argc, argv, args);

	//the first arguement is the device's name ("RAM Controller" in this case)

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
	if(args.size() == 4)
	{
		convertSuccess = stringToValue(args.at(2), module);
		convertSuccess &= stringToValue(args.at(3), addressValue);
		
		if(convertSuccess && args.at(1).compare("setStartAddress") == 0)
		{
			RAM_blocks.at(module).setStartAddress(addressValue);
			result = valueToString(true);
		}
		if(convertSuccess && args.at(1).compare("setEndAddress") == 0)
		{
			RAM_blocks.at(module).setEndAddress(addressValue);
			result = valueToString(true);
		}
		refreshAttributes();
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


bool RAM_Controller_Device::calculateBufferSize()
{

	//all registered devices call this function; the controller must wait
	//until all have called before returning since it needs to know all minimum wait times

	const PartnerDeviceMap& partners = this->getPartnerDeviceMap();

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
			RAM_blocks.at(0).setStartAddress(0);	//make sure the 0th module starts at the beginning of the RAM
			for(i = 0; i < writeTimes.size(); i++)
			{
				newSize = ( (writeTimes.at(i) * RAM_blocks.at(i).getSizeInWords()) / totalTime);
				setBufferSize(i, newSize);

				//RAM_blocks.at(i).setRAM_Block_Size(newSize);
				//
				//if( i < RAM_blocks.size() )
				//{
				//	RAM_blocks.at(i + 1).setStartAddress( 
				//		RAM_blocks.at(i).getEndAddress() + FPGA_RAM_Block::getRAM_Word_Size() );
				//}
			}
		}
	}

	if (acceptable)
	{
		allocationCycles = 0;	//reset for next time;
	}

	return acceptable;
}






bool RAM_Controller_Device::setBufferSize(unsigned module, uInt32 value)
{
	uInt32 newSize, temp;
	unsigned i;
	int j;

	if(module < RAM_blocks.size())
	{
		RAM_blocks.at(module).setRAM_Block_Size(value);
		newSize = RAM_blocks.at(module).getSizeInWords();

		for(i = module + 1; i < RAM_blocks.size(); i++)
		{
			temp = RAM_blocks.at(i - 1).getEndAddress();
			RAM_blocks.at(i).setStartAddress( RAM_blocks.at(i - 1).getEndAddress() 
				+ FPGA_RAM_Block::getRAM_Word_Size() );
			temp = RAM_blocks.at(i).getStartAddress();
		}

		if(newSize < value)
		{
			//it didn't fit; move the start address to make it fit
		
			RAM_blocks.at(module).setStartAddress(
				RAM_blocks.at(module).getStartAddress() - 
				(value - newSize) * FPGA_RAM_Block::getRAM_Word_Size() );
		
			for(j = module - 1; j >= 0; j--)
			{
				RAM_blocks.at(j).setEndAddress( RAM_blocks.at(j + 1).getStartAddress() 
					- FPGA_RAM_Block::getRAM_Word_Size() );
			}
		}

		for(i = 0; i < RAM_blocks.size(); i++)
		{
			cout << "Addresses " << i << ": (" 
				<< RAM_blocks.at(i).getStartAddress() << ", " 
				<< RAM_blocks.at(i).getEndAddress() << ")" << endl;
		}

		cout << "------------" << endl;
		return true;
	}
	return false;
}



bool RAM_Controller_Device::updateAttribute(string key, string value)
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
    if(key.find("BufferSize") != string::npos && convertSuccess && module < 8)
    {
        success = setBufferSize(module, intValue);
    }
    if(key.find("MinWriteTime") != string::npos && convertSuccess && module < 8)
    {
        writeTimes.at(module) = intValue;
        success = true;
    }
    return success;
}


//{
//
//
//    bool success = false;
//	uInt32 maxSize = FPGA_RAM_Block::getTotal_RAM_Size_Words() - RAM_blocks.at(module).getStartAddress() - 2*(7 - module)
//	if(module < RAM_blocks.size() && value < )
//	{
//		success = RAM_blocks.at(module).setEndAddress(RAM_blocks.at(module).getStartAddress() + value - FPGA_RAM_Block::getRAM_Word_Size());
//		
//
//		if(success)
//		{
//			for(unsigned i = module + 1; i < RAM_blocks.size(); i++)
//			{
//				success &= RAM_blocks.at(i).setStartAddress(RAM_blocks.at(i - 1).getEndAddress() + FPGA_RAM_Block::getRAM_Word_Size() );
//			}
//		}
//		if(!success)
//		{
//			// call recursively with smaller values until it works
//			success = setBufferSize(module, value - 2 * FPGA_RAM_Block::getRAM_Word_Size() );
//		}
//	}
//    return success;
//}




