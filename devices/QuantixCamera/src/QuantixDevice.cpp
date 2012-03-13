/*! \file QuantixDevice.cpp
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

#include "QuantixDevice.h"


QuantixDevice::QuantixDevice(
		ORBManager* orb_manager, 
		std::string DeviceName, 
		std::string Address,
		unsigned short ModuleNumber, int16 handle) : QuantixCamera(handle),
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	digitalChannel = 0;
	slowAnalogChannel = 0;
	minimumAbsoluteStartTime = 1000000; //1 ms buffer before any pictures can be taken

	cameraTriggerDevice = SlowAnalogBoard;

/*	cameraAttributes.push_back(readoutSpeed_t);
	cameraAttributes.push_back(clearMode_t);
	cameraAttributes.push_back(clockingMode_t);
	cameraAttributes.push_back(gain_t);
	cameraAttributes.push_back(shutterMode_t);
	cameraAttributes.push_back(triggerMode_t);*/

	initialized = true;  //Will be true to start device
}

void QuantixDevice::printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}

QuantixDevice::~QuantixDevice()
{
	
}

bool QuantixDevice::deviceMain(int argc, char **argv)
{
	bool error = false;

	std::cout << "Main OK." << std::endl;

	return false;
}


void QuantixDevice::defineAttributes()
{
//	std::vector <QuantixCamera::CameraAttribute> cameraAttribute;
//	cameraAttribute.push_back(clockingModeC);
//	cameraAttribute.push_back(shutterModeC);

	//Attributes
	addAttribute("*Folder Path for saved files", getFilePath());
//	addAttribute("*Cooler setpoint", getCoolerSetpt());
	addAttribute("Trigger Select", "Slow Analog Out", "Slow Analog Out, Digital Out");

//	addAttribute("*Camera Temperature", getCameraTemp());


	std::vector <QuantixState::CameraAttribute*>::iterator it;
	for (it = cameraState->guiAttributes.begin(); 
		it != cameraState->guiAttributes.end(); it++)
	{
		if ((*it)->choices.empty())
			addAttribute((*it)->name,cameraState->get(*(*it)));
		else
			addAttribute((*it)->name, (*it)->getLabel(), makeAttributeString((*(*it)).choices));
	}

/*
	if (!clearMode_t->choices.empty())
		addAttribute(clearMode_t->name, clearMode_t->currentString, makeAttributeString(clearMode_t->choices)); //Clear Mode
	if (!clockingMode_t->choices.empty())
		addAttribute(clockingMode_t->name, clockingMode_t->currentString, makeAttributeString(clockingMode_t->choices)); //Clocking Mode (FT or MPP?)
	if (!gain_t->choices.empty())
		addAttribute(gain_t->name, gain_t->currentString, makeAttributeString(gain_t->choices)); //PreAmp gain
	if (!readoutSpeed_t->choices.empty())
		addAttribute(readoutSpeed_t->name, readoutSpeed_t->currentString, makeAttributeString(readoutSpeed_t->choices)); //Readout Speed
	if (!shutterMode_t->choices.empty())
		addAttribute(shutterMode_t->name, shutterMode_t->currentString, makeAttributeString(shutterMode_t->choices)); // Shutter control
	if (!triggerMode_t->choices.empty())
		addAttribute(triggerMode_t->name, triggerMode_t->currentString, makeAttributeString(triggerMode_t->choices)); //Trigger Mode
		// Clear cycles attribute?
	*/
}

