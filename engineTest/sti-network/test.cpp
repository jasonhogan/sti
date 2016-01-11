



class LocalDevice : public DeviceInterface
{
};

class NetworkDevice : public DeviceInterface
{
};

typedef	STI::Utils::Distributer<STI::Device::DeviceID, STI::Device::LocalDevice> LocalDeviceDistributer;
typedef	STI::Utils::Distributer<STI::Device::DeviceID, STI::Device::NetworkDevice> NetworkDeviceDistributer;

template<class A, class B>
class ConvertDeviceInterface
{
	virtual A_ptr convert(const B_ptr& input) = 0;
	virtual B_ptr convert(const A_ptr& input) = 0;
};

template<class ID>
class DistributerBase
{
	virtual void removeNode(const ID& id) = 0;

	virtual void cleanup() = 0;
	virtual void distribute() = 0;
	virtual void refresh() = 0;
};

template <class ID, class A, class B, class Convert<A,B> >
class JointDistributer : public DistributerBase<ID>

	
typedef STI::Utils::Collector<STI::Device::DeviceID, STI::Device::DeviceInterface> DeviceCollector;

class NetworkDeviceCollector : public DeviceCollector;
{
};

template<class ID, class T, class U>	//T:U
class DerivedCollector : public STI::Utils::Collector<ID, U>
{
	virtual bool get(const ID& id, T_ptr& node) const = 0;
	virtual bool add(const ID& id, const T_ptr& node) = 0;
};

template<class ID, class T, class U, class Convert<T,U> >	//T:U
class LocalCollector : public Collector<ID, U>
{
	//as before, plus:
	virtual bool get(const ID& id, T_ptr& node) const = 0;
	virtual bool add(const ID& id, const T_ptr& node) = 0;
	//Implement in terms of saving T_ptr's; convert to U_ptr for get and add.

	//conversion of add(U_ptr) must create a T_ptr that contains a U_ptr (in other words, it must be possible to create a T using a U:
	//T(const U_ptr* t) constructor must exist.
};




template <class A, class B>
class RefConverter
{
	typedef boost::shared_ptr<A> A_ptr;
	typedef boost::shared_ptr<B> B_ptr;

	virtual bool convert(const A_ptr&, B_ptr&) = 0;
	virtual bool convert(const B_ptr&, A_ptr&) = 0;
};


template <class A, class B, class C>
class JointDistributerConvert : public RefConverter<A,C>, public RefConverter<B,C>, public RefConverter<A,B>
{
};


class JointDeviceDistributerConvert : public JointDistributerConvert<LocalDevice,NetworkDevice,DevicInterface>
{
	//typedef DistributeConvert<LocalDevice,DeviceInterface>::A_ptr LocalToInterfaceA_ptr;
	//typedef DistributeConvert<LocalDevice,DeviceInterface>::B_ptr LocalToInterfaceB_ptr;
	//typedef DistributeConvert<NetworkDevice,DeviceInterface>::A_ptr NetworkToInterfaceA_ptr;
	//typedef DistributeConvert<NetworkDevice,DeviceInterface>::B_ptr NetworkToInterfaceB_ptr;
	//typedef DistributeConvert<LocalDevice,NetworkDevice>::A_ptr LocalToNetworkA_ptr;
	//typedef DistributeConvert<LocalDevice,NetworkDevice>::B_ptr LocalToNetworkB_ptr;
	
	typedef DistributeConvert<LocalDevice,DeviceInterface>::A_ptr Local_ptr;
	typedef DistributeConvert<NetworkDevice,DeviceInterface>::A_ptr Network_ptr;
	typedef DistributeConvert<NetworkDevice,DeviceInterface>::B_ptr Device_ptr;

	virtual bool convert(const Local_ptr&,   Device_ptr&);
	virtual bool convert(const Device_ptr&,  Local_ptr&);
	
	virtual bool convert(const Network_ptr&, Device_ptr&);
	virtual bool convert(const Device_ptr&,  Network_ptr&);
	
	virtual bool convert(const Local_ptr&,   Network_ptr&);
	virtual bool convert(const Network_ptr&, Local_ptr&);

};


template<class ID, class C>
class DistributerInterface
{
protected:
	typedef Collector<ID, C> CollectorC;
	typedef boost::shared_ptr<CollectorC> CollectorC_ptr;
	typedef boost::shared_ptr<C> C_ptr;

public:
//	virtual bool addNode(const ID& id, const C_ptr& node) = 0;
	virtual bool addCollector(const ID& id, const CollectorC_ptr& collector) = 0;

