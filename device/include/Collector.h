#ifndef STI_UTILS_COLLECTOR_H
#define STI_UTILS_COLLECTOR_H

#include <boost/shared_ptr.hpp>

namespace STI
{
namespace Utils
{


template<class ID, class T>
class Collector
{
protected:
	typedef boost::shared_ptr<T> T_ptr;

public:

	virtual ~Collector() {}

	virtual void getIDs(std::set<ID>& ids) const = 0;
	virtual bool get(const ID& id, T_ptr& node) const = 0;
	virtual bool contains(const ID& id) const = 0;

	virtual bool add(const ID& id, const T_ptr& node) = 0;
	virtual bool remove(const ID& id) = 0;
	virtual void cleanup(const std::set<ID>& ids) = 0;	//remove any collected nodes that are NOT in ids
	virtual void clear() = 0;
};

}
}

#endif

