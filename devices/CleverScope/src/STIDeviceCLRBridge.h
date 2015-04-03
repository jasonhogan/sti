#ifndef STIDEVICECLRBRIDGE_H
#define STIDEVICECLRBRIDGE_H


class STIDeviceCLRBridge
{
public:

	virtual void stiError(char* message) = 0;
	virtual void deviceShutdown() = 0;

	virtual void sendData(float* waveform, int length, short channel, double timebase, double verticalScale, double verticalOffset) = 0;
};


class STIDeviceCLRBridgeWrapper
{
public:

	STIDeviceCLRBridgeWrapper() : deviceBridge_ptr(0) {}
	
	void setDeviceBridge(STIDeviceCLRBridge* bridge)
	{
		deviceBridge_ptr = bridge;
	}

	void stiError(char* message)
	{
		if(deviceBridge_ptr == 0) {
			return;
		}
		deviceBridge_ptr->stiError(message);
	}

	void deviceShutdown()
	{
		if(deviceBridge_ptr == 0) {
			return;
		}
		deviceBridge_ptr->deviceShutdown();
	}
	void sendData(float* waveform, int length, short channel, double timebase, double verticalScale, double verticalOffset)
	{
		if(deviceBridge_ptr == 0) {
			return;
		}
		deviceBridge_ptr->sendData(waveform, length, channel, timebase, verticalScale, verticalOffset);
	}
private:
	
	STIDeviceCLRBridge* deviceBridge_ptr;
};


#endif
