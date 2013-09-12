// SimpleScope.cpp : main project file.

#include "cscope interface.h"
#include "Cscope Control Driver.h"
#include "SimpleForm.h"


#pragma unmanaged
#include "StartDevice.h"
#pragma managed


using namespace System;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;

using namespace SimpleScope;

//using namespace System::Threading::SynchronizationContext;
///using namespace System::ComponentModel::AsyncOperationManager;

#include "DeviceStarterWrapper.h"


[STAThreadAttribute]


int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	//Bridge class holding a pointer to access STI_Device hooks
	STIDeviceCLRBridgeWrapper* deviceWrapper = new STIDeviceCLRBridgeWrapper();

	// Create the main scope window
	SimpleForm^ simpleForm = gcnew SimpleForm(deviceWrapper);

	//Attempt to do callbacks correctly, using a delegate.  Works!
	SimpleScope::ManagedCallback^ managed = gcnew SimpleScope::ManagedCallback(simpleForm, &SimpleForm::Auto_Click_External);
    IntPtr unmanaged = Marshal::GetFunctionPointerForDelegate(managed);

	//Channel A Callback
	SimpleScope::ManagedCallback^ managedA = gcnew SimpleScope::ManagedCallback(simpleForm, &SimpleForm::AcquireAndReturnTraceChannelA);
    IntPtr unmanagedA = Marshal::GetFunctionPointerForDelegate(managedA);
	
	//Channel B Callback
	SimpleScope::ManagedCallback^ managedB = gcnew SimpleScope::ManagedCallback(simpleForm, &SimpleForm::AcquireAndReturnTraceChannelB);
    IntPtr unmanagedB = Marshal::GetFunctionPointerForDelegate(managedB);


	////Start device in new thread, with data!
	//DeviceStarterWrapper^ deviceStarterWrapper = gcnew DeviceStarterWrapper(unmanaged, unmanagedA, unmanagedB, deviceWrapper);
	//ThreadStart^ threadDelegate = gcnew ThreadStart(deviceStarterWrapper, &DeviceStarterWrapper::StartDeviceWithHandle );
	//Thread^ deviceThread = gcnew Thread(threadDelegate);
	//deviceThread->Start();


	//****Attempt to do a better job at respecting the UI thread...
	//Start device in new thread, with data!
	DeviceStarterWrapper2^ deviceStarterWrapper = gcnew DeviceStarterWrapper2(managed, managedA, managedB, deviceWrapper);
	ThreadStart^ threadDelegate = gcnew ThreadStart(deviceStarterWrapper, &DeviceStarterWrapper2::StartDeviceWithHandle );
	Thread^ deviceThread = gcnew Thread(threadDelegate);
	deviceThread->Start();


	//Run the Scope form
	Application::Run(simpleForm);

    GC::KeepAlive(managed);	//Need this so the garbage collector doesn't get rid of the callback delegate
    GC::KeepAlive(managedA);	//Need this so the garbage collector doesn't get rid of the callback delegate
    GC::KeepAlive(managedB);	//Need this so the garbage collector doesn't get rid of the callback delegate

	return 0;
}
