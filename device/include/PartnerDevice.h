/*! \file PartnerDevice.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class PartnerDevice
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

#ifndef PARTNERDEVICE_H
#define PARTNERDEVICE_H

#include <device.h>
#include <string>
#include <vector>

#include <MixedValue.h>
#include <MixedData.h>
#include <EventParsingException.h>

class CommandLine_i;
class RawEvent;

class PartnerDevice
{
public:

	PartnerDevice() {};
	PartnerDevice(bool dummy);
	PartnerDevice(std::string PartnerName, std::string IP, short module, std::string deviceName, bool required, bool mutual);
	PartnerDevice(std::string PartnerName, std::string deviceID, bool required, bool mutual);
	
	PartnerDevice(std::string PartnerName, CommandLine_i* LocalCommandLine,  bool required, bool mutual);

	~PartnerDevice();

	void setDeviceID(std::string deviceID);

	std::string name() const;
	STI::Types::TDevice device() const;
	std::string execute(std::string args);
	std::string getDeviceID() const;

	bool setAttribute(std::string key, std::string value);
	std::string getAttribute(std::string key);

	
	void registerPartnerDevice(STI::Server_Device::CommandLine_ptr commandLine);
	void registerLocalPartnerDevice(CommandLine_i* LocalCommandLine);
	bool registerMutualPartner(STI::Server_Device::CommandLine_ptr partner);
	void unregisterPartner() {registered = false;};

	template<typename T>
	void event(double time, unsigned short channel, const T& value, const RawEvent& referenceEvent, 
		bool isMeasurement=false, std::string description="") throw(std::exception)
	{
		try {
			event(time, channel, MixedValue(value), referenceEvent, isMeasurement, description);
		} catch(EventParsingException& e) {
			throw e;
		}
	};

	void event(double time, unsigned short channel, const MixedValue& value, const RawEvent& referenceEvent, bool isMeasurement=false, std::string description="") throw(std::exception);
	void event(double time, unsigned short channel, const STI::Types::TValMixed& value, const RawEvent& referenceEvent, bool isMeasurement=false, std::string description="") throw(std::exception);


	bool read(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool write(unsigned short channel, const MixedValue& value);


	bool isRegistered() const;
	bool isAlive();
	bool isLocal();
	bool exists() {return !_dummy;};

	bool isRequired() const;
	bool isMutual() const;

	void enablePartnerEvents();
	void disablePartnerEvents();
	bool getPartnerEventsSetting();

	void resetPartnerEvents();
	std::vector<STI::Types::TPartnerDeviceEvent>& getEvents();


private:

	std::vector<STI::Types::TPartnerDeviceEvent> partnerEvents;

	bool registered;
	bool local;
	bool _required;
	bool _mutual;

	bool _dummy;

	bool partnerEventsEnabled;
	bool partnerEventsEnabledLocked;

	std::string partnerName;
	STI::Types::TDevice partnerDevice;
	std::string _deviceID;
	STI::Server_Device::CommandLine_var commandLine_l;
	CommandLine_i* localCommandLine;

	void setCommandLine(STI::Server_Device::CommandLine_ptr commandLine);

};

#endif

