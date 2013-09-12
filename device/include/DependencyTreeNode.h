#ifndef STI_UTILS_DEPENDENCYTREENODE
#define STI_UTILS_DEPENDENCYTREENODE

#include <boost/shared_ptr.hpp>

#include <vector>

namespace STI
{
namespace Utils
{

//template<class T> class DependencyTreeNode;


template<class T>
class DependencyTreeNode
{
	typedef boost::shared_ptr< DependencyTreeNode<T> > DependencyTreeNode_ptr;

public:
	DependencyTreeNode(const T& content) : nodeContent(content), dependencyCount_l(0) {}
	~DependencyTreeNode()
	{
		releaseTargets();
	}

	const T& getNodeContent() const { return nodeContent; }
	int dependencyCount() const { return dependencyCount_l; }
	void addTargetNode(const DependencyTreeNode_ptr& node)
	{
		targetNodes.push_back(node);
		node->addDependency();
	}

	void releaseTargets()
	{
		for(unsigned i = 0; i < targetNodes.size(); i++) {
			targetNodes.at(i)->removeDependency();
		}
		targetNodes.clear();
	}

	void addDependency() { dependencyCount_l++; }
	void removeDependency() { dependencyCount_l--; }

private:
	T nodeContent;
	std::vector<DependencyTreeNode_ptr> targetNodes;
	int dependencyCount_l;
};

}
}

#endif

