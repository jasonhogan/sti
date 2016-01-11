#ifndef STI_UTILS_DUALDISTRIBUTER_H
#define STI_UTILS_DUALDISTRIBUTER_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "AbstractDistributer.h"
#include "Collector.h"
#include "Distributer.h"
#include "RefConverter.h"


namespace STI
{
namespace Utils
{

template<class ID, class T, typename R>
class Redistributer? : public LocalCollector<ID, T, T>
{
};

Distributer<DeviceID, LocalDevice> hubDistributer;

Redistributer<DeviceID, LocalDevice, NetworkDevice> serverCollector;
serverCollector.setPolicy(new Policy

hubDistributer.addCollector(networkHubID, serverCollector);


template<class ID, class T1, class T2, class B>	//T1:B, T2:B
class DualDistributer : public AbstractDistributer<ID>
{
protected:

	typedef boost::shared_ptr<T1> T1_ptr;		//class LocalDevice : public DeviceInterface
	typedef boost::shared_ptr<T2> T2_ptr;		//class NetworkDevice : public DeviceInterface

	typedef STI::Utils::Collector<ID, T1, B> CollectorT1;
	typedef boost::shared_ptr<CollectorT1> CollectorT1_ptr;

	typedef STI::Utils::Collector<ID, T2, B> CollectorT2;
	typedef boost::shared_ptr<CollectorT2> CollectorT2_ptr;

	typedef Distributer<ID, T1, B> DistributerT1;
	typedef boost::shared_ptr<DistributerT1> DistributerT1_ptr;

	typedef Distributer<ID, T2, B> DistributerT2;
	typedef boost::shared_ptr<DistributerT2> DistributerT2_ptr;
		
	typedef AbstractDistributer<ID> DistributerBase;
	typedef boost::shared_ptr<DistributerBase> DistributerBase_ptr;
	typedef std::vector<DistributerBase_ptr> DistributerVector;
	
	typedef RefConverter<T1, T2> Converter12;
	typedef boost::shared_ptr<Converter12> Converter12_ptr;

public:

	DualDistributer(const DistributerT1_ptr& distributer1, const DistributerT2_ptr& distributer2, 
		const Converter12_ptr& refConverter) : 
	converter(refConverter)
	{
		distributers.push_back(distributer1);
		distributers.push_back(distributer2);
	}

	DistributerT1_ptr distributer1() { return distributers.at(0); }
	DistributerT2_ptr distributer2() { return distributers.at(1); }

	bool addNode(const ID& id, const T1_ptr& node1)
	{
		T2_ptr node2;
		if( converter.convert(node1, node2) && 
			distributer1()->addNode(id, node1) && distributer2()->addNode(id, node2) ) {
				return true;
		} 
		else {
			removeNode(id);
			return false;
		}
	}	
	bool addNode(const ID& id, const T2_ptr& node2)
	{
		T1_ptr node1;
		if( converter.convert(node2, node1) && 
			distributer2()->addNode(id, node2) && distributer1()->addNode(id, node1) ) {
				return true;
		} 
		else {
			removeNode(id);
			return false;
		}
	}
	bool addCollector(const ID& id, const CollectorT1_ptr& collector) {
		return distributer1()->addCollector(id, collector);
	}
	bool addCollector(const ID& id, const CollectorT2_ptr& collector) {
		return distributer2()->addCollector(id, collector);
	}

	virtual void removeNode(const ID& id)
	{
		DistributerVector::iterator it;
		for(it = distributers.begin(); it != distributers.end(); ++it) {
			(*it)->removeNode(id);
		}
	}
	virtual void removeCollector(const ID& id)
	{
		DistributerVector::iterator it;
		for(it = distributers.begin(); it != distributers.end(); ++it) {
			(*it)->removeCollector(id);
		}
	}

	virtual void cleanup()
	{
		DistributerVector::iterator it;
		for(it = distributers.begin(); it != distributers.end(); ++it) {
			(*it)->cleanup();
		}
	}

	virtual void distribute()
	{
		DistributerVector::iterator it;
		for(it = distributers.begin(); it != distributers.end(); ++it) {
			(*it)->distribute();
		}
	}

	virtual void refresh()
	{
		DistributerVector::iterator it;
		for(it = distributers.begin(); it != distributers.end(); ++it) {
			(*it)->refresh();
		}
	}

private:

	DistributerVector distributers;


	Converter12_ptr converter;

};


} //namespace Utils
} //namespace STI


#endif

