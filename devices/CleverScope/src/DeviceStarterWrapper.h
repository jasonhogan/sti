
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
//		  syncContext = AsyncOperationManager.SynchronizationContext;
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


ref class DeviceStarterWrapper2
{
public:
	
	DeviceStarterWrapper2(
						  SimpleScope::ManagedCallback^ managed, 
						  SimpleScope::ManagedCallback^ channelAmanaged, 
						  SimpleScope::ManagedCallback^ channelBmanaged, 
						  STIDeviceCLRBridgeWrapper* deviceWrapper) : 		
		SSmanagedU(managed), 
		SSmanagedA(channelAmanaged), 
		SSmanagedB(channelBmanaged), 
		_deviceWrapper(deviceWrapper)
	  {
		  syncContext.SetSynchronizationContext(AsyncOperationManager::SynchronizationContext);
//		  syncContext = AsyncOperationManager.SynchronizationContext;

		  //Make managed pointers to member functions.
		  managedU = gcnew SimpleScope::ManagedCallback(this, &DeviceStarterWrapper2::ManagedUCallback);
		  managedA = gcnew SimpleScope::ManagedCallback(this, &DeviceStarterWrapper2::ManagedACallback);
		  managedB = gcnew SimpleScope::ManagedCallback(this, &DeviceStarterWrapper2::ManagedBCallback);

		  //These unmanged pointers point to member functions in this object, not to functions on the UI thread.
	      unmanaged_ptr = Marshal::GetFunctionPointerForDelegate(managedU);
	      channelA_unmanaged_ptr = Marshal::GetFunctionPointerForDelegate(managedA);
	      channelB_unmanaged_ptr = Marshal::GetFunctionPointerForDelegate(managedB);
	  }

	  void StartDeviceWithHandle()
	  {
		  //Give device pointers to functions in this object which run in a different thread from the UI.
		  makeAndRunDeviceWithHandle((UnmanagedCallback)(void*)unmanaged_ptr, 
			  (UnmanagedCallback)(void*)channelA_unmanaged_ptr, (UnmanagedCallback)(void*)channelB_unmanaged_ptr,
			  _deviceWrapper);
	  }
private: void ManagedUCallback2(System::Object^)	//like Run()
		 {
			 SSmanagedU();
		 }
private: void ManagedUCallback()	//like Run()
	{
		System::Object^ args = nullptr;
		SendOrPostCallback^ callback = gcnew SendOrPostCallback(this, &DeviceStarterWrapper2::ManagedUCallback2);
		syncContext.Post(callback, args);
	}

private: void ManagedACallback2(System::Object^)	//like Run()
		 {
			 SSmanagedA();
		 }
private: void ManagedACallback()	//like Run()
	{
		System::Object^ args = nullptr;
		SendOrPostCallback^ callback = gcnew SendOrPostCallback(this, &DeviceStarterWrapper2::ManagedACallback2);
		syncContext.Post(callback, args);
	}
private: void ManagedBCallback2(System::Object^)	//like Run()
		 {
			 SSmanagedB();
		 }
private: void ManagedBCallback()	//like Run()
	{
		System::Object^ args = nullptr;
		SendOrPostCallback^ callback = gcnew SendOrPostCallback(this, &DeviceStarterWrapper2::ManagedBCallback2);
		syncContext.Post(callback, args);
	}

private:

	//Point to member functions; redirect in thread safe way to UI thread.
	SimpleScope::ManagedCallback^ managedU;
	SimpleScope::ManagedCallback^ managedA;
	SimpleScope::ManagedCallback^ managedB;

	//Point to functions that run on the UI thread
	SimpleScope::ManagedCallback^ SSmanagedU;
	SimpleScope::ManagedCallback^ SSmanagedA; 
	SimpleScope::ManagedCallback^ SSmanagedB;

	//Unmanaged pointers to member functions.
	IntPtr unmanaged_ptr;
	IntPtr channelA_unmanaged_ptr;
	IntPtr channelB_unmanaged_ptr;

	STIDeviceCLRBridgeWrapper* _deviceWrapper;

private: System::Threading::SynchronizationContext syncContext;

};


#endif
