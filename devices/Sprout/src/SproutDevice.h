/*! \file SproutDevice.h
 *  \author Jason Hogan
 *  \brief header file for SproutDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef SPROUT_DEVICE_H
#define SPROUT_DEVICE_H


#include <STI_Device_Adapter.h>
#include "rs232Controller.h"


class SproutDevice : public STI_Device_Adapter
{
public:

	SproutDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber, unsigned short comPort);

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);

	std::string execute(int argc, char* argv[]);


	bool changeMode(std::string value);
	std::string getMode();

private:

	rs232Controller* serialController;

	int rs232QuerySleep_ms;
};

#endif