	virtual void removeNode(const ID& id) = 0;
	virtual void removeCollector(const ID& id) = 0;
	virtual void cleanup() = 0;
	virtual void distribute() = 0;
	virtual void refresh() = 0;
};
template<class ID, class T>
class Distributer : public DistributerInterface<ID>
{
	//as before
};

template<class ID, class T, class C, RefConverter<T, C> converter>
class Distributer : public DistributerInterface<ID>
{
protected:
	typedef Collector<ID, T> CollectorT;
	typedef boost::shared_ptr<CollectorT> CollectorT_ptr;
	typedef boost::shared_ptr<T> T_ptr;

public:

	//The following 2 functions could be implemented in DistributerInterface<ID, C> using protected: CollectorC list
	//do conversion
	virtual bool addNode(const ID& id, const DistributerInterface<ID,C>::C_ptr& node)
	{
		T_ptr tNode;
		return converter.convert(node, tNode) && addNode(id, tNode);
	}

	//maybe should just accept CollectorT_ptr? Breaks interface.
	//Instead, maintain separate lists of generic and specific collectors.

	virtual bool addCollector(const ID& id, const DistributerInterface<ID,C>::CollectorC_ptr& collector)
	{
		return addCollector(id, collector, collectors);
	}
	bool addCollector(const ID& id, const DistributerInterface<ID,C>::CollectorC_ptr& collector, const CollectorsC& collectors)
	{
		if(collector != 0 && collectorsC.add(id, collector)) {
			distribute(collectorsC);
			return true;
		}
		return false;

//		return addCollector(id, tCollector)
//		CollectorT_ptr tCollector;
//		return converter.convert(collector, tCollector) && addCollector(id, tCollector);
	}

	//same as current Distributer<ID, T>
};


template<class ID, class A, class B, class C, RefConverter<A,B> converter>	//A:C, B:C
class DualDistributer : public DistributerInterface<ID>
{
	typedef boost::shared_ptr<A> A_ptr;		//class LocalDevice : public DeviceInterface
	typedef boost::shared_ptr<B> B_ptr;		//class NetworkDevice : public DeviceInterface
//	typedef boost::shared_ptr<C> C_ptr;		//class NetworkDevice : public DeviceInterface

	typedef STI::Utils::Collector<ID, A> ACollector;
	typedef STI::Utils::Collector<ID, B> BCollector;
//	typedef STI::Utils::Collector<ID, C> CCollector;

	typedef Distributer<ID, A> ADistributer;
	typedef Distributer<ID, B> BDistributer;
	typedef boost::shared_ptr<ADistributer> ADistributer_ptr;
	typedef boost::shared_ptr<BDistributer> BDistributer_ptr;
		
	typedef DistributerInterface<ID,C> CDistributerInterface;
	typedef boost::shared_ptr<CDistributerInterface> CDistributerInterface_ptr;

	ADistributer_ptr aDistributer;
	BDistributer_ptr bDistributer;

	vector<CDistributerInterface_ptr> distributers;

	JointDeviceDistributer(const ADistributer_ptr& aDistributer, const BDistributer_ptr& BDistributer)
	{
		distributers.push_back(aDistributer);
		distributers.push_back(bDistributer);
	}

	bool addNode(const DeviceID& id, const A_ptr& node)
	{
		B_ptr bNode;
		return aDistributer.addNode(id, node) 
			&& converter.convert(node, bNode) && bDistributer.addNode(id, bNode);
	}	
	bool addNode(const DeviceID& id, const B_ptr& node)
	{
		A_ptr aNode;
		return bDistributer.addNode(id, node) 
			&& converter.convert(node, aNode) && aDistributer.addNode(id, aNode);
	}

	//...//

	void cleanup()
	{
		for(unsigned i = 0; i < distributers.size(); i++) {
			distributers.at(i)->cleanup();
		}
	}

};

JoinedDistributer
MixedDistributer
DoubleDistributer
Split
BiDistributer
DualDistributer


DeviceJoinDistibuterConverter deviceConverter;
JointDistributer<DeviceID, LocalDevice, NetworkDevice, DeviceInterface, deviceConverter>

