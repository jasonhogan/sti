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

	attributes["BiasVoltage"] = Attribute("1.2", "");
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

//	addStreamingChannel(2, Enabled, 100 ms, 50);

//	Attributes
//	Ch_2_Streaming
//	Ch_2_SamplePeriod
//	Ch_2_DataBuffer

}

bool testDevice::deviceMain()
{


	int temp;
	double temp2;
	string jason = "a23.4e-6";
	
	cerr << "string to value: " << stringToValue("23.4e-6", temp2) << " -> ";
	cerr << temp2 << endl;

	cerr << "string to value: " << stringToValue("-23E-2", temp) << " -> ";
	cerr << temp << endl;


	int x;
	cin >> x;

	ServerConfigureRef->removeDevice(tDevice->deviceID);

	return true;
}