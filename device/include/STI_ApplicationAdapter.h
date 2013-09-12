/*! \file STI_ApplicationAdapter.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_ApplicationAdapter
 *  \section license License
 *
 *  Copyright (C) 2011 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef STI_APPLICATIONADAPTER_H
#define STI_APPLICATIONADAPTER_H

#include <STI_Application.h>
#include <MixedData.h>

//About STI_ApplicationAdapter
//This adapter class is used for quickly making an STI_Application.
//All of the pure virtual functions of STI_Application have been (trivially) implemented,
//so derived classes of STI_ApplicationAdapter are not initially abstract.  All of the interface
//functions are still virtual, so the desired functions can be modified in the base class
//without having to reimplement the others.


class STI_ApplicationAdapter : public STI_Application
{
public:

	STI_ApplicationAdapter(ORBManager* orb_manager, std::string ApplicationName, std::string configFilename)
		: STI_Application(orb_manager, ApplicationName, configFilename) {};
	STI_ApplicationAdapter(std::string GUIpath, ORBManager* orb_manager, std::string ApplicationName, 
		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".") 
		: STI_Application(GUIpath, orb_manager, ApplicationName, IPAddress, ModuleNumber, logDirectory) {};

	virtual ~STI_ApplicationAdapter() {};

protected:

	virtual void defineFunctionCalls() {};
	virtual MixedData handleFunctionCall(std::string functionName, std::vector<MixedValue> args) {return MixedData();};
	virtual bool appMain(int argc, char* argv[]) {return false;};	//called in a loop while it returns true

	// Device Attributes
	virtual void defineAttributes() {};
	virtual void refreshAttributes() {};
	virtual bool updateAttribute(std::string key, std::string value) {return true;};

	// Device Channels
	virtual void defineAppChannels() {};
	virtual bool readAppChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) {return false;};
	virtual bool writeAppChannel(unsigned short channel, const MixedValue& value) {return false;};

	// Device Command line interface setup
	virtual void definePartnerDevices() {};
	virtual std::string Execute(int argc, char* argv[]) {return "";};
	

	// Device-specific event parsing
	virtual void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) 
	{parseDeviceEventsDefault(eventsIn, eventsOut);};

	virtual void stopEventPlayback() {};	//for devices that require non-generic stop commands
	virtual void pauseEventPlayback() {};	//for devices that require non-generic pause commands
	virtual void resumeEventPlayback() {}; //for devices that require non-generic resume commands

};

#endif