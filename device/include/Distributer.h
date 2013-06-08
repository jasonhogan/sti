#ifndef STI_UTILS_DISTRIBUTER_H
#define STI_UTILS_DISTRIBUTER_H

#include <boost/shared_ptr.hpp>
#include "SynchronizedMap.h"

#include "Collector.h"
#include "LocalCollector.h"


//template<class ID, class T>
//class CollectorPolicy
//{
//	virtual bool include(const ID& id) = 0;
//
//};

//};
//
//template<class ID, class T>
//class Collector
//{
//	void getPolicy(const CollectionPolicy_ptr& policy);
//	void get
//private:
//	Collection<ID, T> collection;
//
//};


//template<class ID, class T>
//class Collectable
//{
//};



namespace STI
{
namespace Utils
{

template<class ID, class T>
class Distributer
{
protected:

	typedef Collector<ID, T> CollectorT;
	typedef boost::shared_ptr<CollectorT> CollectorT_ptr;
	typedef boost::shared_ptr<T> T_ptr;

public:

	bool addNode(const ID& id, const T_ptr& node)
	{
		if(node != 0 && nodes.add(id, node)) {
			distributeAdd(id, node);
			return true;
		}
		return false;
	}

	bool addCollector(const ID& id, const CollectorT_ptr& collector) {
		if(collector != 0 && collectors.add(id, collector)) {
			distribute(collector);
			return true;
		}
		return false;
	}

	void removeNode(const ID& id)
	{
		if(nodes.remove(id)) {
			distributeRemove(id);
		}
	}
	void removeCollector(const ID& id)
	{
		collectors.remove(id);
	}
	void cleanup()
	{
		std::set<ID> nodeIDs;
		nodes.getKeys(nodeIDs);
		
		std::set<ID> collectorIDs;
		collectors.getKeys(collectorIDs);

		cleanup(nodeIDs, collectorIDs);
	}

	void distribute()
	{
		std::set<ID> nodeIDs;
		nodes.getKeys(nodeIDs);

		std::set<ID> collectorIDs;
		collectors.getKeys(collectorIDs);

		distribute(nodeIDs, collectorIDs);
	}

	void refresh()
	{
		std::set<ID> nodeIDs;
		nodes.getKeys(nodeIDs);

		std::set<ID> collectorIDs;
		collectors.getKeys(collectorIDs);

		cleanup(nodeIDs, collectorIDs);
		distribute(nodeIDs, collectorIDs);
	}

private:

	void distributeAdd(const ID& id, const T_ptr& node)
	{
		std::set<ID> ids;
		collectors.getKeys(ids);
		CollectorT_ptr collector;

		for(std::set<ID>::iterator collectorID = ids.begin(); collectorID != ids.end(); collectorID++) {
			if(collectors.get(*collectorID, collector) && collector != 0) {
				collector->add(id, node);	//conditionally adds the node, based on collector policy
			}
		}
	}

	void distributeRemove(const ID& id)
	{
		std::set<ID> ids;
		collectors.getKeys(ids);
		CollectorT_ptr collector;

		for(std::set<ID>::iterator collectorID = ids.begin(); collectorID != ids.end() collectorID++) {
			if(collectors.get(*collectorID, collector) && collector != 0) {
				collector->remove(id);
			}
		}
	}

	void distribute(const CollectorT_ptr& targetCollector)
	{
		std::set<ID> nodeIDs;
		nodes.getKeys(nodeIDs);

		distribute(nodeIDs, targetCollector);
	}

	
	void distribute(const std::set<ID>& nodeIDs, const CollectorT_ptr& targetCollector)
	{
		T_ptr node;

		for(std::set<ID>::const_iterator id = nodeIDs.begin(); id != nodeIDs.end(); id++) {
			if(nodes.get(*id, node)) {
				targetCollector->add(*id, node);	//conditionally add based on collector policy
			}
		}
	}

	void distribute(const std::set<ID>& nodeIDs, const std::set<ID>& collectorIDs)
	{
		CollectorT_ptr collector;

		for(std::set<ID>::iterator id = ids.begin(); id != ids.end() id++) {
			if(collectors.get(*collectorID, collector) && collector != 0) {
				distribute(nodeIDs, collector);
			}
		}
	}
	void cleanup(const std::set<ID>& nodeIDs, const std::set<ID>& collectorIDs)
	{
		CollectorT_ptr collector;

		for(std::set<ID>::iterator id = collectorIDs.begin(); id != collectorIDs.end() id++) {
			if(collectors.get(*id, collector) && collector != 0) {
				collector->cleanup(nodeIDs);	//force all collectors to remove nodes not found in the distributer's collection
			}
		}
	}



	LocalCollector<ID, T> nodes;
//	STI::Utils::SynchronizedMap<ID, T_ptr> nodes;
	STI::Utils::SynchronizedMap<ID, CollectorT_ptr> collectors;
};

}
}



//	template<typename N> bool addNode2(ID id, const N& node) { return false; }
	//template<typename N> bool addNode(ID id, const N& node)
	//{
	//	bool success = false;
	//	if( isCollectorType(node) ) {
	//		success |= addCollector(id, node);
	//	}
	//	if( isNodeType(node) ) {
	//		success |= addNode2(id, node);
	//	}
	//	return success;
	//}
//	template<typename N> bool addCollector(ID id, const N& node) { return false; }
//	bool addCollector(ID id, const boost::shared_ptr<Collector<ID, T> >& collector) { return collectors.add(id, collector); }


//	template<typename N> bool isNodeType(N any) { return false; }
//	bool isNodeType(const T_ptr& node) { return true; }

//	bool isCollectorType(const CollectorT_ptr& collector) { return true; }
//	bool isCollectorType(const boost::shared_ptr<Collector<ID, T> >& collector) { return true; }
	//template<typename N> bool isCollectorType(N any) { return false; }
	//template<> bool isCollectorType<>(boost::shared_ptr<Collector<ID, T> > collector) { return true; }
//	bool isCollectorType(const Collector<ID, T>& collector) { return true; }
//	bool isCollectorType(const T_ptr& any) { return false; }



#endif

