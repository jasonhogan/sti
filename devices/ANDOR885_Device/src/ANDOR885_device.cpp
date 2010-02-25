//MUST INCLUDE ATMCD32M.lib among linker files. Keep this lib files and the atmcd32d.h files in the same directory as these files


/*! \file ANDOR885_device.cpp
 *  \author Susannah Dickerson 
 *  \brief Source file for the iXon DV-885 camera
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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

#include "andor885_device.h"


ANDOR885_Device::ANDOR885_Device(
		ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber) :
ANDOR885_Camera(),
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	numEventExposures = 0;
	digitalChannel = 0;
}

ANDOR885_Device::~ANDOR885_Device()
{
}

bool ANDOR885_Device::deviceMain(int argc, char **argv)
{
	bool error = false;
	int message;

	ofstream cerrLog;
	char tempChar[MAX_PATH];
	std::string tempString;

	tempString = getFilePath() + "cerr.log";
	strcpy(tempChar,tempString.c_str());
	
	cerrLog.open(tempChar);

	streambuf* cerrBuffer = cerr.rdbuf(); // save cerr's output buffer

	cerr.rdbuf (cerrLog.rdbuf()); // redirect output into the file	

	std::cout << "*****************\n";
	std::cout << "* DO NOT Ctrl-c *\n";
	std::cout << "*****************\n";
	std::cout << "Press any key to cleanly shutdown camera and program... ";
	std::cin >> message;
	
	cerr.rdbuf (cerrBuffer); // restore old output buffer
	cerrLog.close();

	error = deviceExit();

	std::cout << "Camera off" << std::endl;

	return error;
}


void ANDOR885_Device::defineAttributes()
{

	//Attributes
	addAttribute("*Camera status", "On", "On, Off");
	addAttribute("Acquisition status", "Off", "On, Off");
	addAttribute(acquisitionMode_t.name, acquisitionMode_t.initial, acquisitionMode_t.makeAttributeString());
	addAttribute(triggerMode_t.name, triggerMode_t.initial, triggerMode_t.makeAttributeString()); //trigger mode
//	addAttribute("Read mode", "Image","Image, Multi-track, Random-track, Single-track"); //readout mode of data
	addAttribute(readMode_t.name, readMode_t.initial, readMode_t.makeAttributeString()); //readout mode of data
//	addAttribute("Spool mode", "Off", "On, Off"); //spooling of data
	
	addAttribute(shutterMode_t.name, shutterMode_t.initial, shutterMode_t.makeAttributeString()); // Shutter control
//	addAttribute("Shutter open time (ms)", openTime); //time it takes shutter to open
//	addAttribute("Shutter close time (ms)", closeTime); //time it takes shutter to close
	
	addAttribute("Exposure time (s)", getExposureTime()); //length of exposure

	addAttribute("*Folder Path for saved files", getFilePath());
	addAttribute("Number of exposures", getNumExposures());

	addAttribute("*Cooler setpoint", getCoolerSetpt());
	addAttribute("*Cooler status", "Off", "On, Off");

	try {
		addAttribute("Camera temperature", getCameraTemp());
	}
	catch (ANDOR885_Exception &e) {
		std::cerr << e.printMessage() << std::endl;
		addAttribute("Camera temperature", -999);
	}

	addAttribute("Save mode", "On", "On, Off");

	addAttribute(preAmpGain_t.name, preAmpGain_t.initial, preAmpGain_t.makeAttributeString()); //PreAmp gain
	
	addAttribute("Num exp per file", getNumPerFile());

	addAttribute(verticalShiftSpeed_t.name, verticalShiftSpeed_t.initial, verticalShiftSpeed_t.makeAttributeString()); // Vertical shift speed of pixels
	addAttribute(verticalClockVoltage_t.name, verticalClockVoltage_t.initial, verticalClockVoltage_t.makeAttributeString()); // Vertical clock voltage
	addAttribute(horizontalShiftSpeed_t.name, horizontalShiftSpeed_t.initial, horizontalShiftSpeed_t.makeAttributeString()); // Horizontal shift speed of pixels
}

void ANDOR885_Device::refreshAttributes()
{
	
	// All attributes are stored in c++, none are on the fpga
	//Attributes not set in serial commands

	setAttribute("*Camera status", (getCameraStat() == ANDOR_ON) ? "On" : "Off");

	setAttribute("Acquisition status", (getAcquisitionStat() == ANDOR_ON) ? "On" : "Off");

	setAttribute(acquisitionMode_t.name, acquisitionMode_t.choices.find(getAcquisitionMode())->second);	

	setAttribute(triggerMode_t.name, triggerMode_t.choices.find(getTriggerMode())->second); //trigger mode?

	setAttribute(readMode_t.name, readMode_t.choices.find(getReadMode())->second);

//	setAttribute("Spool mode", (spoolMode == ON) ? "On" : "Off");

	setAttribute(shutterMode_t.name, shutterMode_t.choices.find(getShutterMode())->second);	
//	setAttribute("Shutter open time (ms)", getOpenTime()); 
//	setAttribute("Shutter close time (ms)", getCloseTime()); 

	setAttribute("Exposure time (s)", getExposureTime());

	setAttribute("*Folder Path for saved files", getFilePath());
	setAttribute("Number of exposures", getNumExposures());

	setAttribute("*Cooler setpoint", getCoolerSetpt());
	setAttribute("*Cooler status", (getCoolerStat() == ANDOR_ON) ? "On" : "Off");
	
	try {
		setAttribute("Camera temperature", getCameraTemp());
	}
	catch (ANDOR885_Exception& e) {
		std::cerr << e.printMessage() << std::endl;
		setAttribute("Camera temperature", -999);
	}

	setAttribute("Save mode", (getSaveMode() == ANDOR_ON) ? "On" : "Off");

	setAttribute(preAmpGain_t.name,preAmpGain_t.choices.find(getPreAmpGain())->second);

	setAttribute("Num exp per file", getNumPerFile());

	setAttribute(verticalShiftSpeed_t.name,verticalShiftSpeed_t.choices.find(getVerticalShiftSpeed())->second);
	setAttribute(verticalClockVoltage_t.name,verticalClockVoltage_t.choices.find(getVerticalClockVoltage())->second);
	setAttribute(horizontalShiftSpeed_t.name,horizontalShiftSpeed_t.choices.find(getHorizontalShiftSpeed())->second);
}

bool ANDOR885_Device::updateAttribute(std::string key, std::string value)
{

	double tempDouble;
	int tempInt;
//	char tempChar[MAX_PATH];
	std::string tempString;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = false;


	// When the camera is acquiring, the user is prohibited from changing the attributes,
	// except to turn off the acquisition.
	try {
		eventStatMutex->lock();
		if (eventStat == ANDOR_ON) {
			throw ANDOR885_Exception("Event Acquisition in progress. Wait until acquisition completes to change attributes");
		}
		eventStatMutex->unlock();
		if((getAcquisitionStat() != ANDOR_ON)|| 
			(key.compare("Acquisition status") == 0 && value.compare("Off") == 0) ||
			(key.compare("Save mode") == 0) || (key.compare("Camera temperature") == 0)) {

			if(key.compare("*Camera status") == 0) {
				success = true;

				if (value.compare("On") == 0){
					setCameraStat(ANDOR_ON);
				} 
				else if (value.compare("Off") == 0) {
					setCameraStat(ANDOR_OFF);
				} 
				else {
					throw ANDOR885_Exception("Device: Unrecognized Camera Status requested");
				}
			} 
			
			else if(key.compare("Acquisition status") == 0) {
				success = true;

				if (value.compare("Off") == 0) {
					setAcquisitionStat(ANDOR_OFF);
				} 
				else if (value.compare("On") == 0) {
					setAcquisitionStat(ANDOR_ON);
				}
				else {
					throw ANDOR885_Exception("Device: Unrecognized Acquisition Status requested");
				}
			}

			else if(key.compare(acquisitionMode_t.name) == 0) {
				success = true;
				setAcquisitionMode(acquisitionMode_t.inverseFind(value));
			}

			else if (key.compare(triggerMode_t.name) == 0) {
				success = true;
				setTriggerMode(triggerMode_t.inverseFind(value));
			}

			else if(key.compare(readMode_t.name) == 0) {
				success = true;
				setReadMode(readMode_t.inverseFind(value));
			}

	/*		else if(key.compare("Spool mode") == 0)
			{
				success = true;
				if (value.compare("On") == 0) {
					if (acquisitionMode == ACQMODE_SINGLE_SCAN){
						error = SetSpool(0,0,NULL,10);  //Disabled
						std::cerr << "6. Cannot spool in single scan read mode\n";
						if (error == DRV_SUCCESS) {
							std::cerr << "6. Spooling disabled\r\n";
							spoolMode = OFF;
						} else {
							std::cerr << "6. Error disabling spooling" << std::endl;
						}
					}
					else if (spoolMode != ON) {
						strcpy(tempChar,filePath.c_str());
						error = SetSpool(1,0,tempChar,10); //Enabled; 10 images can be stored in RAM before error
						if (error == DRV_SUCCESS) {
							std::cerr << "6. Spooling Enabled\r\n";
							spoolMode = ON;
						} else {
							std::cerr << "6. Error enabling spooling" << std::endl;
						}
					} else {
						std::cerr << "6. Spooling already enabled" << std::endl;
					}
				}

				else if (value.compare("Off") == 0) {
					if (spoolMode != OFF) {
						spoolMode = OFF;
						SetSpool(0,0,NULL,10);  //Disabled
						std::cerr << "6. Spooling Disabled" << std::endl;
					} else {
						std::cerr << "6. Spooling already disabled" << std::endl;
					}
				} 
				
				else {
					success = false;
					std::cerr << "6. Error selecting Spool mode" << std::endl;
				}
			}
	*/
			else if(key.compare("Shutter mode") == 0) {
				success = true;
				setShutterMode(shutterMode_t.inverseFind(value));
			}		

	/*		else if(key.compare("Shutter open time (ms)") == 0 && successInt)
			{
				success = true;
				setOpenTime(tempInt);
			}

			else if(key.compare("Shutter close time (ms)") == 0 && successInt)
			{
				success = true;
				setCloseTime(tempInt);
			}
	*/
			else if(key.compare("Exposure time (s)") == 0 && successDouble) {
				success = true;
				setExposureTime((float) tempDouble);
			}

			else if(key.compare("*Folder Path for saved files") == 0) {
				success = true;
				setFilePath(value);
			}

			else if (key.compare("Number of exposures") == 0 && successInt) {
				success = true;
				setNumExposures(tempInt);
			}

			else if (key.compare("*Cooler setpoint") == 0 && successInt) {
				success = true;
				setCoolerSetpt(tempInt);
			}

			else if (key.compare("*Cooler status") == 0) {
				success = true;
				if (value.compare("On") == 0){
					setCoolerStat(ANDOR_ON);
				} 

				else if (value.compare("Off") == 0) {
					setCoolerStat(ANDOR_OFF);
				}
				else {
					throw ANDOR885_Exception("Device: Unerecognized Cooler Status requested");
				}
			}
			else if (key.compare("Camera temperature") == 0){
				success = true;
				//This doesn't really get actively set to a value
			} 

			else if (key.compare("Save mode") == 0){
				success = true;
				if (value.compare("On") == 0) {
					setSaveMode(ANDOR_ON);
				}
				else if (value.compare("Off") == 0) {
					setSaveMode(ANDOR_OFF);
				}
				else {
					throw ANDOR885_Exception("Device: Unrecognized Save Mode requested");
				}
			}
			else if (key.compare(preAmpGain_t.name)==0){
				success = true;
				setPreAmpGain(preAmpGain_t.inverseFind(value));
			}
			else if(key.compare("Num exp per file") == 0 && successInt) {
				success = true;
				setNumPerFile(tempInt);
			}
			else if(key.compare(verticalShiftSpeed_t.name) == 0) {
				success = true;
				setVerticalShiftSpeed(verticalShiftSpeed_t.inverseFind(value));
			}
			else if(key.compare(verticalClockVoltage_t.name) == 0) {
				success = true;
				setVerticalClockVoltage(verticalClockVoltage_t.inverseFind(value));
			}
			else if(key.compare(horizontalShiftSpeed_t.name) == 0) {
				success = true;
				setHorizontalShiftSpeed(horizontalShiftSpeed_t.inverseFind(value));
			}

		}
		else {
			throw ANDOR885_Exception("Stop acquisition before changing the parameters");
		}
	} catch (ANDOR885_Exception& e){
		std::cerr << e.printMessage() << std::endl;
		success = false;
	}
	
	return success;
}



