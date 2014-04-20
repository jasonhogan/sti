
//*** Minimum requirements for making a new STI Device ****//

//	(1) Make the device header file to define your device class.
//			* Follow this file as a template.
//			* The device class definition should be in a new *.h file with the same name as your class. 
//			For example, if your device is call MyDevice, then the header file would be MyDevice.h
//			* Make sure to use "include gaurds" around your code in the header file (at the very top and very bottom of the file).
//			These are a set of #ifndef...#define...#endif preprocessor commands that prevent multiple includes by defining a unique macro name.
//			As a naming convention, start the macro with "STI_". For example, for the MyDevice class the macro name would be STI_MYDEVICE_H
//			Use all capitals and make sure the macro name is unique!
//	(2) Make the device cpp file.
//			* This is where you implement your device. This is the code that gets run when STI function hooks (like writeChannel, etc.) get called.
//			* The device cpp filename should match the header file. For example, for MyDevice.h you would write MyDevice.cpp.
//			* #include your device's header file at the top.
//			* Implement all your device class's functions.
//	(3) Make a main.cpp file.
//			* This defines the entry point for the executable containing your device.
//			* Inside the main(...) function, create an instance of an ORBManager and an instance of your device class.
//			* At the end of main(...), call the ORBManager's run() command.
//


#ifndef STI_EXAMPLE_DEVICE_H
#define STI_EXAMPLE_DEVICE_H

#include <STI_Device_Adapter.h>

class ExampleDevice : public STI_Device_Adapter
{
public:

	ExampleDevice(ORBManager* orb_manager, std::string configFilename);

};

#endif

