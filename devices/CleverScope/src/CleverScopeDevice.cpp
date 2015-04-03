
#include "CleverScopeDevice.h"


CleverScopeDevice::CleverScopeDevice(ORBManager* orb_manager, std::string deviceName, 
				  std::string IPAddress, unsigned short moduleNumber) : 
STI_Device_Adapter(orb_manager, deviceName, IPAddress, moduleNumber)
{
	initialize();
}
CleverScopeDevice::CleverScopeDevice(ORBManager* orb_manager, std::string deviceName, 
				  std::string IPAddress, unsigned short moduleNumber, void (*handle)(void)) : 
STI_Device_Adapter(orb_manager, deviceName, IPAddress, moduleNumber), _handle(handle)
{
	initialize();
}


void CleverScopeDevice::initialize()
{
	dataReceived = false;
	stopWaiting = false;

	//Setup collection modes, channel A
	normalModeA.mode = "Normal Mode (N/A)";
	normalModeA.modeParameter = 0;
	thresholdModeUpperA.mode = "Threshold Mode > (Volts)";
	thresholdModeUpperA.modeParameter = 0;
	thresholdModeLowerA.mode = "Threshold Mode < (Volts)";
	thresholdModeLowerA.modeParameter = 0;

	collectionModesA.push_back(&normalModeA);
	collectionModesA.push_back(&thresholdModeUpperA);
	collectionModesA.push_back(&thresholdModeLowerA);

	currentCollectionModeA = collectionModesA.at(0);

	//Setup collection modes, channel B
	normalModeB.mode = "Normal Mode (N/A)";
	normalModeB.modeParameter = 0;
	thresholdModeUpperB.mode = "Threshold Mode > (Volts)";
	thresholdModeUpperB.modeParameter = 0;
	thresholdModeLowerB.mode = "Threshold Mode < (Volts)";
	thresholdModeLowerB.modeParameter = 0;

	collectionModesB.push_back(&normalModeB);
	collectionModesB.push_back(&thresholdModeUpperB);
	collectionModesB.push_back(&thresholdModeLowerB);

	currentCollectionModeB = collectionModesB.at(0);

}

CleverScopeDevice::~CleverScopeDevice() 
{
}

void CleverScopeDevice::addHandleChannelA(void (*handle)(void))
{
	_handleChannelA = handle;
}

void CleverScopeDevice::addHandleChannelB(void (*handle)(void))
{
	_handleChannelB = handle;
}

void CleverScopeDevice::addHandleTrigger(void (*handle)(void))
{
	_handleTrigger = handle;
}

void CleverScopeDevice::defineAttributes()
{
	std::string collectionModeOptions = "";
	std::vector <CleverCollectionMode*>::iterator it;
	for (it = collectionModesA.begin(); it != collectionModesA.end(); it++)
	{
		if (it != collectionModesA.begin())
			collectionModeOptions += ", ";
		collectionModeOptions += (*it)->mode;
	}

	addAttribute("(A) Data collection mode", currentCollectionModeA->mode, collectionModeOptions);
	addAttribute("(A) Mode parameter", currentCollectionModeA->modeParameter);


	collectionModeOptions = "";
	for (it = collectionModesB.begin(); it != collectionModesB.end(); it++)
	{
		if (it != collectionModesB.begin())
			collectionModeOptions += ", ";
		collectionModeOptions += (*it)->mode;
	}

	addAttribute("(B) Data collection mode", currentCollectionModeB->mode, collectionModeOptions);
	addAttribute("(B) Mode parameter", currentCollectionModeB->modeParameter);
}

void CleverScopeDevice::refreshAttributes()
{
	setAttribute("(A) Data collection mode", currentCollectionModeA->mode);
	setAttribute("(A) Mode parameter", currentCollectionModeA->modeParameter);

	setAttribute("(B) Data collection mode", currentCollectionModeB->mode);
	setAttribute("(B) Mode parameter", currentCollectionModeB->modeParameter);
}

bool CleverScopeDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	
	if (key.compare("(A) Data collection mode") == 0)
	{
		std::vector <CleverCollectionMode*>::iterator it;
		for (it = collectionModesA.begin(); it != collectionModesA.end(); it++)
		{
			if ((*it)->mode.compare(value) == 0)
			{
				currentCollectionModeA = (*it);
				success = true;
			}
		}
	}
	else if (key.compare("(A) Mode parameter") == 0)
	{
		double tempDouble;
		if (STI::Utils::stringToValue(value, tempDouble))
		{
			currentCollectionModeA->modeParameter = tempDouble;
			success = true;
		}
	}
	else if (key.compare("(B) Data collection mode") == 0)
	{
		std::vector <CleverCollectionMode*>::iterator it;
		for (it = collectionModesB.begin(); it != collectionModesB.end(); it++)
		{
			if ((*it)->mode.compare(value) == 0)
			{
				currentCollectionModeB = (*it);
				success = true;
			}
		}
	}
	else if (key.compare("(B) Mode parameter") == 0)
	{
		double tempDouble;
		if (STI::Utils::stringToValue(value, tempDouble))
		{
			currentCollectionModeB->modeParameter = tempDouble;
			success = true;
		}
	}

	return success;
}

