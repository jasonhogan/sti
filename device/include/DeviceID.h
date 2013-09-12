#ifndef STI_TIMINGENGINE_DEVICEID_H
#define STI_TIMINGENGINE_DEVICEID_H

#include <string>
#include <sstream>

#include "utils.h"

namespace STI
{
namespace Device
{

class DeviceIDBase;
typedef boost::shared_ptr<DeviceIDBase> DeviceIDBase_ptr;


class DeviceIDBase 
{
public:

	DeviceIDBase(const std::string& name, const std::string& address, unsigned short module)
		: name_l(name), address_l(address), module_l(module) { generateID();}

//	bool operator<(const DeviceID &rhs) const { return deviceID_l.compare(rhs.deviceID_l) < 0; }
//	bool operator==(const DeviceID &rhs) const { return deviceID_l.compare(rhs.deviceID_l) == 0; }

	bool operator<(const DeviceIDBase &rhs) const
	{
		return deviceID_l.compare(rhs.deviceID_l) < 0;
	}
	bool operator==(const DeviceIDBase &rhs) const { return deviceID_l.compare(rhs.deviceID_l) == 0; }
	bool operator!=(const DeviceIDBase &rhs) const { return !((*this)==rhs); }


	const std::string& getName() const { return name_l; }
	const std::string& getAddress() const { return address_l; }
	unsigned short getModule() const { return module_l; }
	const std::string& getID() const { return deviceID_l; }

	void setName(const std::string& name) { name_l = name; generateID();}
	void setAddress(const std::string& address) { address_l = address; generateID();}
	void setModule(unsigned short module) { module_l = module; generateID();}
//	void setID(const std::string& deviceID) { deviceID_l = deviceID; }



private:
	std::string name_l;
	std::string address_l;
	unsigned short module_l;
	std::string deviceID_l;

	void generateID()
	{
		std::stringstream id;
		id << address_l << "_" << module_l << "_" << name_l;
		deviceID_l = id.str();
	}
};


class DeviceID
{
public:
	DeviceID(const std::string& name, const std::string& address, unsigned short module)
	{ 
		deviceIDBase = DeviceIDBase_ptr(new DeviceIDBase(name, address, module));
	}
	
	bool operator<(const DeviceID &rhs) const  { return (*deviceIDBase) < (*(rhs.deviceIDBase));}
	bool operator==(const DeviceID &rhs) const { return (*deviceIDBase) == (*(rhs.deviceIDBase));}
	bool operator!=(const DeviceID &rhs) const { return (*deviceIDBase) != (*(rhs.deviceIDBase));}
	
	const std::string& getName() const { return deviceIDBase->getName(); }
	const std::string& getAddress() const { return deviceIDBase->getAddress(); }
	unsigned short getModule() const { return deviceIDBase->getModule(); }
	const std::string& getID() const { return deviceIDBase->getID(); }

	void setName(const std::string& name) { deviceIDBase->setName(name); }
	void setAddress(const std::string& address) { deviceIDBase->setAddress(address); }
	void setModule(unsigned short module) { deviceIDBase->setModule(module); }
//	void setID(const std::string& deviceID) { deviceIDBase->setID(deviceID); }
	
	static bool stringToDeviceID(const std::string& deviceIDin, DeviceID& deviceIDout)
	{
		std::vector<std::string> idComponents;
		unsigned short module;
		bool success = false;

		STI::Utils::splitString(deviceIDin, "_", idComponents);

		if(idComponents.size() == 3) {
			deviceIDout.setAddress(idComponents.at(0));
			deviceIDout.setName(idComponents.at(2));
			if(STI::Utils::stringToValue(idComponents.at(1), module)) {
				deviceIDout.setModule(module);
				success = true;
			}
		}
		return success;
	}

private:

	DeviceIDBase_ptr deviceIDBase;
};



}
}

#endif
