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

	DeviceID(const std::string& name, const std::string& address, unsigned short module)
		: name_l(name), address_l(address), module_l(module) {}

	bool operator<(const DeviceID &rhs) const { return deviceID_l.compare(rhs.deviceID_l) < 0; }
	bool operator==(const DeviceID &rhs) const { return deviceID_l.compare(rhs.deviceID_l) == 0; }

	const std::string& getName() const { return name_l; }
	const std::string& getAddress() const { return address_l; }
	unsigned short getModule() const { return module_l; }
	const std::string& getID() const { return deviceID_l; }

	void setName(const std::string& name) { name_l = name; }
	void setAddress(const std::string& address) { address_l = address; }
	void setModule(unsigned short module) { module_l = module; }
	void setID(const std::string& deviceID) { deviceID_l = deviceID; }

private:
	std::string name_l;
	std::string address_l;
	unsigned short module_l;
	std::string deviceID_l;
};


}
}

#endif
