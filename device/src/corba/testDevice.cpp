#ifdef _MSC_VER
#  pragma warning( disable : 4786 ) // ...identifier was truncated to '255' 
                                    // characters in the browser information
#endif


#include "testDevice.h"

#include "STI_Device.h"

#include <string>
#include <map>
using std::string;
using std::map;

#include <iostream>
using namespace std;



//**********Test********//

std::string testDevice::deviceType() 
{
	return "Temp Device";
}

void testDevice::defineAttributes() 
{
	const char *static_arr[] = {"one", "two", "three"};
	attributes["key1"] = Attribute("test device attribute", static_arr);

	attributes["key2"] = Attribute("attribute#2", "2, 5, none, full open, true");

}

bool testDevice::updateAttribute(string key, string value)
{
	return true;
}

void testDevice::defineChannels()
{
	addChannel(22, Output, DataNone, ValueNumber);
	addOutputChannel(57, ValueNumber);
	addOutputChannel(58, ValueNumber);
}

bool testDevice::deviceMain()
{
	int x;
	cin >> x;

	ServerConfigureRef->removeDevice(tDeviceID->deviceID);

	return true;
}