class JointDeviceDistributer
{
	typedef boost::shared_ptr<LocalDevice> LocalDevice_ptr;		//class LocalDevice : public DeviceInterface
	typedef boost::shared_ptr<NetworkDevice> NetworkDevice_ptr;		//class NetworkDevice : public DeviceInterface

	typedef STI::Utils::LocalCollector<STI::Device::DeviceID, LocalDevice> LocalDeviceCollector;
	typedef STI::Utils::NetworkCollector<STI::Device::DeviceID, NetworkDevicee> NetworkDeviceCollector;

	LocalDeviceDistributer localDistributer;
	NetworkDeviceDistributer networkDistributer;
	vector<DistributerInterface<Device> > distributers;

	distributers.push_back(localDistributer);
	distributers.push_back(networkDistributer);

	bool addNode(const DeviceID& id, const LocalDevice_ptr& node)
	{
		NetworkDevice_ptr otherNode = convert(node);
		return localDistributer.addNode(id, node) && networkDistributer.addNode(id, otherNode);
	}
	bool addNode(const DeviceID& id, const NetworkDevice_ptr& node)
	{
		LocalDevice_ptr otherNode = convert(node);
		return localDistributer.addNode(id, otherNode) && networkDistributer.addNode(id, node);
	}
	bool addCollector(const DeviceID& id, const LocalDeviceCollector& collector) {
		return localDistributer.addCollector(id, collector);
	}
	bool addCollector(const DeviceID& id, const NetworkDeviceCollector& collector) {
		return networkDistributer.addCollector(id, collector);
	}

	void removeNode(const ID& id)
	{
		localDistributer->removeNode(id);
		networkDistributer->removeNode(id);
	}
	void removeCollector(const ID& id)
	{
		localDistributer->removeCollector(id);
		networkDistributer->removeCollector(id);
	}
	void cleanup()
	{
		localDistributer->cleanup();
		networkDistributer->cleanup();

		for(unsigned i = 0; i < distributers.size(); i++) {
			distributers.at(i)->cleanup();
		}
	}

	void distribute()
	{
		localDistributer->distribute();
		networkDistributer->distribute();
	}

	void refresh()
	{
		localDistributer->refresh();
		networkDistributer->refresh();
	}


};



template<class ID, class B>
class Collection
{
protected:
	typedef boost::shared_ptr<B> B_ptr;

public:

	virtual ~Collection() {}

	//template<typename Y>
	//bool get(const ID& id, boost::shared_ptr<Y>& node) const	//works if T:Y
	//{
	//	T_ptr tNode;
	//	return get(id, tNode) && !((node = tNode) == 0);
	//}

	virtual bool get(const ID& id, B_ptr& node) const = 0;

	virtual bool contains(const ID& id) const = 0;
	virtual void getIDs(std::set<ID>& ids) const = 0;

	virtual bool remove(const ID& id) = 0;
	virtual void cleanup(const std::set<ID>& ids) = 0;	//remove any collected nodes that are NOT in ids
	virtual void clear() = 0;
};

template<typename ID, typename T, typename B = T>
class Collector : public Collection<ID, B>
{
protected:
	typedef boost::shared_ptr<T> T_ptr;

public:

	virtual ~Collector() {}

	////goes in base
	//template<typename Y>
	//bool get(const ID& id, boost::shared_ptr<Y>& node) const	//works if T:Y
	//{
	//	T_ptr tNode;
	//	return get(id, tNode) && !((node = tNode) == 0);
	//}

	virtual bool addNode(const ID& id, const T_ptr& node) = 0;
	virtual bool getNode(const ID& id, T_ptr& node) const = 0;

};


template<class ID, class T, typename B = T>
class LocalCollector : public Collector<ID, T, B>
{
public:
	typedef STI::Utils::SynchronizedMapPolicy<ID> LocalCollectorPolicy;
	typedef boost::shared_ptr<LocalCollectorPolicy> LocalCollectorPolicy_ptr;

	LocalCollector() {}
	LocalCollector(const LocalCollectorPolicy_ptr& policy) : nodes(policy) {}

	bool addNode(const ID& id, const typename Collector<ID, T>::T_ptr& node) { return nodes.add(id, node); }
	bool getNode(const ID& id, typename Collector<ID, T, B>::T_ptr& node) const { return nodes.get(id, node); }