void ANDOR885_Device::defineChannels()
{
	//this->add
	addInputChannel(0, DataString, ValueVector);
}

std::string ANDOR885_Device::execute(int argc, char **argv)
{
	return "";
}

void ANDOR885_Device::definePartnerDevices()
{
	addPartnerDevice("Digital Board", "ep-timing1.stanford.edu", 2, "Digital Out");
	partnerDevice("Digital Board").enablePartnerEvents();
}


void ANDOR885_Device::parseDeviceEvents(const RawEventMap &eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception)
{ 
	double digitalMinAbsStartTime = 10000; //Must be same as DigitalOut min start time
	double minimumAbsoluteStartTime = digitalMinAbsStartTime;
	double startTimeBuffer = 1000000000; //1000 ms in nanoseconds - buffer against any errors in shutter opening time
	double prepEventTime; //time when the FPGA should trigger in order to have the output ready in time
	double eventTime;
	double previousTime; //time when the previous event occurred
	double previousExposureTime;
	double ns = 1000000000; // Conversion factor from seconds to ns
	double ms2ns = 1000000; // Conversion factor from ms to ns
	double endTimeBuffer = 1000000; // 1 ms in ns - buffer after last exposure ends before the clean-up event starts.

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator previousEvents;

	Andor885Event* andor885Event;
	Andor885Event* andor885InitEvent;
	Andor885Event* andor885EndEvent;

	//For saving pictures, I need the metadata encoded in the event
	EventMetadatum eventMetadatum;
	eventMetadata.clear();

	int exposureIndex;


	//Minimum absolute start time depends on opening time of camera shutter
	if (getShutterMode() != SHUTTERMODE_OPEN)
	{
		minimumAbsoluteStartTime += getOpenTime() * ms2ns;
	}
	minimumAbsoluteStartTime += startTimeBuffer;

	for(events = eventsIn.begin(), exposureIndex = 0; events != eventsIn.end(); events++, exposureIndex++)
	{	
		if (events == eventsIn.begin())
		{
			//Make sure digital line is initialized
			partnerDevice("Digital Board").event(digitalMinAbsStartTime, digitalChannel, 0, events->second.at(0));

			//Small hold-off to make sure initialization even occurs after digital line is low
			andor885InitEvent = new Andor885Event(digitalMinAbsStartTime, this, INIT_EVENT);
			andor885InitEvent->exposureIndex = -42;
			eventsOut.push_back( andor885InitEvent );
		}

		eventTime = events->first;

		if (events->second.at(0).getValueType() == MixedValue::Vector)
		{
			//Doesn't this create a new sizeOfTuple at each iteration?
			unsigned sizeOfTuple = events->second.at(0).value().getVector().size();

			const std::vector <MixedValue>& eVector = events->second.at(0).value().getVector();

			//Check that each type in tuple is correct. The first two switch statements
			// are deliberately un-break'd.
			switch(sizeOfTuple)
			{
			case 3:
				if(eVector.at(2).getType() != MixedValue::String)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera image description must be a string");
				}
			case 2:
				if(eVector.at(1).getType() != MixedValue::String)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera filename must be a string");
				}
			case 1:
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera exposure time must be a double");
				}
				break;

			default:
				throw EventParsingException(events->second.at(0),
					"Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}

/*
			switch(sizeOfTuple)
			{
			case 3:
				if(eVector.at(2).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera number of exposures per file must be an integer.");
				}
			case 2:
				if(eVector.at(1).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera number of exposures must be an integer.");
				}
			case 1:
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"Andor camera exposure time must be a double.");
				}
				break;

			default:
				throw EventParsingException(events->second.at(0),
					"Andor camera commands must be a tuple in the form (double exposureTime, int numExposures, int numExpPerFile)");
				break;
			}
			*/

			switch(sizeOfTuple)
			{
			case 3:
				andor885Event = new Andor885Event(eventTime, this, 
					eVector.at(0).getDouble(), eVector.at(1).getString(), eVector.at(2).getString());
				eventMetadatum.assign(eVector.at(0).getDouble(), eVector.at(1).getString(), eVector.at(2).getString());
				break;
			case 2:
				andor885Event = new Andor885Event(eventTime, this, 
					eVector.at(0).getDouble(), eVector.at(1).getString());
				eventMetadatum.assign(eVector.at(0).getDouble(), eVector.at(1).getString());
				break;
			case 1:
				andor885Event = new Andor885Event(eventTime, this, 
					eVector.at(0).getDouble());
				eventMetadatum.assign(eVector.at(0).getDouble());
				break;
				
			default:
				throw EventParsingException(events->second.at(0), "Never should get here, but Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}

/*			switch(sizeOfTuple)
			{
			case 3:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = eVector.at(1).getDouble();
				andor885Event->eventNumExpPerFile = eVector.at(2).getDouble();
				break;
			case 2:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = eVector.at(1).getDouble();
				andor885Event->eventNumExpPerFile = numPerFile;
				break;
			case 1:
				andor885Event->eventExposureTime = eVector.at(0).getDouble();
				andor885Event->eventNumExposures = numExposures;
				andor885Event->eventNumExpPerFile = numPerFile;
				break;
				
			default:
				delete andor885Event;
				throw EventParsingException(events->second.at(0), "Never should get here, but Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}
*/
			
		// Check that the camera can keep up with the events
			if(events != eventsIn.begin())
			{
				previousEvents = --events;
				events++;
				previousTime = previousEvents->first;
				previousExposureTime = previousEvents->second.at(0).value().getVector().at(0).getDouble();
			}
			else
			{
				previousEvents = events;
				previousTime = minimumAbsoluteStartTime;
				previousExposureTime = 0;
			}
			
			// The kinetic time gets set whenever the exposure time is changed.
			// it depends on the vertical and horizontal shift speeds, and adds on the exposure time
			prepEventTime = eventTime - (getKineticTime() * ns + previousExposureTime) * events->second.size();

			if( prepEventTime < previousTime  && events != eventsIn.begin())
		//	if (eventTime - previousTime < holdoff)
			{
				delete andor885Event;
				throw EventConflictException(previousEvents->second.at(0), 
					events->second.at(0), 
					"Given vertical and horizontal shift speeds, the camera cannot take pictures faster than" + valueToString(getKineticTime()) + " s, plus the exposure time" );
			}
			else if (prepEventTime < previousTime)
			{
				delete andor885Event;
				throw EventConflictException(previousEvents->second.at(0), 
					events->second.at(0), 
					"The camera must have a " + valueToString(minimumAbsoluteStartTime/ns) + " s buffer before the first image." );
			}
			
			andor885Event->exposureIndex = exposureIndex;

			sendDigitalLineExposureEvents(eventTime, events->second.at(0), andor885Event->exposureTime);
			eventsOut.push_back( andor885Event );

			//Add measurement
			eventsOut.back().addMeasurement( events->second.at(0) );

			//Store event metadata for saving purposes
			eventMetadata.push_back(eventMetadatum);

			previousTime = eventTime;
			events++;
			if (events == eventsIn.end())
			{
				andor885EndEvent = new Andor885Event(previousTime + eVector.at(0).getDouble() + endTimeBuffer, this, END_EVENT);
				andor885EndEvent->exposureIndex = -42;
				eventsOut.push_back( andor885EndEvent );
			}
			events--;
		}
		else
		{
			std::cerr << "The Andor camera does not support that data type" << std::endl;
			throw EventParsingException(events->second.at(0),
						"The Andor camera does not support that data type.");
		}
		
	}

	numEventExposures = eventsIn.size();
	//Assuming no other errors, record the number of exposures

}

void ANDOR885_Device::sendDigitalLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime)
{
	partnerDevice("Digital Board").event(eventTime, 
					digitalChannel, 1, evt);
	partnerDevice("Digital Board").event(eventTime + exposureTime, 
					digitalChannel, 0, evt);
}

