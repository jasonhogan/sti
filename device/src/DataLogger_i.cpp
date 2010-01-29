/*! \file DataLogger_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class DataLogger_i
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

#include "DataLogger_i.h"


DataLogger_i::DataLogger_i()
{
}

DataLogger_i::~DataLogger_i()
{
}

STI::Types::TNetworkFileSeq* DataLogger_i::getLogFiles()
{
	using STI::Types::TNetworkFileSeq;
	using STI::Types::TNetworkFileSeq_var;

	TNetworkFileSeq_var fileSeq( new TNetworkFileSeq );
//	fileSeq->length( logFiles.size() );

	return fileSeq._retn();
}

void DataLogger_i::clearLocalLogFiles()
{
//	logFiles.clear();
}
