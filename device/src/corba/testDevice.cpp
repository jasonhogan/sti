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
	attributes["key2"] = Attribute("none", "2, 5, none, full open, true");

}

bool testDevice::updateAttribute(string key, string value)
{
	return true;
}

void testDevice::defineChannels()
{

//	bool (*ptr)(unsigned short, STI_Server_Device::TDeviceEvent&) = writeTestChannel;
	
	addInputChannel(2, DataNumber);

	addOutputChannel(22, ValueNumber);
	addOutputChannel(57, ValueNumber);
	addOutputChannel(58, ValueNumber);

	enableStreaming(2,"1e1");

//	Attributes
//	Ch_2_Streaming
//	Ch_2_SamplePeriod
//	Ch_2_DataBuffer

}

bool testDevice::writeChannel(unsigned short Channel, STI_Server_Device::TDeviceEvent & Event)
{
	return true;
}

bool testDevice::readChannel(STI_Server_Device::TMeasurement & Measurement)
{
	switch(Measurement.channel)
	{
	case 2:
		Measurement.data.number(1e-6 * Measurement.time);
		break;
	default:
		break;
	}

	return true;
}

bool testDevice::deviceMain()
{

//	int temp;
//	double temp2;
	
//	cerr << "string to value: " << stringToValue("23.4e-6", temp2) << " -> ";
//	cerr << temp2 << endl;

//	cerr << "string to value: " << stringToValue("-23E-2", temp) << " -> ";
//	cerr << temp << endl;




	int x;
	cin >> x;

//	ServerConfigureRef->removeDevice(tDevice->deviceID);

	return true;
}