	bool get(const ID& id, typename Collection<ID, B>::B_ptr& node) const 
	{
		Collector<ID, T, B>::T_ptr tNode;
		return getNode(id, tNode) && !((node = tNode) == 0);	//T::B
	}
	void getIDs(std::set<ID>& ids) const { nodes.getKeys(ids); }
	bool contains(const ID& id) const { return nodes.contains(id); }

	bool remove(const ID& id) { return nodes.remove(id); }
	void cleanup(const std::set<ID>& ids)
	{
		typename std::set<ID> storedIDs;
		getIDs(storedIDs);

		typename std::set<ID>::const_iterator clean;

		for(typename std::set<ID>::iterator id = storedIDs.begin(); id != storedIDs.end(); id++) {
			clean = ids.find(*id);
			if(clean == ids.end()) {
				remove(*id);
			}
		}
	}
	void clear() { nodes.clear(); }

private:
	STI::Utils::SynchronizedMap<ID, typename Collector<ID, T>::T_ptr> nodes;
};

DistributerInterface
AbstractDistributer
GenericDistributer
NodeDistributer
DistributerBase


template<class ID>
class AbstractDistributer
{
public:

	virtual void removeNode(const ID& id) = 0;
	virtual void removeCollector(const ID& id) = 0;

	virtual void cleanup() = 0;
	virtual void distribute() = 0;
	virtual void refresh() = 0;
};

template<typename ID, typename T, typename B=T>
class Distributer : public AbstractDistributer<ID>
{
protected:

	typedef Collector<ID, T, B> CollectorT;
	typedef boost::shared_ptr<CollectorT> CollectorT_ptr;
	typedef boost::shared_ptr<T> T_ptr;

public:
	bool addNode(const ID& id, const T_ptr& node) {...}
	bool addCollector(const ID& id, const CollectorT_ptr& collector) {...}

	//same as before

};


template<class ID, class T1, class T2, class B, RefConverter<A,B> converter>	//T1:B, T2:B
class DualDistributer : public AbstractDistributer<ID>
{
	typedef boost::shared_ptr<T1> T1_ptr;		//class LocalDevice : public DeviceInterface
	typedef boost::shared_ptr<T2> T2_ptr;		//class NetworkDevice : public DeviceInterface

	typedef STI::Utils::Collector<ID, T1, B> T1Collector;
	typedef STI::Utils::Collector<ID, T2, B> T2Collector;

	typedef Distributer<ID, T1, B> T1Distributer;
	typedef boost::shared_ptr<T1Distributer> T1Distributer_ptr;

	typedef Distributer<ID, T2, B> T2Distributer;
	typedef boost::shared_ptr<T2Distributer> T2Distributer_ptr;
		
	typedef AbstractDistributer<ID> DistributerBase;
	typedef boost::shared_ptr<DistributerBase> DistributerBase_ptr;

	std::vector<DistributerBase_ptr> distributers;
	T1Distributer_ptr distributer1;
	T2Distributer_ptr distributer2;


	JointDeviceDistributer(const T1Distributer_ptr& aDistributer, const T2Distributer_ptr& BDistributer)
	{
		distributers.push_back(distributer1);
		distributers.push_back(distributer2);
	}

	bool addNode(const DeviceID& id, const T1_ptr& node1)
	{
		T2_ptr node2;

		if( converter.convert(node1, node2) && 
			distributer1.addNode(id, node1) && distributer2.addNode(id, node2) ) {
				return true;
		} 
		else {
			removeNode(id);
			return false;
		}
	}	
	bool addNode(const DeviceID& id, const T2_ptr& node2)
	{
		T1_ptr node1;
		return bDistributer.addNode(id, node2) 
			&& converter.convert(node2, node1) && aDistributer.addNode(id, node1);
	}

	//...//

	void cleanup()
	{
		for(unsigned i = 0; i < distributers.size(); i++) {
			distributers.at(i)->cleanup();
		}
	}

};


template <class T1, class T2>
class RefConverter
{
	typedef boost::shared_ptr<T1> T1_ptr;
	typedef boost::shared_ptr<T2> T2_ptr;

	virtual bool convert(const T1_ptr&, T2_ptr&) = 0;
	virtual bool convert(const T2_ptr&, T1_ptr&) = 0;
};

class DeviceConverter : public RefConverter<LocalDevice, NetworkDevice>
{
};


DeviceConverter deviceConverter;
DualDistributer<DeviceID, LocalDevice, NetworkDevice, DeviceInterface, deviceConverter> distributer;