void QuantixDevice::refreshAttributes()
{
	bool success;
	MixedValue inVector;
	MixedData outString;
	std::string tempString;

	setAttribute("*Folder Path for saved files", getFilePath());
//	setAttribute("*Cooler setpoint", getCoolerSetpt());
	setAttribute("Trigger Select", (cameraTriggerDevice == SlowAnalogBoard) ? "Slow Analog Out" : "Digital Out");

	std::vector <QuantixState::CameraAttribute*>::iterator it;
	for (it = cameraState->guiAttributes.begin(); 
		it != cameraState->guiAttributes.end(); it++)
	{
		if ((*it)->choices.empty())
			setAttribute((*it)->name,cameraState->get(*(*it)));
		else
			setAttribute((*it)->name, (*it)->getLabel());
	}


/*	try
	{
		addAttribute("*Camera Temperature", getCameraTemp());
	}
	catch (CameraException &e)
	{
		std::cerr << "Refresh attributes error: " << e.what() << std::endl;
		setAttribute("Camera temperature", -999);
	}*/

/*	setAttribute(clearMode_t->name, clearMode_t->getString());
	setAttribute(clockingMode_t->name, clockingMode_t->getString());
	setAttribute(gain_t->name, gain_t->getString());
	setAttribute(readoutSpeed_t->name, readoutSpeed_t->getString());
	setAttribute(shutterMode_t->name, shutterMode_t->getString());
	setAttribute(triggerMode_t->name, triggerMode_t->getString());*/


}

bool QuantixDevice::updateAttribute(std::string key, std::string value)
{

	double tempDouble;
	int tempInt;
//	char tempChar[MAX_PATH];
	std::string tempString;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = false;

	if(key.compare("*Folder Path for saved files") == 0) {
		success = true;
		setFilePath(value);
	}
	else if (key.compare("Trigger Select") == 0) {
		success = true;
		if (value.compare("Digital Out") == 0){
			cameraTriggerDevice = DigitalBoard;
			//partnerDevice("Slow Analog Out").disablePartnerEvents();
		} 

		else if (value.compare("Slow Analog Out") == 0) {
			cameraTriggerDevice = SlowAnalogBoard;
			//partnerDevice("Digital Out").disablePartnerEvents();
		}
		else {
			success = false;
		}
	}

	std::vector <QuantixState::CameraAttribute*>::iterator it;
	for (it = cameraState->guiAttributes.begin(); 
		it != cameraState->guiAttributes.end(); it++)
	{
		if ((*it)->name.compare(key) == 0)
		{
			try 
			{
				cameraState->set(*(*it), value);
			}
			catch (CameraException &e)
			{
				std::cerr << "Error updating attribute: " << e.what() << std::endl;
				success = false;
			}
			success = true;
			break;
		}
	}

	
	return success;  //if none match, this will be false
}



void QuantixDevice::defineChannels()
{
	//this->add
	//addInputChannel(0, DataString, ValueVector);
}
bool QuantixDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut) 
{
	//return readChannelDefault(channel, valueIn, dataOut, minimumAbsoluteStartTime + 10);
	return true;
}
std::string QuantixDevice::execute(int argc, char **argv)
{
	return "";
}

void QuantixDevice::definePartnerDevices()
{
/*	addPartnerDevice("Digital Out", "ep-timing1.stanford.edu", 2, "Digital Out");
	//partnerDevice("Digital Out").enablePartnerEvents();
	
	addPartnerDevice("Slow Analog Out", "ep-timing1.stanford.edu", 4, "Slow Analog Out");
	partnerDevice("Slow Analog Out").enablePartnerEvents();*/

}


