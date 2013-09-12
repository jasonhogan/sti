#ifndef STI_UTILS_LOCALCOLLECTOR_H
#define STI_UTILS_LOCALCOLLECTOR_H

#include "Collector.h"
#include "SynchronizedMap.h"

#include <set>

namespace STI
{
namespace Utils
{

template<class ID, class T>
class LocalCollector : public Collector<ID, T>
{
public:
	typedef STI::Utils::SynchronizedMapPolicy<ID> LocalCollectorPolicy;
	typedef boost::shared_ptr<LocalCollectorPolicy> LocalCollectorPolicy_ptr;

	LocalCollector() {}
	LocalCollector(const LocalCollectorPolicy_ptr& policy) : nodes(policy) {}

	void getIDs(std::set<ID>& ids) const { nodes.getKeys(ids); }
	bool get(const ID& id, typename Collector<ID, T>::T_ptr& node) const { return nodes.get(id, node); }
	bool contains(const ID& id) const { return nodes.contains(id); }

	bool add(const ID& id, const typename Collector<ID, T>::T_ptr& node) { return nodes.add(id, node); }
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

}
}

#endif