void CleverScopeDevice::defineChannels()
{
//	addOutputChannel(2, ValueNone, "Handle");
	addInputChannel(ChannelA, DataVector, ValueVector, "A");
	addInputChannel(ChannelB, DataVector, ValueVector, "B");
}

bool CleverScopeDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
//	_handle();
	return true;
}

bool CleverScopeDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	MixedValue newValueIn;
	
	

	if (valueIn.getType() == MixedValue::Empty)
	{
		newValueIn.addValue(0);
		newValueIn.addValue(0);

		return readChannelDefault(channel, newValueIn, dataOut);
	}
	else 
		return readChannelDefault(channel, valueIn, dataOut);

	

	/*
		bool success = false;

	if(channel == ChannelA) { //Channel A
		dataReceived = false;
		_handleChannelA();

		bool waitForNewData = true;
		if (valueIn.getVector().size() == 1)
		{
			waitForNewData = valueIn.getVector().at(0).getBoolean();
		}

		if (waitForNewData)
			success = waitForData();
		else
			success = true;

		if(success) {
//			MixedData temp;
//			getScopeDataFromPointer(temp, channel);
			getScopeDataFromPointer(dataOut, channel);
			
//			dataOut.addValue(1);
//			dataOut.addValue(2);

			std::stringstream message;
//			message << "Converted data: " << dataOut.getVector().size();
//			message << "Converted data: " << dataOut.print();
			STI_Device::stiError(message.str());
		}
	}
	if(channel == ChannelB) { //Channel B
		dataReceived = false;
		_handleChannelB();

		bool waitForNewData = true;
		if (valueIn.getVector().size() == 1)
		{
			waitForNewData = valueIn.getVector().at(0).getBoolean();
		}

		if (waitForNewData)
			success = waitForData();
		else
			success = true;


		success = waitForData();
		if(success) {
			getScopeDataFromPointer(dataOut, channel);

			std::stringstream message;
			STI_Device::stiError(message.str());
		}
	}

	return success;
	*/
}

void CleverScopeDevice::parseDeviceEvents(const RawEventMap &eventsIn, SynchronousEventVector& eventsOut) throw (std::exception)
{
	RawEventMap::const_iterator iter;
	

	std::map<unsigned short, RawEvent> previousChannels;
	std::map<unsigned short, RawEvent>::const_iterator channelMapIter;


	for(iter = eventsIn.begin(); iter != eventsIn.end(); iter++)
	{

		STI_Device::stiError("Parsing...");
		std::vector<RawEvent>::const_iterator rawEvent;

		double eventTime = iter->first;

		previousChannels.clear();

		for(rawEvent = iter->second.begin(); rawEvent != iter->second.end(); rawEvent++)
		{

			//Check that if events are simultaneous, that they happen on different channels
			if (rawEvent != iter->second.begin())
			{
				channelMapIter = previousChannels.find(rawEvent->channel());
				if (channelMapIter != previousChannels.end())
				{
					throw EventConflictException(channelMapIter->second, *rawEvent,
						"The CleverScope allows only one event on the same channel at a time.");
				}
			}
			previousChannels.insert(pair<unsigned short, RawEvent>(rawEvent->channel(), *rawEvent));

			/*
			if (rawEvent == iter->second.begin())
				isFirstSimultaneousEvent = true;
			else
				isFirstSimultaneousEvent = false;


			rawEvent++;
			if (rawEvent == iter->second.end())
				isLastSimultaneousEvent = true;
			else
				isLastSimultaneousEvent = false;
			rawEvent--;
*/

		}
		
		eventsOut.push_back( 
			new CleverScopeDevice::CleverScopeEvent(eventTime, this) );

		//cycle back through once more to add measurements in same order as measurement Channels.
		for (rawEvent = iter->second.begin(); rawEvent != iter->second.end(); rawEvent++)
		{
			if( rawEvent->isMeasurementEvent() )	// measurement event
					eventsOut.back().addMeasurement( *rawEvent );
		}
	}
	STI_Device::stiError("...Done Parsing");
}

void CleverScopeDevice::CleverScopeEvent::playEvent()
{

	cleverScopeDevice->dataReceived = false;

	for (unsigned int i = 0; i < eventMeasurements.size(); i++)
	{
		if(eventMeasurements.at(i)->channel() == ChannelA) { //Channel A
			cleverScopeDevice->_handleChannelA();
		}
		if(eventMeasurements.at(i)->channel() == ChannelB) { //Channel B
			cleverScopeDevice->_handleChannelB();
		}
	}

	cleverScopeDevice->_handleTrigger();

}

void CleverScopeDevice::CleverScopeEvent::collectMeasurementData()
{
	MixedData dataOut;
	bool success;

	success = cleverScopeDevice->waitForData();

	if (success)
	{
		for (unsigned int i = 0; i < eventMeasurements.size(); i++)
		{
			cleverScopeDevice->getScopeDataFromPointer(dataOut, eventMeasurements.at(i)->channel());
			eventMeasurements.at(i)->setData(dataOut);
		}
	}

	std::stringstream message;
	cleverScopeDevice->stiErrorStr(message.str());
}

