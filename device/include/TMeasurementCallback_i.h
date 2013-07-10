/*! \file TMeasurementCallback_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class TMeasurementCallback_i
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef TMEASUREMENTCALLBACK_I_H
#define TMEASUREMENTCALLBACK_I_H

#include "device.h"
#include "MeasurementCallback.h"

#include <boost/shared_ptr.hpp>

class TMeasurementCallback_i;
typedef boost::shared_ptr<TMeasurementCallback_i> TMeasurementCallback_i_ptr;


class TMeasurementCallback_i : public POA_STI::Server_Device::TMeasurementCallback
{
public:

	TMeasurementCallback_i(const MeasurementCallback_ptr& measCallback) : localMeasurementCallback(measCallback) {}
	~TMeasurementCallback_i() {}

	void returnResult(const STI::Types::TMeasurement& measurement)
	{
		if(localMeasurementCallback != 0) {
			localMeasurementCallback->handleResult(measurement);
		}
	}

private:

	MeasurementCallback_ptr localMeasurementCallback;
};

#endif