void QuantixDevice::parseDeviceEvents(const RawEventMap &eventsIn, 
		SynchronousEventVector& eventsOut) throw(std::exception)
{ 
	double digitalMinAbsStartTime = 10000;
	double startTimeBuffer = minimumAbsoluteStartTime;
	double prepEventTime; //time when the FPGA should trigger in order to have the output ready in time
	double eventTime;
	double previousTime; //time when the previous event occurred
	double previousExposureTime;
	double ns = 1000000000; // Conversion factor from seconds to ns
	double ms2ns = 1000000; // Conversion factor from ms to ns
	double endTimeBuffer = 1000; // 1 us in ns - buffer after last exposure ends before the clean-up event starts.

	/*RawEventMap::const_iterator events;
	RawEventMap::const_iterator previousEvents;

	Andor885Event* andor885Event;
	Andor885Event* andor885InitEvent;
	Andor885Event* andor885EndEvent;

	//For saving pictures, I need the metadata encoded in the event
	EventMetadatum eventMetadatum;

	//For the image crop vector, I need a vector of ints and a string for error messages
	std::vector <int> cropVector;
	std::string cropVectorMessage;

	//Minimum absolute start time depends on opening time of camera shutter
	if (getShutterMode() != SHUTTERMODE_OPEN)
	{
		startTimeBuffer += getOpenTime() * ms2ns;
	}

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{	

		// Create initialization event
		if (events == eventsIn.begin())
		{
			
			//Make sure trigger line is initialized
			if(cameraTriggerDevice == DigitalBoard)
			{
				partnerDevice("Digital Out").event(digitalMinAbsStartTime, digitalChannel, 0, events->second.at(0));
			}
			if(cameraTriggerDevice == SlowAnalogBoard)
			{
				partnerDevice("Slow Analog Out").event(digitalMinAbsStartTime, slowAnalogChannel, 0, events->second.at(0));
			}

			//Small hold-off to make sure initialization even occurs after digital line is low
			andor885InitEvent = new Andor885Event(digitalMinAbsStartTime, this);
			andor885InitEvent->eventMetadatum.assign(INIT_EVENT);
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
			case 4:
				if(eVector.at(3).getType() != MixedValue::Vector)
				{
					delete andor885InitEvent;
					throw EventParsingException(events->second.at(0),
						"Andor camera crop vector must be a vector");
				}
				

				//Check to see if crop vector has the right form
				cropVectorMessage = testCropVector(eVector.at(3).getVector(), cropVector);
				if (cropVector.empty())
				{
					delete andor885InitEvent;
					throw EventParsingException(events->second.at(0), cropVectorMessage);
				}
				else if (cropVectorMessage.compare("") != 0)
				{
					//requested crop probably got clipped. Print the warning(s)
					std::cout << cropVectorMessage << std::endl;
				}

			case 3:
				if(eVector.at(2).getType() != MixedValue::String)
				{
					delete andor885InitEvent;
					throw EventParsingException(events->second.at(0),
						"Andor camera image description must be a string");
				}
			case 2:
				if(eVector.at(1).getType() != MixedValue::String)
				{
					delete andor885InitEvent;
					throw EventParsingException(events->second.at(0),
						"Andor camera filename must be a string");
				}
			case 1:
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					delete andor885InitEvent;
					throw EventParsingException(events->second.at(0),
						"Andor camera exposure time must be a double");
				}
				break;

			default:
				delete andor885InitEvent;
				throw EventParsingException(events->second.at(0),
					"Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename, vector cropVector). The crop vector can be of the form (int pixelULX, int pixelULY, int fullWidthX, int fullWidthY) or (int centerPixelX, int centerPixelY, int halfWidth)." );
				break;
			}

			//Check that the exposure time is not too short
			if (eVector.at(0).getDouble() < 10000)
			{
				delete andor885InitEvent;
				throw EventParsingException(events->second.at(0), "Andor camera requires an exposure time of greater than 10 us");
			}


			switch(sizeOfTuple)
			{
			case 4:
				andor885Event = new Andor885Event(eventTime, this);
				andor885Event->eventMetadatum.assign(eVector.at(0).getDouble(), eVector.at(1).getString(), eVector.at(2).getString(), cropVector);
				break;
			case 3:
				andor885Event = new Andor885Event(eventTime, this);
				andor885Event->eventMetadatum.assign(eVector.at(0).getDouble(), eVector.at(1).getString(), eVector.at(2).getString());
				break;
			case 2:
				andor885Event = new Andor885Event(eventTime, this);
				andor885Event->eventMetadatum.assign(eVector.at(0).getDouble(), eVector.at(1).getString());
				break;
			case 1:
				andor885Event = new Andor885Event(eventTime, this);
				andor885Event->eventMetadatum.assign(eVector.at(0).getDouble());
				break;
				
			default:
				delete andor885InitEvent;
				throw EventParsingException(events->second.at(0), "Never should get here, but Andor camera commands must be a tuple in the form (double exposureTime, string description, string filename)");
				break;
			}

			//Set the base filename
			andor885Event->filenameBase = andor885Event->eventMetadatum.filename;

			//Push back the metadatum onto the init_event's vector
			andor885InitEvent->eventMetadata.push_back(andor885Event->eventMetadatum);
			
		// Check that the camera can keep up with the events
			if(events != eventsIn.begin())
			{
				previousEvents = --events;
				events++;
				previousTime = previousEvents->first;
				previousExposureTime = previousEvents->second.at(0).value().getVector().at(0).getDouble();
				// The kinetic time gets set whenever the exposure time is changed.
				// it depends on the vertical and horizontal shift speeds, and adds on the exposure time
				prepEventTime = eventTime - (getKineticTime() * ns + previousExposureTime) * events->second.size();
			}
			else
			{
				previousEvents = events;
				previousTime = startTimeBuffer;
				prepEventTime = eventTime;
			}

			if( prepEventTime < previousTime  && events != eventsIn.begin())
			{
				delete andor885InitEvent;
				delete andor885Event;
				throw EventConflictException(previousEvents->second.at(0), 
					events->second.at(0), 
					"Given vertical and horizontal shift speeds, the camera cannot take pictures faster than" + valueToString(getKineticTime()) + " s, plus the exposure time" );
			}
			else if (prepEventTime < previousTime)
			{
				delete andor885InitEvent;
				delete andor885Event;
				throw EventConflictException(previousEvents->second.at(0), 
					events->second.at(0), 
					"The camera must have a " + valueToString(startTimeBuffer/ns) + " s buffer before the first image." );
			}

			if(cameraTriggerDevice == DigitalBoard)
			{
				sendDigitalLineExposureEvents(eventTime, events->second.at(0), andor885Event->eventMetadatum.exposureTime);
			}
			else if(cameraTriggerDevice == SlowAnalogBoard)
			{
				sendSlowAnalogLineExposureEvents(eventTime, events->second.at(0), andor885Event->eventMetadatum.exposureTime);
			}

			eventsOut.push_back( andor885Event );

			//Add measurement
			eventsOut.back().addMeasurement( events->second.at(0) );

			previousTime = eventTime;
		}
		else
		{
			delete andor885InitEvent;
			std::cerr << "The Andor camera does not support that data type" << std::endl;
			throw EventParsingException(events->second.at(0),
						"The Andor camera does not support that data type.");
		}
		
	}

	//create end event
	events--;
	if (!eventsIn.empty()) {
		andor885EndEvent = new Andor885Event(events->first + events->second.at(0).value().getVector().at(0).getDouble() + endTimeBuffer, this);
		andor885EndEvent->eventMetadatum.assign(END_EVENT);
		eventsOut.push_back( andor885EndEvent );

		//insert initalization event
		eventsOut.insert(eventsOut.begin(), andor885InitEvent);
	}*/

}
std::string QuantixDevice::testCropVector(const MixedValueVector& cropVectorIn, std::vector <int>& cropVectorOut)
{
	int i;
	std::string tempString = "";
	/*********************************************************************/
//	int imageWidth = getImageWidth();		HACK HACK HACK HACK HACK HACK
//	int imageHeight = getImageHeight();		HACK HACK HACK HACK HACK HACK
	/*********************************************************************/
	int imageWidth = 1000;
	int imageHeight = 1000;
	int tempCenterX = 1;
	int tempCenterY = 1;
	int tempHalfWidth = 99;

	int validStartPixelX = 1;
	int validStartPixelY = 1;

	cropVectorOut.clear();

	if (cropVectorIn.size() != 4 && cropVectorIn.size() !=  3)
	{
		return ("Andor camera crop vector must have a length of 3 or 4: (int pixelULX, int pixelULY, int fullWidthX, int fullWidthY) or (int centerPixelX, int centerPixelY, int halfWidth)");
	}
	for (i = 0; i < (signed) cropVectorIn.size(); i++)
	{
		if (cropVectorIn.at(i).getType() != MixedValue::Double)
		{
			cropVectorOut.clear();
			return ("Andor camera crop vector element must be an Int.");
		}
		if (cropVectorIn.at(i).getDouble() < 1)
		{
			cropVectorOut.clear();
			return ("Andor camera crop vector elements must be > 1.");
		}
		cropVectorOut.push_back((int) cropVectorIn.at(i).getDouble());
	}

	// convert center-referenced vector to a corner vector
	if (cropVectorIn.size() == 3) {
		tempCenterX = cropVectorOut.at(0);
		tempCenterY = cropVectorOut.at(1);
		tempHalfWidth = cropVectorOut.at(2);
		
		cropVectorOut.clear();

		cropVectorOut.push_back(tempCenterX - tempHalfWidth);
		cropVectorOut.push_back(tempCenterY - tempHalfWidth);
		cropVectorOut.push_back(tempHalfWidth * 2 + 1);
		cropVectorOut.push_back(tempHalfWidth * 2 + 1);
	}

	//check input; clip if necessary
	i = cropVectorOut.at(0);
	if(i < validStartPixelX)
	{
		tempString += "Crop vector starts at an invalid x-pixel: " + 
			STI::Utils::valueToString(i) + " < " + 
			STI::Utils::valueToString(validStartPixelX) + ". Clipping crop vector...  ";
		
		cropVectorOut.at(0) = validStartPixelX;
	}

	i = cropVectorOut.at(1);
	if(i < validStartPixelY)
	{
		tempString += "Crop vector starts at an invalid y-pixel: " + 
			STI::Utils::valueToString(i) + " < " + 
			STI::Utils::valueToString(validStartPixelY) + ". Clipping crop vector...  ";
		
		cropVectorOut.at(1) = validStartPixelY;
	}

	i = cropVectorOut.at(0) + cropVectorOut.at(2) - 1;
	if (i > imageWidth) {
		tempString += "Crop vector exceeds image width: " + 
				STI::Utils::valueToString(i) + " > " + 
				STI::Utils::valueToString(imageWidth) + ". Clipping crop vector...  ";

		cropVectorOut.at(2) -= i - imageWidth;
	}
	

	i = cropVectorOut.at(1) + cropVectorOut.at(3) - 1;
	if (i > imageHeight) 
	{
		tempString += "Crop vector exceeds image height: " + 
			STI::Utils::valueToString(i) + " > " + 
			STI::Utils::valueToString(imageHeight) + ". Clipping crop vector...  ";

		cropVectorOut.at(3) -= i - imageHeight;
	}

	// convert user-friendly pixel definitions to computer-friendly ones
	cropVectorOut.at(0) -= 1; // now referenced from 0
	cropVectorOut.at(1) -= 1; // now referenced from 0
	cropVectorOut.at(2) -= 1; // now adding to 0 yeilds location of last pixel
	cropVectorOut.at(3) -= 1; // now adding to 1 yeilds location of last pixel

	return (tempString);
}
void QuantixDevice::sendDigitalLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime)
{
	/*partnerDevice("Digital Out").event(eventTime, 
					digitalChannel, 1, evt);
	partnerDevice("Digital Out").event(eventTime + exposureTime, 
					digitalChannel, 0, evt);*/
}
void QuantixDevice::sendSlowAnalogLineExposureEvents(double eventTime, const RawEvent& evt, double exposureTime)
{
	/*partnerDevice("Slow Analog Out").event(eventTime, 
					slowAnalogChannel, 5, evt);	//5 volts
	partnerDevice("Slow Analog Out").event(eventTime + exposureTime, 
					slowAnalogChannel, 0, evt);*/
}

