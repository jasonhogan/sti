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


	Distributer()
	{
		nodes = CollectorT_ptr(new LocalCollector<ID, T>());
	}
	Distributer(const CollectorT_ptr& nodeCollection)
	{
		nodes = nodeCollection;
	}
	
	//template<class N>
	//bool addNode(const ID& id, const boost::shared_ptr<N>& node) 
	//{
	//	if(isTypeT(node.get()))
	//		return addNode(id, node);
	//	return false;
	//}

	//template<class N>
	//bool isTypeT(N* n) { return false; }
	//bool isTypeT(T* n) { return true; }

	bool addNode(const ID& id, const T_ptr& node)
	{
		if(node != 0 && nodes->add(id, node)) {
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
		if(nodes->remove(id)) {
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
		typename std::set<ID> ids;
		collectors.getKeys(ids);
		CollectorT_ptr collector;

		for(typename std::set<ID>::iterator collectorID = ids.begin(); collectorID != ids.end(); ++collectorID) {
			if(collectors.get(*collectorID, collector) && collector != 0) {
				collector->add(id, node);	//conditionally adds the node, based on collector policy
			}
		}
	}

	void distributeRemove(const ID& id)
	{
		typename std::set<ID> ids;
		collectors.getKeys(ids);
		CollectorT_ptr collector;

		for(typename std::set<ID>::iterator collectorID = ids.begin(); collectorID != ids.end(); ++collectorID) {
			if(collectors.get(*collectorID, collector) && collector != 0) {
				collector->remove(id);
			}
		}
	}

	void distribute(const CollectorT_ptr& targetCollector)
	{
		typename std::set<ID> nodeIDs;
		nodes->getIDs(nodeIDs);

		distribute(nodeIDs, targetCollector);
	}

	
	void distribute(const std::set<ID>& nodeIDs, const CollectorT_ptr& targetCollector)
	{
		T_ptr node;

		for(typename std::set<ID>::const_iterator id = nodeIDs.begin(); id != nodeIDs.end(); ++id) {
			if(nodes->get(*id, node)) {
				targetCollector->add(*id, node);	//conditionally add based on collector policy
			}
		}
	}

	void distribute(const std::set<ID>& nodeIDs, const std::set<ID>& collectorIDs)
	{
		CollectorT_ptr collector;

		for(typename std::set<ID>::const_iterator id = collectorIDs.begin(); id != collectorIDs.end(); ++id) {
			if(collectors.get(*id, collector) && collector != 0) {
				distribute(nodeIDs, collector);
			}
		}
	}
	void cleanup(const std::set<ID>& nodeIDs, const std::set<ID>& collectorIDs)
	{
		CollectorT_ptr collector;

		for(typename std::set<ID>::iterator id = collectorIDs.begin(); id != collectorIDs.end(); ++id) {
			if(collectors.get(*id, collector) && collector != 0) {
				collector->cleanup(nodeIDs);	//force all collectors to remove nodes not found in the distributer's collection
			}
		}
	}



	CollectorT_ptr nodes;

//	LocalCollector<ID, T> nodes;
//	STI::Utils::SynchronizedMap<ID, T_ptr> nodes;
	typename STI::Utils::SynchronizedMap<ID, CollectorT_ptr> collectors;
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

