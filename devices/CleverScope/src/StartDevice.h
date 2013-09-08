
#ifndef STARTDEVICE_H
#define STARTDEVICE_H

#ifdef DLLDIR_EX
   #define DLLDIR  __declspec(dllexport)   // export DLL information
 
#else
   #define DLLDIR  __declspec(dllimport)   // import DLL information
 
#endif 

#include "STIDeviceCLRBridge.h"


typedef void (* UnmanagedCallback)(void);

void DLLDIR makeAndRunDevice();
void DLLDIR makeAndRunDeviceWithHandle(UnmanagedCallback callback,
									   UnmanagedCallback callbackA, UnmanagedCallback callbackB, 
									   STIDeviceCLRBridgeWrapper* _deviceWrapper);

#endif