void QuantixDevice::stopEventPlayback()
{
	/*stopEventMutex->lock();
		stopEvent = true;
		waitForCleanupEventMutex->lock();
			cleanupEvent = true;
		waitForCleanupEventMutex->unlock();
		AbortIfAcquiring();
	stopEventMutex->unlock();

	numAcquiredMutex->lock();
		numAcquiredCondition->broadcast();
	numAcquiredMutex->unlock();*/
}


void QuantixDevice::QuantixEvent::playEvent()
{
	/*std::string fn;

	//For initialization event, setup camera for acquisition
	if (eventMetadatum.exposureTime == INIT_EVENT)
	{
		std::cout << "Starting setup" << std::endl;
		ANDORdevice_->setupEventAcquisition(&eventMetadata);
		std::cout << "Finished setup" << std::endl;
	}
	else if (eventMetadatum.exposureTime == END_EVENT)
	{
		std::cout << "Starting cleanup" << std::endl;
		ANDORdevice_->cleanupEventAcquisition();
		std::cout << "Finished cleanup" << std::endl;
	}
	else
	{
		//Add timestamp to device and camera's copy of the filename
		ANDORdevice_->stopEventMutex->lock();
		if (!(ANDORdevice_->stopEvent) || 
			((ANDORdevice_->stopEvent) && (ANDORdevice_->numAcquired >= (signed) getEventNumber()))) {
			eventMetadatum.filename = ANDORdevice_->timeStampFilename(filenameBase);
			ANDORdevice_->eventMetadata->at((signed) getEventNumber() - 1).filename = 
				eventMetadatum.filename;
			eventMetadatum.filename = eventMetadatum.filename + ANDORdevice_->extension;
		} else {
			eventMetadatum.filename = ANDORdevice_->timeStampFilename(filenameBase + " should not have been saved");
			ANDORdevice_->eventMetadata->at((signed) getEventNumber() - 1).filename = 
				eventMetadatum.filename;
			eventMetadatum.filename = eventMetadatum.filename + ANDORdevice_->extension;
		}
		ANDORdevice_->stopEventMutex->unlock();
	}*/

}

