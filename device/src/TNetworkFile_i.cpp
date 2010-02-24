/*! \file TNetworkFile_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class TNetworkFile_i
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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

#include "TNetworkFile_i.h"
#include <boost/filesystem/path.hpp>

using namespace std;


TNetworkFile_i::TNetworkFile_i(std::string filename) : filename_l(filename)
{
	fileStream = new ifstream(filename_l.c_str(), ios::in|ios::binary|ios::ate);
}

TNetworkFile_i::~TNetworkFile_i()
{
	fileStream->close();
	delete fileStream;
}

::CORBA::Boolean TNetworkFile_i::getBytes(::CORBA::Long startByte, ::CORBA::Long numBytes, STI::Types::TOctetSeq_out bytes)
{
	ifstream::pos_type memSize = numBytes;
	char* memblock;
	bool success = false;

	if (fileStream->is_open() && !fileStream->fail())
	{
		int fileLength = length();
		int over = (startByte + numBytes) - fileLength; // number of requested bytes that are over the end of the file
		
		if(over > 0)
			memSize -= over;

		memblock = new char[memSize];

		fileStream->seekg(startByte, ios::beg); // place get pointer startByte away from beginning
		fileStream->read(memblock, memSize);
		success = !fileStream->fail();

		if(success)
		{
		}

		delete[] memblock;
	}

	return success;
}

char* TNetworkFile_i::fileName()
{
	//return only the filename (drop the path)
	CORBA::String_var name( getFileNameNoPath().c_str() );
	return name._retn();
}

::CORBA::Long TNetworkFile_i::length()
{
	fileStream->seekg(0, ios::end);
	int len = fileStream->tellg();

	if(fileStream->fail())
		return 0;
	else
		return len;
}

std::string TNetworkFile_i::getFileNameNoPath()
{
	boost::filesystem::path pathSeparator("/", boost::filesystem::native);
	std::string nativePathSeparator = pathSeparator.native_directory_string();

	std::size_t found = filename_l.find_last_of(nativePathSeparator);
	
	if( found == std::string::npos )
		return filename_l;
	else
		return filename_l.substr(found + 1);
}

::CORBA::Boolean TNetworkFile_i::deleteRemoteFile()
{
	return false;
}

