/*! \file STI_Application.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class STI_Application
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


#include <STI_Application.h>
#include <ConfigFile.h>
#include <ORBManager.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

#include <fstream>
#include <string>
using std::string;

STI_Application::STI_Application(ORBManager* orb_manager, std::string ApplicationName, std::string configFilename) :
STI_Device(orb_manager, ApplicationName, configFilename), applicationConfigFilename(configFilename)
{

	bool parseSuccess;
	ConfigFile config(configFilename);
	
	parseSuccess = config.getParameter("JAR_RelativePath", appGUIpathName);
	parseSuccess &= config.getParameter("STI_ApplicationClassResolvedName", GUIjavaclasspath);

	appGUIpath = fs::system_complete( fs::path(appGUIpathName, fs::native) );
	appGUIpathName = appGUIpath.native_directory_string();

	if(!fs::exists(appGUIpath))	{
		std::cerr << "Error: Cannot find GUI file." << endl
			<< "Application cannot initialize." << endl;
		orb_manager->ORBshutdown();
	} else {
		loadGUI();
	}
	installApplicationAttibutes();
}

STI_Application::STI_Application(std::string GUIpath, ORBManager* orb_manager, std::string ApplicationName, 
								 std::string IPAddress, unsigned short ModuleNumber, std::string logDirectory) :
STI_Device(orb_manager, ApplicationName, IPAddress, ModuleNumber, logDirectory), appGUIpathName(GUIpath)
{
	applicationConfigFilename = "";

	appGUIpath = fs::system_complete( fs::path(appGUIpathName, fs::native) );
	appGUIpathName = appGUIpath.native_directory_string();
		
	if(!fs::exists(appGUIpath))
	{
		std::cerr << "Error: Cannot find GUI file." << endl
			<< "Application cannot initialize." << endl;
		orb_manager->ORBshutdown();
	} else {
		loadGUI();
	}

	installApplicationAttibutes();
}

STI_Application::~STI_Application()
{
	delete networkFile;
}

//MixedData STI_Application::call(std::string function, MixedValue arguments)
//{
//	this->sendRefreshEvent
//}

void STI_Application::loadGUI()
{
	networkFile = new NetworkFileSource(appGUIpathName);

	//Put the GUI into a file
	STI::Types::TFile file;

	file.description = CORBA::string_dup("");
	file.fileName = CORBA::string_dup(appGUIpathName.c_str());
	file.fileServerAddress = CORBA::string_dup("");
	file.fileServerDirectory = CORBA::string_dup("");
	file.networkFile = networkFile->getNetworkFileReference();

	//Put the loaded GUI into the labeled data as a File
	MixedData guiData;
	guiData.addValue(GUIjavaclasspath);
	guiData.addValue(file);
	setLabeledData("JavaGUI", guiData);
}

void STI_Application::installApplicationAttibutes()
{
	addAttributeUpdater( new ApplicationAttributeUpdater(this) );
}

void STI_Application::ApplicationAttributeUpdater::defineAttributes()
{
	addAttribute("STI_App_GUIPath", app_->appGUIpath);
}

bool STI_Application::ApplicationAttributeUpdater::updateAttributes(std::string key, std::string value)
{
	if(key.compare("STI_App_GUIPath") == 0) 
	{
		return true; //SMD edit 03/30/11
	}
	
	return false;
}

void STI_Application::ApplicationAttributeUpdater::refreshAttributes()
{}

bool STI_Application::deviceMain(int argc, char* argv[])
{
	return appMain(argc, argv);
}


// Device Channels
void STI_Application::defineChannels() 
{
	//Channel 0 reserved for application function calls from client
	addInputChannel(0, STI::Types::DataVector, STI::Types::ValueVector);

	defineAppChannels();	//pure virtual
}

bool STI_Application::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) {
	if(channel == 0) {
		dataOut.setValue(
			handleFunctionCall(
			valueIn.getVector().at(0).getString(), valueIn.getVector().at(1).getVector())
			);
		return true;
	}
	return readAppChannel(channel, valueIn, dataOut);
}

bool STI_Application::writeChannel(unsigned short channel, const MixedValue& value) 
{
	return writeAppChannel(channel, value);
}

bool STI_Application::executeDelegatedSpecialCommands(std::vector<std::string> arguments, std::string& output)
{
	//This handles STI_Application special execute commands (commands of the form sti [command]).
	//STI_Device delegates to this function if [command] doesn't match any built in commands.

	if(arguments.at(2).compare("isApplication")==0) {
		//The user (i.e., the STI Console probably) called "sti isApplication".
		output = "yes";
		return true;
	}

	return false;
}
std::string STI_Application::printDelegatedSpecialCommandOptions() {
	return "    isApplication                --  Tests if this device is an STI_Application.";
}

std::string STI_Application::execute(int argc, char* argv[]) 
{
	//std::vector<std::string> args;
	//STI::Utils::convertArgs(argc, argv, args);

	//if(args.size() > 1) {
	//	if(args.at(1).compare("isApplication") == 0) {
	//		return "yes";
	//	}
	//}

	return Execute(argc, argv);
}


