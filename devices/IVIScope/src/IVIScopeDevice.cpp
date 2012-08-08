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
							std::string    Address, 
							unsigned short ModuleNumber) : 
STI_Device_Adapter(orb_manager, DeviceName, Address, ModuleNumber)
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

	try {
		checkViError(
			IviScope_InitWithOptions ("PitScope1", VI_FALSE, VI_FALSE, 
			"Simulate=0,RangeCheck=1,QueryInstrStatus=1,Cache=1", &session) );
		
		checkViError( 
			IviScope_ConfigureAcquisitionType(session, IVISCOPE_VAL_NORMAL) );
		
		checkViError( configureTrigger() );
		
		checkViError( configureChannels() );

	} catch(IVIScopeException& ex) {
		cout << "IVI error code:  " << IVIgetError(ex.error) << endl;
	}
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
	ViInt32 bufferSize = 0;
	
	channelNames.clear();

	do {
		error = IviScope_GetChannelName(session, index, bufferSize, name);

		if( name != VI_NULL && name != NULL ) {
			channelNames.push_back(name);
			name = VI_NULL;
			index ++;
		} else {
			abortCount--;
		}

	} while( error == VI_SUCCESS && abortCount > 1);

	if(channelNames.size() > 0)
		return VI_SUCCESS;
	else
		return error;
}

ViStatus IVIScopeDevice::configureTrigger()
{
	ViStatus error;
	
	error = IviScope_ConfigureEdgeTriggerSource(session, IVISCOPE_VAL_EXTERNAL, 1.0, IVISCOPE_VAL_POSITIVE);
	
	if(error != VI_SUCCESS ) return error;
	
	error = IviScope_ConfigureTriggerCoupling(session, IVISCOPE_VAL_DC);

	return error;
}

void IVIScopeDevice::checkViError(ViStatus error, std::string description = "") throw(IVIScopeException)
{
	if(error != VI_SUCCESS)
		throw IVIScopeException(error, description);
}

std::string IVIScopeDevice::IVIgetError(ViStatus error)
{
	ViChar errStr[2048];
	IviScope_GetError(session, &error, 2048, errStr);
	string str = errStr;
	return str;
}

void IVIScopeDevice::defineChannels()
{
	for(unsigned i = 0; i < channelNames.size(); i++) {
		addInputChannel(i + 1, STI::Types::DataVector, STI::Types::ValueVector, "CH" + STI::Utils::valueToString(i+1));
	}
}
void IVIScopeDevice::parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut)
throw(std::exception)
{
	double minimumAbsoluteStartTime = 0;
	double minimumEventSpacing = 0;
	double holdoff = 0;

	RawEventMap::const_iterator events;

	double eventTime; //time when the FPGA should trigger in order to have the output ready in time
	double previousTime; //time when the previous event occurred

	IVIScopeEvent* scopeEvent;
	ChannelConfig* channelConfig;

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

		channelConfig = new ChannelConfig(events->second.at(0).channel(), channelNames.at(events->second.at(0).channel() - 1), events->second.at(0).value());

		scopeEvent = new IVIScopeEvent(eventTime, *channelConfig, session, this);
		
		delete channelConfig;

		scopeEvent->addMeasurement(events->second.at(0));
		eventsOut.push_back(scopeEvent);

//		events->second.at(0).value()
	}


}

void IVIScopeDevice::IVIScopeInitializationEvent::playEvent()
{
}

void IVIScopeDevice::IVIScopeEvent::playEvent()
{
	ViStatus error = VI_SUCCESS;
	ViReal64 initialX, incrementX;
	ViInt32 actualRecordLength, actualPts;
	ViReal64* waveform = VI_NULL;
	
 	if(error == VI_SUCCESS)
		error = IviScope_ConfigureChannel(session, "CH1", channelConfig.verticalScale, channelConfig.verticalOffset, IVISCOPE_VAL_DC, 1.0, VI_TRUE);

	if(error == VI_SUCCESS)
		error = IviScope_ConfigureChanCharacteristics(session, "CH1", channelConfig.probeAttenuation, 20.0e+6);

	if(error == VI_SUCCESS)
		//IviScope_ConfigureAcquisitionRecord (ViSession vi,ViReal64 timePerRecord, ViInt32 minimumRecordLength, ViReal64 acqStartTime);
		error = IviScope_ConfigureAcquisitionRecord(session, channelConfig.timePerRecord, 1000, 0);	//2e-1

	if(error == VI_SUCCESS)
		error = IviScope_ActualRecordLength(session, &actualRecordLength);
	
	if(error == VI_SUCCESS)
		waveform = new ViReal64[actualRecordLength];	

	ViInt32 maxTime_ms = 10000;

	if(error == VI_SUCCESS)
		error = IviScope_ReadWaveform (session, 
                                         "CH1", 
                                         actualRecordLength,
                                         maxTime_ms, 
                                         waveform, 
                                         &actualPts,
                                         &initialX, 
                                         &incrementX);

	MixedData vec;
	vec.addValue(string("TimeBase"));
	vec.addValue(static_cast<double>(incrementX));
	scopeData.addValue(vec);

	vec.clear();
	vec.addValue(string("VerticalScale"));
	vec.addValue(10.0);
	scopeData.addValue(vec);

	vec.clear();
	if(error == VI_SUCCESS) {
		for(int i = 0; i < actualPts; i++) {
			vec.addValue( waveform[i] );
		}
	}
	scopeData.addValue(vec);

	if(error != VI_SUCCESS)
		cout << "IVI error code:  " << "Unknown!!" << endl;
}

void IVIScopeDevice::IVIScopeEvent::collectMeasurementData()
{
	eventMeasurements.at(0)->setData( scopeData );
}

IVIScopeDevice::ChannelConfig::ChannelConfig(unsigned short channel, const MixedValue& value) 
: ch(channel), chName(channelName)
{
	parseValue(value);
}

void IVIScopeDevice::ChannelConfig::parseValue(const MixedValue& value)
{
	timePerRecord = value.getVector().at(0).getDouble();
	verticalScale = value.getVector().at(1).getDouble();
	verticalOffset = value.getVector().at(2).getDouble();
//	probeAttenuation = value.getVector().at(3).getDouble();
	probeAttenuation = 1.0e6;
}
