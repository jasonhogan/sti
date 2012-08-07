/*! \file IVIScopeDevice.h
 *  \author Jason Hogan
 *  \brief header file for the class IVIScopeDevice
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

#ifndef IVISCOPEDEVICE_H
#define IVISCOPEDEVICE_H

#include <STI_Device_Adapter.h>
#include <iviScope.h>
#include <MixedData.h>

class IVIScopeDevice : public STI_Device_Adapter
{
public:
	
	IVIScopeDevice(ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address, 
		unsigned short ModuleNumber);
	~IVIScopeDevice();

private:

    // Device Attributes
//    void defineAttributes();
//    void refreshAttributes();
//    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
//	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
//	bool writeChannel(unsigned short channel, const MixedValue& value);

    // Device Command line interface setup
//	std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception);


	ViStatus configureTrigger();

public:

	std::string IVIgetError(ViStatus error);

private:
	
	ViSession session;

	class IVIScopeEvent : public SynchronousEvent
	{
	public:
		IVIScopeEvent(double time, ViSession& viSession, STI_Device* device) : SynchronousEvent(time, device), session(viSession)
		{}
		void setupEvent() {}
		void loadEvent() {}
		void playEvent();
		void collectMeasurementData();
	private:
		MixedData scopeData;
		ViSession& session;
	};

};

#endif

