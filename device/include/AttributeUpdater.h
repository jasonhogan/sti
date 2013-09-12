#ifndef STI_DEVICE_ATTRIBUTEUPDATER_H
#define STI_DEVICE_ATTRIBUTEUPDATER_H

#include <string>
#include <vector>


namespace STI
{
namespace Device
{

class STI_Device;

class AttributeUpdater
{
public: 
	AttributeUpdater(STI_Device* thisDevice) : device_(thisDevice) {};
	virtual ~AttributeUpdater() {}
	
	virtual void defineAttributes() = 0;
	virtual bool updateAttributes(std::string key, std::string value) = 0; 
	virtual void refreshAttributes() = 0;

protected:
	template<class T>
	void addAttribute(const std::string& key, T initialValue, const std::string& allowedValues = "")
	{
		device_->addAttribute(key, initialValue, allowedValues);
	}
	
	template<class T> 
	bool setAttribute(const std::string& key, T value)
	{
		return device_->setAttribute(key, value);
	}

private:
	STI_Device* device_;
};

}
}

#endif