void CleverScopeDevice::stopEventPlayback()
{
	boost::unique_lock< boost::shared_mutex > lock(waitForDataMutex);

	stopWaiting = true;
	waitForDataCondition.notify_all();
}


bool CleverScopeDevice::waitForData()
{
	boost::unique_lock< boost::shared_mutex > lock(waitForDataMutex);
	stopWaiting = false;

	double timeout_ms = 2000;

	boost::system_time wakeTime;
	
	STI_Device::stiError("waitForData()");
	
	//Wait for data to come it or for a "stop" call
	while( !dataReceived && !stopWaiting ) {
		
		wakeTime = boost::get_system_time()
			+ boost::posix_time::milliseconds( static_cast<long>(timeout_ms) );

		waitForDataCondition.timed_wait(lock, wakeTime);
		STI_Device::stiError("sleep...");
//		dataReceived = true;
	}

	return dataReceived;
}

//Called by SimpleForm.h
void CleverScopeDevice::sendData(float* waveform, int length, short channel, double timebase, double verticalScale, double verticalOffset)
{
	//std::stringstream data;
	//for(int i = 0; i < length; i++) {
	//	data << waveform[i] << ",";
	//}
	//data << "END";
	//STI_Device::stiError(data.str());

	//MixedData scopeData;
	//for(int i = 0; i < length; i++) {
	//	scopeData.addValue(waveform[i]);
	//}

	//STI_Device::stiError(scopeData.print());
	
	
	switch(channel)
	{
	case ChannelA:
		waveformA.waveform = waveform;
		waveformA.length = length;
		waveformA.timebase = timebase;
		waveformA.verticalScale = verticalScale;
		waveformA.verticalOffset = verticalOffset;
		break;
	case ChannelB:
		waveformB.waveform = waveform;
		waveformB.length = length;
		waveformB.timebase = timebase;
		waveformB.verticalScale = verticalScale;
		waveformB.verticalOffset = verticalOffset;
		break;
	default:
		//Error; bad channel number
		dataReceived = false;
		stopWaiting = false;
		break;
	}

	boost::unique_lock< boost::shared_mutex > lock(waitForDataMutex);
	dataReceived = true;
	waitForDataCondition.notify_all();

	std::stringstream message;
	message << "Got Data: " << length;
	STI_Device::stiError(message.str());

}

void CleverScopeDevice::getScopeDataFromPointer(MixedData& data, short channel)
{
	unsigned downSample = 1;

	data.clear();

	float* waveform;
	int length;
	double timebase = 1;

	switch(channel)
	{
	case ChannelA:
		waveform = waveformA.waveform;
		length = waveformA.length;

		//TimeBase
		data.addValue(MixedData());
		data.getValueAt(0).addValue(std::string("TimeBase"));
		timebase = waveformA.timebase;
		data.getValueAt(0).addValue(timebase * downSample);

		//VerticalScale
		data.addValue(MixedData());
		data.getValueAt(1).addValue(std::string("VerticalScale"));
		data.getValueAt(1).addValue(waveformA.verticalScale);

		//VerticalOffset
		data.addValue(MixedData());
		data.getValueAt(2).addValue(std::string("VerticalOffset"));
		data.getValueAt(2).addValue(waveformA.verticalOffset);

		break;
	case ChannelB:
		waveform = waveformB.waveform;
		length = waveformB.length;

		//TimeBase
		data.addValue(MixedData());
		data.getValueAt(0).addValue(std::string("TimeBase"));
		timebase = waveformB.timebase;
		data.getValueAt(0).addValue(timebase * downSample);

		//VerticalScale
		data.addValue(MixedData());
		data.getValueAt(1).addValue(std::string("VerticalScale"));
		data.getValueAt(1).addValue(waveformB.verticalScale);

		//VerticalOffset
		data.addValue(MixedData());
		data.getValueAt(2).addValue(std::string("VerticalOffset"));
		data.getValueAt(2).addValue(waveformB.verticalOffset);

		break;
	default:
		//Error; invalid channel
		data.addValue("Invalid Channel");
		return;
	}

	//for(int i = 0; i < length; i++) {
	//	data.addValue(static_cast<double>(waveform[i]));
	//}

	data.addValue(MixedData());
	
	//std::stringstream message;
	//message << "MixedData length: " << data.getVector().size() << endl;
	//STI_Device::stiError(message.str());

	switch(channel)
	{
	case ChannelA:
		currentCollectionModeA->setYOffset(waveformA.verticalOffset);
		currentCollectionModeA->processData(data.getValueAt(3), waveform, length, static_cast<float>(timebase), downSample);
		break;
	case ChannelB:
		currentCollectionModeB->setYOffset(waveformB.verticalOffset);
		currentCollectionModeB->processData(data.getValueAt(3), waveform, length, static_cast<float>(timebase), downSample);
		break;
	default:
		return;
	}
	
}
