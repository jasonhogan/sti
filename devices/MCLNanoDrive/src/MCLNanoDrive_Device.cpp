/*! \file MCLNanoDrive_Device.cpp
 *  \author Susannah Dickerson
 *  \brief Controls Mad City Labs nanopositioners
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
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



#include "MCLNanoDrive_Device.h"


MCLNanoDrive_Device::MCLNanoDrive_Device(ORBManager*    orb_manager, 
					   std::string    DeviceName, 
					   std::string    Address, 
					   unsigned short ModuleNumber) : 
STI_Device(orb_manager, DeviceName, Address, ModuleNumber)
{
	deviceErrorCode = -10;
	accuracy = 1000;

	//SetA.tag must end in same character as X.tag for setPostions to work
	setX.tag = "Set X";
	setY.tag = "Set Y";
	setZ.tag = "Set Z";

	setth.tag = "Set theta";
	setph.tag = "Set phi";
	setz.tag = "Set z";

	X.tag = "Position X";
	Y.tag = "Position Y";
	Z.tag = "Position Z";

	th.tag = "Mirror Angle: theta";
	ph.tag = "Rotation Axis angle from x: phi";
	z.tag = "Height at RCS center: z";

	initialized = initializeDevice();

	if (initialized)
	{
		setX.value = 0;
		setY.value = 0;
		setZ.value = 0;

		setth.value = 0;
		setph.value = 0;
		setz.value = 0;

		if (!getAngles())
			initialized = false;
	}
}

MCLNanoDrive_Device::~MCLNanoDrive_Device()
{

	/* MCL_ReleaseHandle() should be called before the program exits */
	MCL_ReleaseHandle(handle);

}


bool MCLNanoDrive_Device::deviceMain(int argc, char **argv)
{

	//refreshAttributes();
	
	return false;
}

void MCLNanoDrive_Device::defineAttributes() 
{
	bool success;

	addAttribute(setX.tag, setX.value);
	addAttribute(setY.tag, setY.value);
	addAttribute(setZ.tag, setZ.value);

	addAttribute(setth.tag, setth.value);
	addAttribute(setph.tag, setph.value);
	addAttribute(setz.tag, setz.value);

	addAttribute(X.tag, getPosition(X.tag, success));
	addAttribute(Y.tag, getPosition(Y.tag, success));
	addAttribute(Z.tag, getPosition(Z.tag, success));

	addAttribute(th.tag, getAngle(THETA, success));
	addAttribute(ph.tag, getAngle(PHI, success));
	addAttribute(z.tag, getAngle(ZENUM, success));
}

void MCLNanoDrive_Device::refreshAttributes() 
{
	bool success = true;

	//All the set's should be refreshed once the user-changed attribute has been changed
	setAttribute(setX.tag, setX.value);
	setAttribute(setY.tag, setY.value);
	setAttribute(setZ.tag, setZ.value);

	setAttribute(setth.tag, setth.value);
	setAttribute(setph.tag, setph.value);
	setAttribute(setz.tag, setz.value);

	setAttribute(X.tag, getPosition(X.tag, success));
	setAttribute(Y.tag, getPosition(Y.tag, success));
	setAttribute(Z.tag, getPosition(Z.tag, success));

	setAttribute(th.tag, getAngle(THETA, success));
	setAttribute(ph.tag, getAngle(PHI, success));
	setAttribute(z.tag, getAngle(ZENUM, success));

}

bool MCLNanoDrive_Device::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	attributeMCL tempAttr;

	if( (key.compare(setX.tag) == 0 || key.compare(setY.tag) == 0 || key.compare(setZ.tag) == 0) && successDouble)
	{
		success = setPosition(key, tempDouble);	
	}
	else if((key.compare(setth.tag) == 0 || key.compare(setph.tag) == 0 || key.compare(setz.tag) == 0)&& successDouble)
	{
		success = setAngle(key, tempDouble);
	}
	else if (key.compare(th.tag) == 0 && successDouble)
	{
		success = true;
		th.value = tempDouble;
	}
	else if (key.compare(ph.tag) == 0 && successDouble)
	{
		success = true;
		ph.value = tempDouble;
	}
	else if (key.compare(z.tag) == 0 && successDouble)
	{
		success = true;
		z.value = tempDouble;
	}
	else if (key.compare(X.tag) == 0 && successDouble)
	{
		success = true;
		X.value = tempDouble;
		
	}
	else if (key.compare(Y.tag) == 0 && successDouble)
	{
		success = true;
		Y.value = tempDouble;
	}
	else if (key.compare(Z.tag) == 0 && successDouble)
	{
		success = true;
		Z.value = tempDouble;
	}
	else
	{
		success = false;
	}

	return success;
}

