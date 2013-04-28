#ifndef STI_TIMINGENGINE_DEVICEID_H
#define STI_TIMINGENGINE_DEVICEID_H

#include <string>

namespace STI
{
namespace Device
{

class DeviceID 
{
public:
	std::string deviceName;
	std::string address;
	unsigned short moduleNum;
	std::string deviceID;
};


}
}

#endif
