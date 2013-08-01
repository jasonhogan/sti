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

	isCalibrated = false;
	parseAngles = true;
}

MCLNanoDrive_Device::~MCLNanoDrive_Device()
{

	/* MCL_ReleaseHandle() should be called before the program exits */
	MCL_ReleaseHandle(handle);

}


bool MCLNanoDrive_Device::deviceMain(int argc, char **argv)
{

	std::cerr << "OK" << std::endl;
	//refreshAttributes();
	
	return false;
}

void MCLNanoDrive_Device::defineAttributes() 
{
	/*bool success;

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
	addAttribute(z.tag, getAngle(ZENUM, success));*/

	addAttribute("Calibration","Off", "On, Off, Refresh");
	addAttribute("Event parse mode", "Angles", "Angles, Positions" );
}

void MCLNanoDrive_Device::refreshAttributes() 
{
	bool success = true;

	//All the set's should be refreshed once the user-changed attribute has been changed
/*	setAttribute(setX.tag, setX.value);
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
	setAttribute(z.tag, getAngle(ZENUM, success));*/

	// The attribute should never stay on "Refresh"
	setAttribute("Calibration", (isCalibrated) ? "On" : "Off" );
	setAttribute("Event parse mode", (parseAngles) ? "Angles" : "Positions" );

}

bool MCLNanoDrive_Device::updateAttribute(string key, string value)
{
	double tempDouble;  //the value entered, converted to a number
	int tempInt;

	bool successDouble = stringToValue(value, tempDouble);
	bool successInt = stringToValue(value, tempInt);

	bool success = successDouble || successInt;

	attributeMCL tempAttr;

/*	if( (key.compare(setX.tag) == 0 || key.compare(setY.tag) == 0 || key.compare(setZ.tag) == 0) && successDouble)
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
	else*/ 
	if (key.compare("Calibration") == 0)
	{
		success = true;
		// fill calibration vector if the calibration is not already set
		// or if the user has requested the vector to be refreshed
		if ((value.compare("On") == 0 && !isCalibrated) || value.compare("Refresh") == 0)
		{
			isCalibrated = getCalibration();
			success = isCalibrated;
		} else if (value.compare("Off") == 0 && isCalibrated) {
			isCalibrated = false;
			xCalibrationVector.clear();
			yCalibrationVector.clear();
			zCalibrationVector.clear();
			success = true;
		}
	}
	else if (key.compare("Event parse mode") == 0)
	{
		success = true;
		// fill calibration vector if the calibration is not already set
		// or if the user has requested the vector to be refreshed
		if (value.compare("Angles") == 0)
		{
			success = true;
			parseAngles = true;
		} else if (value.compare("Positions") == 0) {
			success = true;
			parseAngles = false;
		}
		else
		{
			success = false;
			std::cerr << "Error in finding Event parse mode attribute name" << std::endl;
		}
	}
	else
	{
		success = false;
	}

	return success;
}

void MCLNanoDrive_Device::defineChannels()
{
	addOutputChannel(0, ValueVector); // for loading the waveform
	addOutputChannel(1, ValueVector); // for triggering the waveform
	//addInputChannel(2, STI::Types::DataVector);
	addOutputChannel(10, ValueVector); // for (theta, phi, z)
	addInputChannel((11, STI::Types::DataVector);
	addOutputChannel(20, ValueVector); // for (X, Y, Z)
	addInputChannel(21, STI::Types::DataVector);
}

bool MCLNanoDrive_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	std::vector<std::string>::iterator it;
	std::vector <double> measurement;
	bool success = true;
	std::vector <double> positions;
	std::vector <double> angles;

	if (channel == 11)
	{
		success = getAngles(angles, positions);
		if(success)
		{
			angles.at(0) *= 1000000;
			dataOut.setValue(angles);
		}
		else
		{
			angles.clear();
			angles.push_back(-1);
			angles.push_back(-1);
			angles.push_back(-1);
			dataOut.setValue(angles);
		}
	}
	else if (channel == 21)
	{
		success = getAngles(angles, positions);
		if(success)
		{
			dataOut.setValue(positions);
		}
		else
		{
			positions.clear();
			positions.push_back(-1);
			positions.push_back(-1);
			positions.push_back(-1);
			dataOut.setValue(positions);
		}
	}
	else
	{
		std::cerr << "Expecting channel 11 or 21 for angles and positions respectively" << std::endl;
		return false;
	}

	return success;
}

