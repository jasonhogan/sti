
#ifndef DEVICESTARTERWRAPPER_H
#define DEVICESTARTERWRAPPER_H

ref class DeviceStarterWrapper
{
public:

	DeviceStarterWrapper(IntPtr unmanaged, IntPtr channelAunmanaged, IntPtr channelBunmanaged, 
						STIDeviceCLRBridgeWrapper* deviceWrapper) : 
		unmanaged_ptr(unmanaged), 
		channelA_unmanaged_ptr(channelAunmanaged), 
		channelB_unmanaged_ptr(channelBunmanaged), 
		_deviceWrapper(deviceWrapper)
	  {
		  syncContext.SetSynchronizationContext(AsyncOperationManager::SynchronizationContext);
	  }

	  void StartDeviceWithHandle()
	  {
		  makeAndRunDeviceWithHandle((UnmanagedCallback)(void*)unmanaged_ptr, 
			  (UnmanagedCallback)(void*)channelA_unmanaged_ptr, (UnmanagedCallback)(void*)channelB_unmanaged_ptr,
			  _deviceWrapper);
	  }

private:

	IntPtr unmanaged_ptr;
	STIDeviceCLRBridgeWrapper* _deviceWrapper;

	IntPtr channelA_unmanaged_ptr;
	IntPtr channelB_unmanaged_ptr;


private: System::Threading::SynchronizationContext syncContext;

};


#endif
