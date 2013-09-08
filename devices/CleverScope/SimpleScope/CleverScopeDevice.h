
#ifndef CLEVERSCOPEDEVICE
#define CLEVERSCOPEDEVICE

#include <STI_Device_Adapter.h>

class CleverScopeDevice : public STI_Device_Adapter
{
public:

	CleverScopeDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber);
	~CleverScopeDevice();
};


#endif

