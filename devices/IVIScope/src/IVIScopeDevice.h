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

#include <vector>
#include <string>

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
	ViStatus configureChannels();

public:

	std::string IVIgetError(ViStatus error);
	void checkViError(ViStatus error, std::string description = "") throw(IVIScopeException);

private:

	class IVIScopeException
	{
	public:
		IVIScopeException(ViStatus err) : error(err) {}
		IVIScopeException(ViStatus err, std::string desc) : error(err), description(desc) {}
		
		ViStatus error;
		std::string description;
	};

	ViSession session;
	std::vector<std::string> channelNames;

	class ChannelConfig
	{
	public:
		ChannelConfig(unsigned short channel, std::string channelName, const MixedValue& value) : ch(channel)
		{ parseValue(value); }

		void parseValue(const MixedValue& value);

		double timePerRecord;
		double verticalScale;
		double verticalOffset;
		double probeAttenuation;
		std::string chName;		//e.g., "CH1"
		unsigned short ch;
	};
		
	class IVIScopeBaseEvent : public SynchronousEvent
	{
	public:
		IVIScopeBaseEvent(double time, ViSession& viSession, STI_Device* device) : SynchronousEvent(time, device), session(viSession)
		{}
		void setupEvent() {}
		void loadEvent() {}
		virtual void playEvent() = 0;
		virtual void collectMeasurementData() = 0;
	
	protected:
		ViSession& session;
	};


	class IVIScopeInitializationEvent : public IVIScopeBaseEvent
	{
		IVIScopeInitializationEvent(double time, ChannelConfig& config, ViSession& viSession, STI_Device* device) 
			: IVIScopeBaseEvent(time, viSession, device), channelConfig(config) {}
		
		void playEvent();
		void collectMeasurementData() {}

		ChannelConfig channelConfig;
	};



	class IVIScopeEvent : public IVIScopeBaseEvent
	{
	public:
		IVIScopeEvent(double time, ChannelConfig& config, ViSession& viSession, STI_Device* device) 
			: IVIScopeBaseEvent(time, viSession, device), channelConfig(config)
		{  }
		void playEvent();
		void collectMeasurementData();
	private:
		MixedData scopeData;
		ChannelConfig channelConfig; // temporary, until Ini events are working
	};

};

#endif

