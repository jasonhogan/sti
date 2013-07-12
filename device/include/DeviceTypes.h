#ifndef STI_DEVICE_DEVICETYPES_H
#define STI_DEVICE_DEVICETYPES_H

#include <string>
#include <vector>
#include <map>
#include <set>

#include "SynchronizedMap.h"
#include "Collector.h"
#include "LocalCollector.h"
#include "Distributer.h"

namespace STI
{	
	namespace Device
	{
		typedef std::string DeviceIDString;

		class DeviceID;
		class DeviceTimingEngineInterface;

		typedef std::set<DeviceID> DeviceIDSet;
		typedef boost::shared_ptr<DeviceIDSet> DeviceIDSet_ptr;

		class STI_Device;
		
		class DeviceInterface;
		typedef boost::shared_ptr<DeviceInterface> DeviceInterface_ptr;
		typedef STI::Utils::SynchronizedMap<DeviceID, DeviceInterface_ptr> DeviceMap;
		typedef boost::shared_ptr<DeviceMap> DeviceMap_ptr;

		typedef STI::Utils::Collector<STI::Device::DeviceID, STI::Device::DeviceInterface> DeviceCollector;
		typedef boost::shared_ptr<DeviceCollector> DeviceCollector_ptr;

		typedef	STI::Utils::Distributer<STI::Device::DeviceID, STI::Device::DeviceInterface> DeviceDistributer;

	
		typedef STI::Utils::LocalCollector<STI::Device::DeviceID, STI::Device::DeviceInterface> LocalDeviceCollector;

		class PartnerDevice;
		typedef boost::shared_ptr<PartnerDevice> PartnerDevice_ptr;
		typedef std::string PartnerAlias;
		typedef STI::Utils::SynchronizedMap<PartnerAlias, PartnerDevice_ptr> PartnerDeviceMap;
		typedef std::set<PartnerAlias> PartnerAliasSet;

		class Attribute;
		typedef boost::shared_ptr<Attribute> Attribute_ptr;
		typedef STI::Utils::SynchronizedMap<std::string, Attribute_ptr> AttributeMap;

		class AttributeUpdater;
		typedef boost::shared_ptr<AttributeUpdater> AttributeUpdater_ptr;
		typedef std::vector<AttributeUpdater_ptr> AttributeUpdaterVector;

	}

}

#endif