void MCLNanoDrive_Device::defineChannels()
{
	addOutputChannel(0, ValueVector);
	addInputChannel(1, STI::Types::DataVector);
}

bool MCLNanoDrive_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	return false;
}

bool MCLNanoDrive_Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	return false;
}

void MCLNanoDrive_Device::definePartnerDevices()
{
}

std::string MCLNanoDrive_Device::execute(int argc, char **argv)
{
	return "";
}

void MCLNanoDrive_Device::parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception)
{
	double digitalMinAbsStartTime = 10000;
	double startTimeBuffer = digitalMinAbsStartTime;
	double eventTime;
	double previousTime; //time when the previous event occurred
	double ns = 1000000000; // Conversion factor from seconds to ns
	double ms2ns = 1000000; // Conversion factor from ms to ns
	double prepEventTime;
	double eventHoldoff = 2*ms2ns; //time between events

	RawEventMap::const_iterator events;
	RawEventMap::const_iterator previousEvents;

	MCLEvent* mclEvent;


	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{	

		eventTime = events->first;

		if (events->second.at(0).getValueType() == MixedValue::Vector && !(events->second.at(0).isMeasurementEvent()))
		{
			unsigned sizeOfTuple = events->second.at(0).value().getVector().size();

			const std::vector <MixedValue>& eVector = events->second.at(0).value().getVector();

			//Check that each type in tuple is correct. The first two switch statements
			// are deliberately un-break'd.
			if (sizeOfTuple == 3) {
				if(eVector.at(2).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"MCL z position must be a double");
				}
				if(eVector.at(1).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"MCL phi angle must be a double");
				}
				if(eVector.at(0).getType() != MixedValue::Double)
				{
					throw EventParsingException(events->second.at(0),
						"MCL theta angle must be a double");
				}
				//Make a new event
				mclEvent = new MCLEvent(eventTime, this);
			
				mclEvent->angles.push_back(eVector.at(0).getDouble());
				mclEvent->angles.push_back(eVector.at(1).getDouble());
				mclEvent->angles.push_back(eVector.at(2).getDouble());

				//Check that the angles are within range
				if (!inRange(mclEvent->angles))
				{
					throw EventParsingException(events->second.at(0), "Choice of angles out of range");
				}
			
			}
			else {
				throw EventParsingException(events->second.at(0),
					"MCL commands must be a tuple in the form (double theta, double phi, double z)." );
				break;
			}
		}
		else if (events->second.at(0).getValueType() == MixedValue::Empty && events->second.at(0).isMeasurementEvent()) {

			//Make a new event
			mclEvent = new MCLEvent(eventTime, this);

			
		}
		else
		{
			std::cerr << "The MCL device does not support that data type" << std::endl;
			throw EventParsingException(events->second.at(0),
						"The MCL device does not support that data type.");
		}
		
		// Check that the camera can keep up with the events
		if(events != eventsIn.begin())
		{
			previousEvents = --events;
			events++;
			previousTime = previousEvents->first;
			// The kinetic time gets set whenever the exposure time is changed.
			// it depends on the vertical and horizontal shift speeds, and adds on the exposure time
			prepEventTime = eventTime - eventHoldoff * events->second.size();
		}
		else
		{
			previousEvents = events;
			previousTime = startTimeBuffer;
			prepEventTime = eventTime;
		}

		if( prepEventTime < previousTime  && events != eventsIn.begin())
		{
			delete mclEvent;
			throw EventConflictException(previousEvents->second.at(0), 
				events->second.at(0), 
				"The MCL device can't run events faster than " + valueToString(eventHoldoff/ms2ns) + " ms, apart." );
		}
		else if (prepEventTime < previousTime)
		{
			delete mclEvent;
			throw EventConflictException(previousEvents->second.at(0), 
				events->second.at(0), 
				"The MCL device requires at least " + valueToString(startTimeBuffer/ns) + " s before the first event." );
		}

		eventsOut.push_back( mclEvent );

		//Add measurement
		if (events->second.at(0).isMeasurementEvent()) {
			eventsOut.back().addMeasurement( events->second.at(0) );
		}

		previousTime = eventTime;
	}
}

