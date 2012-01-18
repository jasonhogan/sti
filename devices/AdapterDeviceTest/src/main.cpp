
#include <ORBManager.h>

#include <STI_Device_Adapter.h>

ORBManager* orbManager;


class MyDevice : public STI_Device_Adapter
{
public:

	MyDevice(ORBManager* orb_manager, std::string DeviceName, 
		std::string IPAddress, unsigned short ModuleNumber) 
		: STI_Device_Adapter(orb_manager, DeviceName, IPAddress, ModuleNumber) {}

	void defineAttributes()
	{
		addAttribute("test", 5, "5,6,8");
	}
};


int main(int argc, char* argv[])
{
	orbManager = new ORBManager(argc, argv);    

	MyDevice myDevice(orbManager, "My Device Adapter", "epcamera.stanford.edu", 68);

	orbManager->run();

	return 0;
}

