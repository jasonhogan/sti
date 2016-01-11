#ifndef STI_UTILS_ABSTRACTDISTRIBUTER_H
#define STI_UTILS_ABSTRACTDISTRIBUTER_H


namespace STI
{
namespace Utils
{

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

} //namespace Utils
} //namespace STI

#endif