void MCLNanoDrive_Device::stopEventPlayback()
{
}

void MCLNanoDrive_Device::MCLEvent::playEvent()
{ 
	//Do something only if you're supposed to 
	if (getNumberOfMeasurements() == 0) {
		eventSuccess = MCLdevice_->setAngles(angles);
		if (!eventSuccess)
			std::cout << "Error playing MCLEvent " << valueToString(getEventNumber()) << "." << std::endl;
	} else if (getNumberOfMeasurements() == 1)
	{
		eventSuccess = MCLdevice_->getAngles(angles, positions);
	} else {
		eventSuccess = false;
		std::cout << "Error playing MCLEvent " << valueToString(getEventNumber()) << ". Unexpected number of measurements." << std::endl;
	}

}
void MCLNanoDrive_Device::MCLEvent::collectMeasurementData()
{
	if (getNumberOfMeasurements() == 1) {
		MixedData data;
		data.addValue(angles);
		data.addValue(positions);

		eventMeasurements.at(0)->setData(data);
	} else if (getNumberOfMeasurements() == 0) {
		// do nothing; this isn't a measurement event
	} else {
		// should be handled by playEvent
	}

}
bool MCLNanoDrive_Device::initializeDevice()
{
	int j;
	axisInfo info;

	/* Prior to calling any other device function MCL_InitHandle() should be called */
	handle = MCL_InitHandle();
	if(handle == 0) {
		std::cerr << "Cannot get a handle to the device\n" << std::endl;
		return false;
	}

/* Fills a structure with information about the NanoDrive */
	MCL_GetProductInfo(&pi, handle);

	/*Loop 3 times.
		Check if X is valid.
		Check if Y is valid.
		Check if Z is valid.
	*/
	for(j = 0; j < 3; j++)
	{
		/*Checks if an axis is valid*/
		if((pi.axis_bitmap & (0x01 << j)) == 0)		
			continue;

		/*Load axis indices*/
		switch(j) {
			case 0:
				info.setInfo(&X, &setX, j+1);
				axes.push_back(info);
				xRange = MCL_GetCalibration(j+1, handle);
				break;
			case 1:
				info.setInfo(&Y, &setY, j+1);
				axes.push_back(info);
				yRange = MCL_GetCalibration(j+1, handle);
				break;
			case 2:
				info.setInfo(&Z, &setZ, j+1);
				axes.push_back(info);
				zRange = MCL_GetCalibration(j+1, handle);
				break;
			default:
				std::cerr << "Unrecognized axis" << std::endl;
				return false;
				break;
		}
	}

	return true;
}

bool MCLNanoDrive_Device::getPositions()
{
	double position;
	bool success;
	std::vector <axisInfo>::iterator it;
	
	for (it = axes.begin(); it != axes.end(); it++)
	{
		position = MCL_SingleReadN(it->index, handle);

		printError((int) position); //print an error, but don't do anything about it

		it->getAttr->value = position;
		success = ((int) position >= 0);
	}
	return success;
}

