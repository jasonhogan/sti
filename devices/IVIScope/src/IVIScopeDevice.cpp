/*! \file IVIScopeDevice.cpp
 *  \author David M.S. Johnson and Jason Hogan
 *  \brief Source-file for the class IVIScopeDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
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



#include "IVIScopeDevice.h"
#include <iostream>
#include <string>
using namespace std;

IVIScopeDevice::IVIScopeDevice(ORBManager* orb_manager, 
							std::string    DeviceName, 
							std::string configFilename) : 
STI_Device_Adapter(orb_manager, DeviceName, configFilename)
{
	//ViStatus error = VI_SUCCESS;
	//error = IviScope_InitWithOptions ("PitScope1", VI_FALSE, VI_FALSE, "Simulate=0,RangeCheck=1,QueryInstrStatus=1,Cache=1", &session);

	//if(error == VI_SUCCESS)
	//	error = IviScope_ConfigureAcquisitionType(session, IVISCOPE_VAL_NORMAL);
	//
	//if(error == VI_SUCCESS)
	//	error = configureTrigger();

	//if(error == VI_SUCCESS)
	//	error = configureChannels();

	//if(error != VI_SUCCESS)
	//	cout << "IVI error code:  " << IVIgetError(error) << endl;


	configFile = ConfigFile_ptr(new ConfigFile(configFilename));
//	configFile = new ConfigFile(configFilename);

	if (!(configFile->getParameter("triggerSource", triggerSource)))
		triggerSource = IVISCOPE_VAL_EXTERNAL;

	if (!(configFile->getParameter("triggerLevel", triggerLevel)))
		triggerLevel = 1.0;

	if (!(configFile->getParameter("triggerSlope", triggerSlope)))
		triggerSlope = IVISCOPE_VAL_POSITIVE;

	try {

		checkViError(
			IviScope_InitWithOptions (&DeviceName[0], VI_FALSE, VI_FALSE, 
			"Simulate=0,RangeCheck=1,QueryInstrStatus=1,Cache=1", &session) );
		cout << "Alpha" << endl;
		checkViError( 
			IviScope_ConfigureAcquisitionType(session, IVISCOPE_VAL_NORMAL) );
		cout << "Beta" << endl;
		checkViError( configureTrigger() );
		cout << "Gamma" << endl;
		checkViError( configureChannels() );
		cout << "Delta" << endl;
	} catch(IVIScopeException& ex) {
		cout << "IVI error code:  " << IVIgetError(ex.error) << endl;
	}

	sampleRate = 0.01;
	measurementDuration = 2;
}


IVIScopeDevice::~IVIScopeDevice()
{
	if(session)
		IviScope_close(session);
}

ViStatus IVIScopeDevice::configureChannels()
{
	ViStatus error = VI_SUCCESS;

	ViInt32 index = 1;
	int abortCount = 5;
	ViChar* name = VI_NULL;
	ViInt32 bufferSize = 50;
	name = new ViChar[bufferSize];
	
	channelNames.clear();
	ViInt32 channelCount;

	//Get channel count
	try {
		checkViError(
			IviScope_GetAttributeViInt32(session, "", IVISCOPE_ATTR_CHANNEL_COUNT, &channelCount));
///		checkViError(
//			IviScope_GetAttributeViInt32(session, "", IVI_ATTR_BASE + 802, &channelCount2));
	} catch(IVIScopeException& ex) {
		return ex.error;
	}

	do {
		error = IviScope_GetChannelName(session, index, bufferSize, name);

		if( name != VI_NULL && name != NULL ) {
			channelNames.push_back(name);
			index++;
		} else {
			abortCount--;
		}

	} while( index <= channelCount && error == VI_SUCCESS && abortCount > 1);

	if(channelNames.size() > 0)
		return VI_SUCCESS;
	else
		return error;
}

ViStatus IVIScopeDevice::configureTrigger()
{
	ViStatus error;
	
//	IVISCOPE_VAL_TRIGGER_TYPE_CLASS_EXT_BASE
	cout << "A" << endl;

	ViReal64 holdoff = 0.01;

	error = IviScope_ConfigureTrigger (session, IVISCOPE_VAL_EDGE_TRIGGER, holdoff);
	if(error != VI_SUCCESS ) return error;

	cout << triggerSource << endl;
	error = IviScope_ConfigureEdgeTriggerSource(session, triggerSource.c_str(), triggerLevel, triggerSlope);
	//error = IviScope_ConfigureEdgeTriggerSource(session, &triggerSource[0], 1.0, IVISCOPE_VAL_POSITIVE);
	
	if(error != VI_SUCCESS ) return error;
	
	cout << "C" << endl;
	error = IviScope_ConfigureTriggerCoupling(session, IVISCOPE_VAL_DC);

	return error;
}


//static
void IVIScopeDevice::checkViError(ViStatus error, std::string description) throw(IVIScopeException)
{
	if(error != VI_SUCCESS)
		throw IVIScopeException(error, description);
}

//static
std::string IVIScopeDevice::IVIgetError(ViStatus error, ViSession viSession)
{
	ViChar errStr[2048];
	IviScope_GetError(viSession, &error, 2048, errStr);
	string str = errStr;
	return str;
}

//local
std::string IVIScopeDevice::IVIgetError(ViStatus error)
{
	return IVIgetError(error, session);
}

void IVIScopeDevice::defineAttributes() 
{

	string triggerSourceOptions = IVISCOPE_VAL_EXTERNAL;

	if (channelNames.size() > 0)
	{
		for(unsigned i = 0; i < channelNames.size(); i++) {
			triggerSourceOptions += ",";
			triggerSourceOptions += channelNames.at(i);
		}
	}

	addAttribute("Trigger Source", triggerSource, triggerSourceOptions);
	addAttribute("Trigger Level", triggerLevel);

	addAttribute("Sample rate (#/s)", sampleRate);
	addAttribute("Measurement Duration (s)", measurementDuration);
	
}

void IVIScopeDevice::refreshAttributes()
{
	setAttribute("Trigger Source", triggerSource);
	setAttribute("Trigger Level", triggerLevel);

	setAttribute("Sample rate (#/s)", sampleRate);
	setAttribute("Measurement Duration (s)", measurementDuration);
}

bool IVIScopeDevice::updateAttribute(std::string key, std::string value) 
{ 
	bool success = false;


	//Trigger attributes

	if(key.compare("Trigger Source") == 0 ||
		key.compare("Trigger Level") == 0)
	{
		string oldTriggerSource = triggerSource;
		ViReal64 oldTriggerLevel = triggerLevel;

		if(key.compare("Trigger Source") == 0)
		{
			triggerSource = value;			
		}
		else if (key.compare("Trigger Level") == 0)
		{
			ViReal64 tempViReal64;
			bool successValue = stringToValue(value, tempViReal64);
			if (successValue)
				triggerLevel = tempViReal64;
			else
				return false;
		}

		try {
			checkViError( configureTrigger() );
			success = true;

		} catch(IVIScopeException& ex) {
			cout << "IVI error code:  " << IVIgetError(ex.error) << endl;
			triggerSource = oldTriggerSource;
			triggerLevel = oldTriggerLevel;
			success = false;
		}
	}


	//All other attributes

	if(key.compare("Sample rate (#/s)") == 0)
	{
		double tempDouble;
		success = stringToValue(value, tempDouble);
		if (success)
			sampleRate = tempDouble;
	}
	else if (key.compare("Measurement Duration (s)") == 0)
	{
		double tempDouble;
		success = stringToValue(value, tempDouble);
		if (success)
			sampleRate = tempDouble;
	}

	return success; 
}


void IVIScopeDevice::defineChannels()
{
	for(unsigned i = 0; i < channelNames.size(); i++) {
		addInputChannel(i + 1, STI::Types::DataVector, STI::Types::ValueVector, channelNames.at(i));
	}
}
void IVIScopeDevice::parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{
	double holdoff = 1e9;	// 1 second; the scope need to be put in aquisition mode at least 520 ms before the trigger
	double minimumAbsoluteStartTime = 0;
	double minimumEventSpacing = 0;

	double nextInitializationTime = 0;	//the next absolute time that the scope is free to run an initialization event

	RawEventMap::const_iterator events;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	IVIScopeEvent* scopeEvent;
	//IVIScopeInitializationEvent* initializationEvent;
	ChannelConfig* channelConfig;

	double lastTimePerRecord;
	double lastMinimumRecordLength;


	for(events = eventsIn.begin(); events != eventsIn.end(); events++)
	{
		if(events != eventsIn.begin())
		{
			events--;
			previousTime = events->first;
			events++;
		}
		else
			previousTime = minimumAbsoluteStartTime - minimumEventSpacing;
		
		eventTime = events->first - holdoff; //we can put events closer together than this, but they don't happen until 2*us later

		if( (events->first - minimumEventSpacing) < previousTime)
		{
			if(events != eventsIn.begin())
				throw EventParsingException(events->second.at(0),
						"The IVIScope needs " + valueToString(minimumEventSpacing) + " ns between events.");
			else
				throw EventParsingException(events->second.at(0),
						"The IVIScope needs " + valueToString(minimumAbsoluteStartTime)+ " ns at the beginning of the timing file.");
		}

		lastTimePerRecord = 0;
		lastMinimumRecordLength = 0;

		////temporarily(?) prevent simultaneous aquisitions on separate channels
		//if(events->second.size() > 1) {
		//	throw EventConflictException(events->second.at(0), events->second.at(1),
		//		"Only one channel can be measured at a time (for now...).");
		//}

		if(events->second.size() > 0) {
			
			scopeEvent = new IVIScopeEvent(eventTime, session, this);

			for(unsigned i = 0; i < events->second.size(); i++) {
				
				channelConfig = new ChannelConfig(
					events->second.at(i).channel(), 
					channelNames.at(events->second.at(i).channel() - 1), 
					events->second.at(i).value());

				if(i > 0 && (lastTimePerRecord != channelConfig->timePerRecord) && false) {
					throw EventConflictException(events->second.at(i - 1), events->second.at(i),
						"Inconsistent time bases for simultaneous channel waveforms. Time per record must be the same for both channels.");
				}
				if(i > 0 && (lastMinimumRecordLength != channelConfig->minimumRecordLength) && false) {
					throw EventConflictException(events->second.at(i - 1), events->second.at(i),
						"Inconsistent minimum record lengths for simultaneous channel waveforms. Record lengths must be the same for both channels.");
				}

				scopeEvent->addChannel(*channelConfig);
				delete channelConfig;

				scopeEvent->addMeasurement(events->second.at(i));
			}

			eventsOut.push_back(scopeEvent);
		}


//		for(unsigned i = 0; i < events->second.size(); i++) {
//			
//			channelConfig = new ChannelConfig(events->second.at(i).channel(), channelNames.at(events->second.at(i).channel() - 1), events->second.at(i).value());
//
//			if(i > 0 && (lastTimePerRecord != channelConfig->timePerRecord)) {
//				throw EventConflictException(events->second.at(i - 1), events->second.at(i),
//					"Inconsistent time bases for simultaneous channel waveforms. Time per record must be the same for both channels.");
//			}
//			if(i > 0 && (lastMinimumRecordLength != channelConfig->minimumRecordLength)) {
//				throw EventConflictException(events->second.at(i - 1), events->second.at(i),
//					"Inconsistent minimum record lengths for simultaneous channel waveforms. Record lengths must be the same for both channels.");
//			}
//
//			lastTimePerRecord = channelConfig->timePerRecord;
//			lastMinimumRecordLength = channelConfig->minimumRecordLength;
//
//			//initializationEvent = new IVIScopeInitializationEvent(nextInitializationTime, *channelConfig, session, this);
//
//			scopeEvent = new IVIScopeEvent(eventTime, *channelConfig, session, this);
////			scopeEvent->addChannel(channelConfig);
//		
//			delete channelConfig;
//
//			scopeEvent->addMeasurement(events->second.at(i));
//
//			//eventsOut.push_back(initializationEvent);
//			eventsOut.push_back(scopeEvent);
//		}

		//cannot attempt to initialize again until the scope is done recording this waveform
		nextInitializationTime = eventTime + channelConfig->timePerRecord;

	}


}

void IVIScopeDevice::IVIScopeInitializationEvent::playEvent()
{
	try {

		//checkViError( IviScope_ConfigureChannel(session, channelConfig.chName.c_str(), channelConfig.verticalScale, channelConfig.verticalOffset, IVISCOPE_VAL_DC, 1.0, VI_TRUE), "IviScope_ConfigureChannel");

		//checkViError( IviScope_ConfigureChanCharacteristics(session, channelConfig.chName.c_str(), channelConfig.probeAttenuation, 20.0e+6), "IviScope_ConfigureChanCharacteristics");

		////IviScope_ConfigureAcquisitionRecord (ViSession vi,ViReal64 timePerRecord, ViInt32 minimumRecordLength, ViReal64 acqStartTime);
		//checkViError( IviScope_ConfigureAcquisitionRecord(session, channelConfig.timePerRecord, channelConfig.minimumRecordLength, 0), "IviScope_ConfigureAcquisitionRecord");	//2e-1
	
	} catch(IVIScopeException& ex) {
		cout << "Error in IVIScopeInitializationEvent::playEvent():" << endl;
		cout << "IVI error code:  " << IVIgetError(ex.error, session) << endl;
		cout << "Function call:   " << ex.description << endl;
	}

}

void IVIScopeDevice::IVIScopeEvent::loadEvent()
{
	ViReal64 holdoff = 0.01;
	
	try {
		checkViError( 
			IviScope_ConfigureTrigger(
			session, IVISCOPE_VAL_EDGE_TRIGGER, holdoff), 
			"IviScope_ConfigureTrigger");

		checkViError( 
			IviScope_ConfigureTriggerCoupling(
			session, IVISCOPE_VAL_DC), 
			"IviScope_ConfigureTriggerCoupling");

		checkViError( 
			IviScope_ConfigureEdgeTriggerSource(
			session, iviScopeDevice_->triggerSource.c_str(), iviScopeDevice_->triggerLevel, iviScopeDevice_->triggerSlope),
			"IviScope_ConfigureEdgeTriggerSource");


	} catch(IVIScopeException& ex) {
		cout << "Error in IVIScopeEvent::loadEvent():" << endl;
		cout << "IVI error code:  " << IVIgetError(ex.error, session) << endl;
		cout << "Function call:   " << ex.description << endl;
	}
}

void IVIScopeDevice::IVIScopeEvent::playEvent()
{
//	ViStatus error = VI_SUCCESS;
	ViReal64 initialX = 0;
	ViReal64 incrementX = 0;
	ViInt32 actualRecordLength, actualPts;
	ViReal64* waveform = VI_NULL;
	
	ViInt32 maxTime_ms = 10000;

	MixedData vec;
	scopeData.clear();

	bool success = true;

	try {

		//initialize channels
		for(unsigned i = 0; i < channelConfigs.size(); i++) {
			
			checkViError(
				IviScope_ConfigureChannel(
				session, channelConfigs.at(i).chName.c_str(), 
				channelConfigs.at(i).verticalScale, 
				channelConfigs.at(i).verticalOffset, IVISCOPE_VAL_DC, 1.0, VI_TRUE), 
				"IviScope_ConfigureChannel");

			checkViError(
				IviScope_ConfigureChanCharacteristics(
				session, channelConfigs.at(i).chName.c_str(), channelConfigs.at(i).probeAttenuation, 20.0e+6), 
				"IviScope_ConfigureChanCharacteristics");

			//IviScope_ConfigureAcquisitionRecord (ViSession vi,ViReal64 timePerRecord, ViInt32 minimumRecordLength, ViReal64 acqStartTime);
			
			checkViError( 
				IviScope_ConfigureAcquisitionRecord(
				session, channelConfigs.at(i).timePerRecord, channelConfigs.at(i).minimumRecordLength, 0), 
				"IviScope_ConfigureAcquisitionRecord");	//2e-1
		}
		//end initialization

		//begin measurement
		checkViError( IviScope_ActualRecordLength(session, &actualRecordLength), "IviScope_ActualRecordLength");

		waveform = new ViReal64[actualRecordLength];	

		checkViError( IviScope_ReadWaveform (session, channelConfigs.at(0).chName.c_str(), 
			actualRecordLength,
			maxTime_ms, 
			waveform, 
			&actualPts,
			&initialX, 
			&incrementX), 
			"IviScope_ReadWaveform");

		for(unsigned j = 0; j < channelConfigs.size(); j++) {
			vec.clear();
			scopeData.push_back(vec);
			
			//The j = 0 channel was already aquired by the previous call to IviScope_ReadWaveform
			if(j > 0) {
				checkViError( 
					IviScope_FetchWaveform(
					session, channelConfigs.at(j).chName.c_str(), 
					actualRecordLength,
					waveform, 
					&actualPts,
					&initialX, 
					&incrementX), 
					"IviScope_FetchWaveform");
			}

			cout << "Sample Rate: " << (iviScopeDevice_->sampleRate) << endl;
			cout << "Actual sample rate: " << incrementX << endl;
			int downSample = (iviScopeDevice_->sampleRate)/incrementX;
			if (downSample < 1)
				downSample = 1;

			cout << "Downsample: " << downSample << endl;

			vec.clear();
			vec.addValue(string("TimeBase"));
			vec.addValue(static_cast<double>(incrementX) * downSample);
			scopeData.at(j).addValue(vec);

			vec.clear();
			vec.addValue(string("VerticalScale"));
			vec.addValue(channelConfigs.at(j).verticalScale);
			scopeData.at(j).addValue(vec);

			vec.clear();
			int numCombined = 0;
			ViReal64 runningTotal = 0;
			for(int k = 0; k < actualPts; k++) {
				runningTotal += waveform[k];
				numCombined++;
				if (numCombined == downSample)
				{
					vec.addValue( runningTotal / ((ViReal64) downSample));
					runningTotal = 0;
					numCombined = 0;
				}
			}

			cout << "Actual Points: " << actualPts << endl;
			cout << "Saved Points: " << vec.getVector().size() << endl;

/*			vec.clear();
			for(int k = 0; k < actualPts; k++) {
				vec.addValue( waveform[k] );
			}
*/			
			scopeData.at(j).addValue(vec);
		}

	} catch(IVIScopeException& ex) {
		success = false;
		cout << "Error in IVIScopeEvent::playEvent():" << endl;
		cout << "IVI error code:  " << IVIgetError(ex.error, session) << endl;
		cout << "Function call:   " << ex.description << endl;
	}


//	if(error != VI_SUCCESS)
//		cout << "IVI error code:  " << "Unknown!!" << endl;
}

void IVIScopeDevice::IVIScopeEvent::collectMeasurementData()
{
	for(unsigned i = 0; i < eventMeasurements.size() && i < scopeData.size(); i++) {
		eventMeasurements.at(i)->setData( scopeData.at(i) );
	}
}


IVIScopeDevice::ChannelConfig::ChannelConfig(unsigned short channel, std::string channelName, const MixedValue& value) 
: ch(channel), chName(channelName)
{
	parseValue(value);
}

void IVIScopeDevice::ChannelConfig::parseValue(const MixedValue& value)
{
	timePerRecord = value.getVector().at(0).getDouble();
	verticalScale = value.getVector().at(1).getDouble();
	verticalOffset = value.getVector().at(2).getDouble();
	minimumRecordLength = static_cast<int>( value.getVector().at(3).getDouble() );
//	probeAttenuation = value.getVector().at(3).getDouble();
	probeAttenuation = 1.0e6;
}
