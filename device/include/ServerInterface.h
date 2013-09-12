#ifndef STI_SERVER_SERVERINTERFACE_H
#define STI_SERVER_SERVERINTERFACE_H

#include "ServerTypes.h"
#include "DeviceTypes.h"
#include "MixedValue.h"

namespace STI
{
namespace Server
{

class ServerInterface;

class ServerInterface
{
public:

	virtual ~ServerInterface() {}

	virtual const STI::Device::DeviceID& getDeviceID() const = 0;

	virtual bool addDevice(const STI::Device::DeviceInterface_ptr& device) = 0;
	virtual bool removeDevice(const STI::Device::DeviceID& deviceID) = 0;

	virtual bool getDeviceEventHandler(STI::Server::DeviceEventHandler_ptr& handler) = 0;

};

}
}

#endif

