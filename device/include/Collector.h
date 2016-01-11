#ifndef STI_UTILS_COLLECTOR_H
#define STI_UTILS_COLLECTOR_H

#include <boost/shared_ptr.hpp>

#include "Collection.h"


namespace STI
{
namespace Utils
{

template<typename ID, typename T, typename B = T>
class Collector : public Collection<ID, B>
{
protected:
	typedef boost::shared_ptr<T> T_ptr;

public:

	virtual ~Collector() {}

	virtual bool addNode(const ID& id, const T_ptr& node) = 0;
	virtual bool getNode(const ID& id, T_ptr& node) const = 0;

};

} //namespace Utils
} //namespace STI

#endif
