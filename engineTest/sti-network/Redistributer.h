#ifndef STI_UTILS_REDISTRIBUTER_H
#define STI_UTILS_REDISTRIBUTER_H

#include <boost/shared_ptr.hpp>

#include "AbstractDistributer.h"
#include "Collector.h"

#include "LocalCollector.h"
#include "RefConverter.h"


namespace STI
{
namespace Utils
{

//Stores reference to OUT instances and distributes them to OUT Collector.
//Collects IN references and converts these to OUT references using the
//provided RefConverter object. Converted IN references are conditionally added to the
//OUT distributer based on a provided policy.

template<class ID, class IN, class OUT>
class Redistributer : public Collector<ID, IN>, public AbstractDistributer<ID, OUT>
{
protected:

	typedef Collector<ID, OUT> CollectorOUT;
	typedef boost::shared_ptr<CollectorOUT> CollectorOUT_ptr;
	typedef LocalCollector<ID, OUT>::LocalCollectorPolicy_ptr CollectorPolicy_ptr;
	typedef RefConverter<IN, OUT> RefConverter_ptr;
	typedef boost::shared_ptr<IN> IN_ptr;
	typedef boost::shared_ptr<OUT> OUT_ptr;

	template <typename X, typename Y>
	struct SymRefConverter_ptr {
		typedef boost::shared_ptr< SymRefConverter<X, Y> > type;
	};

public:

	//Support passing either SymRefConverter<X, Y> or SymRefConverter<Y, X>
	template<typename X, typename Y> 
	Redistributer(const SymRefConverter_ptr<X, Y>::type& refConverter, const CollectorPolicy_ptr& policy) 
		: converter(refConverter), 
		collectorPolicy(policy), 
		nodes( new LocalCollector<ID, OUT>(collectorPolicy) ), 
		distributer(nodes)
	{
	}

	//STI::Utils::Distributer
	virtual bool addNode(const ID& id, const OUT_ptr& node)
	{
		//Conditionally add based on distributer's OUT Collector policy (collectorPolicy)
		return distributer.addNode(id, node);
	}

	virtual bool addCollector(const ID& id, const CollectorOUT_ptr& collector)
	{
		return distributer.addCollector(id, collector);
	}
	virtual void removeNode(const ID& id)
	{
		remove(id);
	}
	virtual void removeCollector(const ID& id)
	{
		remove(id);
	}

	virtual void cleanup()
	{
		distributer.cleanup();
	}
	virtual void distribute()
	{
		distributer.distribute();
	}
	virtual void refresh()
	{
		distributer.refresh();
	}

	//STI::Utils::Collector
	virtual bool addNode(const ID& id, const IN_ptr& node)
	{
		OUT_ptr outNode;

		//Conditionally add to distributer. Check include() first to avoid unnecessary conversions.
		return collectorPolicy->include(id) && 
			converter.convert(node, outNode) && 
			distributer.add(id, outNode);
	}

	virtual bool getNode(const ID& id, IN_ptr& node) const
	{
		OUT_ptr outNode;
		return nodes->get(id, outNode) && converter.convert(outNode, node);
	}	

	//STI::Utils::Collection
	virtual bool get(const ID& id, IN_ptr& node) const
	{
		return getNode(id, node);
	}

	virtual bool contains(const ID& id) const
	{
		return nodes->contains(id);
	}

	virtual void getIDs(std::set<ID>& ids) const
	{
		nodes->getIDs(ids);
	}

	virtual bool remove(const ID& id)
	{
		return distributer.remove(id);
	}

	virtual void cleanup(const std::set<ID>& ids)	//remove any collected nodes that are NOT in ids
	{
		nodes->cleanup(ids);
		distributer.cleanup();	//Cleans any collectors that have unwanted nodes.
	}
//	virtual void cleanup() = 0;		//provided by Distributer

	virtual void clear()
	{
		nodes->clear();
		refresh();
	}

private:

	RefConverter_ptr converter;				//Converts between IN references and OUT references.
	
	CollectorPolicy_ptr collectorPolicy;	//Determines if an OUT reference gets added to the distributer.
	
	CollectorOUT_ptr nodes;
	Distributer<ID, OUT> distributer;	//Uses 'nodes' to store OUT references. 
										//Distributes these to its Collectors.
};

	//class RedistributerPolicy : public STI::Utils::SynchronizedMapPolicy<ID>
	//{
	//public:
	//	virtual bool include(const ID& key) const { return (distributer->getCollectorCount() > 0); }
	//	virtual bool replace(const ID& oldKey, const ID& newKey) const { return true; }
	//};

} //namespace Utils
} //namespace STI


#endif
