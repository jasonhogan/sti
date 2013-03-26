#ifndef STI_TIMINGENGINE_EVENTENGINEMANAGER_H
#define STI_TIMINGENGINE_EVENTENGINEMANAGER_H

#include <EventEngineManager.h>


namespace STI
{
namespace TimingEngine
{

//Server-side wrapper for remote object
//public RemoteEventEngineManagerWrapper : public EventEngineManager
public RemoteEventEngineManager : public EventEngineManager
{
	RemoteEventEngineManager(RemoteEventEngineManager_ptr remoteEventEngineManagerRef);
	
	void load(const EngineID& id)
	{
		STI::Utils::convert(id, engineID);

		try {
			remoteEngineManager->load(engineID);
		} catch(CORBA::TRANSIENT ex) {
			throw DeviceException("RemoteEventEngineManager");
		}
	}
};

}
}

#endif


