#ifndef STI_UTILS_LOCALCOLLECTOR_H
#define STI_UTILS_LOCALCOLLECTOR_H

#include "Collector.h"
#include "SynchronizedMap.h"

#include <set>

namespace STI
{
namespace Utils
{

template<class ID, class T, typename B = T>
class LocalCollector : public Collector<ID, T, B>
{
public:
	typedef STI::Utils::SynchronizedMapPolicy<ID> LocalCollectorPolicy;
	typedef boost::shared_ptr<LocalCollectorPolicy> LocalCollectorPolicy_ptr;

	LocalCollector() {}
	LocalCollector(const LocalCollectorPolicy_ptr& policy) : nodes(policy) {}

	bool addNode(const ID& id, const typename Collector<ID, T, B>::T_ptr& node) { return nodes.add(id, node); }
	bool getNode(const ID& id, typename Collector<ID, T, B>::T_ptr& node) const { return nodes.get(id, node); }
	
	bool get(const ID& id, typename Collection<ID, B>::B_ptr& node) const 
	{
		Collector<ID, T, B>::T_ptr tNode;
		return getNode(id, tNode) && !((node = tNode) == 0);	//class T : public B
	}
	
	void getIDs(std::set<ID>& ids) const { nodes.getKeys(ids); }
	bool contains(const ID& id) const { return nodes.contains(id); }

	bool remove(const ID& id) { return nodes.remove(id); }
	void cleanup(const std::set<ID>& ids)
	{
		typename std::set<ID> storedIDs;
		getIDs(storedIDs);

		typename std::set<ID>::const_iterator clean;

		for(typename std::set<ID>::iterator id = storedIDs.begin(); id != storedIDs.end(); ++id) {
			clean = ids.find(*id);
			if(clean == ids.end()) {
				remove(*id);
			}
		}
	}
	virtual void cleanup()
	{
		nodes.cleanup();
	}
	void clear() { nodes.clear(); }

private:
	STI::Utils::SynchronizedMap<ID, typename Collector<ID, T, B>::T_ptr> nodes;
};

}
}

#endif

