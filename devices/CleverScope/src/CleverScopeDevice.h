
#ifndef CLEVERSCOPEDEVICE_H
#define CLEVERSCOPEDEVICE_H

#include <STI_Device_Adapter.h>
#include "STIDeviceCLRBridge.h"

#include <sstream>
#include <boost/thread.hpp>

#include "CollectionMode.h"

//enum ScopeEvent {ScopeTriggerStateEvent, ScopeTriggerStateEvent}
//enum ScopeTriggerState {TriggerAuto, TriggerSingle, TriggerStop}

class CleverScopeDevice : public STI_Device_Adapter, public STIDeviceCLRBridge
{
public:

	CleverScopeDevice(ORBManager* orb_manager, std::string deviceName, 
		std::string IPAddress, unsigned short moduleNumber);
	CleverScopeDevice(ORBManager* orb_manager, std::string deviceName, 
		std::string IPAddress, unsigned short moduleNumber, void (*handle)(void));
	~CleverScopeDevice();

	void defineChannels();
	bool writeChannel(unsigned short channel, const MixedValue& value);
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);

	void defineAttributes();
	void refreshAttributes();
	bool updateAttribute(std::string key, std::string value);


	void addHandleChannelA(void (*handle)(void));
	void addHandleChannelB(void (*handle)(void));

	void deviceShutdown()
	{
		STI_Device::deviceShutdown();
	}
	virtual void stiError(char* message)
	{
		STI_Device::stiError(std::string(message));
	}

	void sendData(float* waveform, int length, short channel);
	
	void stopEventPlayback();


private:
	enum Channels {ChannelA = 0, ChannelB = 1};

	struct waveform_ptr
	{
		float* waveform;
		int length;
	};

	void getScopeDataFromPointer(MixedData& data, short channel);
	void initialize();
	bool waitForData();

	bool dataReceived;
	bool stopWaiting;

	waveform_ptr waveformA;
	waveform_ptr waveformB;

	void (*_handleChannelA)(void);
	void (*_handleChannelB)(void);
	void (*_handle)(void);

	mutable boost::shared_mutex waitForDataMutex;
	mutable boost::condition_variable_any waitForDataCondition;


	//Collection modes (thresholding and downsampling)
	STI::Scope::NormalMode<float, float, double> normalMode;
	STI::Scope::ThresholdModeUpper<float, float, double> thresholdModeUpper;
	STI::Scope::ThresholdModeLower<float, float, double> thresholdModeLower;

	typedef STI::Scope::CollectionMode<float, float, double> CleverCollectionMode;

	std::vector<CleverCollectionMode*> collectionModes;
	CleverCollectionMode* currentCollectionMode;



};


#endif

