
//template<class ID, class IN, class OUT>
//class Redistributer : public Collector<ID, IN>, public AbstractDistributer<ID>
//{
//	typedef Collector<ID, OUT> CollectorOUT;
//	bool addCollector(const ID& id, const CollectorOUT_ptr& collector)
//	Distributer<ID, OUT> distributer;
//};
class NetworkDeviceHub
{
public:

	NetworkDeviceHub() : 
		hubServant(this), 
		localRedistributer(converter, localPolicy),
		networkRedistributer(converter, networkPolicy)
	{
		localRedstributer.addCollector(networkRedistributer);
		networkRedistributer.addCollector(localRedstributer);

		//activate and publish network reference...
		hubServant;
	}
	
	void addNode(DeviceID id, const LocalDevice_ptr& device)
	{
		localRedistributer.distributer.addNode(id, device);
	}

	void run()
	{
		orbManager->run();
	}

private:

	HubServant<NetworkDevice> hubServant;

	Redistributer<DeviceID, NetworkDevice, LocalDevice> localRedistributer;	//Collector<ID, NetworkDevice>; addCollector(Collector<ID, LocalDevice>)
	Redistributer<DeviceID, LocalDevice, NetworkDevice> networkRedistributer; //Collector<ID, LocalDevice>; addCollector(Collector<ID, NetworkDevice>)
};



class DeviceRefConverter : public RefConverter<LocalDevice, NetworkDevice>, public RefConverter<NetworkDevice, LocalDevice>
{
protected:
	typedef boost::shared_ptr<T1> T1_ptr;
	typedef boost::shared_ptr<T2> T2_ptr;

public:

	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
	virtual bool convert(const T2_ptr& t2, T1_ptr& t1) = 0;
};

class NetworkDeviceHub
{
	Distributer<DeviceID, LocalDevice> localDistributer;
	Redistributer<DeviceID, LocalDevice, NetworkDevice> networkDistributer;

	RefConverter<LocalDevice, NetworkDevice> converter;

	NetworkDeviceHub() : networkDistributer(converter)
	{
		localDistributer.addCollector(networkDistributer);
	}
	
	void addLocalDevice(DeviceID id, AbstractDevice device)	//For local devices
	{
		localDistributer.addNode(id, device);
	}

	void addNetworkServer(DeviceID id, Collector<DeviceID, NetworkDevice> server)
	{
		networkDistributer.addCollector(id, server);
		localDistributer.refresh();
	}
};



//IDL
interface Hub
{
	void connect(TNodeID id, Any nodeCollector);
};
class DeviceHub_i : public CORBATYPE::Hub
{
	void connect(TNodeID id, CORBA::Any nodeCollector)
	{
		TNetworkDeviceCollector_var tDeviceCollector << nodeCollector;
		NetworkDeviceCollector_ptr deviceCollector(tDeviceCollector);

		networkDistributer.addCollector(convert(id), deviceCollector);
	}
};


class NetworkDeviceCollector : public Collector<DeviceID, NetworkDevice>
{
	NetworkDeviceCollector(TNetworkDeviceCollector_var collector);	//constructed from ORB reference passed to Hub::connect()

	virtual bool addNode(const ID& id, const IN_ptr& node)
	{
		try {
			collector->addNode(id, node);
		}
		catch(CORBA::Exception& ex)
		{
			throw NetworkCollectorException(ex);
		}
	}
	virtual bool getNode(const ID& id, IN_ptr& node) const

};