#ifndef STI_SERVER_SERVERTYPES_H
#define STI_SERVER_SERVERTYPES_H


namespace STI
{	
	namespace Server
	{
		class ServerInterface;
		typedef boost::shared_ptr<ServerInterface> ServerInterface_ptr;

		class DeviceEventHandler;
		typedef boost::shared_ptr<DeviceEventHandler> DeviceEventHandler_ptr;

	}

}

#endif
