

template<typename ID, typename T>
class CollectorNode : public T
{
public:

	typedef Collector<ID, T> Collector_type;
	typedef boost::shared_ptr<collector_type> Collector_type_ptr;
	
	virtual bool getCollector(const Collector_type_ptr& collector) = 0;
};


//class STI_Device : public LocalDevice;
//class NetworkDevice : public AbstractDevice {};



class STI_Device : public CollectorNode<DeviceID, LocalDevice>;

class LocalDevice : public AbstractDevice {};

//class DeviceServant : public CollectorNode<DeviceID, LocalDevice> {};

class NetworkDeviceReference
{
	NetworkDeviceReference(LocalDevice_ptr device)
	{
		deviceServant = new NetworkDevice_i(device);
	}

	ORB::NetworkDevice_var getNetworkDeviceReference()
	{
		NetworkDevice_var reference(NetworkDevice_i->_this());	//wrap in _var so remove_ref() gets called
		return reference;
	}

	NetworkDevice_i* NetworkDevice_i;
};


class NetworkDeviceCollector : public Collector<DeviceID, NetworkDeviceReference>
{
	NetworkDeviceCollector(TNetworkDeviceCollector_var collector);	//constructed from ORB reference passed to Hub::connect()

	virtual bool addNode(const ID& id, const NetworkDeviceReference_ptr& node)
	{
		try {
			//this is an IDL function pointing to a servant that the server provided
			collector->addNode(id, node->getNetworkDeviceReference());
		}
		catch(CORBA::Exception& ex)
		{
			throw NetworkCollectorException(ex);
		}
	}
	virtual bool remove(const ID& id) 
	{
		try {
			//this is an IDL function pointint to a servant that the server provided
			collector->remove(id);
		}...
	}

};

//On device hub:
Redistributer<DeviceID, LocalDevice, NetworkDeviceReference>

//devices on this computer must have servants called Device_i that are network
//accessible (have an IDL) and wrap AbstractDevice. HAS_A AbstractDevice.
//Should be collected by the Redistributer<DeviceID, LocalDevice, NetworkDevice> in the Hub

//Redistributer<DeviceID, LocalDevice, NetworkDevice> collects LocalDevice, distributes NetworkDevice.
//in particular, only the Server is a collector; once it joins, the Redistributer policy adds all LocalDevice
//nodes, converts them to NetworkDevice nodes, and adds them to the server's collector.