bool MCLNanoDrive_Device::getAngles()
{
	bool success;
	bool error;
	std::vector <double> positions;
	
	success = getPositions();

	positions.push_back(X.value);
	positions.push_back(Y.value);
	positions.push_back(Z.value);

	if (success)
	{
		th.value = rcs.calculatetheta(positions, error);
		success = !error;
	}

	if (success)
	{
		ph.value = rcs.calculatephi(positions, error);
		success = !error;
	}

	if (success)
	{
		z.value = rcs.calculatez(positions, error);
		success = !error;
	}

	return success;
}
double MCLNanoDrive_Device::getAngle(anglesEnum a, bool &success)
{
	bool error = false;
	std::vector <double> positions;
	double temp;

	positions.push_back(getPosition(X.tag, success));
	positions.push_back(getPosition(Y.tag, success));
	positions.push_back(getPosition(Z.tag, success));

	if (success)
	{
		switch(a){
			case THETA:
				temp = rcs.calculatetheta(positions, error);
				break;
			case PHI:
				temp = rcs.calculatephi(positions, error);
				break;
			case ZENUM:
				temp = rcs.calculatez(positions, error);
				break;
			default:
				error = true;
				temp = deviceErrorCode;
				break;
		}
		success = !error;
	}
	else {
		success = false;
		temp = deviceErrorCode;
	}
	return temp;
}
bool MCLNanoDrive_Device::getAngles(std::vector <double> &angles, std::vector <double> &positions)
{
	bool success;
	bool error;
	
	success = getPositions();

	positions.push_back(X.value);
	positions.push_back(Y.value);
	positions.push_back(Z.value);

	if (success)
	{
		angles.push_back(rcs.calculatetheta(positions, error));
		success = !error;
	}

	if (success)
	{
		angles.push_back(rcs.calculatephi(positions, error));
		success = !error;
	}

	if (success)
	{
		angles.push_back(rcs.calculatez(positions, error));
		success = !error;
	}

	return success;
}

bool MCLNanoDrive_Device::setPosition(std::string key, double positionUM)
{
	int errorCode;

	bool success = true;

	std::vector <axisInfo>::iterator it;
	std::string keyEnd;
	std::string tagEnd;

	for (it = axes.begin(); it != axes.end(); it++)
	{
		if (key.compare(it->setAttr->tag) == 0) 
			break;
	}
	
	if (it == axes.end()) {
		return false;
	}
	
	//The following takes care of the cases where positionUM is a small negative number (less than -.001)
	if (positionUM < 0)
	{
		if ((int) (accuracy*positionUM) == 0)
		{
			positionUM = 0;
		}
	}

	//How does this handle out-of-range numbers?
	errorCode = MCL_SingleWriteN(positionUM, it->index, handle);

	success = !printError(errorCode);

	if (success)
	{
		it->setAttr->value = positionUM;
		success = !updateSetThPhZ();
	}

	return success;
}

bool MCLNanoDrive_Device::setAngle(std::string key, double angle)
{
	attributeMCL *attr;
	std::vector <double> angles;

	double zPos;

	angles.push_back(setth.value);
	angles.push_back(setph.value);
	angles.push_back(setz.value);

	bool success = true;
	bool error = false;

	// replace with the new value
	if (key.compare(setth.tag) == 0) {
		attr = &setth;
		angles.at(0) = angle;
	} else if (key.compare(setph.tag) == 0) {
		attr = &setph;
		angles.at(1) = angle;
	} else if (key.compare(setz.tag) == 0) {
		attr = &setz;
		angles.at(2) = angle;
	}
	else {
		return false;
	}
	
	// Calculate and set each axis
	zPos = rcs.calculateX(angles, error);
	success = success && !error;
	if (success) {
		success = setPosition(setX.tag,zPos);
	}
	
	zPos = rcs.calculateY(angles, error);
	success = success && !error;
	if (success) {
		success = setPosition(setY.tag,zPos);
	}

	zPos = rcs.calculateZ(angles, error);
	success = success && !error;
	if (success) {
		success = setPosition(setZ.tag,zPos);
	}

	if (success) {
		attr->value = angle;
	}

	return success;
}

bool MCLNanoDrive_Device::setAngles(std::vector <double> &angles)
{
	double xPos, yPos, zPos;

	bool error = false;

	// Calculate and set each axis
	xPos = rcs.calculateX(angles, error);
	yPos = rcs.calculateY(angles, error);
	zPos = rcs.calculateZ(angles, error);

	error = !setPosition(setX.tag,xPos);
	if (!error) {
		error = !setPosition(setY.tag,yPos);
	}
	if (!error) {
		error = !setPosition(setZ.tag,zPos);
	}

	return !error;
}