void QuantixDevice::QuantixEvent::waitBeforeCollectData()
{
	/*std::cout << "Waiting for waitBeforeCollectData" << std::endl;
	ANDORdevice_->numAcquiredMutex->lock();
		while (ANDORdevice_->numAcquired < (signed) getEventNumber() && eventMetadatum.exposureTime > 0 && !(ANDORdevice_->stopEvent))
		{
			ANDORdevice_->numAcquiredCondition->wait();
		}
	ANDORdevice_->numAcquiredMutex->unlock();
	std::cout << "Done waiting for waitBeforeCollectData" << std::endl;*/
}

void QuantixDevice::QuantixEvent::collectMeasurementData()
{
	/*if (eventMeasurements.size() == 1 && !(ANDORdevice_->stopEvent))
	{
		eventMeasurements.at(0)->setData(eventMetadatum.filename);
	}
	else if (eventMetadatum.exposureTime < 0)
	{
		//do nothing; Not a measurement event;
	}
	else 
	{
		if(!(ANDORdevice_->stopEvent)) {
		std::cerr << "Error in collectMeasurementData: expect only one measurement. Received " 
			<< eventMeasurements.size() << std::endl;
		}
		else {
			std::cerr << "Stopped collecting data" << std::endl;
		}
	}*/
}