bool MCLNanoDrive_Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	bool error = false;

	if (channel == 0) {
		if (value.getType() != MixedValue::Vector) {
			std::cerr << "Expect vector as input: (axis-- X, Y, or Z in quotes, amplitude in um, freq in Hz, offset in um)" << std::endl;	
			return true;
		} 
		
		MixedValueVector valueVector = value.getVector();

		if (valueVector.size() != 4) {
			std::cerr << "Expect vector as input: (axis-- X, Y, or Z in quotes, amplitude in um, freq in Hz, offset in um)" << std::endl;	
			return true;
		}
		
		std::vector <axisInfo>::iterator it;
		std::string key = valueVector.at(0).getString();

		for (it = axes.begin(); it < axes.end(); it++)
		{
			if (it->getAttr->tag.find(key) != std::string::npos) {
				break;
			}
		}
		
		if (it == axes.end()) {
			std::cerr << "No axis found." << std::endl;
			return true;
		}

		error = loadSinWaveform(it->index, valueVector.at(1).getNumber(), valueVector.at(2).getNumber(), valueVector.at(3).getNumber());
	}
	else if (channel == 1)
	{
		if (value.getType() != MixedValue::Vector) {
			std::cerr << "Expect vector as input: (axis-- X, Y, or Z in quotes, number of repeats, delay time (ns))" << std::endl;	
			return true;
		} 
		
		MixedValueVector valueVector = value.getVector();

		if (valueVector.size() != 3)
		{
			std::cerr << "Expect vector as input: (axis-- X, Y, or Z in quotes, number of repeats, delay time (ns))" << std::endl;	
			return true;
		}

		std::vector <axisInfo>::iterator it;
		
		std::string key = valueVector.at(0).getString();

		for (it = axes.begin(); it < axes.end(); it++)
		{
			if (it->getAttr->tag.find(key) != std::string::npos) {
				break;
			}
		}
		
		if (it == axes.end()) {
			std::cerr << "No axis found." << std::endl;
			return true;
		}

		int numRepeats = (int) valueVector.at(1).getNumber();

		int errorCode;
		for (int i = 0; i < numRepeats; i++)
		{
			errorCode = MCL_Trigger_LoadWaveFormN(it->index, handle);
			error = printError(errorCode);
			if (error)
				return error;

			//Sleep for the length of the waveform plus 5 ms.
			//Delay determined experimentally.
			Sleep(waveformExecutionTimeMS + valueVector.at(2).getNumber());
		}
		
	} else if (channel == 10)
	{
		if (value.getType() != MixedValue::Vector) {
			std::cerr << "Expect vector as input: (theta (in urad), phi (in rad), z (in um))" << std::endl;	
			return true;
		} 
		
		MixedValueVector valueVector = value.getVector();

		if (valueVector.size() != 3) {
			std::cerr << "Expect vector as input: (theta (in urad), phi (in rad), z (in um))" << std::endl;	
			return true;
		}
		
		std::vector <double> angles;
		angles.push_back(valueVector.at(0).getDouble() * .000001);
		angles.push_back(valueVector.at(1).getDouble());
		angles.push_back(valueVector.at(2).getDouble());

		error = !setAngles(angles);
	} else if (channel == 20)
	{
		if (value.getType() != MixedValue::Vector) {
			std::cerr << "Expect vector as input: (x , y, z) in um" << std::endl;	
			return true;
		} 
		
		MixedValueVector valueVector = value.getVector();

		if (valueVector.size() != 3) {
			std::cerr << "Expect vector as input: (x , y, z) in um" << std::endl;	
			return true;
		}
		
		std::vector <double> positions;
		positions.push_back(valueVector.at(0).getDouble());
		positions.push_back(valueVector.at(1).getDouble());
		positions.push_back(valueVector.at(2).getDouble());

		error = !setPositions(positions);
	}

	return error;
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
			
				if (parseAngles)
				{
					mclEvent->angles.push_back(eVector.at(0).getDouble());
					mclEvent->angles.push_back(eVector.at(1).getDouble());
					mclEvent->angles.push_back(eVector.at(2).getDouble());
				}
				else
				{
					mclEvent->positions.push_back(eVector.at(0).getDouble());
					mclEvent->positions.push_back(eVector.at(1).getDouble());
					mclEvent->positions.push_back(eVector.at(2).getDouble());
				}

				//Check that the angles are within range
				if (parseAngles && !inRange(mclEvent->angles))
				{
					throw EventParsingException(events->second.at(0), "Choice of angles out of range");
				}
				else if (!parseAngles)
				{
					if (mclEvent->positions.at(0) < 0 || mclEvent->positions.at(0) > xRange ||
						mclEvent->positions.at(1) < 0 || mclEvent->positions.at(1) > yRange ||
						mclEvent->positions.at(2) < 0 || mclEvent->positions.at(2) > zRange)
						throw EventParsingException(events->second.at(0), "Choice of positions out of range");
				}
			
			}
			else {
				if (parseAngles)
				{
					throw EventParsingException(events->second.at(0),
						"MCL command must be a tuple in the form (double theta, double phi, double z)." );
				}
				else
				{
					throw EventParsingException(events->second.at(0),
						"MCL command must be a tuple in the form (double x, double y, double z)." );
				}
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
		if (MCLdevice_->parseAngles)
			eventSuccess = MCLdevice_->setAngles(angles);
		else
			eventSuccess = MCLdevice_->setPositions(positions);

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
std::string MCLNanoDrive_Device::getDeviceHelp()
{
	int i = 0;
	std::string message = "Parsing:\n";
	message.append("\t Depending on the \"Event parse mode\" attribute, you can choose to \n");
	message.append("\t parse either angle tuplets or position tuplets for writing: \n");
	message.append("\t event(ch 10, time, (theta in rad, phi in rad, z in um)) \n");
	message.append("\t OR \n");
	message.append("\t event(ch 20, time, (x in um, y in um, z in um)) \n");
	message.append("\t phi is measured from the X axis \n");
	message.append("\t (Note that it is generally a good idea to center z (at 15) to get the max angular range.) \n");
	message.append("\n");
	message.append("\t It is also possible to measure the angles and positions (one meas gets both): \n");
	message.append("\t meas(ch 11, time) \n");
	message.append("\n");
	message.append("\n");
	message.append("Channel 0: Load waveform \n");
	message.append("\t (axis-- X, Y, or Z in quotes, amplitude in um, freq in Hz, offset in um)\n");
	message.append("\t Example: (\"X\", 5, 1, 10) => (5 um)*sin[2*pi*(1 Hz)t] + (10 um)\n");
	message.append("\t You can load a waveform for all three axes, then trigger any one with channel 1. \n");
	message.append("\t Max frequency: 200 Hz; Number of periods: frequency * 0.167 ms per point * 6666 points\n");
	message.append("\n" );
	message.append("Channel 1: Write waveform \n");
	message.append("\t (axis-- X, Y, or Z in quotes, number of repeats, delay time (ns))\n");
	message.append("\t Example: (\"X\", 10, 1*s) => 10 repetitions of the waveform loaded by channel 0, 1s delay between repetitions \n");
	message.append("\n");
	message.append("Channel 10 & 11: Write & Read angles \n");
	message.append("\t (theta in urad, phi in rad, z in um); phi measured from X axis \n");
	message.append("\t NOTE: different convention than parsing \n");
	message.append("\n");
	message.append("Channel 20 & 21: Write & Read positions \n");
	message.append("\t (x in um, y in um, z in um) \n");

	return (message);
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

	double calibratedUM;

	for (it = axes.begin(); it != axes.end(); it++)
	{
		if (key.compare(it->setAttr->tag) == 0) 
			break;
	}
	
	if (it == axes.end()) {
		return false;
	}
	
	//calibrate returns positionUM if isCalibrated is false
	calibratedUM = calibrate(positionUM, it->setAttr->tag);

	//The following takes care of the cases where positionUM is a small negative number (less than -.001)
	if (calibratedUM < 0)
	{
		if ((int) (accuracy*calibratedUM) == 0)
		{
			calibratedUM = 0;
		}
	}

	//How does this handle out-of-range numbers?
	errorCode = MCL_SingleWriteN(calibratedUM, it->index, handle);

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
bool MCLNanoDrive_Device::setPositions(std::vector <double> &positions)
{
	double xPos, yPos, zPos;

	bool error = false;

	// Calculate and set each axis
	xPos = positions.at(0);
	yPos = positions.at(1);
	zPos = positions.at(2);

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

double MCLNanoDrive_Device::calibrate(double positionUM, std::string tag)
{
	double calibratedUM;
	unsigned int i;
	double setLB, setUB, measLB, measUB; // lower and upper bounds in calibration lookup table
	std::vector <pair <double,double> > *calibrationVector;

	if (setX.tag.compare(tag) == 0) {
		calibrationVector = &xCalibrationVector;
	} else if (setY.tag.compare(tag) == 0) {
		calibrationVector = &yCalibrationVector;
	} else if (setZ.tag.compare(tag) == 0) {
		calibrationVector = &zCalibrationVector;
	} else {
		std::cerr << "Error in MCLNanoDrive_Device::calibrate. Axis not found" << std::endl;
		return positionUM;
	}

	// if the device isn't calibrated (and hence calibrationVector is empty), return uncalibrated position
	// Returns uncalibrated data if calibrationVector is not long enough.
	if (!isCalibrated || calibrationVector->size() < 2)
		return positionUM;

	/* Assumptions: calibrationVector is sorted from smallest to largest*/
	for (i = 0; i < calibrationVector->size(); i++) {
	
		if (positionUM < calibrationVector->at(i).second) {
			
			if (i != 0) {
				// measured lower and upper bounds to the desired position
				measLB = calibrationVector->at(i-1).second;
				measUB = calibrationVector->at(i).second;
				// the values to which the nanopositioners were set in order to generate the measured bounds above
				setLB = calibrationVector->at(i-1).first;
				setUB = calibrationVector->at(i).first;
			}
			else {
				// measured lower and upper bounds to the desired position
				measLB = calibrationVector->at(0).second;
				measUB = calibrationVector->at(1).second;
				// the values to which the nanopositioners were set in order to generate the measured bounds above
				setLB = calibrationVector->at(0).first;
				setUB = calibrationVector->at(1).first;
			}
			
			// if i == 0, the second term will be negative
			calibratedUM = setLB + (setUB-setLB)*(positionUM-measLB)/(measUB-measLB);
			break;
		}
	}

	//Case: positionUM is larger than the largest calibration data point
	if (i == calibrationVector->size())
	{
		measLB = calibrationVector->at(i-2).second;
		measUB = calibrationVector->at(i-1).second;
		setLB = calibrationVector->at(i-2).first;
		setLB = calibrationVector->at(i-1).first;

		calibratedUM = setUB + (setUB - setLB)*(positionUM - measUB)/(measUB-measLB);
	}

	return calibratedUM;
}
bool MCLNanoDrive_Device::getCalibration()
{
	std::string directory = "\\\\epsrv1\\EP\\RCS\\Calibration Files\\";
	std::string xAxisFilename = "x-axis.txt";
	std::string yAxisFilename = "y-axis.txt";
	std::string zAxisFilename = "z-axis.txt";
	std::string fullFilename;

	pair <double,double> calPair;

	std::ifstream file;

	xCalibrationVector.clear();
	yCalibrationVector.clear();
	zCalibrationVector.clear();

	// load x-axis	
	fullFilename = directory + xAxisFilename;
	file.open(&fullFilename[0]);

	file >> calPair.first;
	while(file.good())
	{
		file >> calPair.second;
		xCalibrationVector.push_back(calPair);
		file >> calPair.first;
	}

	file.clear();
	file.close();

	fullFilename = directory + yAxisFilename;
	file.open(&fullFilename[0]);

	file >> calPair.first;
	while(file.good())
	{
		file >> calPair.second;
		yCalibrationVector.push_back(calPair);
		file >> calPair.first;
	}

	file.clear();
	file.close();

	fullFilename = directory + zAxisFilename;
	file.open(&fullFilename[0]);

	file >> calPair.first;
	while(file.good())
	{
		file >> calPair.second;
		zCalibrationVector.push_back(calPair);
		file >> calPair.first;
	}

	file.clear();
	file.close();

	if (xCalibrationVector.empty() || yCalibrationVector.empty() || zCalibrationVector.empty())
	{
		xCalibrationVector.clear();
		yCalibrationVector.clear();
		zCalibrationVector.clear();
		return false;
	}

	return true;
}

bool MCLNanoDrive_Device::loadSinWaveform(unsigned int axis, double amplitudeUM, double frequencyHZ, double offsetUM)
{
	unsigned int dataPoints = 6666; //max num data points allowed
	double waveform[6666] = {0.0}; // initialize waveform to 0
	double msLowerBound = 0.167;
	double msUpperBound = 5.0;
	double milliseconds;
	bool error = false;
	int errorCode;


	if(1/frequencyHZ < msUpperBound/1000)
		return true;

	int numWaves = (int) (frequencyHZ*msLowerBound* ((double) dataPoints)/1000) + 1;

	milliseconds = numWaves / frequencyHZ / ((double) dataPoints + 1) * 1000;

	for (unsigned int i = 0; i < dataPoints; i++) {
		waveform[i] = amplitudeUM * sin(2 * 3.14159 * frequencyHZ * i * milliseconds / 1000) + offsetUM;
		//Should find calibration with particular axis
		if (waveform[i] < 0 || waveform[i] > 30)
		{
			std::cerr << "Waveform out of range: " << waveform[i] << std::endl;
			return true;
		}
	}

	std::cerr << "Loading..." << std::endl;
	errorCode = MCL_Setup_LoadWaveFormN(axis, dataPoints, milliseconds, waveform, handle);

	error = printError(errorCode);
	std::cerr << "Loaded" << std::endl;
	if(!error)
		waveformExecutionTimeMS = dataPoints*milliseconds;
	else
		waveformExecutionTimeMS = 0;

	return error;
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