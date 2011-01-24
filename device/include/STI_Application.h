/*! \file STI_Application.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class STI_Application
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

#ifndef STI_APPLICATION_H
#define STI_APPLICATION_H

#include <STI_Device.h>
#include <NetworkFileSource.h>

using boost::filesystem::path;

class STI_Application : public STI_Device
{
public:

	STI_Application(ORBManager* orb_manager, std::string ApplicationName, std::string configFilename);
	STI_Application(std::string GUIpath, ORBManager* orb_manager, std::string ApplicationName, 
		std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory=".");
	virtual ~STI_Application();

private:

	// *** Begin STI_Application Interface definition *** //

	// Application function calls
	virtual void defineFunctionCalls() = 0;
	virtual MixedData handleFunctionCall(std::string functionName, std::vector<MixedValue> args) = 0;

	// main()
	virtual bool appMain(int argc, char* argv[]) = 0;	//called in a loop while it returns true

	// Attributes
	virtual void defineAttributes() = 0;
	virtual void refreshAttributes() = 0;
	virtual bool updateAttribute(std::string key, std::string value) = 0;

	// Channels
	virtual void defineAppChannels() = 0;
	virtual bool readAppChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) = 0;
	virtual bool writeAppChannel(unsigned short channel, const MixedValue& value) = 0;

	// Device Command line interface setup
	virtual void definePartnerDevices() = 0;
	virtual std::string Execute(int argc, char* argv[]) = 0;

	// Device-specific event parsing
	virtual void parseDeviceEvents(const RawEventMap& eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception) = 0;

	// Event Playback control
	virtual void stopEventPlayback() = 0;	//for non-generic stop commands
	virtual void pauseEventPlayback() = 0;	//for non-generic pause commands
	virtual void resumeEventPlayback() = 0; //for non-generic resume commands

	// *** End STI_Application Interface definition *** //

private:
	
	//STI_Device functions that are overridden by STI_Application
	
	bool deviceMain(int argc, char* argv[]);	//called in a loop while it returns true
	std::string execute(int argc, char* argv[]);

	void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

private:

	void loadGUI();
	void installApplicationAttibutes();

	NetworkFileSource* networkFile;

	std::string appGUIpathName;	
	std::string GUIjavaclasspath;

	std::string applicationConfigFilename;
	boost::filesystem::path appGUIpath;
	
	class ApplicationAttributeUpdater : public AttributeUpdater
	{
	public:
		ApplicationAttributeUpdater(STI_Application* app) : 
		  AttributeUpdater(app), app_(app) {}
		void defineAttributes();
		bool updateAttributes(std::string key, std::string value);
		void refreshAttributes();
	private:
		STI_Application* app_;
	};


};

#endif