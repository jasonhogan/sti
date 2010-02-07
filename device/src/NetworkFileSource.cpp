/*! \file NetworkFileSource.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class NetworkFileSource
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

#include "NetworkFileSource.h"



NetworkFileSource::NetworkFileSource(std::string fileName) : fileNameWithPath(fileName)
{
	networkFileServant = new TNetworkFile_i( fileNameWithPath );
}

NetworkFileSource::~NetworkFileSource()
{
	delete networkFileServant;
}

std::string NetworkFileSource::getFileName()
{
	return fileNameWithPath;
}


STI::Types::TNetworkFile_ptr NetworkFileSource::getNetworkFileReference()
{
	return networkFileServant->_this();
}


