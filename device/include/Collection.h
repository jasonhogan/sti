#ifndef STI_UTILS_COLLECTION_H
#define STI_UTILS_COLLECTION_H

#include <set>

#include <boost/shared_ptr.hpp>

namespace STI
{
namespace Utils
{

template<class ID, class B>
class Collection
{
protected:
	typedef boost::shared_ptr<B> B_ptr;

public:

	virtual ~Collection() {}

	virtual bool get(const ID& id, B_ptr& node) const = 0;

	virtual bool contains(const ID& id) const = 0;
	virtual void getIDs(std::set<ID>& ids) const = 0;

	virtual bool remove(const ID& id) = 0;
	virtual void cleanup(const std::set<ID>& ids) = 0;	//Remove any collected nodes that are NOT in ids
	virtual void cleanup() = 0;							//Remove any collected nodes that violate policy
	virtual void clear() = 0;
};

} //namespace Utils
} //namespace STI

#endif
