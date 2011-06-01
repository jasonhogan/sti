/*! \file NetworkAnalyzer4395A_Device.cpp
 *  \author Jason Hogan
 *  \brief Source-file for the class NetworkAnalyzer4395A_Device
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

#include "NetworkAnalyzer4395A_Device.h"

NetworkAnalyzer4395A_Device::NetworkAnalyzer4395A_Device(ORBManager*    orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string logDirectory,
							std::string GCipAddress,
							unsigned short GCmoduleNumber) : 
GPIB_Device(orb_manager, DeviceName, Address, ModuleNumber, logDirectory, GCipAddress, GCmoduleNumber)
{ 
	//primaryAddress = primaryGPIBAddress; //normally 1
	//secondaryAddress = 0;
	//gpibID = "Have Not Queried"; // initializes with null result - haven't checked yet
	//initialized = false;

	frequency = 0;
	power = 0;

	minimumEventSpacing = 500; // 500 nanoseconds - this is experimentally verified
	minimumAbsoluteStartTime = 500000; //10*us in nanoseconds - this is a guess right now to let everything get sorted out

	maxSweepPoints = 800;

}

void NetworkAnalyzer4395A_Device::defineGpibAttributes()
{
	//addGpibAttribute("GPIB ID", "*IDN", "", true);
	//addGpibAttribute(":SYST:VERS?", ":SYST:VERS", "", true);
//	addGpibAttribute("Output Power (dBm)", "POW:LEV", "", false);
//	addGpibAttribute("Output Frequency (Hz)", "FREQ:CW", "", false);
}

void NetworkAnalyzer4395A_Device::defineChannels()
{
//	addInputChannel(0, DataDouble);
//	addInputChannel(1, DataDouble);
	addOutputChannel(0, ValueVector);
//	addOutputChannel(3, ValueNumber);
}
void NetworkAnalyzer4395A_Device::definePartnerDevices()
{
	addPartnerDevice("External Trigger", "ep-timing1.stanford.edu", 2, "Digital Out");
	partnerDevice("External Trigger").enablePartnerEvents();
	externalTriggerChannel = 3;
}

bool NetworkAnalyzer4395A_Device::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	////
	//
	//bool measureSuccess;
	//std::string measurementResult;

	//if(channel == 0)
	//{
	//	measurementResult = queryDevice("FREQ:CW?");
	//	std::cerr << measurementResult << std::endl;
	//	//measurementResult.erase(0,2);
	//	measureSuccess = stringToValue(measurementResult, frequency, std::ios::dec, 10);
	//	//wavelength = wavelength * 1000000000; // multiply by 10^9
	//	std::cerr.precision(10);
	//	std::cerr << "The output frequency is:" << frequency << " Hz" << std::endl;
	//	dataOut.setValue(frequency);
	//	return measureSuccess;
	//}
	//else if(channel == 1)
	//{
	//	measurementResult = queryDevice("POW:LEV?");
	//	std::cerr << measurementResult << std::endl;
	//	//measurementResult.erase(0,2);
	//	measureSuccess = stringToValue(measurementResult, power);
	//	std::cerr << "The output power is: " << power << "dBm" << std::endl;
	//	dataOut.setValue(power);
	//	return measureSuccess;
	//}

	//std::cerr << "Expecting either Channel 0 or 1" << std::endl;
	return false;
}

bool NetworkAnalyzer4395A_Device::writeChannel(unsigned short channel, const MixedValue& value)
{
	return writeChannelDefault(0, value, 700000);
	//if(channel == 2)
	//{
	//	double measuredValue = updateGPIBAttribute("FREQ:CW", value.getDouble(), true);
	//	if (measuredValue != -1)
	//		return true;
	//	else
	//		return false;
	//}
	//else if(channel == 3)
	//{
	//	double measuredValue = updateGPIBAttribute("POW:LEV", value.getDouble(), true);
	//	if (measuredValue != -1)
	//		return true;
	//	else
	//		return false;
	//}

	//std::cerr << "Expecting either Channel 0 or 1" << std::endl;
//	return false;
}


void NetworkAnalyzer4395A_Device::parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{

	std::string sweepTypeError = 
		"Sweep event must be a vector (startFreq, stopFreq, time) or a vector of such vectors.";


	RawEventMap::const_iterator events;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	double fStart;
	double fStop;
	double segmentTime;

	unsigned short channel;

	NetworkAnalyzerEvent* sweepEvent;

	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events->second.size() > 1)
		{
			throw EventConflictException(events->second.at(0), events->second.at(1),
						"Only one sweep is allowed at a time.");
		}

		if(events->second.at(0).getValueType() != MixedValue::Vector 
			|| events->second.at(0).value().getType() != MixedValue::Vector)
		{
			throw EventParsingException(events->second.at(0), sweepTypeError);
		}

		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
		{
			previousTime = minimumAbsoluteStartTime - minimumEventSpacing;
		}

		if(events->second.at(0).value().getVector().size() == 0)
		{
			throw EventParsingException(events->second.at(0), sweepTypeError);
		}

		if(events->second.at(0).value().getVector().at(0).getType() != MixedValue::Vector)
		{
			//**********Single segment sweep section************
	
			if(events->second.at(0).value().getVector().size() != 3)
			{
				throw EventParsingException(events->second.at(0), sweepTypeError);
			}
			for(unsigned j = 0; j < 3; j++)
			{
				if(events->second.at(0).value().getVector().at(j).getType() != MixedValue::Double 
					&& events->second.at(0).value().getVector().at(j).getType() != MixedValue::Int)
				{
					throw EventParsingException(events->second.at(0), sweepTypeError);
				}
			}

			fStart = events->second.at(0).value().getVector().at(0).getNumber();
			fStop = events->second.at(0).value().getVector().at(1).getNumber();
			segmentTime = events->second.at(0).value().getVector().at(2).getNumber();

			checkSweepSegmentParamters(fStart, fStop, segmentTime, events->second.at(0));

			sweepEvent = new NetworkAnalyzerEvent(events->first, segmentTime, this);

			sweepEvent->addSegment(
				events->second.at(0).value().getVector().at(0).getNumber(),
				events->second.at(0).value().getVector().at(1).getNumber(),
				maxSweepPoints
				);

		}
		else
		{
			//**********Multiple segment sweep section************
	
			double totalSweepTime = 0;
			
			//Calculate total sweep time by summing the time of each segment
			for(unsigned k = 0; k < events->second.at(0).value().getVector().size(); k++)
			{
				//check that each segment is a vector with at least three elements in it
				if(events->second.at(0).value().getVector().at(k).getType() != MixedValue::Vector ||
					events->second.at(0).value().getVector().at(k).getVector().size() < 3)
				{
					throw EventParsingException(events->second.at(0), sweepTypeError);
				}

				fStart      = events->second.at(0).value().getVector().at(k).getVector().at(0).getNumber();
				fStop       = events->second.at(0).value().getVector().at(k).getVector().at(1).getNumber();
				segmentTime = events->second.at(0).value().getVector().at(k).getVector().at(2).getNumber();	//time of segement "k"
				
				//Do this now to avoid a memory leak caused by throwing an exception after creating the event.
				checkSweepSegmentParamters(fStart, fStop, segmentTime, events->second.at(0));

				totalSweepTime += segmentTime;
			}

			sweepEvent = new NetworkAnalyzerEvent(events->first, totalSweepTime, this);

			for(unsigned i = 0; i < events->second.at(0).value().getVector().size(); i++)
			{
				fStart      = events->second.at(0).value().getVector().at(i).getVector().at(0).getNumber();
				fStop       = events->second.at(0).value().getVector().at(i).getVector().at(1).getNumber();				
				segmentTime = events->second.at(0).value().getVector().at(i).getVector().at(2).getNumber();

				//The number of points in the segment is determined by the fraction of the 
				//total sweep time spent in this segment, times the total sweep points (maxSweepPoints).
				sweepEvent->addSegment(fStart, fStop, static_cast<unsigned>(maxSweepPoints * (segmentTime / totalSweepTime)) );
			}
		}

		eventsOut.push_back( sweepEvent );
		partnerDevice("External Trigger").event(events->first - 500000, externalTriggerChannel, 0, events->second.at(0), "Network Analyzer 4395A External Trigger");
		partnerDevice("External Trigger").event(events->first, externalTriggerChannel, 1, events->second.at(0), "Network Analyzer 4395A External Trigger");
//		partnerDevice("External Trigger").event(events->first + 500000, externalTriggerChannel, 0, events->second.at(0), "Network Analyzer 4395A External Trigger");
	}
}


void NetworkAnalyzer4395A_Device::checkSweepSegmentParamters(double fStart, double fStop, double segmentTime, const RawEvent& evt) throw(std::exception)
{
	double minFreq = 0;
	double maxFreq = 510;

	if(fStart < minFreq)
	{
		throw EventParsingException(evt, "Start frequency (" + STI::Utils::valueToString(fStart) + ") must be larger than " 
			+ STI::Utils::valueToString(minFreq) + ".");
	}
	if(fStop > maxFreq)
	{
		throw EventParsingException(evt, "Stop frequency (" + STI::Utils::valueToString(fStop) + ") must be smaller than " 
			+ STI::Utils::valueToString(maxFreq) + ".");
	}
	if(fStart >= fStop)
	{
		throw EventParsingException(evt, "Stop frequency (" 
			+ STI::Utils::valueToString(fStop) + ") must be larger than Start frequency (" 
			+ STI::Utils::valueToString(fStart) + ").");
	}
	if(segmentTime <= 0)
	{
		throw EventParsingException(evt, "Sweep time must be positive.");
	}
}

std::string NetworkAnalyzer4395A_Device::execute(int argc, char** argv)
{
	//command structure:  >analogIn readChannel 1
	//returns the value as a string

	if(argc < 3)
		return "Error: Invalid argument list. Expecting 'channel'.";

	int channel;
	bool channelSuccess = stringToValue(argv[2], channel);

	if(channelSuccess && channel >=0 && channel <= 1)
	{
		MixedData data;
		bool success = readChannel(channel, 0, data);

		if(success)
		{
			cerr << "Result to transfer = " << data.getDouble() << endl;
			return valueToString( data.getDouble() );
		}
		else
			return "Error: Failed when attempting to read.";
	}

	return "Error";
}


void NetworkAnalyzer4395A_Device::NetworkAnalyzerEvent::loadEvent()
{

	double MHzToHz = 1.0e6;

	sendGPIBcommand("SWETAUTO OFF");	//Manual sweep time (page 11-23)
	sendGPIBcommand("SWET " + STI::Utils::valueToString(sweepTime) );	//Set sweep time (page 11-23)
	sendGPIBcommand("POIN " + STI::Utils::valueToString(device4395A_->maxSweepPoints) );	//Set number of points (page 11-23)

	if(segments.size() == 1)
	{
		//single segment sweep
		sendGPIBcommand("SWPT LINF");	//set to Sweep type: Lin Freq (page 11-23)
		sendGPIBcommand("STAR " + STI::Utils::valueToString( segments.at(0).getStart() * MHzToHz ));	//Start freq (page 11-27)
		sendGPIBcommand("STOP " + STI::Utils::valueToString( segments.at(0).getStop() * MHzToHz ));		//Stop freq (page 11-27)
	}
	else
	{
		sendGPIBcommand("EDITLIST");	//Start editing list (page 11-23)
		sendGPIBcommand("CLEL");		//Clear list (page 11-23)

		for(unsigned i = 0; i < segments.size(); i++)
		{
			sendGPIBcommand("SADD 1");		//Add segment (page 11-23)
			sendGPIBcommand("STAR " + STI::Utils::valueToString( segments.at(i).getStart() * MHzToHz ));	//segment start freq (page 11-23)
			sendGPIBcommand("STOP " + STI::Utils::valueToString( segments.at(i).getStop() * MHzToHz ));		//segment stop freq (page 11-23)
			sendGPIBcommand("POIN " + STI::Utils::valueToString( segments.at(i).getPoints() ));				//segment points (page 11-23)
			sendGPIBcommand("SDON");		//segment done (page 11-23)
		}

		sendGPIBcommand("EDITDONE");	//Finish editing list

		sendGPIBcommand("SWPT LIST");	//set to Sweep type: List Freq (page 11-23)
	}

}

void NetworkAnalyzer4395A_Device::NetworkAnalyzerEvent::playEvent()
{
}