bool MCLNanoDrive_Device::updateSetThPhZ()
{
	std::vector <double> axesTemp;
	bool error = false;

	axesTemp.push_back(setX.value);
	axesTemp.push_back(setY.value);
	axesTemp.push_back(setZ.value);

	setth.value = rcs.calculatetheta(axesTemp, error);
	if (!error)
		setph.value = rcs.calculatephi(axesTemp, error);
	if (!error)
		setz.value = rcs.calculatez(axesTemp, error);

	return error;
}



bool MCLNanoDrive_Device::inRange(std::vector <double> &a)
{
	double testx;
	double testy;
	double testz;

	bool error;

	testx = rcs.calculateX(a, error);
	if (!error)
		testy = rcs.calculateY(a, error);
	if (!error)
		testz = rcs.calculateZ(a, error);


	if (!error)
	{
		if(testx < 0)
		{
			if ((int) (accuracy * testx) == 0)
				testx = 0;
		}
		if(testy < 0)
		{
			if ((int) (accuracy * testy) == 0)
				testy = 0;
		}
		if(testz < 0)
		{
			if ((int) (accuracy * testz) == 0)
				testz = 0;
		}

		error = !(0 <= testx && testx <= xRange);
		error = error || !(0 <= testy && testy <= yRange);
		error = error || !(0 <= testz && testz <= zRange);
	}

	//return success
	return !error;
}

bool MCLNanoDrive_Device::printError(int error)
{
	bool isError = false;

	switch (error)
	{
	case MCL_SUCCESS:
		isError = false;
		break;
	case MCL_GENERAL_ERROR:
		isError = true;
		std::cerr << "This error generally occurs due to an internal sanity check failing." << std::endl;
		break;
	case MCL_DEV_ERROR:
		isError = true;
		std::cerr << "A problem occurred when transferring data to the Nano-Drive.  It is likely that the Nano-Drive will have to be power cycled to correct this error." << std::endl;
		break;
	case MCL_DEV_NOT_ATTACHED:
		isError = true;
		std::cerr << "The Nano-Drive cannot complete the task because it is not attached." << std::endl;
		break;
	case MCL_USAGE_ERROR:
		isError = true;
		std::cerr << "Using a function from the library which the Nano-Drive does not support causes this error." << std::endl;
		break;
	case MCL_DEV_NOT_READY:
		isError = true;
		std::cerr << "The Nano-Drive is currently completing or waiting to complete another task." << std::endl;
		break;
	case MCL_ARGUMENT_ERROR:
		isError = true;
		std::cerr << "An argument is out of range or a required pointer is equal to NULL." << std::endl;
		break;
	case MCL_INVALID_AXIS:
		isError = true;
		std::cerr << "Attempting an operation on an axis that does not exist in the Nano-Drive." << std::endl;
		break;
	case MCL_INVALID_HANDLE:
		isError = true;
		std::cerr << "The handle is not valid.  Or at least is not valid in this instance of the DLL." << std::endl;
		break;
	default:
		// no error
		isError = false;
		break;
	}

	return isError;
}





double MCLNanoDrive_Device::getPosition(std::string key, bool &success)
{
	double position;
	std::vector <axisInfo>::iterator it;

	for (it = axes.begin(); it < axes.end(); it++)
	{
		if (key.compare(it->getAttr->tag) == 0) {
			break;
		}
	}
	
	if (it == axes.end()) {
		success = false;
		std::cerr << "No axis found." << std::endl;
		return 0;
	}

	position = MCL_SingleReadN(it->index, handle);

	printError((int) position); //print an error, but don't do anything about it

	//it->getAttr->value = position;

	success = ((int) position >= 0);

	return position;
}

//Unused functions
/*
bool MCLNanoDrive_Device::updateThPhZ()
{
	std::vector <double> axesTemp;
	bool error = false;

	axesTemp.push_back(X.value);
	axesTemp.push_back(Y.value);
	axesTemp.push_back(Z.value);

	th.value = rcs.calculatetheta(axesTemp, error);
	if (!error)
		ph.value = rcs.calculatephi(axesTemp, error);
	if (!error)
		z.value = rcs.calculateZ(axesTemp, error);

	return error;
}
*/