std::string QuantixDevice::makeAttributeString(std::map<std::string, std::string> &choices)
{
	std::string tempString = "";
	std::string filler = ", ";
	std::map<std::string,std::string>::iterator iter;

	for(iter = choices.begin(); iter != choices.end(); iter++){
		tempString += iter->second;
		iter++;
		if (iter != choices.end()){
			tempString += filler;
		}
		iter--;
	}

	return tempString;
}

/*std::string QuantixDevice::makeAttributeString(std::map<int, std::string> &choices)
{
	std::string tempString = "";
	std::string filler = ", ";
	std::map<int,std::string>::iterator iter;

	for(iter = choices.begin(); iter != choices.end(); iter++){
		tempString += iter->second;
		iter++;
		if (iter != choices.end()){
			tempString += filler;
		}
		iter--;
	}

	return tempString;
}*/

/*
void QuantixDevice::printParamAvailability()
{
	uns32 parameters[] = {PARAM_DD_INFO_LENGTH, PARAM_DD_VERSION,PARAM_DD_RETRIES,PARAM_DD_TIMEOUT,
	PARAM_DD_INFO,PARAM_MIN_BLOCK,PARAM_NUM_MIN_BLOCK,PARAM_SKIP_AT_ONCE_BLK,PARAM_NUM_OF_STRIPS_PER_CLR,
	PARAM_CONT_CLEARS,PARAM_ANTI_BLOOMING,PARAM_LOGIC_OUTPUT,PARAM_EDGE_TRIGGER,PARAM_INTENSIFIER_GAIN,
	PARAM_SHTR_GATE_MODE,PARAM_ADC_OFFSET,PARAM_CHIP_NAME,PARAM_COOLING_MODE,PARAM_PREAMP_DELAY,
	PARAM_PREFLASH,PARAM_COLOR_MODE,PARAM_MPP_CAPABLE,PARAM_PREAMP_OFF_CONTROL,PARAM_SERIAL_NUM,PARAM_PREMASK,
	PARAM_PRESCAN,PARAM_POSTMASK,PARAM_POSTSCAN,PARAM_PIX_PAR_DIST,PARAM_PIX_PAR_SIZE,PARAM_PIX_SER_DIST,
	PARAM_PIX_SER_SIZE,PARAM_SUMMING_WELL,PARAM_FWELL_CAPACITY,PARAM_PAR_SIZE,PARAM_SER_SIZE,PARAM_ACCUM_CAPABLE,
	PARAM_FLASH_DWNLD_CAPABLE,PARAM_CONTROLLER_ALIVE,PARAM_READOUT_TIME,PARAM_CLEAR_CYCLES,PARAM_CLEAR_MODE,
	PARAM_FRAME_CAPABLE,PARAM_PMODE,PARAM_CCS_STATUS,PARAM_TEMP,PARAM_TEMP_SETPOINT,PARAM_CAM_FW_VERSION,
	PARAM_HEAD_SER_NUM_ALPHA,PARAM_PCI_FW_VERSION,PARAM_CAM_FW_FULL_VERSION,PARAM_EXPOSURE_MODE,PARAM_BIT_DEPTH,
	PARAM_GAIN_INDEX,PARAM_SPDTAB_INDEX,PARAM_READOUT_PORT,PARAM_PIX_TIME,PARAM_SHTR_CLOSE_DELAY,PARAM_SHTR_OPEN_DELAY,
	PARAM_SHTR_OPEN_MODE,PARAM_SHTR_STATUS,PARAM_SHTR_CLOSE_DELAY_UNIT,PARAM_IO_ADDR,PARAM_IO_TYPE,PARAM_IO_DIRECTION,
	PARAM_IO_STATE,PARAM_IO_BITDEPTH,PARAM_GAIN_MULT_FACTOR,PARAM_GAIN_MULT_ENABLE,PARAM_PP_FEAT_NAME,
	PARAM_PP_INDEX,PARAM_ACTUAL_GAIN,PARAM_PP_PARAM_INDEX,PARAM_PP_PARAM_NAME,PARAM_PP_PARAM,PARAM_READ_NOISE,
	PARAM_PP_FEAT_ID,PARAM_PP_PARAM_ID,PARAM_EXP_TIME,PARAM_EXP_RES,PARAM_EXP_MIN_TIME,PARAM_EXP_RES_INDEX,
	PARAM_BOF_EOF_ENABLE,PARAM_BOF_EOF_COUNT,PARAM_BOF_EOF_CLR,PARAM_CIRC_BUFFER,PARAM_HW_AUTOSTOP};

	char *paramNames[] = {"PARAM_DD_INFO_LENGTH", "PARAM_DD_VERSION", "PARAM_DD_RETRIES", "PARAM_DD_TIMEOUT",
	"PARAM_DD_INFO","PARAM_MIN_BLOCK","PARAM_NUM_MIN_BLOCK","PARAM_SKIP_AT_ONCE_BLK","PARAM_NUM_OF_STRIPS_PER_CLR",
	"PARAM_CONT_CLEARS","PARAM_ANTI_BLOOMING","PARAM_LOGIC_OUTPUT","PARAM_EDGE_TRIGGER","PARAM_INTENSIFIER_GAIN",
	"PARAM_SHTR_GATE_MODE","PARAM_ADC_OFFSET","PARAM_CHIP_NAME","PARAM_COOLING_MODE","PARAM_PREAMP_DELAY",
	"PARAM_PREFLASH","PARAM_COLOR_MODE","PARAM_MPP_CAPABLE","PARAM_PREAMP_OFF_CONTROL","PARAM_SERIAL_NUM","PARAM_PREMASK",
	"PARAM_PRESCAN","PARAM_POSTMASK","PARAM_POSTSCAN","PARAM_PIX_PAR_DIST","PARAM_PIX_PAR_SIZE","PARAM_PIX_SER_DIST",
	"PARAM_PIX_SER_SIZE","PARAM_SUMMING_WELL","PARAM_FWELL_CAPACITY","PARAM_PAR_SIZE","PARAM_SER_SIZE","PARAM_ACCUM_CAPABLE",
	"PARAM_FLASH_DWNLD_CAPABLE","PARAM_CONTROLLER_ALIVE","PARAM_READOUT_TIME","PARAM_CLEAR_CYCLES","PARAM_CLEAR_MODE",
	"PARAM_FRAME_CAPABLE","PARAM_PMODE","PARAM_CCS_STATUS","PARAM_TEMP","PARAM_TEMP_SETPOINT","PARAM_CAM_FW_VERSION",
	"PARAM_HEAD_SER_NUM_ALPHA","PARAM_PCI_FW_VERSION","PARAM_CAM_FW_FULL_VERSION","PARAM_EXPOSURE_MODE","PARAM_BIT_DEPTH",
	"PARAM_GAIN_INDEX","PARAM_SPDTAB_INDEX","PARAM_READOUT_PORT","PARAM_PIX_TIME","PARAM_SHTR_CLOSE_DELAY","PARAM_SHTR_OPEN_DELAY",
	"PARAM_SHTR_OPEN_MODE","PARAM_SHTR_STATUS","PARAM_SHTR_CLOSE_DELAY_UNIT","PARAM_IO_ADDR","PARAM_IO_TYPE","PARAM_IO_DIRECTION",
	"PARAM_IO_STATE","PARAM_IO_BITDEPTH","PARAM_GAIN_MULT_FACTOR","PARAM_GAIN_MULT_ENABLE","PARAM_PP_FEAT_NAME",
	"PARAM_PP_INDEX","PARAM_ACTUAL_GAIN","PARAM_PP_PARAM_INDEX","PARAM_PP_PARAM_NAME","PARAM_PP_PARAM","PARAM_READ_NOISE",
	"PARAM_PP_FEAT_ID","PARAM_PP_PARAM_ID","PARAM_EXP_TIME","PARAM_EXP_RES","PARAM_EXP_MIN_TIME","PARAM_EXP_RES_INDEX",
	"PARAM_BOF_EOF_ENABLE","PARAM_BOF_EOF_COUNT","PARAM_BOF_EOF_CLR","PARAM_CIRC_BUFFER","PARAM_HW_AUTOSTOP"};


	ofstream cerrLog;
	char tempChar[MAX_PATH];
	std::string tempString;

	tempString = "cout.log";
	strcpy(tempChar,tempString.c_str());
	
	cerrLog.open(tempChar);

	streambuf* cerrBuffer = cerr.rdbuf(); // save cerr's output buffer

	cerr.rdbuf (cerrLog.rdbuf()); // redirect output into the file	

	rs_bool availFlag;
	for (int i = 0; i < sizeof(parameters)/sizeof(uns32); i++)
	{
		pl_get_param(cameraHandle, parameters[i], ATTR_AVAIL, (void *) &availFlag);
		std::cerr << paramNames[i] << ": \t \t" << availFlag << std::endl;
	}

	cerr.rdbuf (cerrBuffer); // restore old output buffer
	cerrLog.close();
}
*/