void ANDOR885_Device::stopEventPlayback()
{
	eventStatMutex->lock();
		if (eventStat == ANDOR_ON)
		{
			eventStat = ANDOR_OFF;
			CancelWait();
			AbortAcquisition();
		}
	eventStatMutex->unlock();
}


void ANDOR885_Device::Andor885Event::playEvent()
{
	std::string fn;
	/* ANDORdevice_->exposureTime = eventExposureTime;
	ANDORdevice_->numExposures = eventNumExposures;
	ANDORdevice_->numPerFile = eventNumExpPerFile;

	ANDORdevice_->refreshAttributes();

	ANDORdevice_->acquisitionStat = ON;

	ANDORdevice_->refreshAttributes();
*/


	//For initialization event, setup camera for acquisition
	if (exposureTime == INIT_EVENT)
	{
		std::cout << "Starting setup" << std::endl;
		ANDORdevice_->setupEventAcquisition(ANDORdevice_->numEventExposures);
		std::cout << "Finished setup" << std::endl;
	}
	else if (exposureTime == END_EVENT)
	{
		std::cout << "Starting cleanup" << std::endl;
		ANDORdevice_->cleanupEventAcquisition();
		std::cout << "Finished cleanup" << std::endl;
	}
	else
	{
		//Add timestamp to device and camera's copy of the filename
		filename = ANDORdevice_->timeStampFilename(filenameBase);
		ANDORdevice_->eventMetadata.at(exposureIndex).filename = 
			filename;
		filename = filename + ANDORdevice_->extension;
	}

}

void ANDOR885_Device::Andor885Event::waitBeforeCollectData()
{
	std::cout << "Waiting for waitBeforeCollectData" << std::endl;
	std::cout << "Exposure Index: " << exposureIndex <<std::endl;
	ANDORdevice_->numAcquiredMutex->lock();

	while (ANDORdevice_->numAcquired <= exposureIndex && exposureTime > 0)
	{
		ANDORdevice_->numAcquiredCondition->wait();
	}

	ANDORdevice_->numAcquiredMutex->unlock();
	std::cout << "Done waiting for waitBeforeCollectData" << std::endl;
}

void ANDOR885_Device::Andor885Event::collectMeasurementData()
{
	if (eventMeasurements.size() == 1)
	{
		eventMeasurements.at(0)->setData(filename);
	}
	else if (exposureTime < 0)
	{
		//do nothing;
	}
	else 
	{
		std::cerr << "Error in collectMeasurementData: expect only one measurement. received " 
			<< eventMeasurements.size() << std::endl;
	}
}
