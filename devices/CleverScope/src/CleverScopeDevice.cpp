
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

	//Setup collection modes
	normalMode.mode = "Normal Mode (N/A)";
	normalMode.modeParameter = 0;
	thresholdModeUpper.mode = "Threshold Mode > (Volts)";
	thresholdModeUpper.modeParameter = 0;
	thresholdModeLower.mode = "Threshold Mode < (Volts)";
	thresholdModeLower.modeParameter = 0;

	collectionModes.push_back(&normalMode);
	collectionModes.push_back(&thresholdModeUpper);
	collectionModes.push_back(&thresholdModeLower);

	currentCollectionMode = collectionModes.at(0);

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

void CleverScopeDevice::defineAttributes()
{
	std::string collectionModeOptions = "";
	std::vector <CleverCollectionMode*>::iterator it;
	for (it = collectionModes.begin(); it != collectionModes.end(); it++)
	{
		if (it != collectionModes.begin())
			collectionModeOptions += ", ";
		collectionModeOptions += (*it)->mode;
	}

	addAttribute("Data collection mode", currentCollectionMode->mode, collectionModeOptions);
	addAttribute("Mode parameter", currentCollectionMode->modeParameter);
}

void CleverScopeDevice::refreshAttributes()
{
	setAttribute("Data collection mode", currentCollectionMode->mode);
	setAttribute("Mode parameter", currentCollectionMode->modeParameter);
}

bool CleverScopeDevice::updateAttribute(std::string key, std::string value)
{
	bool success = false;
	
	if (key.compare("Data collection mode") == 0)
	{
		std::vector <CleverCollectionMode*>::iterator it;
		for (it = collectionModes.begin(); it != collectionModes.end(); it++)
		{
			if ((*it)->mode.compare(value) == 0)
			{
				currentCollectionMode = (*it);
				success = true;
			}
		}
	}
	else if (key.compare("Mode parameter") == 0)
	{
		double tempDouble;
		if (STI::Utils::stringToValue(value, tempDouble))
		{
			currentCollectionMode->modeParameter = tempDouble;
			success = true;
		}
	}

	return success;
}

void CleverScopeDevice::defineChannels()
{
//	addOutputChannel(2, ValueNone, "Handle");
	addInputChannel(ChannelA, DataVector, ValueNone, "A");
	addInputChannel(ChannelB, DataVector, ValueNone, "B");
}

bool CleverScopeDevice::writeChannel(unsigned short channel, const MixedValue& value)
{
//	_handle();
	return true;
}

bool CleverScopeDevice::readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut)
{
	bool success = false;

	if(channel == ChannelA) { //Channel A
		dataReceived = false;
		_handleChannelA();
		success = waitForData();
		if(success) {
//			MixedData temp;
//			getScopeDataFromPointer(temp, channel);
			getScopeDataFromPointer(dataOut, channel);
			
//			dataOut.addValue(1);
//			dataOut.addValue(2);

			std::stringstream message;
//			message << "Converted data: " << dataOut.getVector().size();
			message << "Converted data: " << dataOut.print();
			STI_Device::stiError(message.str());
		}
	}
	if(channel == ChannelB) { //Channel B
		dataReceived = false;
		_handleChannelB();
		success = waitForData();
		if(success) {
			getScopeDataFromPointer(dataOut, channel);
		}
	}

	return success;
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

void CleverScopeDevice::sendData(float* waveform, int length, short channel)
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
		break;
	case ChannelB:
		waveformB.waveform = waveform;
		waveformB.length = length;
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
	double incrementX = 1;
	double verticalScale = 1;

	data.clear();
	
	//TimeBase
	data.addValue(MixedData());
	data.getValueAt(0).addValue(std::string("TimeBase"));
	data.getValueAt(0).addValue(incrementX * downSample);

	//VerticalScale
	data.addValue(MixedData());
	data.getValueAt(1).addValue(std::string("VerticalScale"));
	data.getValueAt(1).addValue(verticalScale);

	float* waveform;
	int length;

	switch(channel)
	{
	case ChannelA:
		waveform = waveformA.waveform;
		length = waveformA.length;
		break;
	case ChannelB:
		waveform = waveformB.waveform;
		length = waveformB.length;
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

	currentCollectionMode->processData(data.getValueAt(2), waveform, length, static_cast<float>(incrementX), downSample);
}
